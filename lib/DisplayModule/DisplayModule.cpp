
#include "DisplayModule.h"
#include "config.h"

void DisplayModule::begin() {
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  display.begin();
  display.setFont(u8g2_font_ncenB08_tr);
  face.begin(&display, STRIP_WIDTH, 0); // face origin sits right of the icon strip
}

void DisplayModule::update() {
  display.clearBuffer();

  // --- Status strip: 32px on the left (128 - 96 face width) ---
  display.drawVLine(STRIP_WIDTH - 1, 0, 64); // divider line, right edge of strip

  // WiFi icon, centered horizontally in the strip, near the top
  // Swaps automatically between connected/disconnected glyphs based on state
  if (wifiConnected) {
    int wifiX = (STRIP_WIDTH - WIFI_ICON_WIDTH) / 2;
    display.drawXBMP(wifiX, 6, WIFI_ICON_WIDTH, WIFI_ICON_HEIGHT, wifi_icon);
  } else {
    int wifiX = (STRIP_WIDTH - WIFI_OFF_ICON_WIDTH) / 2;
    display.drawXBMP(wifiX, 6, WIFI_OFF_ICON_WIDTH, WIFI_OFF_ICON_HEIGHT, wifi_off_icon);
  }

  // Battery icon, centered, middle of the strip
  int battX = (STRIP_WIDTH - BATTERY_ICON_WIDTH) / 2;
  display.drawXBMP(battX, 27, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT, battery_icon);

  // Clock text, bottom of the strip - biased 1px left so it doesn't
  // crowd the divider line (moving the line itself would risk cutting
  // into the face bitmap, since strip + face exactly fill 128px)
  char timeStr[6];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeHour, timeMinute);
  display.setFont(u8g2_font_6x13_tr);
  int textWidth = display.getStrWidth(timeStr);
  int textX = (STRIP_WIDTH - textWidth) / 2 - 1;
  if (textX < 0) textX = 0;
  display.drawStr(textX, 60, timeStr);

  // --- Face: border + eyes are now procedural (FaceModule) ---
  face.drawBorder();
  face.drawEyes(EyeStyle::OPEN);   // ignored while a blink is in progress
  face.updateBlink();              // owns eye height during a blink; auto-blinks on a timer too

  // Brows/mouth/whiskers are still the original hand-drawn bitmap pieces,
  // drawn at the same coordinates they occupied in the old full-face
  // bitmap, just offset past the icon strip. These move into FaceModule
  // in a later pass (BrowStyle / MouthStyle), same treatment as the eyes.
  int fx = STRIP_WIDTH;
  display.drawXBMP(fx + 24, 16, BROW_LEFT_WIDTH,  BROW_LEFT_HEIGHT,  brow_left_bmp);
  display.drawXBMP(fx + 59, 16, BROW_RIGHT_WIDTH, BROW_RIGHT_HEIGHT, brow_right_bmp);
  display.drawXBMP(fx + 42, 46, MOUTH_WIDTH,      MOUTH_HEIGHT,      mouth_bmp);
  display.drawXBMP(fx + 14, 41, WHISKER_LEFT_WIDTH,  WHISKER_LEFT_HEIGHT,  whisker_left_bmp);
  display.drawXBMP(fx + 76, 41, WHISKER_RIGHT_WIDTH, WHISKER_RIGHT_HEIGHT, whisker_right_bmp);

  display.sendBuffer();
}

void DisplayModule::triggerBlink() {
  face.triggerBlink();
}

void DisplayModule::setWifiConnected(bool connected) {
  wifiConnected = connected;
}

void DisplayModule::setBatteryPercent(uint8_t percent) {
  batteryPercent = percent;
}

void DisplayModule::setTime(uint8_t hour, uint8_t minute) {
  timeHour = hour;
  timeMinute = minute;
}