extern "C" {
#include "GL/glew.h"
#ifdef unix
# include <ncursesw/curses.h>
# undef COLOR_BLUE
# undef COLOR_CYAN
# undef COLOR_RED
# undef COLOR_YELLOW
#endif
}

extern bool curses_initialized;
void init_curses();
