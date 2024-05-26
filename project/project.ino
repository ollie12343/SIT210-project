#include <Vector.h>

#include "CameraOV7670.h"

const uint16_t lineLength = 160;
const uint16_t lineCount = 120;
const uint32_t baud  = 115200;
const uint16_t lineBufferLength = lineLength;
const bool isSendWhileBuffering = true;
CameraOV7670 camera(CameraOV7670::RESOLUTION_QQVGA_160x120, CameraOV7670::PIXEL_YUV422, 17);

uint16_t processedByteCountDuringCameraRead = 0;
uint8_t lineBuffer [lineBufferLength]; // Two bytes per pixel
uint8_t * lineBufferSendByte;

Vector<Vector<int>> img;

void setup() {
  // This is not necessary and has no effect for ATMEGA based Arduinos.
  // WAVGAT Nano has slower clock rate by default. We want to reset it to maximum speed
  //CLKPR = 0x80; // enter clock rate change mode
  //CLKPR = 0; // set prescaler to 0. WAVGAT MCU has it 3 by default.

  Serial.begin(500000);
  camera.init();
}


void loop() {
  processFrame();

  for (int i = 0; i < img.size(); i++)
  {
   for (int j = 0; j < img[0].size(); j++)
   {
     Serial.print(img[i][j]);
   }
   Serial.println("");
 }


  delay(5000);
}



// this is called in Arduino loop() function
void processFrame() {
  processedByteCountDuringCameraRead = 0;

  noInterrupts();

  processGrayscaleFrameBuffered();

  interrupts();
}


void processGrayscaleFrameBuffered() {
camera.waitForVsync();

  camera.ignoreVerticalPadding();

  for (uint16_t y = 0; y < lineCount; y++) {
    lineBufferSendByte = &lineBuffer[0];
    camera.ignoreHorizontalPaddingLeft();

    Vector<int> temp;

    uint16_t x = 0;
    while ( x < lineBufferLength) {
      camera.waitForPixelClockRisingEdge(); // YUV422 grayscale byte
      camera.readPixelByte(lineBuffer[x]);
      lineBuffer[x] = formatPixelByteGrayscaleFirst(lineBuffer[x]);

      //Serial.print(lineBuffer[x]);
      //Serial.print(":");
      temp.push_back(lineBuffer[x]);

      camera.waitForPixelClockRisingEdge(); // YUV422 color byte. Ignore.

      x++;

      
      camera.waitForPixelClockRisingEdge(); // YUV422 grayscale byte
      camera.readPixelByte(lineBuffer[x]);
      lineBuffer[x] = formatPixelByteGrayscaleSecond(lineBuffer[x]);

      camera.waitForPixelClockRisingEdge(); // YUV422 color byte. Ignore.
      //Serial.print(lineBuffer[x]);
      //Serial.print(", ");
      x++;
    }
    camera.ignoreHorizontalPaddingRight();

    // Debug info to get some feedback how mutch data was processed during line read.
    processedByteCountDuringCameraRead = lineBufferSendByte - (&lineBuffer[0]);

    //Serial.println("");
    img.push_back(temp);

  };
}

uint8_t formatPixelByteGrayscaleFirst(uint8_t pixelByte) {
  // For the First byte in the parity check byte pair the last bit is always 0.
  pixelByte &= 0b11111110;
  if (pixelByte == 0) {
    // Make pixel color always slightly above 0 since zero is a command marker.
    pixelByte |= 0b00000010;
  }
  return pixelByte;
}

uint8_t formatPixelByteGrayscaleSecond(uint8_t pixelByte) {
  // For the second byte in the parity chek byte pair the last bit is always 1.
  return pixelByte | 0b00000001;
}


int readBinaryString(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}