// Map DF's CP437 to Unicode
// see: http://dwarffortresswiki.net/index.php/Character_table
static int charmap[256] = {
  ' ', 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
  0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
  0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
  0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
  /* 0x20 */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2302,
  /* 0x80 */
  0xC7, 0xFC, 0xE9, 0xE2, 0xE4, 0xE0, 0xE5, 0xE7,
  0xEA, 0xEB, 0xE8, 0xEF, 0xEE, 0xEC, 0xC4, 0xC5,
  0xC9, 0xE6, 0xC6, 0xF4, 0xF6, 0xF2, 0xFB, 0xF9,
  0xFF, 0xD6, 0xDC, 0xA2, 0xA3, 0xA5, 0x20A7, 0x192,
  0xE1, 0xED, 0xF3, 0xFA, 0xF1, 0xD1, 0xAA, 0xBA,
  0xBF, 0x2310, 0xAC, 0xBD, 0xBC, 0xA1, 0xAB, 0xBB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
  0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
  0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
  0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
  0x3B1, 0xDF/*yay*/, 0x393, 0x3C0, 0x3A3, 0x3C3, 0xB5, 0x3C4,
  0x3A6, 0x398, 0x3A9, 0x3B4, 0x221E, 0x3C6, 0x3B5, 0x2229,
  0x2261, 0xB1, 0x2265, 0x2264, 0x2320, 0x2321, 0xF7, 0x2248,
  0xB0, 0x2219, 0xB7, 0x221A, 0x207F, 0xB2, 0x25A0, 0xA0
};

static bool curses_initialized = false;

static void endwin_void() {
  if (curses_initialized) {
    endwin();
    curses_initialized = false;
  }
}

class renderer_curses : public renderer {
  std::map<std::pair<int,int>,int> color_pairs; // For curses. MOVEME.

  // Map from DF color to ncurses color
  static int ncurses_map_color(int color) {
    if (color < 0) abort();
    switch (color) {
    case 0: return 0;
    case 1: return 4;
    case 2: return 2;
    case 3: return 6;
    case 4: return 1;
    case 5: return 5;
    case 6: return 3;
    case 7: return 7;
    default: return ncurses_map_color(color - 7);
    }
  }

  // Look up, or create, a curses color pair
  int lookup_pair(pair<int,int> color) {
    map<pair<int,int>,int>::iterator it = color_pairs.find(color);
    if (it != color_pairs.end()) return it->second;
    // We don't already have it. Make sure it's in range.
    if (color.first < 0 || color.first > 7 || color.second < 0 || color.second > 7) return 0;
    // We don't already have it. Generate a new pair if possible.
    if (color_pairs.size() < COLOR_PAIRS - 1) {
      const short pair = color_pairs.size() + 1;
      init_pair(pair, ncurses_map_color(color.first), ncurses_map_color(color.second));
      color_pairs[color] = pair;
      return pair;
    }
    // We don't have it, and there's no space for more. Panic!
    endwin();
    puts("Ran out of space for color pairs! Ask Baughn to implement a fallback!");
    exit(EXIT_FAILURE);
  }

public:

  void update_tile(int x, int y) {
    const int ch   = gps.screen[x*gps.dimy*4 + y*4 + 0];
    const int fg   = gps.screen[x*gps.dimy*4 + y*4 + 1];
    const int bg   = gps.screen[x*gps.dimy*4 + y*4 + 2];
    const int bold = gps.screen[x*gps.dimy*4 + y*4 + 3];

    const int pair = lookup_pair(make_pair<int,int>(fg,bg));

    if (ch == 219 && !bold) {
      // It's █, which is used for borders and digging designations.
      // A_REVERSE space looks better if it isn't completely tall.
      // Which is most of the time, for me at least.
      // █ <-- Do you see gaps?
      // █
      // The color can't be bold.
      wattrset(*stdscr_p, COLOR_PAIR(pair) | A_REVERSE);
      mvwaddstr(*stdscr_p, y, x, " ");
    } else {
      wattrset(*stdscr_p, COLOR_PAIR(pair) | (bold ? A_BOLD : 0));
      wchar_t chs[2] = {charmap[ch] ? charmap[ch] : ch,0};
      mvwaddwstr(*stdscr_p, y, x, chs);
    }
  }

