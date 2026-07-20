#include "MenuModule.h"
#include "menu_data.h"

void MenuModule::begin(U8G2* d) {
  display = d;
}

bool MenuModule::isActive() const {
  return state != State::HOME;
}

void MenuModule::enterMenu() {
  if (state == State::HOME) {
    state = State::GROUP;
    groupIndex = 0;
  }
}

void MenuModule::clampSelection(int& index, int count) {
  if (count <= 0) { index = 0; return; }
  if (index < 0) index = count - 1;      // wrap around, scrolling past the top
  if (index >= count) index = 0;         // wrap around, scrolling past the bottom
}

bool MenuModule::update(int rotationDelta, bool confirmPressed, bool backPressed) {
  switch (state) {

    case State::HOME:
      if (confirmPressed) {
        enterMenu();
      }
      return false;

    case State::GROUP:
      if (rotationDelta != 0) {
        groupIndex += rotationDelta;
        clampSelection(groupIndex, GROUP_COUNT);
      }
      if (backPressed) {
        state = State::HOME;
        return false;
      }
      if (confirmPressed) {
        state = State::FEATURE;
        itemIndex = 0;
      }
      return false;

    case State::FEATURE: {
      int count = GROUP_ITEM_COUNTS[groupIndex];
      if (rotationDelta != 0) {
        itemIndex += rotationDelta;
        clampSelection(itemIndex, count);
      }
      if (backPressed) {
        state = State::GROUP;
        return false;
      }
      if (confirmPressed) {
        return true; // caller reads selectedGroup()/selectedItem() this frame
      }
      return false;
    }
  }
  return false;
}

int MenuModule::selectedGroup() const { return groupIndex; }
int MenuModule::selectedItem() const { return itemIndex; }

void MenuModule::render() {
  if (!display) return;

  if (state == State::GROUP) {
    renderList("MENU", GROUP_NAMES, GROUP_COUNT, groupIndex);
  } else if (state == State::FEATURE) {
    renderList(GROUP_NAMES[groupIndex], GROUP_ITEMS[groupIndex],
               GROUP_ITEM_COUNTS[groupIndex], itemIndex);
  }
}

void MenuModule::renderList(const char* title, const char* const* items, int count, int selected) {
  // Header: current group/menu name, underlined
  display->setFont(u8g2_font_6x10_tr);
  display->drawStr(2, 9, title);
  display->drawHLine(0, 11, 128);

  // Scrolling window - show up to 4 items at a time, keeping the
  // highlighted one in view even when the list is longer than that
  // (e.g. Games has 5 items).
  const int VISIBLE = 4;
  int start = selected - VISIBLE / 2;
  if (start > count - VISIBLE) start = count - VISIBLE;
  if (start < 0) start = 0;

  int y = 22;
  for (int i = start; i < start + VISIBLE && i < count; i++) {
    if (i == selected) {
      display->drawBox(0, y - 9, 128, 11);
      display->setDrawColor(0); // inverted text on the highlighted row
      display->drawStr(4, y, items[i]);
      display->setDrawColor(1);
    } else {
      display->drawStr(4, y, items[i]);
    }
    y += 13;
  }
}
