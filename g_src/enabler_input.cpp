#include <SDL/SDL.h>
#include <map>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
using namespace std;

#include "enabler_input.h"
#include "init.h"
extern initst init;
#include "platform.h"

#include <boost/regex.hpp>

// These change dynamically in the normal process of DF
static multimap<Time,Event> timeline; // A timeline of pending key events (for next get_input)
static set<EventMatch> pressed_keys; // Keys we consider "pressed"
  
// These do not change as part of the normal dynamics of DF, only at startup/when editing.
static multimap<EventMatch,InterfaceKey> keymap;
static map<InterfaceKey,Repeat> repeatmap;
static map<InterfaceKey,string> keydisplay; // Used only for display, not for meaning

static void update_keydisplay(InterfaceKey binding, string display) {
  // As a heuristic, we use whichever display string is shortest
  map<InterfaceKey,string>::iterator it = keydisplay.find(binding);
  if (it == keydisplay.end() || it->second.length() > display.length()) {
    keydisplay[binding] = display;
  }
}

static void assertgood(ifstream &s) {
  if (s.eof())
    MessageBox(NULL, "EOF while parsing keyboard bindings", 0, 0);
  else if (!s.good())
    MessageBox(NULL, "I/O error while parsing keyboard bindings", 0, 0);
  else
    return;
  abort();
}

// Decodes an UTF-8 encoded string into a /single/ UTF-8 character,
// discarding any overflow. Returns 0 on parse error.
static int decode_utf8(const string &s) {
  int unicode = 0, length, i;
  if (s.length() == 0) return 0;
  else if ((s[0] & 0x80) == 0) { // 1-byte, ascii series
    unicode = s[0];
    length = 1;
  }
  else if ((s[0] & 0xe0) == 0xc0) { // 2-byte utf-8
    unicode = s[0] & 0x1f;
    length = 2;
  }
  else if ((s[0] & 0xf0) == 0xe0) { // 3-byte utf-8
    unicode = s[0] & 0x0f;
    length = 3;
  }
  else if ((s[0] & 0xf8) == 0xf0) { // 4-byte utf-8
    unicode = s[0] & 0x07;
    length = 4;
  }
  else return 0; // Broken text

  // Concatenate the follow-up bytes
  if (s.length() < length) return 0;
  for (i = 1; i < length; i++) {
    if ((s[i] & 0xc0) != 0x80) return 0;
    unicode = (unicode << 6) | (s[i] & 0x3f);
  }
  return unicode;
}

// Encode an arbitrary unicode value as a string. Returns an empty
// string if the value is out of range.
static string encode_utf8(int unicode) {
  string s;
  int i;
  if (unicode < 0 || unicode > 0x10ffff) return ""; // Out of range for utf-8
  else if (unicode <= 0x007f) { // 1-byte utf-8
    s.resize(1, 0);
  }
  else if (unicode <= 0x07ff) { // 2-byte utf-8
    s.resize(2, 0);
    s[0] = 0xc0;
  }
  else if (unicode <= 0xffff) { // 3-byte utf-8
    s.resize(3, 0);
    s[0] = 0xe0;
  }
  else { // 4-byte utf-8
    s.resize(4, 0);
    s[0] = 0xf0;
  }

  // Build up the string, right to left
  for (i = s.length()-1; i > 0; i--) {
    s[i] = 0x80 | (unicode & 0x3f);
    unicode >>= 6;
  }
  // Finally, what's left goes in the low bits of s[0]
  s[0] |= unicode;
  return s;
}

static string display(Uint8 mod, string rest) {
  string disp;
  if (mod & 1) disp += "Shift+";
  if (mod & 2) disp += "Ctrl+";
  if (mod & 4) disp += "Alt+";
  return disp + rest;
}

