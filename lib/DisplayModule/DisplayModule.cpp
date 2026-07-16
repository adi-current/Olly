
#include "DisplayModule.h"
#include "config.h"

void DisplayModule::begin() {
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  display.begin();
  display.setFont(u8g2_font_ncenB08_tr);
}

void DisplayModule::update() {
  display.clearBuffer();
  // Face canvas is 96x64, centered on the 128x64 panel -> 16px margin each side
  display.drawXBMP(16, 0, OLLY_FACE_WIDTH, OLLY_FACE_HEIGHT, olly_face_neutral);
  display.sendBuffer();
}