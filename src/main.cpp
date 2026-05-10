#include <Arduino.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Microrrede OK");
  delay(1000);
}
