#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "esp32-hal-cpu.h"

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

  if (scale.is_ready())
  {
    scale.set_scale();
    Serial.println("SETTING SCALE...\n");
    delay(3000); // time to 'zero' out the scale
    scale.tare();
    Serial.println("TARING COMPLETE\n");
    Serial.println("MEASURING...\n");
    delay(3000);                        // time to place actual weight on the scale
    long measure = scale.get_units(10); // performs 10 readings;
    Serial.print("Value: ");
    Serial.println(measure);
  }

  // TESTING CODE

  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(100);

  // Serial.print("WAITING\n");

  // digitalWrite(LED_BUILTIN, LOW);
  // delay(100);
}
