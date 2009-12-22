#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <SDL/SDL.h>
#include <string>
#include <set>
#include <list>

#include "ViewBase.h"
#include "keybindings.h"

// When a key is mapped to multiple actions with different repeat
// levels, the lowest level is picked.
enum Repeat {
  REPEAT_NOT,  // Don't repeat at all. Furthermore, cancel other repeats.
  REPEAT_SLOW, // Repeat normally.
  REPEAT_FAST  // Repeat instantly, without waiting for the first-repeat interval.
};

class enabler_inputst {
  std::set<InterfaceKey> key_translation(SDL_Event &e);
  Repeat key_repeat(InterfaceKey);

 public:
  void add_input(SDL_Event &e, Uint32 now);
  std::list<InterfaceKey> get_input();
  void clear_input();

  void load_keybindings(const std::string &file){}
  void save_keybindings(const std::string &file){}
  std::string GetKeyDisplay(int binding){return "I DO NOT EXIST";}
  std::string GetBindingDisplay(int binding){return "I DO NOT EXIST EITHER";}
};


#endif
