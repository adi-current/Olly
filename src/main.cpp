#include <Arduino.h>
#include "DisplayModule.h"

DisplayModule display;

void setup() {
  display.begin();
}

void loop() {
  display.update();
}