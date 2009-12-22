#include <SDL.h>
#include <map>

#include "enabler_input.h"
#include "init.h"

extern initst init;

using namespace std;

typedef Uint32 Time;

struct Event {
  Time t; // Time it was first inserted, i.e. when the key was pressed
  InterfaceKey k;
};

// A timeline of pending key events (for next get_input)
static multimap<Time,Event> timeline;

static bool is_release(SDL_Event &e) {
  switch (e.type) {
  case SDL_KEYUP:
  case SDL_MOUSEBUTTONUP:
    return true;
  default:
    return false;
  }
}

void enabler_inputst::add_input(SDL_Event &e, Uint32 now) {
  // If this is a key-press event, we add it to the queue. If it's a
  // release, we remove any pending repeats after the current time.
  // (Naturally, all presses /after/ the current time are /repeats/..)
  set<InterfaceKey> keys = key_translation(e);
  if (is_release(e)) {
    multimap<Time,Event>::iterator it = timeline.upper_bound(now);
    while (it != timeline.end()) {
      if (keys.count(it->second.k)) {
        multimap<Time,Event>::iterator el = it++;
        timeline.erase(el);
      } else {
        ++it;
      }
    }
  } else {
    set<InterfaceKey>::iterator key;
    for (key = keys.begin(); key != keys.end(); ++key) {
      Event e = {now, *key};
      timeline.insert(pair<Time,Event>(now,e));
      if (key_repeat(*key) == REPEAT_NOT) {
        // Cancel all future actions
        timeline.erase(timeline.upper_bound(now), timeline.end());
      }
    }
  }
}

void enabler_inputst::clear_input() {
  timeline.clear();
}

list<InterfaceKey> enabler_inputst::get_input() {
  Time now = SDL_GetTicks();

  // We walk the timeline, returning all events scheduled in the
  // past/present, and inserting repeats in the future as appropriate
  list<InterfaceKey> input;
  map<Time,Event>::iterator it = timeline.begin();
  while (it != timeline.end() && it->first <= now) {
    Event ev = it->second;
    input.push_back(ev.k);
    // Schedule a repeat
    switch (key_repeat(ev.k)) {
    case REPEAT_NOT:
      break;
    case REPEAT_SLOW:
      if (it->first == ev.t) {
        timeline.insert(pair<Time,Event>(it->first + init.input.hold_time, ev));
        break;
      }
    case REPEAT_FAST: // Or REPEAT_SLOW not on its first repeat
      timeline.insert(pair<Time,Event>(it->first + init.input.repeat_time, ev));
      break;
    }
    // Delete the event from the timeline and iterate
    map<Time,Event>::iterator it2 = it++;
    timeline.erase(it2);
  }
  return input;
}

set<InterfaceKey> enabler_inputst::key_translation(SDL_Event &e) {
  set<InterfaceKey> keys;
  
  switch (e.type) {
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    switch (e.key.keysym.sym) {
    case SDLK_SPACE: keys.insert(INTERFACEKEY_LEAVESCREEN); break;
    case SDLK_UP: keys.insert(INTERFACEKEY_STANDARDSCROLL_UP); break;
    case SDLK_DOWN: keys.insert(INTERFACEKEY_STANDARDSCROLL_DOWN); break;
    }
  }

  return keys;
}

Repeat enabler_inputst::key_repeat(InterfaceKey) {
  return REPEAT_NOT;
}
