#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <SDL/SDL.h>
#include <string>
#include <set>
#include <list>

#include "ViewBase.h"
#include "keybindings.h"

typedef Uint32 Time;

enum Repeat {
  REPEAT_NOT,  // Don't repeat at all. Furthermore, cancel other repeats.
  REPEAT_SLOW, // Repeat normally.
  REPEAT_FAST  // Repeat instantly, without waiting for the first-repeat interval.
};

struct Event {
  Repeat r; // If REPEAT_SLOW, will be set to REPEAT_FAST next cycle.
  InterfaceKey k;
};

enum MatchType { type_unicode, type_key, type_button };


struct EventMatch {
  MatchType type;
  Uint8 mod;      // not defined for type=unicode. 1: shift, 2: ctrl, 4:alt
  Uint8 scancode; // not defined for type=button
  union {
    Uint16 unicode;
    SDLKey key;
    Uint8 button;
  };
  
  bool operator== (EventMatch other) const {
    if (mod != other.mod) return false;
    if (type != other.type) return false;
    switch (type) {
    case type_unicode: return unicode == other.unicode;
    case type_key: return key == other.key;
    case type_button: return button == other.button;
    }
  }
  
  bool operator< (EventMatch other) const {
    if (mod != other.mod) return mod < other.mod;
    if (type != other.type) return type < other.type;
    switch (type) {
    case type_unicode: return unicode < other.unicode;
    case type_key: return key < other.key;
    case type_button: return button < other.button;
    }
  }
};

struct KeyEvent {
  bool release;
  EventMatch match;
};

class enabler_inputst {
  std::set<InterfaceKey> key_translation(KeyEvent &e);
  Repeat key_repeat(InterfaceKey);
  
 public:
  // In practice.. do not use this one.
  void add_input(SDL_Event &e, Uint32 now);
  // Use this one. It's much nicer.
  void add_input_refined(KeyEvent &e, Uint32 now);
  std::list<InterfaceKey> get_input();
  void clear_input();

  void load_keybindings(const std::string &file);
  void save_keybindings(const std::string &file);
  std::string GetKeyDisplay(int binding);
  std::string GetBindingDisplay(int binding);
};


#endif
