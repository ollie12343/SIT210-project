#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "secrets.h"

// Wifi and MQTT
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient client;
MqttClient mqttClient(client);

const char broker[] = "broker.emqx.io";
int port = 1883;
const char topic[] = "SIT210/wave/Oliver";

int trigPin = 11;
int echoPin = 12;
int led = 10;
long now, wave_delay, last, temp;

long duration, cm;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);

  
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
  last = 0;

  mqttClient.subscribe(topic);
}

void loop() {
  now = millis();

  //allows a wave/pat every 5 seconds from the end of the last wave/pat
  if (now - last >= 5000) {
    
    cm = get_dist();
    //Serial.println(cm);

    if (cm <= 40) {
      delay(500);
      wave_delay = millis();
      while (millis() - wave_delay < 1000) {
        temp = get_dist();
        if (30 < temp && temp < 40) {
          publish("wave");
          last = millis();
          break;
        } 
        else if (temp < 10 && temp) {
            publish("pat");
            last = millis();
          break;
        }
      }
    }
  }
   
  // // subscribe to mqtt here
  int messageSize = mqttClient.parseMessage();
  String message;
  for (int i = 0; i < messageSize;i++)
  {
    message += (char)mqttClient.read();
  }
  if (messageSize != 0)
  {
    if (message == "wave") {
      waveLed();
      Serial.println("Oliver Waved!");
    }
    else if (message == "pat"){
      patLed();
      Serial.println("Oliver Patted!");
    } 
  }
}


long get_dist() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  duration = pulseIn(echoPin, HIGH);

  return (duration / 2) / 29.1;
}

void publish(String text) {
  // publish to topic
  mqttClient.beginMessage(topic);
  mqttClient.print(text);
  mqttClient.endMessage();
}

void patLed() {
  flash(1000, 100);
  flash(1000, 100);
}

void waveLed() {
  flash(100, 100);
  flash(100, 100);
  flash(100, 100);
}

void flash(int on, int off) {
  digitalWrite(led, HIGH);
  delay(on);
  digitalWrite(led, LOW);
  delay(off);
}