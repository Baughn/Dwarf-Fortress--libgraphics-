#ifndef SCHEME_BASE_H
#define SCHEME_BASE_H

#include <string>

extern "C" {
#include <racket/scheme.h>
}

// Startup stub for Racket
int scheme_start(Scheme_Env *e, int argc, char **argv);
// A scheme_eval_string variant that sets an error handler, logging errors to console
Scheme_Object *scheme_eval_safe(const std::string &sexp);
Scheme_Object *scheme_apply_safe(Scheme_Object *f, int argc, Scheme_Object **argv);

#endif