void enabler_inputst::load_keybindings(const string &file) {
  cout << "Loading bindings from " << file << endl;
  ifstream s(file.c_str());
  if (!s.good()) {
    MessageBox(NULL, (file + " not found, or I/O error encountered").c_str(), 0, 0);
    abort();
  }

  list<string> lines;
  while (s.good()) {
    string line;
    getline(s, line);
    lines.push_back(line);
  }
  
  static const boost::regex bind("\\[BIND:([^:]+):([^\\]]+)\\]");
  //static const boost::regex macro("\\[MACRO:([^:]+):([^\\]]+)\\]");

  static const boost::regex sym("\\[SYM:([^:]+):([^\\]]+)\\]");
  static const boost::regex key("\\[KEY:(.+)\\]");
  static const boost::regex button("\\[BUTTON:([^:]+):([^\\]]+)\\]");

  boost::smatch match;
  list<string>::iterator line = lines.begin();

  while (line != lines.end()) {
    if (boost::regex_search(*line, match, bind)) {
      map<string,InterfaceKeyType>::iterator it = bindingNames.right.find(match[1]);
      if (it != bindingNames.right.end()) {
        InterfaceKeyType binding = it->second;
        // Parse repeat data
        if (match[2] == "REPEAT_FAST")
          repeatmap[(InterfaceKey)binding] = REPEAT_FAST;
        else if (match[2] == "REPEAT_SLOW")
          repeatmap[(InterfaceKey)binding] = REPEAT_SLOW;
        else if (match[2] == "REPEAT_NOT")
          repeatmap[(InterfaceKey)binding] = REPEAT_NOT;
        else {
          repeatmap[(InterfaceKey)binding] = REPEAT_NOT;
          cout << "Broken repeat request: " << match[2] << endl;
        }
        ++line;
        // Add symbols/keys/buttons
        while (line != lines.end()) {
          EventMatch matcher;
          // SDL Keys
          if (boost::regex_search(*line, match, sym)) {
            map<string,SDLKey>::iterator it = sdlNames.right.find(match[2]);
            if (it != sdlNames.right.end()) {
              matcher.mod  = atoi(string(match[1]).c_str());
              matcher.type = type_key;
              matcher.key  = it->second;
              keymap.insert(pair<EventMatch,InterfaceKey>(matcher, (InterfaceKey)binding));
              update_keydisplay(binding, display(matcher.mod, match[2]));
            } else {
              cout << "Unknown SDLKey: " << match[1] << endl;
            }
            ++line;           
          } // Unicode
          else if (boost::regex_search(*line, match, key)) {
            matcher.type = type_unicode;
            matcher.unicode = decode_utf8(match[1]);
            matcher.mod = KMOD_NONE;
            if (matcher.unicode) {
              keymap.insert(pair<EventMatch,InterfaceKey>(matcher, (InterfaceKey)binding));
              if (matcher.unicode < 256) {
                // This unicode key is part of the latin-1 mapped portion of unicode, so we can
                // actually display it. Nice.
                char c[2] = {matcher.unicode, 0};
                update_keydisplay(binding, display(matcher.mod, string(c)));
              }
            } else {
              cout << "Broken unicode: " << *line << endl;
            }
            ++line;
          } // Mouse buttons
          else if (boost::regex_search(*line, match, button)) {
            matcher.type = type_button;
            string str = match[2];
            matcher.button = atoi(str.c_str());
            if (matcher.button) {
              matcher.mod  = atoi(string(match[1]).c_str());
              keymap.insert(pair<EventMatch,InterfaceKey>(matcher, (InterfaceKey)binding));
              update_keydisplay(binding, "Button " + match[2]);
            } else {
              cout << "Broken button (should be [BUTTON:#:#]): " << *line << endl;
            }
            ++line;
          } else {
            break;
          }
        }
      } else {
        cout << "Unknown binding: " << match[1] << endl;
      }
    } else {
      // Retry with next line
      ++line;
    }
  }
}

void enabler_inputst::save_keybindings(const string &file) {
  cout << "Saving bindings to " << file << endl;
}

