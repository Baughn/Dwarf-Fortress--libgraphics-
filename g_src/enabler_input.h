#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <SDL/SDL.h>
#include <string>
#include <set>
#include <list>

#include "ViewBase.h"
#include "keybindings.h"
#include "curses.h"

typedef Uint32 Time;

enum Repeat {
  REPEAT_NOT,  // Don't repeat at all. Furthermore, cancel other repeats.
  REPEAT_SLOW, // Repeat normally.
  REPEAT_FAST  // Repeat instantly, without waiting for the first-repeat interval.
};

enum MatchType { type_unicode, type_key, type_button };

std::string translate_mod(Uint8 mod);
int decode_utf8(const std::string &s);
int decode_utf8_predict_length(char byte);
std::string encode_utf8(int unicode);

#define DFMOD_SHIFT 1
#define DFMOD_CTRL 2
#define DFMOD_ALT 4

struct EventMatch {
  MatchType type;
  Uint8 mod;      // not defined for type=unicode. 1: shift, 2: ctrl, 4:alt
  Uint8 scancode; // not defined for type=button
  union {
    Uint16 unicode;
    SDLKey key;
    Uint8 button;
  };
  
  bool operator== (const EventMatch &other) const {
    if (mod != other.mod) return false;
    if (type != other.type) return false;
    switch (type) {
    case type_unicode: return unicode == other.unicode;
    case type_key: return key == other.key;
    case type_button: return button == other.button;
    }
  }
  
  bool operator< (const EventMatch &other) const {
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

typedef std::list<std::set<InterfaceKey> > macro;

class enabler_inputst {
  std::set<InterfaceKey> key_translation(EventMatch &match);
  Repeat key_repeat(InterfaceKey);
  void load_macro_from_file(const std::string &file);
  void save_macro_to_file(const std::string &file, const std::string &name, const macro &);
  
 public:
  // In practice.. do not use this one.
  void add_input(SDL_Event &e, Time now);
  // Use this one. It's much nicer.
  void add_input_refined(KeyEvent &e, Time now, int serial);
  // Made specifically for curses. <0 = unicode, >0 = ncurses symbols.
#ifdef CURSES
  void add_input_ncurses(int key, Time now, bool esc);
#endif
  std::set<InterfaceKey> get_input(Time now);
  void clear_input();

  void load_keybindings(const std::string &file);
  void save_keybindings(const std::string &file);
  void save_keybindings();
  std::string GetKeyDisplay(int binding);
  std::string GetBindingDisplay(int binding);

  // Macros
  void record_input(); // Records input until such a time as you say stop
  void record_stop(); // Stops recording, saving it as the active macro
  bool is_recording();
  void play_macro(); // Runs the active macro, if any
  std::list<string> list_macros();
  void load_macro(string name); // Loads some macro as the active one
  void save_macro(string name); // Saves the active macro under some name
  void delete_macro(string name);

  // Updating the key-bindings
  void register_key(bool unicode, InterfaceKey key); // Adds the next event to be generated to the keymap for this interfacekey. If unicode is true, ignores SDL syms; otherwise, ignores unicode. Mouse buttons work either way.
  bool is_registering();
  std::list<EventMatch> list_keys(InterfaceKey key); // Returns a list of events matching this interfacekey
  void remove_key(InterfaceKey key, EventMatch ev); // Removes a particular matcher from the keymap.
};


#endif
