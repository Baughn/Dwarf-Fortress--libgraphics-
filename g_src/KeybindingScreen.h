#ifndef KEYBINDINGSCREEN_H
#define KEYBINDINGSCREEN_H

#include "interface.h"
#include "ViewBase.h"
#include "enabler.h"

#include <set>
#include <string>


class KeybindingScreen : public viewscreenst {
  enum { mode_main, mode_keyL, mode_keyR, mode_macro } mode;
  enum keyR_type { sel_unicode, sel_sdl, sel_event };
  enum main_selector { sel_macros, sel_just_exit, sel_save_exit, sel_first_group };
  bool was_registering;

  struct keyR_selector {
    keyR_type sel;
    EventMatch event; // Uninitialized if sel != sel_event
  };
  
  widgets::menu<int> main; // Representing main_selector
  widgets::menu<InterfaceKey> keyL;
  widgets::menu<keyR_selector> keyR;
  widgets::menu<std::string> macro;
  
  void render_main();
  void render_macro();
  void render_key();

  void reset_keyR();
  
  void enter_key(int group);
  void enter_macros();
  
public:
  KeybindingScreen(); 
  virtual void feed(std::set<InterfaceKey> &events);
  virtual void render();
  virtual void help();
  virtual void logic();
};

class MacroScreenLoad : public viewscreenst {
  widgets::menu<string> menu;
  int width, height;
  
 public:
  MacroScreenLoad();
  virtual void render();
  virtual void feed(std::set<InterfaceKey> &events);
};

class MacroScreenSave : public viewscreenst {
  widgets::textbox id;
public:
  MacroScreenSave();
  virtual void render();
  virtual void feed(std::set<InterfaceKey> &events);
};


#endif
