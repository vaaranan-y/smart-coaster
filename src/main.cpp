#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "esp32-hal-cpu.h"

#define AWS_PUB_TOPIC = "smart_coaster/pub"
#define AWS_SUB_TOPIC = "smart_coaster/sub"

const int LOADCELL_SCK = 4;
const int LOADCELL_DOUT = 16;

HX711 scale;

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
