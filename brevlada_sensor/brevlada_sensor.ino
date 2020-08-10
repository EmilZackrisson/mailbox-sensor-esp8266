#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include "secrets.h"

const int power = 4; // D2

ADC_MODE(ADC_VCC);
int Batt;
int timesRun;

//Enter your mqtt server configurations
const char* mqttServer = "192.168.0.56";    //Enter Your mqttServer address
const int mqttPort = 1883;       //Port number
int willQoS = 1;
const char* willTopic = "willTopic";
const char* willMessage = "hejsan";
boolean willRetain = false;

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

    if (client.connect("ESP8266_brevlada", user, pass, willTopic, willQoS, willRetain, willMessage)) {

      Serial.println("connected");

    } else {

      Serial.print("Cound not connect to MQTT");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);
      digitalWrite(power, LOW); // Turn ESP off
      delay(500);
    }
  }
}

void loop() {
  if (timesRun <= 3) {
    client.loop();

    Batt = ESP.getVcc() / 10;
    char buf[4];
    sprintf (buf, "%03i", Batt);

    char* openMessage = "on";
    int openLength = strlen(openMessage);
    boolean retained = true;
    client.publish("brevlada/state", (byte*)openMessage, openLength, retained);
    Serial.println("on");

    char* batteryMessage = buf;
    int battLength = strlen(batteryMessage);
    client.publish("brevlada/battery", (byte*)batteryMessage, battLength, retained);
    Serial.println(batteryMessage);
    delay(2000);

    char* closedMessage = "off";
    int closedLength = strlen(closedMessage);
    client.publish("brevlada/state", (byte*)closedMessage, closedLength, retained);
    Serial.println("off");

    delay(500);
    digitalWrite(power, LOW); // Turn ESP off
    timesRun++;
    Serial.println(timesRun);
    delay(10000);
  }

  if (timesRun >= 3) {
    Serial.println("Going to sleep");
    WiFi.disconnect();
    WiFi.forceSleepBegin();
    delay(1); //For some reason the modem won't go to sleep unless you do a delay
  }
}