  void update_all() {
    for (int x = 0; x < init.display.grid_x; x++)
      for (int y = 0; y < init.display.grid_y; y++)
        update_tile(x, y);
  }

  void render() {
    refresh();
  }

  void resize(int w, int h) {
    if (enabler.overridden_grid_sizes.size() == 0)
      gps_allocate(w, h);
    erase();
    // Force a full display cycle
    gps.force_full_display_count = 1;
    enabler.flag |= ENABLERFLAG_RENDER;
  }

  void grid_resize(int w, int h) {
    gps_allocate(w, h);
  }

  renderer_curses() {
    init_curses();
  }

  bool get_mouse_coords(int &x, int &y) {
    return false;
  }
};

// Reads from getch, collapsing utf-8 encoding to the actual unicode
// character.  Ncurses symbols (left arrow, etc.) are returned as
// positive values, unicode as negative. Error returns 0.
static int getch_utf8() {
  int byte = wgetch(*stdscr_p);
  if (byte == ERR) return 0;
  if (byte > 0xff) return byte;
  int len = decode_utf8_predict_length(byte);
  if (!len) return 0;
  string input(len,0); input[0] = byte;
  for (int i = 1; i < len; i++) input[i] = wgetch(*stdscr_p);
  return -decode_utf8(input);
}

void enablerst::eventLoop_ncurses() {
  int x, y, oldx = 0, oldy = 0;
  renderer_curses *renderer = static_cast<renderer_curses*>(this->renderer);
  
  while (loopvar) {
    // Check for terminal resize
    getmaxyx(*stdscr_p, y, x);
    if (y != oldy || x != oldx) {
      pause_async_loop();
      renderer->resize(x, y);
      unpause_async_loop();
      oldx = x; oldy = y;
    }
    
    // Deal with input
    Uint32 now = SDL_GetTicks();
    // Read keyboard input, if any, and transform to artificial SDL
    // events for enabler_input.
    int key;
    bool paused_loop = false;
    while ((key = getch_utf8())) {
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      bool esc = false;
      if (key == KEY_MOUSE) {
        MEVENT ev;
        if (getmouse(&ev) == OK) {
          // TODO: Deal with curses mouse input. And turn it on above.
        }
      } else if (key == -27) { // esc
        int second = getch_utf8();
        if (second) { // That was an escape sequence
          esc = true;
          key = second;
        }
      }
      add_input_ncurses(key, now, esc);
    }

    if (paused_loop)
      unpause_async_loop();

    // Run the common logic
    do_frame();
  }
}


//// libncursesw stub ////

