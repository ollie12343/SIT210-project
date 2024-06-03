#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <Servo.h>
#include "secrets.h"

Servo myservo;

const int buttonPin = 2;
const int servoPin = 9;

int buttonState  = 0;
int toggle = 0;


// Wifi and MQTT
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient client;
MqttClient mqttClient(client);

const char broker[] = BROKER;
int port = 1883;
const char topic[] = TOPIC;


void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);

  pinMode(buttonPin, INPUT);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    WiFi.begin(ssid, pass);
  }
  Serial.println("Connected!");

  Serial.println("Connecting to MQTT...");
  while (!mqttClient.connect(broker, port)) {
    Serial.print("Connection failed! Error code: ");
    Serial.println(mqttClient.connectError());
  }
  Serial.println("Connected!");

  mqttClient.subscribe(topic);

  myservo.write(140);
}


void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && toggle == 0)
  {
    publish("On");
    Serial.println("On");
    toggle = 1;
  } 
  else if (buttonState == LOW && toggle == 1)
  {
    publish("Off");
    Serial.println("Off");
    toggle = 0;
  }

  int messageSize = mqttClient.parseMessage();
  String message;
  for (int i = 0; i <messageSize; i++)
  {
    message += (char)mqttClient.read();
  }
  if (messageSize !=0)
  {
    if (message == "Unlock")
    {
      myservo.write(40);
    }
    else if (message == "Lock")
    {
      myservo.write(140);
    }
  }

}

void publish(String text) {
  // publish to topic
  mqttClient.beginMessage(topic);
  mqttClient.print(text);
  mqttClient.endMessage();
}
