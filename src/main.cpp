#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "esp32-hal-cpu.h"
#include "secrets.h"

#define AWS_PUB_TOPIC "smart_coaster/pub"
#define AWS_SUB_TOPIC "smart_coaster/sub"

const int LOADCELL_SCK = 4;
const int LOADCELL_DOUT = 16;

int rawScaleValue;
double calibratedMassValue;

HX711 scale;
WiFiClientSecure network = WiFiClientSecure();
PubSubClient pubSubClient(network);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("WAITING");
  }

  Serial.println("WiFi Network Connection Successful");
  // Configure WiFiClientSecure with AWS IoT Thing credentials
  network.setCACert(AWS_CERT_CA);
  network.setCertificate(AWS_CERT_CRT);
  network.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the AWS endpoint
  pubSubClient.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  pubSubClient.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!pubSubClient.connect(THINGNAME))
  {
    Serial.println("WAITING");
    delay(100);
  }

  if (!pubSubClient.connected())
  {
    Serial.println("AWS IoT Timeout");
    return;
  }

  // Subscribe to a topic
  pubSubClient.subscribe(AWS_SUB_TOPIC);

  Serial.println("AWS IOT Connection Successful");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["rawScaleValue"] = rawScaleValue;
  doc["calibratedMassValue"] = calibratedMassValue;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  pubSubClient.publish(AWS_PUB_TOPIC, jsonBuffer);
}

void setup()
{
  Serial.begin(115200); // Set the baud rate to 115200 (change if needed)
  setCpuFrequencyMhz(RTC_CPU_FREQ_80M);
  scale.begin(LOADCELL_DOUT, LOADCELL_SCK);

  Serial.print("ESP32 Serial Monitor Test\n"); // Print a message to the serial monitor
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // Your main code goes here
  // Add any additional functionality or logic as needed

  double scaleVal = 395.32;

  if (scale.is_ready())
  {
    scale.set_scale();
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
    Serial.print("ACTUAL MEASURE: ");
    Serial.print(reading / 395.32);
    Serial.println("g");

    rawScaleValue = reading;
    calibratedMassValue = reading / 395.32;

    publishMessage();
    pubSubClient.loop();
  }
  else
  {
    Serial.println("HX711 not found.");
  }
  delay(1000);

  // TESTING CODE

  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(100);

  // Serial.print("WAITING\n");

  // digitalWrite(LED_BUILTIN, LOW);
  // delay(100);
}
