#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

class DisplayModule {
  public:
    void begin();
    void update();

  private:
    // Full-buffer SH1106 128x64 driver over hardware I2C
    // (many "SSD1306" 0.96" OLEDs are actually SH1106 with a 132-col RAM
    // and a 2px offset — using the wrong driver causes edge garbage/shift)
    U8G2_SH1106_128X64_NONAME_F_HW_I2C display{U8G2_R0, U8X8_PIN_NONE};
};

#endif