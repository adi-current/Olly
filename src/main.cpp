#include <Arduino.h>
#include "DisplayModule.h"
#include "EncoderModule.h"
#include "TouchModule.h"
#include "GamesModule.h"
#include "FunModule.h"

DisplayModule display;
EncoderModule encoder;
TouchModule touch;
GamesModule games;
FunModule fun;

// Which of Games/Fun's 5 items lives at each menu index, per menu_data.h's
// GAME_ITEMS order: {"Rock Paper Scissors","Coin Toss","Dice Roll",
// "Random Motivation","Random Joke"}. Indices 0-2 are games, 3-4 are fun.
enum class AppMode { HOME_MENU, GAME_ACTIVE, FUN_ACTIVE };
AppMode appMode = AppMode::HOME_MENU;

void setup() {
  display.begin();
  encoder.begin();
  touch.begin();
  games.begin(display.getRawDisplay());
  fun.begin(display.getRawDisplay());
}

void loop() {
  encoder.update();
  touch.update();

  int rotation = encoder.takeRotation();
  bool confirm = encoder.wasClicked();     // encoder click = forward/commit, everywhere
  bool actionShort = touch.wasPressed();   // touch short press = feature-specific action
  bool backLong = touch.wasLongPressed();  // touch long press = back one level, everywhere

  if (appMode == AppMode::HOME_MENU) {
    bool featureSelected = display.update(rotation, confirm, backLong);

    if (featureSelected) {
      int group = display.selectedGroup(); // 0 = Timers, 1 = Games, 2 = Settings
      int item = display.selectedItem();

      if (group == 1) { // Games group (also holds Motivation/Joke, per menu_data.h)
        if (item <= 2) {
          games.start((GameType)item);
          appMode = AppMode::GAME_ACTIVE;
        } else {
          fun.start(item == 3 ? FunType::MOTIVATION : FunType::JOKE);
          appMode = AppMode::FUN_ACTIVE;
        }
      }
      // group 0 (Timers) / group 2 (Settings): not built yet - selecting
      // one of those items currently does nothing, stays on the Feature list.
    }

  } else if (appMode == AppMode::GAME_ACTIVE) {
    bool done = games.update(rotation, actionShort, backLong);

    U8G2* raw = display.getRawDisplay();
    raw->clearBuffer();
    games.render();
    raw->sendBuffer();

    if (done) appMode = AppMode::HOME_MENU;

  } else if (appMode == AppMode::FUN_ACTIVE) {
    bool done = fun.update(rotation, actionShort, backLong);

    U8G2* raw = display.getRawDisplay();
    raw->clearBuffer();
    fun.render();
    raw->sendBuffer();

    if (done) appMode = AppMode::HOME_MENU;
  }
}
