#ifndef DF_CURSES_H
#define DF_CURSES_H

#ifdef CURSES

extern "C" {
#ifndef __APPLE__
#ifdef unix
#ifdef HAVE_NCURSESW
/* defining _XOPEN_SOURCE_EXTENDED activates NCURSES_WIDECHAR support */
#define _XOPEN_SOURCE_EXTENDED
#endif
#ifdef HAVE_NCURSESW_CURSES_H
# include <ncursesw/curses.h>
#else
# include <ncurses.h>
#endif
# undef COLOR_BLUE
# undef COLOR_CYAN
# undef COLOR_RED
# undef COLOR_YELLOW
# include <dlfcn.h>
#endif
#endif
}

#ifndef __APPLE__
#ifdef unix
extern "C" {
  void init_curses();
  extern WINDOW **stdscr_p;
};
#endif
#endif

#endif

#endif
