#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "secrets.h"



const int buttonPin = 2; 

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
  Serial.begin(9000);

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

}

void publish(String text) {
  // publish to topic
  mqttClient.beginMessage(topic);
  mqttClient.print(text);
  mqttClient.endMessage();
}
