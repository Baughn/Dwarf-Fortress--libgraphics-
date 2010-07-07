// HACK HACK HACK HACK HACK It would be good to extend raco so we can actually include
// enabler.h and won't need this. For the love of Armok...
class enablerst {
public:
  // IN ORDER TO AVOID PROBLEMS, HEED THIS:
  // 1: This enablerst copy must contain ONLY functions
  // 2: It must NOT contain any virtual functions of any kind
  void async_loop();  
};
extern enablerst enabler;

#include "scheme-base.h"
#include "scheme-modules.cpp"

// static Scheme_Env *env = NULL;

int scheme_start(Scheme_Env *e, int argc, char **argv) {
  // MZ_REGISTER_STATIC(env);
  // env = e;
  // Declare embedded modules from scheme-modules.c
  declare_modules(e);
  scheme_namespace_require(scheme_intern_symbol("racket/base"));

  // Call back into the enabler
  enabler.async_loop();
}

