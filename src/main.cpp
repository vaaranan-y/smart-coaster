#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "WiFi.h"

#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"

const int LOADCELL_SCK = 4;
const int LOADCELL_DOUT = 16;

int rawScaleValue;
double calibratedMassValue;

HX711 scale;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  if (strstr(topic, "esp32/sub"))
  {
    Serial.print("Message Received from Sub Topic!");
  }
  Serial.println();
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["rawScaleValue"] = rawScaleValue;
  doc["calibratedMassValue"] = calibratedMassValue;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  Serial.println("Sending Data");
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup()
{
  Serial.begin(115200);
  setCpuFrequencyMhz(RTC_CPU_FREQ_80M);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");

  Serial.print("ESP32 Serial Monitor Test\n"); // Print a message to the serial monitor
  pinMode(LED_BUILTIN, OUTPUT);
  scale.set_scale();
  // Serial.println("Tare... remove any weights from the scale.");
  // // delay(5000);
  // scale.tare();
  // Serial.println("Tare done...");
}

void loop()
{
  Serial.print("Place a known weight on the scale...");
  delay(2500);
  double scaleVal = 395.32;
  long reading = scale.get_units(10);
  rawScaleValue = reading;
  calibratedMassValue = reading / 395.32;

  if (isnan(rawScaleValue) || isnan(calibratedMassValue)) // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Error: failed to read from load cell"));
    return;
  }

  publishMessage();
  client.loop();
  delay(1000);
}