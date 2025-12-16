#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give serial time to initialize
  Serial.println("Hello World!");
}

void loop() {
  Serial.println("Hello World!");
  delay(1000);
}
