#include <Arduino.h>

void setup()
{
  Serial.begin(115200); // Set the baud rate to 115200 (change if needed)
  delay(2000);          // Delay for 2 seconds

  Serial.print("ESP32 Serial Monitor Test"); // Print a message to the serial monitor
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // Your main code goes here
  // Add any additional functionality or logic as needed

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  Serial.print("WAITING\n");

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