void enabler_inputst::add_input(SDL_Event &e, Uint32 now) {
  // Before we can use this input, there are some issues to deal with:
  // - SDL provides unicode translations only for key-press events, not
  //   releases. We need to keep track of pressed keys, and generate
  //   unicode release events whenever any modifiers are hit, or if
  //   that raw keycode is released.
  // - Generally speaking, when modifiers are hit/released, we discard those
  //   events and generate press/release events for all pressed non-modifiers.
  // - It's possible for multiple events to be generated on the same tick.
  //   However, in reality they're distinct keypresses, and not simultaneous at
  //   all. We fix this up by making sure 'now' is always later than the last time.

  static Uint32 last_now = 0;
  if (now <= last_now) {
    last_now = now++;
  } else {
    last_now = now;
  }

  list<KeyEvent> synthetics;
  set<EventMatch>::iterator pkit;

  // Convert modifier state changes
  if ((e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) &&
      (e.key.keysym.sym == SDLK_RSHIFT ||
       e.key.keysym.sym == SDLK_LSHIFT ||
       e.key.keysym.sym == SDLK_RCTRL  ||
       e.key.keysym.sym == SDLK_LCTRL  ||
       e.key.keysym.sym == SDLK_RALT   ||
       e.key.keysym.sym == SDLK_LALT   )) {
    for (pkit = pressed_keys.begin(); pkit != pressed_keys.end(); ++pkit) {
      // Release currently pressed keys
      KeyEvent synth;
      synth.release = true;
      synth.match = *pkit;
      synthetics.push_back(synth);
      // Re-press them, with new modifiers, if they aren't unicode. We can't re-translate unicode.
      if (synth.match.type != type_unicode) {
        synth.release = false;
        synth.match.mod = SDL_GetModState();
        synthetics.push_back(synth);
      }
    }
  } else {
    // It's not a modifier. If this is a key release, then we still need
    // to find and release pressed unicode keys with this scancode
    if (e.type == SDL_KEYUP) {
      for (pkit = pressed_keys.begin(); pkit != pressed_keys.end(); ++pkit) {
        if (pkit->type == type_unicode && pkit->scancode == e.key.keysym.scancode) {
          KeyEvent synth;
          synth.release = true;
          synth.match = *pkit;
          synthetics.push_back(synth);
        }
      }
    }
    // Since it's not a modifier, we also pass on symbolic/button (always) and unicode (if defined) events
    KeyEvent real;
    real.release = (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP) ? true : false;
    real.match.mod = SDL_GetModState();
    if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN) {
      real.match.type = type_button;
      real.match.scancode = 0;
      real.match.button = e.button.button;
      synthetics.push_back(real);
    }
    if (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) {
      real.match.type = type_key;
      real.match.scancode = e.key.keysym.scancode;
      real.match.key = e.key.keysym.sym;
      synthetics.push_back(real);
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.unicode) {
      real.match.mod = KMOD_NONE;
      real.match.type = type_unicode;
      real.match.scancode = e.key.keysym.scancode;
      real.match.unicode = e.key.keysym.unicode;
      synthetics.push_back(real);
    }
    if (e.type == SDL_QUIT) {
      // This one, we insert directly into the timeline.
      Event e = {REPEAT_NOT, (InterfaceKey)INTERFACEKEY_OPTIONS};
      timeline.insert(pair<Time,Event>(now, e));
    }
  }

  list<KeyEvent>::iterator lit;
  for (lit = synthetics.begin(); lit != synthetics.end(); ++lit) {
    // Add or remove the key from pressed_keys, keeping that up to date
    if (lit->release) pressed_keys.erase(lit->match);
    else pressed_keys.insert(lit->match);
    // And pass the event on deeper.
    add_input_refined(*lit, now);
  }
}

