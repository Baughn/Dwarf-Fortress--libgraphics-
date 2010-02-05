#include "graphics.h"
#include "KeybindingScreen.h"
#include "init.h"
#include "keybindings.h"

#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <ctype.h>

using namespace std;

struct BindingGroup {
  string name;
  InterfaceKey start, end;
};

const BindingGroup groups[] = {
  {"General"    , INTERFACEKEY_NONE,        WORLDKEY_START-1},
  {"World"      , WORLDKEY_START,           ADVENTURERKEY_START-1},
  {"Adventurer" , ADVENTURERKEY_START,      EMBARKKEY_START-1},
  {"Dwarf mode" , DWARFMAINKEY_START,       MILITIAKEY_START-1},
  {"Embark"     , EMBARKKEY_START,          BUILDINGKEY_START-1},
  {"Building"   , BUILDINGKEY_START,        WORKSHOPKEY_START-1},
  {"Workshop"   , WORKSHOPKEY_START,        PILEZONEKEY_START-1},
  {"Pilezone"   , PILEZONEKEY_START,        STOCKORDERKEY_START-1},
  {"Stockorder" , STOCKORDERKEY_START,      DWARFMAINKEY_START-1},
  {"Militia"    , MILITIAKEY_START,         INTERFACEKEY_SQUADINFO_ZOOM},
  {"Text entry" , INTERFACEKEY_STRING_A000, INTERFACEKEY_STRING_A255}
};

KeybindingScreen::KeybindingScreen() {
  gview.addscreen(this, INTERFACE_PUSH_AT_BACK, NULL); // HACK
  mode = mode_main;
  was_registering = false;

  main.add("Macros", sel_macros);
  for (int i = 0; i < ARRSZ(groups); i++)
    main.set(i+2, groups[i].name, sel_first_group + i);
  main.set(ARRSZ(groups)+3, "Save and exit", sel_save_exit);
  main.add("Exit without saving", sel_just_exit);
}

void KeybindingScreen::feed(set<InterfaceKey> &input) {
  if (input.count(INTERFACEKEY_STANDARDSCROLL_PAGEUP) ||
      input.count(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN) ||
      input.count(INTERFACEKEY_STANDARDSCROLL_UP) ||
      input.count(INTERFACEKEY_STANDARDSCROLL_DOWN)) {
    switch (mode) {
    case mode_main: main.feed(input); break;
    case mode_keyL: keyL.feed(input); reset_keyR(); break;
    case mode_keyR: keyR.feed(input); break;
    case mode_macro: macro.feed(input); break;
    }
  }
  if (mode == mode_keyL && input.count(INTERFACEKEY_STANDARDSCROLL_RIGHT))
    mode = mode_keyR;
  if (mode == mode_keyR && input.count(INTERFACEKEY_STANDARDSCROLL_LEFT))
    mode = mode_keyL;
  if (input.count(INTERFACEKEY_STRING_A000)) { // Backspace: Delete something.
    switch (mode) {
    case mode_macro:
      if (macro.get_selection() != "") {
        enabler.delete_macro(macro.get_selection());
        macro.del_selection();
      }
      break;
    case mode_keyR:
      keyR_selector sel = keyR.get_selection();
      if (sel.sel == sel_event) {
        enabler.remove_key(keyL.get_selection(), sel.event);
        reset_keyR();
      }
      break;
    }
  }
  if (input.count(INTERFACEKEY_SELECT)) {
    switch (mode) {
    case mode_main:
      if (main.get_selection() == sel_macros) { // Macros
        enter_macros();
      } else if (main.get_selection() == sel_save_exit) { // Save and exit
        enabler.save_keybindings();
        breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
        return;
      } else if (main.get_selection() == sel_just_exit) { // Just exit
        breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
        return;
      } else { // Some key-binding group
        enter_key(main.get_selection() - sel_first_group);
      }
      break;
    case mode_keyR:
      switch (keyR.get_selection().sel) {
      case sel_unicode:
        enabler.register_key(true, keyL.get_selection());
        was_registering = true;
        break;
      case sel_sdl:
        enabler.register_key(false, keyL.get_selection());
        was_registering = true;
        break;
      }
      break;
    }
  }
  if (input.count(INTERFACEKEY_LEAVESCREEN)) {
    mode = mode_main;
  }
}

void KeybindingScreen::logic() {
  if (was_registering && !enabler.is_registering()) {
    was_registering = false;
    reset_keyR();
  }
  enabler.flag|=ENABLERFLAG_RENDER;
  gps.renewscreen();
}

void KeybindingScreen::enter_macros() {
  mode = mode_macro;
  macro.clear();
  list<string> macros = enabler.list_macros();
  for (list<string>::iterator it = macros.begin(); it != macros.end(); ++it)
    macro.add(*it, *it);
  if (!macros.size())
    macro.add("No macros!", "");
}

