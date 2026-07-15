
#include "DisplayModule.h"
#include "config.h"

void DisplayModule::begin() {
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  display.begin();
  display.setFont(u8g2_font_ncenB08_tr);
}

void DisplayModule::update() {
  display.clearBuffer();
  display.drawStr(0, 20, "Olly is awake");
  display.sendBuffer();
}
