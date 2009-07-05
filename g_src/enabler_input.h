#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <stdint.h>
#include "svector.h"
#include "SDL/SDL_keysym.h"

#define NUM_MOUSE_BUTTONS 20
enum MouseActions {
 KEY_MOUSEDOWN=SDLK_LAST+1,
 KEY_MOUSEUP=KEY_MOUSEDOWN+NUM_MOUSE_BUTTONS,
 KEY_MOUSELAST=KEY_MOUSEUP+NUM_MOUSE_BUTTONS
};

#define KEY_SHIFTFLAG 0x01
#define KEY_CTRLFLAG 0x02
#define KEY_ALTFLAG 0x04
#define KEY_METAFLAG 0x08
#define KEY_SUPERFLAG 0x10

#define KEY_KEYSYMFLAG 0x01
#define KEY_UNICODEFLAG 0x02
#define KEY_EVENTFLAG 0x04
typedef union {
 struct {
  uint16_t symbol;
  uint8_t mods;
  uint8_t flags;
  //Bit 0: keysym
  //Bit 1: unicode
  //Bit 2: Event
 };
 uint32_t Value;
} KeyUnion;

enum InterfaceEvents {
 INTERFACEEVENT_NONE=KEY_EVENTFLAG,
 INTERFACEEVENT_QUIT,
 INTERFACEEVENT_NEW_VIEW,
 INTERFACEEVENT_DEL_VIEW,
 INTERFACEEVENT_MOUSE_MOTION,
 INTERFACEEVENTNUM
};

typedef struct {
 KeyUnion key;
 KeyUnion key2;
 union {
  struct {
   uint32_t processed;
   uint32_t next_process;
  };
  uint64_t procValue;
 };
} InputRec;

class enabler_inputst {
 svector<InputRec> input;
 protected:
 enabler_inputst();
 uint8_t KeyMods;
 uint8_t *keystate;
 public:
 int is_modkey(uint16_t key);
 InputRec* getinput(int number);
 InputRec* currentinput(int now);
 void removeinput(int number);
 int inputcount() {return input.size();}
 void clear_input() {input.clear();}
 void add_input(uint32_t sym, uint32_t unicode);
 //handle the mods change in eventloop
};
#endif
