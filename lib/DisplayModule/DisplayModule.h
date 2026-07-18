#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "status_icons.h"
#include "face_static_parts.h"
#include "FaceModule.h"
// NOTE: olly_face_96x64.h is intentionally no longer included here.
// The face is now drawn procedurally by FaceModule (border + eyes).
// The old file is kept in this folder untouched, purely as a visual
// style reference for building the remaining parts (brows/mouth/etc).

class DisplayModule {
  public:
    void begin();
    void update();

    // Phase 3 status inputs - call these whenever the values change
    void setWifiConnected(bool connected);
    void setBatteryPercent(uint8_t percent);   // 0-100
    void setTime(uint8_t hour, uint8_t minute);

    // Call this (e.g. on a touch event) to make Olly blink right now,
    // instead of waiting for the next automatic blink.
    void triggerBlink();

  private:
    bool wifiConnected = false;
    uint8_t batteryPercent = 100;
    uint8_t timeHour = 12;
    uint8_t timeMinute = 0;

    static const int STRIP_WIDTH = 32; // 128 - 96 (face width)
    // Full-buffer SH1106 128x64 driver over hardware I2C
    // (many "SSD1306" 0.96" OLEDs are actually SH1106 with a 132-col RAM
    // and a 2px offset — using the wrong driver causes edge garbage/shift)
    U8G2_SH1106_128X64_NONAME_F_HW_I2C display{U8G2_R0, U8X8_PIN_NONE};

    FaceModule face;
};

#endif