extern "C" {
  static void *handle;
  WINDOW **stdscr_p;

  int COLOR_PAIRS;
  static int (*_erase)(void);
  static int (*_wmove)(WINDOW *w, int y, int x);
  static int (*_waddnstr)(WINDOW *w, const char *s, int n);
  static int (*_nodelay)(WINDOW *w, bool b);
  static int (*_refresh)(void);
  static int (*_wgetch)(WINDOW *w);
  static int (*_endwin)(void);
  static WINDOW *(*_initscr)(void);
  static int (*_raw)(void);
  static int (*_keypad)(WINDOW *w, bool b);
  static int (*_noecho)(void);
  static int (*_set_escdelay)(int delay);
  static int (*_curs_set)(int s);
  static int (*_start_color)(void);
  static int (*_init_pair)(short p, short fg, short bg);
  static int (*_getmouse)(MEVENT *m);
  static int (*_waddnwstr)(WINDOW *w, const wchar_t *s, int i);

  static void *dlsym_orexit(const char *symbol, bool actually_exit = true) {
    void *sym = dlsym(handle, symbol);
    if (!sym) {
      printf("Symbol not found: %s\n", symbol);
      if (actually_exit)
        exit(EXIT_FAILURE);
    }
    return sym;
  }

  int erase(void) {
    return _erase();
  }
  int wmove(WINDOW *w, int y, int x) {
    return _wmove(w, y, x);
  }
  int waddnstr(WINDOW *w, const char *s, int n) {
    return _waddnstr(w, s, n);
  }
  int nodelay(WINDOW *w, bool b) {
    return _nodelay(w, b);
  }
  int refresh(void) {
    return _refresh();
  }
  int wgetch(WINDOW *w) {
    return _wgetch(w);
  }
  int endwin(void) {
    return _endwin();
  }
  WINDOW *initscr(void) {
    return _initscr();
  }
  int raw(void) {
    return _raw();
  }
  int keypad(WINDOW *w, bool b) {
    return _keypad(w, b);
  }
  int noecho(void) {
    return _noecho();
  }
  int set_escdelay(int delay) {
    if (_set_escdelay)
      return _set_escdelay(delay);
    else
      return 0;
  }
  int curs_set(int s) {
    return _curs_set(s);
  }
  int start_color(void) {
    return _start_color();
  }
  int init_pair(short p, short fg, short bg) {
    return _init_pair(p, fg, bg);
  }
  int getmouse(MEVENT *m) {
    return _getmouse(m);
  }
  int waddnwstr(WINDOW *w, const wchar_t *s, int n) {
    return _waddnwstr(w, s, n);
  }

  void init_curses() {
    static bool stub_initialized = false;
    // Initialize the stub
    if (!stub_initialized) {
      stub_initialized = true;
      // We prefer libncursesw, but we'll accept libncurses if we have to
      handle = dlopen("libncursesw.so.5", RTLD_LAZY);
      if (handle) goto opened;
      handle = dlopen("libncursesw.so", RTLD_LAZY);
      if (handle) goto opened;
      puts("Didn't find any flavor of libncursesw, attempting libncurses");
      sleep(5);
      handle = dlopen("libncurses.so.5", RTLD_LAZY);
      if (handle) goto opened;
      handle = dlopen("libncurses.so", RTLD_LAZY);
      if (handle) goto opened;

    opened:
      if (!handle) {
        puts("Unable to open any flavor of libncurses!");
        exit(EXIT_FAILURE);
      }
      // Okay, look up our symbols
      int *pairs = (int*)dlsym_orexit("COLOR_PAIRS");
      COLOR_PAIRS = *pairs;
      stdscr_p = (WINDOW**)dlsym_orexit("stdscr");
      _erase = (int (*)(void))dlsym_orexit("erase");
      _wmove = (int (*)(WINDOW *w, int y, int x))dlsym_orexit("wmove");
      _waddnstr = (int (*)(WINDOW *w, const char *s, int n))dlsym_orexit("waddnstr");
      _nodelay = (int (*)(WINDOW *w, bool b))dlsym_orexit("nodelay");
      _refresh = (int (*)(void))dlsym_orexit("refresh");
      _wgetch = (int (*)(WINDOW *w))dlsym_orexit("wgetch");
      _endwin = (int (*)(void))dlsym_orexit("endwin");
      _initscr = (WINDOW *(*)(void))dlsym_orexit("initscr");
      _raw = (int (*)(void))dlsym_orexit("raw");
      _keypad = (int (*)(WINDOW *w, bool b))dlsym_orexit("keypad");
      _noecho = (int (*)(void))dlsym_orexit("noecho");
      _set_escdelay = (int (*)(int delay))dlsym_orexit("set_escdelay", false);
      _curs_set = (int (*)(int s))dlsym_orexit("curs_set");
      _start_color = (int (*)(void))dlsym_orexit("start_color");
      _init_pair = (int (*)(short p, short fg, short bg))dlsym_orexit("init_pair");
      _getmouse = (int (*)(MEVENT *m))dlsym_orexit("getmouse");
      _waddnwstr = (int (*)(WINDOW *w, const wchar_t *s, int i))dlsym_orexit("waddnwstr");
    }
    
    // Initialize curses
    if (!curses_initialized) {
      curses_initialized = true;
      initscr();
      raw();
      noecho();
      keypad(*stdscr_p, true);
      nodelay(*stdscr_p, true);
      set_escdelay(25); // Possible bug
      curs_set(0);
      mmask_t dummy;
      // mousemask(ALL_MOUSE_EVENTS, &dummy);
      start_color();
      init_pair(1, COLOR_WHITE, COLOR_BLACK);
      
      atexit(endwin_void);
    }
  }
};

