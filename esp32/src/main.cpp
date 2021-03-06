#include <Arduino.h>
#include <WiFi.h> 
#include <PubSubClient.h> // mqtt client
#include <ArduinoJson.h> // Json
#include "MAX31855.h" 
#include "max6675.h" 
#include <Adafruit_INA219.h>
#include <Wire.h>

Adafruit_INA219 ina219(0x40);
Adafruit_INA219 ina219_2(0x41);
#define I2C_SDA_PIN 23
#define I2C_SCL_PIN 22

// DO=19; CS=5; CLK=18
const int doPin = 19;
const int csPin0 = 4;
const int clPin = 18;

//  next one 
// const int clPin1 = 17;
// MAX31855 tc1(clPin, csPin1, doPin);

MAX31855 tc(clPin, csPin0, doPin);
const int csPin1 = 2;
MAX6675 tc2(clPin, csPin1, doPin);
const int csPin2 = 0;
MAX6675 tc3(clPin, csPin2, doPin);



// Wi-Fi settings
const char* ssid     = "name";
const char* password = "password";

// MQTT settings
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "10.2.0.74";
const char *mqttUser = "test";
const char *mqttPassword = "test";
const int mqtt_port = 1883;

WiFiClient wifi_client;
void callback(char* topic, byte* payload, unsigned int length)
{}

PubSubClient client(mqtt_server, mqtt_port, callback, wifi_client);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP-32-Client", mqttUser, mqttPassword)) {
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 5 seconds; ");
      Serial.print("from IP address: ");
      Serial.println(WiFi.localIP());
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    Serial.println("Still connecting to Wi-Fi...");
  }

  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  tc.begin();
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);// 1000000
  ina219.begin();
  ina219_2.begin();
  ina219.setCalibration_16V_400mA();
  ina219_2.setCalibration_16V_400mA();
}

long iterator = 0;
StaticJsonDocument<200> doc;

void loop() {
  if (!client.connected()) { // TODO: move into setup() for saving CPU's circles 
    reconnect();
  }
  client.loop();
  tc.read();
  doc["id"] = ++iterator;
  doc["t1"] = tc.getTemperature();
  doc["t2"] = tc2.readCelsius();
  doc["t3"] = tc3.readCelsius();
  doc["mA"] = ina219_2.getCurrent_mA();
  doc["V"] = ina219.getShuntVoltage_mV()/1000 + ina219_2.getBusVoltage_V();

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  client.publish("esp32/temperature", buffer, n);
}
