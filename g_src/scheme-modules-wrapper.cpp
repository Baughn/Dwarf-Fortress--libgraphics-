#include "scheme-base.h"
#include "scheme-modules.cpp"

void call_declare_modules(Scheme_Env *env) {
  declare_modules(env);
}
