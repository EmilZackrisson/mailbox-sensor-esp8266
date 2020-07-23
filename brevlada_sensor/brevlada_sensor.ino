#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include "secrets.h"
 
const int power = 4; // D2

ADC_MODE(ADC_VCC);
int Batt;
 
//Enter your mqtt server configurations
const char* mqttServer = "192.168.0.56";    //Enter Your mqttServer address
const int mqttPort = 1883;       //Port number
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  pinMode(power, OUTPUT);
  digitalWrite(power, HIGH);
  delay(100);
  Serial.begin(115200);

 
  WiFi.begin(ssid, password);
  WiFi.softAPdisconnect(true);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266_brevlada", user, pass)) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("Cound not connect to MQTT");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);
 
    }
  }
}
 
void loop() {
  client.loop();

  Batt = ESP.getVcc()/10;
  char buf[4];
  sprintf (buf, "%03i", Batt);
  
    client.publish("brevlada/state", "on");
    client.publish("brevlada/battery", buf);
    Serial.println(Batt);
    delay(2000);
    client.publish("brevlada/state", "off");
    delay(500);
    digitalWrite(power, LOW); // Turn ESP off
}