void enabler_inputst::add_input_refined(KeyEvent &e, Uint32 now) {
  // If this is a key-press event, we add it to the timeline. If it's
  // a release, we remove any pending repeats, but not those that
  // haven't repeated yet (which are on their first cycle); those we
  // just set to non-repeating.

  set<InterfaceKey> keys = key_translation(e);
  if (e.release) {
    multimap<Time,Event>::iterator it = timeline.begin();
    while (it != timeline.end()) {
      multimap<Time,Event>::iterator el = it++;
      if (keys.count(el->second.k)) {
        if (el->second.repeats) {
          timeline.erase(el);
        } else {
          el->second.r = REPEAT_NOT;
        }
      }
    }
  } else {
    set<InterfaceKey>::iterator key;
    // As policy, when the user hits a non-repeating key we want to
    // also cancel any keys that are currently repeating. This allows
    // for easy recovery from stuck keys.
    //
    // Unfortunately, each key may be bound to multiple
    // commands. So, lacking information on which commands are
    // accepted at the moment, there is no way we can know if it's
    // okay to cancel repeats unless /all/ the bindings are
    // non-repeating.
    for (key = keys.begin(); key != keys.end(); ++key) {
      Event e = {key_repeat(*key), *key, 0};
      timeline.insert(pair<Time,Event>(now,e));
    }
    // if (cancel_ok) {
    //   // Set everything on the timeline to non-repeating
    //   multimap<Time,Event>::iterator it;
    //   for (it = timeline.begin(); it != timeline.end(); ++it) {
    //     it->second.r = REPEAT_NOT;
    //   }
  }
}


void enabler_inputst::clear_input() {
  timeline.clear();
  pressed_keys.clear();
}

set<InterfaceKey> enabler_inputst::get_input() {
  Time now = SDL_GetTicks();

  // We walk the timeline, returning all events occurring
  // simultaneously with the first event to occur, in the past, and
  // inserting repeats as appropriate.
  set<InterfaceKey> input;
  multimap<Time,Event>::iterator it = timeline.begin();
  Time current_now = it->first;
  while (it != timeline.end() && it->first <= now && it->first == current_now) {
    Event ev = it->second;
    input.insert(ev.k);
    // Schedule a repeat
    ev.repeats++;
    switch (ev.r) {
    case REPEAT_NOT:
      break;
    case REPEAT_SLOW:
      if (ev.repeats > 1)
        timeline.insert(pair<Time,Event>(it->first + init.input.repeat_time, ev));
      else
        timeline.insert(pair<Time,Event>(it->first + init.input.hold_time, ev));
      break;
    case REPEAT_FAST:
      timeline.insert(pair<Time,Event>(it->first + init.input.repeat_time, ev));
      break;
    }
    // Delete the event from the timeline and iterate
    multimap<Time,Event>::iterator it2 = it++;
    timeline.erase(it2);
  }
#ifdef DEBUG
  if (input.size()) {
    cout << "Returning input:\n";
    set<InterfaceKey>::iterator it;
    for (it = input.begin(); it != input.end(); ++it)
        cout << "    " << GetKeyDisplay(*it) << ": " << GetBindingDisplay(*it) << endl;
  }
#endif
  return input;
}

set<InterfaceKey> enabler_inputst::key_translation(KeyEvent &e) {
  set<InterfaceKey> bindings;
  pair<multimap<EventMatch,InterfaceKey>::iterator,multimap<EventMatch,InterfaceKey>::iterator> its;
  
  for (its = keymap.equal_range(e.match); its.first != its.second; ++its.first)
    bindings.insert((its.first)->second);

  return bindings;
}

string enabler_inputst::GetKeyDisplay(int binding) {
  map<InterfaceKey,string>::iterator it = keydisplay.find(binding);
  if (it != keydisplay.end())
    return it->second;
  else {
    cout << "Missing binding displayed: " + bindingNames.left[(InterfaceKeyType)binding] << endl;
    return "?";
  }
}

string enabler_inputst::GetBindingDisplay(int binding) {
  map<InterfaceKeyType,string>::iterator it = bindingNames.left.find((InterfaceKeyType)binding);
  if (it != bindingNames.left.end())
    return it->second;
  else
    return "NO BINDING";
}

Repeat enabler_inputst::key_repeat(InterfaceKey binding) {
  map<InterfaceKey,Repeat>::iterator it = repeatmap.find(binding);
  if (it != repeatmap.end())
    return it->second;
  else
    return REPEAT_NOT;
}
