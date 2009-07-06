#include "enabler_input.h"

int enabler_inputst::is_modkey(uint16_t key) {
 switch (key) {
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
  case SDLK_RCTRL:
  case SDLK_LCTRL:
  case SDLK_RALT:
  case SDLK_LALT:
  case SDLK_RMETA:
  case SDLK_LMETA:
  case SDLK_RSUPER:
  case SDLK_LSUPER:
  case SDLK_CAPSLOCK:
  case SDLK_NUMLOCK:
  case SDLK_SCROLLOCK:
   return 1;
  default: {
   uint8_t Mods=(keystate[SDLK_LSHIFT]||keystate[SDLK_RSHIFT]);
   if ((key>=SDLK_a)&&(key<=SDLK_z)) Mods=Mods^keystate[SDLK_CAPSLOCK];
   //caps lock is only relevant to alpha keys until someone tells me otherwise
   if (Mods) Mods=KEY_SHIFTFLAG;
   else Mods=0;
   if (keystate[SDLK_LCTRL]||keystate[SDLK_RCTRL]) Mods|=KEY_CTRLFLAG;
   if (keystate[SDLK_LALT]||keystate[SDLK_RALT]) Mods|=KEY_ALTFLAG;
   if (keystate[SDLK_LMETA]||keystate[SDLK_RMETA]) Mods|=KEY_METAFLAG;
   if (keystate[SDLK_LSUPER]||keystate[SDLK_RSUPER])Mods|=KEY_SUPERFLAG;
   KeyMods=Mods;
   return 0;
  }
 }
}

enabler_inputst::enabler_inputst() {
 input.reserve(15);
 KeyMods=0;
}

void enabler_inputst::add_input(uint32_t sym,uint32_t unicode) {
 InputRec newn;
 newn.key.Value=sym;
 newn.key2.Value=unicode;
 if (!newn.key2.symbol) newn.key2.flags=0xFF;
 //mark with bad flags to block further testing
 newn.procValue=0;
 input.push_back(newn);
}

void enabler_inputst::removeinput(int number) {
 if (number<input.size()) input.erase(number);
}

InputRec* enabler_inputst::getinput(int number) {
 if (number<input.size()) return &(input[number]);
 else return 0;
}

InputRec* enabler_inputst::currentinput(int now) {
 int i = input.size();
 if (i) { //have something to do
  InputRec* cur=&(input[0]);
  int j=0;
  while (j<i) {
   if (cur->key.flags&KEY_EVENTFLAG) { //events get no keystate and no mods
    if (cur->processed) {
     input.erase(j);
     --i;
     continue;
    } else return cur;
   }
   if (cur->key.symbol>=SDLK_LAST) { //no keystate check for these, they can't repeat
    if (cur->processed) {
     input.erase(j);
     --i;
     continue;
    } else {
     is_modkey(0);
     cur->key.mods=KeyMods;
     return cur;
    }
   }
   //finally all others
   if (cur->processed) {
    if (keystate[cur->key.symbol]) {
     if ((now<cur->processed)&&(now>cur->next_process)) {
      is_modkey(cur->key.symbol);
      cur->key.mods=KeyMods;
      cur->key2.mods=KeyMods&(~KEY_SHIFTFLAG);
      return cur;
     } else {
      ++j;
      ++cur;
      continue;
     }
    } else {
     input.erase(j);
     --i;
     continue;
    }
   } else {
    is_modkey(cur->key.symbol);
    cur->key.mods=KeyMods;
    cur->key2.mods=KeyMods&(~KEY_SHIFTFLAG);
    return cur;
   }
  } //while
 } //if have input
 return 0;
}


//To do
//in the eventLoop clear inputs when focus lost
