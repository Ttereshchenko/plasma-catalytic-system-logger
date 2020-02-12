#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const int ledPin = 12;

// setting PWM properties
const int freq = 1000;
const int ledChannel = 0;
const int resolution = 12;

// Replace these with your WiFi network settings
#ifdef KAA
const char *ssid = "kaaiot";          //replace this with your WiFi network name
const char *password = "vapemach1ne"; //replace this with your WiFi network password
const char *mqttServer = "10.2.0.207";
#else

#endif
const int led = 18;

const int mqttPort = 1883;
const char *mqttUser = "es0";
const char *mqttPassword = "qwerty";

WiFiClient espClient;
PubSubClient client(espClient);
void callback(char *topic, byte *payload, unsigned int length);

void WifiInit()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.printf("Connecting to %s", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.printf("Connection status: %d\n", WiFi.status());
  }
  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttServer, mqttPort);
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword))
    {

      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish("esp/test", "Hello from ESP8266");
  client.setCallback(callback);
  client.subscribe("esp/echo");
}

void setup()
{
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  WifiInit();
  // put your setup code here, to run once:
}

void loop()
{
  client.loop();
  /*for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }*/
}

const char *TAG_CMD = "cmd";
const char *TAG_PATH = "path";
const char *TAG_VALUE = "value";
const char *CMD_ECHO = "echo";
const char *CMD_SET_LED = "set";

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.printf("Message arrived in topic: %s message: %.*s", topic, length, payload);
  StaticJsonDocument<255> doc;

  DeserializationError error = deserializeJson(doc, (char *)payload);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  const char *cmd = doc[TAG_CMD] | CMD_ECHO;

  if (strncmp(CMD_ECHO, cmd, strlen(CMD_ECHO)) == 0)
  {
    Serial.println();
    serializeJsonPretty(doc, Serial);
  }
  else if (strncmp(CMD_SET_LED, cmd, strlen(CMD_SET_LED)) == 0)
  {
    int dutyCycle = doc[TAG_VALUE] | 0;
    dutyCycle &= (1 << resolution) - 1;
    ledcWrite(ledChannel, dutyCycle);
  }

  /*const char *command = doc["cmd"];
  if (true)
  {
    byte *p = (byte *)malloc(length);
    serializeJson(doc, p, length);
    client.publish("esp/echo/resp", p, length);
    // Free the memory
    free(p);
  }*/
}