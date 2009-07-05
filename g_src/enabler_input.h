#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

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
  unsigned __int16 symbol;
  unsigned __int8 mods;
  unsigned __int8 flags;
  //Bit 0: keysym
  //Bit 1: unicode
  //Bit 2: Event
 };
 unsigned __int32 Value;
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
   unsigned __int32 processed;
   unsigned __int32 next_process;
  };
  unsigned __int64 procValue;
 };
} InputRec;

class enabler_inputst {
 svector<InputRec> input;
 protected:
 enabler_inputst();
 unsigned __int8 KeyMods;
 unsigned __int8 *keystate;
 public:
 int is_modkey(unsigned __int16 key);
 InputRec* getinput(int number);
 InputRec* currentinput(int now);
 void removeinput(int number);
 int inputcount() {return input.size();}
 void clear_input() {input.clear();}
 void add_input(unsigned __int32 sym, unsigned __int32 unicode);
 //handle the mods change in eventloop
};
#endif
