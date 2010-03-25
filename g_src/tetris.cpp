#include <cstring>
#include <list>

#include "graphics.h"
#include "init.h"
#include "interface.h"
#include "random.h"
#include "texture_handler.h"
#include "KeybindingScreen.h"

using namespace std;

graphicst gps;
initst init;
interfacest gview;
texture_handlerst texture;
GameMode gamemode;
long movie_version = 10001;

long basic_seed;
int mt_index[MT_BUFFER_NUM];
short mt_cur_buffer;
short mt_virtual_buffer;
unsigned long mt_buffer[MT_BUFFER_NUM][MT_LEN];

char filecomp_buffer2_aux[80000];
char filecomp_buffer_aux[20000];


bool tetriminoes[7][2][4] =
  {{{0,0,0,0},{1,1,1,1}},
   {{1,0,0,0},{1,1,1,0}},
   {{0,0,0,1},{0,1,1,1}},
   {{0,1,1,0},{0,1,1,0}},
   {{0,0,1,1},{0,1,1,0}},
   {{0,0,1,0},{0,1,1,1}},
   {{1,1,0,0},{0,1,1,0}}};

struct line {
  int x, y; // Location of the topmost character. y may be negative.
  string s;
  int color;
};

class Matrix : public viewscreenst {
  list<line> lines;
  typedef list<line>::iterator lit;
  int frame;

  string random_string(int len) {
    string s(len, 0);
    for (int i=0; i<len; i++)
      s[i] = basic_random(256);
    return s;
  }
public:
  Matrix() {
    frame = 0;
  }
  virtual void feed(set<InterfaceKey> &events) {
    if (events.count(INTERFACEKEY_LEAVESCREEN))
      breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
  }
  virtual void render() {
    gps.erasescreen();
    drawborder("Matrix", 1, 0);
    for (lit i = lines.begin(); i != lines.end(); ++i) {
      for (int j = 0; j < i->s.size(); ++j) {
        gps.addchar(i->x, i->y + j, i->s[j], i->color, 0, 0);
      }
    }
  }
  virtual void logic() {
    if ((frame++) % 2) return;
    enabler.flag |= ENABLERFLAG_RENDER;
    // Advance each line one step
    for (lit i = lines.begin(); i != lines.end();) {
      if ((++(i->y)) >= gps.dimy)
        lines.erase(i++);
      else
        ++i;
    }
    // Maybe add a new line
    if (basic_random(100) < 30) {
      line l;
      l.x = basic_random(gps.dimx);
      int len = basic_random(gps.dimy);
      l.y = -len;
      l.s = random_string(len);
      l.color = basic_random(7) + 1;
      lines.push_back(l);
    }
    // Pretend this takes a while
    // SDL_Delay(2);
    Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 3);
  }
  virtual void resize(int w, int h) {
    // Clear out lines that are now off-screen
    for (lit i = lines.begin(); i != lines.end();) {
      if (i->x >= w || i->y >= h)
        lines.erase(i++);
      else
        ++i;
    }
  }
};

char beginroutine() {
  mt_init();
  viewscreen_movieplayerst *m = viewscreen_movieplayerst::create(INTERFACE_PUSH_AT_BACK);
  m->force_play("data/initial_movies/dwarf_fortress.cmv");
  gview.addscreen(new Matrix, INTERFACE_PUSH_AT_BACK, NULL);
  return 1;
}

char mainloop() {
  return gview.loop();
}

void endroutine() {
}

void dwarf_help_routine() {
}

void dwarf_end_announcements() {
}

void dwarf_option_screen() {
  new KeybindingScreen();
}

void dwarf_remove_screen() {
}


void texture_handlerst::clean() {
}

void process_object_lines(textlinesst &lines, string &chktype) {
}

void drawborder(const char *str, char style, const char *color) {
  gps.erasescreen();
  if (style) {
    gps.changecolor(7,7,0);
    for (int x=0; x < gps.dimx; x++) {
      gps.locate(0, x);
      gps.addchar(' ');
      gps.locate(gps.dimy-1, x);
      gps.addchar(' ');
    }
    for (int y=1; y < gps.dimy-1; y++) {
      gps.locate(y, 0);
      gps.addchar(' ');
      gps.locate(y, gps.dimx-1);
      gps.addchar(' ');
    }
  }
  if (!str) return;
  gps.locate(0, MAX((gps.dimx - strlen(str)) / 2,0));
  if (color)
    gps.addcoloredst(str, color);
  else {
    gps.changecolor(7,0,1);
    gps.addst(str);
  }
}
