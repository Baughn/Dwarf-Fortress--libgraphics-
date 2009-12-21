#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <SDL/SDL.h>
#include <string>

/* typedef union { */
/*   struct { */
/*     uint16_t symbol; */
/*     uint8_t mods; */
/*     /\* flag bits: */
/*        Bit 0: keysym */
/*        Bit 1: unicode */
/*        Bit 2: Event */
/*     *\/ */
/*     uint8_t flags; */
/*   }; */
/*   uint32_t Value; */
/* } KeyUnion; */

class enabler_inputst {
 public:
  void add_input(SDL_Event);
  void clear_input();

  void load_keybindings(const std::string &file){}
  void save_keybindings(const std::string &file){}
  std::string GetKeyDisplay(int binding){return "I DO NOT EXIST";}
  std::string GetBindingDisplay(int binding){return "I DO NOT EXIST EITHER";}
};


#endif
