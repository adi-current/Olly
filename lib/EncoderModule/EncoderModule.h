#pragma once
#include <Arduino.h>

// Driver for a KY-040 rotary encoder module: quadrature rotation (CLK/DT)
// plus its built-in push button (SW).
//
// Rotation uses a state-table quadrature decoder (not naive "read CLK, check
// if DT differs") because cheap encoders like the KY-040 bounce badly on
// their contacts - a naive read routinely registers 2-3 steps for one
// physical click, or even occasional steps in the wrong direction. The
// state table only counts a full, valid quadrature cycle as one step, which
// filters that out almost entirely without needing any capacitors/RC
// filtering on the board itself.
//
// The button gets simple polled debounce (in update()) plus short/long
// press classification, matching the interaction rules already established
// project-wide: encoder click = forward/confirm, touch long-press = back.
class EncoderModule {
  public:
    void begin();

    // Call every loop() iteration. Cheap - just reads the debounce timer
    // and drains the ISR-filled rotation queue.
    void update();

    // Positive = clockwise step(s), negative = counter-clockwise,
    // 0 = no movement since last call. Consumes/resets after reading.
    int takeRotation();

    // Edge-triggered, consume-once flags - each returns true exactly once
    // per physical press, then resets, so calling it in a loop doesn't
    // fire repeatedly for one press.
    bool wasClicked();      // short press released
    bool wasLongPressed();  // held past LONG_PRESS_MS

    bool isButtonDown() const;

  private:
    static const unsigned long DEBOUNCE_MS = 15;
    static const unsigned long LONG_PRESS_MS = 600;

    // Button state
    bool buttonStableState = false;   // true = pressed (debounced)
    bool buttonRawLast = false;
    unsigned long buttonLastChangeMs = 0;
    unsigned long buttonPressedAtMs = 0;
    bool longPressFired = false;
    bool clickPending = false;
    bool longPressPending = false;

    // Rotation - ISR writes into this, update()/takeRotation() drain it.
    // Declared here so the .cpp's ISR (a free function/static) can reach it
    // via a single module-level instance pointer.
    volatile int rotationAccumulator = 0;

    static void IRAM_ATTR onEncoderChangeISR();
    void handleEncoderChange(); // actual state-table logic, called from ISR

    static const int8_t QUADRATURE_TABLE[16];
    uint8_t quadratureState = 0;

    static EncoderModule* instance; // for the static ISR to reach handleEncoderChange()
};
