#include "EncoderModule.h"
#include "config.h"

EncoderModule* EncoderModule::instance = nullptr;

// Standard quadrature state table: index = (prevState<<2 | currState),
// where each state is 2 bits (CLK<<1 | DT). Valid single-step transitions
// give +1 or -1; invalid/bounce transitions (impossible jumps) give 0, so
// contact bounce mostly just produces zeros instead of phantom steps.
const int8_t EncoderModule::QUADRATURE_TABLE[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

void EncoderModule::begin() {
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP); // KY-040 SW is active-low

  instance = this;

  uint8_t clk = digitalRead(ENCODER_CLK_PIN);
  uint8_t dt  = digitalRead(ENCODER_DT_PIN);
  quadratureState = (clk << 1) | dt;

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), onEncoderChangeISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN), onEncoderChangeISR, CHANGE);

  buttonRawLast = (digitalRead(ENCODER_SW_PIN) == LOW);
  buttonStableState = buttonRawLast;
  buttonLastChangeMs = millis();
}

void IRAM_ATTR EncoderModule::onEncoderChangeISR() {
  if (instance) instance->handleEncoderChange();
}

void EncoderModule::handleEncoderChange() {
  uint8_t clk = digitalRead(ENCODER_CLK_PIN);
  uint8_t dt  = digitalRead(ENCODER_DT_PIN);
  uint8_t newState = (clk << 1) | dt;

  uint8_t index = (quadratureState << 2) | newState;
  rotationAccumulator += QUADRATURE_TABLE[index & 0x0F];

  quadratureState = newState;
}

int EncoderModule::takeRotation() {
  // 4 raw quadrature steps = 1 physical detent click on a standard KY-040.
  // Divide down so takeRotation() returns "detent clicks," which is the
  // unit every menu/UI actually wants to think in.
  noInterrupts();
  int raw = rotationAccumulator;
  rotationAccumulator = 0;
  interrupts();

  static int leftoverRaw = 0; // carries fractional steps between calls
  leftoverRaw += raw;
  int clicks = leftoverRaw / 4;
  leftoverRaw -= clicks * 4;
  return clicks;
}

void EncoderModule::update() {
  bool rawPressed = (digitalRead(ENCODER_SW_PIN) == LOW);
  unsigned long now = millis();

  if (rawPressed != buttonRawLast) {
    buttonRawLast = rawPressed;
    buttonLastChangeMs = now;
  }

  if ((now - buttonLastChangeMs) >= DEBOUNCE_MS && rawPressed != buttonStableState) {
    buttonStableState = rawPressed;

    if (buttonStableState) {
      // Just pressed
      buttonPressedAtMs = now;
      longPressFired = false;
    } else {
      // Just released - if we didn't already fire a long press while held,
      // this release counts as a short click.
      if (!longPressFired) {
        clickPending = true;
      }
    }
  }

  // Long press fires while still held, not on release, so the UI can react
  // immediately (e.g. show "release to confirm") rather than waiting.
  if (buttonStableState && !longPressFired && (now - buttonPressedAtMs) >= LONG_PRESS_MS) {
    longPressFired = true;
    longPressPending = true;
  }
}

bool EncoderModule::wasClicked() {
  if (clickPending) {
    clickPending = false;
    return true;
  }
  return false;
}

bool EncoderModule::wasLongPressed() {
  if (longPressPending) {
    longPressPending = false;
    return true;
  }
  return false;
}

bool EncoderModule::isButtonDown() const {
  return buttonStableState;
}
