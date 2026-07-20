#pragma once
#include <Arduino.h>

// Menu content - just data, no logic. Matches timer_configs.h's philosophy:
// keep what's displayed separate from how navigation/rendering works, so
// adding/renaming/reordering features never touches MenuModule itself.

static const char* const GROUP_NAMES[] = { "Timers", "Games", "Settings" };
static const int GROUP_COUNT = 3;

static const char* const TIMER_ITEMS[] = {
  "Pomodoro", "Timer", "Exercise Reminder", "Stopwatch"
};

static const char* const GAME_ITEMS[] = {
  "Rock Paper Scissors", "Coin Toss", "Dice Roll",
  "Random Motivation", "Random Joke"
};

static const char* const SETTINGS_ITEMS[] = {
  "Brightness", "Volume"
  // more to come later
};

static const char* const* const GROUP_ITEMS[] = { TIMER_ITEMS, GAME_ITEMS, SETTINGS_ITEMS };
static const int GROUP_ITEM_COUNTS[] = {
  sizeof(TIMER_ITEMS) / sizeof(TIMER_ITEMS[0]),
  sizeof(GAME_ITEMS) / sizeof(GAME_ITEMS[0]),
  sizeof(SETTINGS_ITEMS) / sizeof(SETTINGS_ITEMS[0])
};