void KeybindingScreen::enter_key(int group) {
  mode = mode_keyL;
  keyL.clear();
  for (InterfaceKey i = groups[group].start; i <= groups[group].end; i++) {
    if (i != INTERFACEKEY_NONE)
      keyL.add(enabler.GetBindingDisplay(i), i);
  }
  reset_keyR();
}

void KeybindingScreen::reset_keyR() {
  keyR.clear();
  struct keyR_selector sel;
  sel.sel = sel_unicode;
  keyR.add("Add binding - unicode", sel);
  sel.sel = sel_sdl;
  keyR.add("Add binding - sdl", sel);
  keyR.add("Add binding - mouse button", sel);
  InterfaceKey key = keyL.get_selection();
  list<EventMatch> matchers = enabler.list_keys(key);
  int i = 4;
  for (list<EventMatch>::iterator it = matchers.begin(); it != matchers.end(); ++it, ++i) {
    ostringstream desc;
    switch (it->type) {
    case type_unicode:
      desc << "Unicode: ";
      if (it->unicode < 256 && isgraph(it->unicode)) // Is it printable?
        desc << (char)it->unicode;
      else
        desc << "U+" << hex << uppercase << it->unicode;
      break;
    case type_key:
      desc << "SDL: " << translate_mod(it->mod) << sdlNames.left[it->key];
      break;
    case type_button:
      desc << "Button: " << (int)it->button;
      break;
    }
    sel.sel = sel_event;
    sel.event = *it;
    keyR.set(i, desc.str(), sel);
    cout << desc.str() << endl;
  }
}

void KeybindingScreen::render_macro() {
  drawborder("Macros");
  macro.render(6, init.display.grid_x-1, 3, init.display.grid_y-1);
}

void KeybindingScreen::render_key() {
  if (enabler.is_registering()) {
    gps.changecolor(4,0,1);
    drawborder("Keybinding - currently registering new key");
  } else
    drawborder("Keybinding");
  keyL.render(6, init.display.grid_x/2 - 1, 3, init.display.grid_y-1);
  if (mode == mode_keyL)
    keyR.bleach(true);
  else
    keyR.bleach(false);
  keyR.render(init.display.grid_x/2 + 1, init.display.grid_x-1, 3, init.display.grid_y-1);
}

// Render the main menu
void KeybindingScreen::render_main() {
  drawborder("Key binding & macro center");
  main.render(6, init.display.grid_x - 3, 3, init.display.grid_y - 4);
}

void KeybindingScreen::render() {
  switch(mode) {
  case mode_main: render_main(); break;
  case mode_keyL: case mode_keyR: render_key(); break;
  case mode_macro: render_macro(); break;
  }
}

void KeybindingScreen::help() {
}


MacroScreenLoad::MacroScreenLoad() {
  list<string> macros = enabler.list_macros();
  width = 10;
  if (!macros.size()) {
    menu.add("No macros!", "");
    height = 1;
  } else
    height = macros.size();

  for (list<string>::iterator it = macros.begin(); it != macros.end(); ++it) {
    if (it->length() > width) width = it->length();
    menu.add(*it, *it);
  }
  render();
  gps.renewscreen();
}

void MacroScreenLoad::feed(set<InterfaceKey> &input) {
  if (input.count(INTERFACEKEY_SELECT)) {
    string id = menu.get_selection();
    if (id != "") enabler.load_macro(id);
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  } else if (input.count(INTERFACEKEY_LEAVESCREEN)) {
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  } else {
    menu.feed(input);
  }
}

void MacroScreenLoad::logic() {
  enabler.flag|=ENABLERFLAG_RENDER;
}

void MacroScreenLoad::render() {
  if (parent) parent->render();
  const int x1 = MAX(init.display.grid_x/2 - ((width + 2) / 2), 0);
  const int x2 = MIN(x1+width+1, init.display.grid_x-1);
  const int y1 = MAX(init.display.grid_y/2 - ((height + 2) / 2), 0);
  const int y2 = MIN(y1 + height + 1, init.display.grid_y-1);
  gps.changecolor(0,3,1);
  gps.draw_border(x1, x2, y1, y2);
  menu.render(x1+1, x2-1, y1+1, y2-1);
}

MacroScreenSave::MacroScreenSave() {
}

void MacroScreenSave::logic() {
  enabler.flag|=ENABLERFLAG_RENDER;
}

void MacroScreenSave::feed(set<InterfaceKey> &input) {
  id.feed(input);
  if (input.count(INTERFACEKEY_SELECT)) {
    string n = id.get_text();
    if (n.length())
      enabler.save_macro(n);
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  }
}

void MacroScreenSave::render() {
  if (parent) parent->render();
  const int x1 = 3,
    x2 = init.display.grid_x-4,
    y1 = init.display.grid_y/2-1,
    y2 = init.display.grid_y/2+1;
  gps.changecolor(0,3,1);
  gps.draw_border(x1, x2, y1, y2);
  id.render(x1+1,x2-1,y1+1,y2-1);
  gps.renewscreen();
}

