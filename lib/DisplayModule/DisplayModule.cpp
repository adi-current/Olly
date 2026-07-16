
#include "DisplayModule.h"
#include "config.h"

void DisplayModule::begin() {
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  display.begin();
  display.setFont(u8g2_font_ncenB08_tr);
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

  // --- Face: 96x64, right-aligned, flush against the strip ---
  display.drawXBMP(STRIP_WIDTH, 0, OLLY_FACE_WIDTH, OLLY_FACE_HEIGHT, olly_face_neutral);

  display.sendBuffer();
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