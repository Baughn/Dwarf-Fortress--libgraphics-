#ifndef SCHEME_BASE_H
#define SCHEME_BASE_H

extern "C" {
#include <racket/scheme.h>

  // Startup stub for Racket
  int scheme_start(Scheme_Env *e, int argc, char **argv);
}

#endif
