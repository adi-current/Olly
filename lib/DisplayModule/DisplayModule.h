#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "status_icons.h"
#include "face_static_parts.h"
#include "FaceModule.h"
#include "MenuModule.h"
// NOTE: olly_face_96x64.h is intentionally no longer included here.
// The face is now drawn procedurally by FaceModule (border + eyes).
// The old file is kept in this folder untouched, purely as a visual
// style reference for building the remaining parts (brows/mouth/etc).

class DisplayModule {
  public:
    void begin();

    // Call every loop() with the real input signals for this frame:
    //   rotationDelta -> encoder clicks since last call (+/-)
    //   confirmPressed -> encoder click (forward/commit, everywhere)
    //   backPressed    -> touch long-press (back one level, everywhere)
    // Draws Home (face + strip) when the menu isn't open, or the menu
    // itself when it is - the two are mutually exclusive on screen.
    // Returns true exactly once, the frame a feature gets selected at the
    // deepest menu level - read selectedGroup()/selectedItem() that frame.
    bool update(int rotationDelta, bool confirmPressed, bool backPressed);

    // Valid only on the frame update() returned true.
    int selectedGroup() const;
    int selectedItem() const;

    // Lets other full-screen features (games, fun, future timers) draw
    // directly on the same physical display when they're active. DisplayModule
    // deliberately doesn't know these features exist - main.cpp is the one
    // that decides whether Home/Menu or some other feature owns the screen
    // this frame, same way it already owns which input goes where.
    U8G2* getRawDisplay() { return &display; }

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
    MenuModule menu;
};

#endif