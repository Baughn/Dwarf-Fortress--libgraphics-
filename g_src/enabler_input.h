#ifndef ENABLER_INPUT_H
#define ENABLER_INPUT_H

#include <SDL/SDL.h>

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
  void add_input(uint32_t sym, uint32_t unicode);
  void clear_input();
};


#endif
