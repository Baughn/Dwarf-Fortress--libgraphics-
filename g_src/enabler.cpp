#ifdef __APPLE__
# include "osx_messagebox.h"
#elif defined(unix)
# include <gtk/gtk.h>
#endif

#include <cassert>

#include "platform.h"
#include "enabler.h"
#include "init.h"
#include "music_and_sound_g.h"

using namespace std;

enablerst enabler;


// For the printGLError macro
int glerrorcount = 0;

// Set to 0 when the game wants to quit
static int loopvar = 1;

// Reports an error to the user, using a MessageBox and stderr.
static void report_error(const char *error_preface, const char *error_message)
{
  char *buf = NULL;
  // +4 = +colon +space +newline +nul
  buf = new char[strlen(error_preface) + strlen(error_message) + 4];
  sprintf(buf, "%s: %s\n", error_preface, error_message);
  MessageBox(NULL, buf, "Error", MB_OK);
  fprintf(stderr, "%s", buf);
  delete [] buf;
}

texture_fullid renderer::screen_to_texid(int x, int y) {
  struct texture_fullid ret;
  const int tile = x * gps.dimy + y;
  const int ch = screen[tile*4 + 0];
  const int bold = (screen[tile*4 + 3] != 0) * 8;
  const int fg = screen[tile*4 + 1] + bold;
  const int bg = screen[tile*4 + 2] + bold;

  assert(fg >= 0 && fg < 16);
  assert(bg >= 0 && bg < 16);
  
  ret.texpos = enabler.is_fullscreen() ?
    init.font.large_font_texpos[ch] :
    init.font.small_font_texpos[ch];
  ret.r = enabler.ccolor[fg][0];
  ret.g = enabler.ccolor[fg][1];
  ret.b = enabler.ccolor[fg][2];
  ret.br = enabler.ccolor[bg][0];
  ret.bg = enabler.ccolor[bg][1];
  ret.bb = enabler.ccolor[bg][2];

  // TODO: Account for graphics mode

  return ret;
}


#ifdef CURSES
# include "renderer_curses.cpp"
#endif
#include "renderer_2d.cpp"
#include "renderer_opengl.cpp"


enablerst::enablerst() {
  fullscreen = false;
  sync = NULL;
  renderer = NULL;
  calculated_fps = calculated_gfps = frame_sum = gframe_sum = frame_last = gframe_last = 0;
  fps = 100; gfps = 20;
  fps_per_gfps = fps / gfps;
  last_tick = 0;
}

void renderer::display()
{
  const int dimx = init.display.grid_x;
  const int dimy = init.display.grid_y;
  static bool use_graphics = init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS);
  if (gps.force_full_display_count) {
    // Update the entire screen
    update_all();
  } else {
    if (use_graphics) {
      int off = 0;
      for (int x2=0; x2 < dimx; x2++) {
        for (int y2=0; y2 < dimy; y2++, ++off) {
          // TODO: Use pointers instead of offsets in this code too, if it's a win. Check in DF later.
          // Partial printing (and color-conversion): Big-ass if.
          if (*(int*)(screen + off) == *(int*)(screen_old + off) &&
              screentexpos[off] == screentexpos_old[off] &&
              screentexpos_addcolor[off] == screentexpos_addcolor_old[off] &&
              screentexpos_grayscale[off] == screentexpos_grayscale_old[off] &&
              screentexpos_cf[off] == screentexpos_cf_old[off] &&
              screentexpos_cbr[off] == screentexpos_cbr_old[off])
            {
              // Nothing's changed, this clause deliberately empty
            } else {
            update_tile(x2, y2);
          }
        }
      }
    } else {
      Uint32 *screenp = (Uint32*)screen, *oldp = (Uint32*)screen_old;
      for (int x2=0; x2 < dimx; ++x2) {
        for (int y2=0; y2 < dimy; ++y2, ++screenp, ++oldp) {
          if (*screenp != *oldp) {
            update_tile(x2, y2);
          }
        }
      }
    }
  }
  if (gps.force_full_display_count > 0) gps.force_full_display_count--;
}

void renderer::gps_allocate(int x, int y) {
  if (screen) delete[] screen;
  if (screentexpos) delete[] screentexpos;
  if (screentexpos_addcolor) delete[] screentexpos_addcolor;
  if (screentexpos_grayscale) delete[] screentexpos_grayscale;
  if (screentexpos_cf) delete[] screentexpos_cf;
  if (screentexpos_cbr) delete[] screentexpos_cbr;
  if (screen_old) delete[] screen_old;
  if (screentexpos_old) delete[] screentexpos_old;
  if (screentexpos_addcolor_old) delete[] screentexpos_addcolor_old;
  if (screentexpos_grayscale_old) delete[] screentexpos_grayscale_old;
  if (screentexpos_cf_old) delete[] screentexpos_cf_old;
  if (screentexpos_cbr_old) delete[] screentexpos_cbr_old;
  
  gps.screen = screen = new unsigned char[x*y*4];
  memset(screen, 0, x*y*4);
  gps.screentexpos = screentexpos = new long[x*y];
  memset(screentexpos, 0, x*y*sizeof(long));
  gps.screentexpos_addcolor = screentexpos_addcolor = new char[x*y];
  memset(screentexpos_addcolor, 0, x*y);
  gps.screentexpos_grayscale = screentexpos_grayscale = new unsigned char[x*y];
  memset(screentexpos_grayscale, 0, x*y);
  gps.screentexpos_cf = screentexpos_cf = new unsigned char[x*y];
  memset(screentexpos_cf, 0, x*y);
  gps.screentexpos_cbr = screentexpos_cbr = new unsigned char[x*y];
  memset(screentexpos_cbr, 0, x*y);

  screen_old = new unsigned char[x*y*4];
  memset(screen_old, 0, x*y*4);
  screentexpos_old = new long[x*y];
  memset(screentexpos_old, 0, x*y*sizeof(long));
  screentexpos_addcolor_old = new char[x*y];
  memset(screentexpos_addcolor_old, 0, x*y);
  screentexpos_grayscale_old = new unsigned char[x*y];
  memset(screentexpos_grayscale_old, 0, x*y);
  screentexpos_cf_old = new unsigned char[x*y];
  memset(screentexpos_cf_old, 0, x*y);
  screentexpos_cbr_old = new unsigned char[x*y];
  memset(screentexpos_cbr_old, 0, x*y);
  
  gps.resize(x,y);
}

void renderer::swap_arrays() {
  screen = screen_old; screen_old = gps.screen; gps.screen = screen;
  screentexpos = screentexpos_old; screentexpos_old = gps.screentexpos; gps.screentexpos = screentexpos;
  screentexpos_addcolor = screentexpos_addcolor_old; screentexpos_addcolor_old = gps.screentexpos_addcolor; gps.screentexpos_addcolor = screentexpos_addcolor;
  screentexpos_grayscale = screentexpos_grayscale_old; screentexpos_grayscale_old = gps.screentexpos_grayscale; gps.screentexpos_grayscale = screentexpos_grayscale;
  screentexpos_cf = screentexpos_cf_old; screentexpos_cf_old = gps.screentexpos_cf; gps.screentexpos_cf = screentexpos_cf;
  screentexpos_cbr = screentexpos_cbr_old; screentexpos_cbr_old = gps.screentexpos_cbr; gps.screentexpos_cbr = screentexpos_cbr;
}

void enablerst::pause_async_loop()  {
  struct async_cmd cmd;
  cmd.cmd = async_cmd::pause;
  async_tobox.write(cmd);
  async_wait();
}

// Wait until the previous command has been acknowledged, /or/
// async_loop has quit. Incidentally execute any requests in the
// meantime.
void enablerst::async_wait() {
  if (loopvar == 0) return;
  async_msg r;
  for (;;) {
    async_frombox.read(r);
    switch (r.msg) {
    case async_msg::quit:
      loopvar = 0;
      return;
    case async_msg::complete:
      return;
    case async_msg::set_fps:
      set_fps(r.fps);
      async_fromcomplete.write();
      break;
    case async_msg::set_gfps:
      set_gfps(r.fps);
      async_fromcomplete.write();
      break;
    case async_msg::push_resize:
      override_grid_size(r.x, r.y);
      async_fromcomplete.write();
      break;
    case async_msg::pop_resize:
      release_grid_size();
      async_fromcomplete.write();
      break;
    }
  }
}

void enablerst::async_loop() {
  async_paused = false;
  async_frames = 0;
  int fps = 100; // Just a thread-local copy
  for (;;) {
    // cout << "FRAMES: " << frames << endl;
    // Check for commands
    async_cmd cmd;
    bool have_cmd = true;
    do {
      if (async_paused || async_frames == 0)
        async_tobox.read(cmd);
      else
        have_cmd = async_tobox.try_read(cmd);
      // Obey the command, would you kindly.
      if (have_cmd) {
        switch (cmd.cmd) {
        case async_cmd::pause:
          async_paused = true;
          // puts("Paused");
          async_frombox.write(async_msg(async_msg::complete));
          break;
        case async_cmd::start:
          async_paused = false;
          async_frames = 0;
          // puts("UNpaused");
          break;
        case async_cmd::render:
          if (flag & ENABLERFLAG_RENDER) {
            renderer->swap_arrays();
            render_things();
            flag &= ~ENABLERFLAG_RENDER;
            update_gfps();
          }
          async_frombox.write(async_msg(async_msg::complete));
          break;
        case async_cmd::inc:
          async_frames += cmd.val;
          if (async_frames > fps*3) async_frames = fps*3; // Just in case
          break;
        case async_cmd::set_fps:
          fps = cmd.val;
          break;
        }
      }
    } while (have_cmd);
    // Run the main-loop, maybe
    if (!async_paused && async_frames) {
      if (mainloop()) {
        async_frombox.write(async_msg(async_msg::quit));
        return; // We're done.
      }
      async_frames--;
      update_fps();
    }
    SDL_NumJoysticks(); // Hook for dfhack
  }
}

void enablerst::do_frame() {
  // Check how long it's been, exactly
  const Uint32 now = SDL_GetTicks();
  const Uint32 interval = CLAMP(now - last_tick, 0, 1000); // Anything above a second doesn't count
  // cout << last_tick << " + " << interval << " = " << now << endl;
  last_tick = now;

  // Update outstanding-frame counts
  outstanding_frames += interval * fps / 1000;
  outstanding_gframes += interval * gfps / 1000;
  if (outstanding_gframes > 3) {
    outstanding_gframes = 3;
  }
  // cout << outstanding_frames << " " << outstanding_gframes << endl;
 
  // Update the loop's tick-counter suitably
  if (outstanding_frames >= 1) {
    async_cmd cmd(async_cmd::inc);
    cmd.val = outstanding_frames;
    outstanding_frames -= cmd.val;
    async_tobox.write(cmd);
  }

  // If it's time to render..
  if (outstanding_gframes >= 1 &&
      (!sync || glClientWaitSync(sync, 0, 0) == GL_ALREADY_SIGNALED)) {
    // Get the async-loop to render_things
    async_cmd cmd(async_cmd::render);
    async_tobox.write(cmd);
    async_wait();
    // Then finish here
    renderer->display();
    renderer->render();
    outstanding_gframes--;
  }

  // Sleep until the next gframe
  if (outstanding_gframes < 1) {
    float fragment = 1 - outstanding_gframes;
    float milliseconds = fragment / gfps * 1000;
    // cout << milliseconds << endl;
    SDL_Delay(milliseconds);
  }
}

void enablerst::eventLoop_SDL()
{
  
  SDL_Event event;
  const SDL_Surface *screen = SDL_GetVideoSurface();
  Uint32 mouse_lastused = 0;
  SDL_ShowCursor(SDL_DISABLE);
 
  // Initialize the grid
  renderer->resize(screen->w, screen->h);

  while (loopvar) {
    Uint32 now = SDL_GetTicks();
    bool paused_loop = false;

    // Check for zoom commands
    zoom_commands zoom;
    while (async_zoom.try_read(zoom)) {
      if (overridden_grid_sizes.size())
        continue; // No zooming in movies
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      if (zoom == zoom_fullscreen)
        renderer->set_fullscreen();
      else
        renderer->zoom(zoom);
    }

    // Check for SDL events
    while (SDL_PollEvent(&event)) {
      // Make sure mainloop isn't running while we're processing input
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      // Handle SDL events
      switch (event.type) {
      case SDL_KEYDOWN:
        // Disable mouse if it's been long enough
        if (mouse_lastused + 5000 < now) {
          if(init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
            // hide the mouse picture
            // enabler.set_tile(0, TEXTURE_MOUSE, enabler.mouse_x, enabler.mouse_y);
          }
          SDL_ShowCursor(SDL_DISABLE);
        }
      case SDL_KEYUP:
      case SDL_QUIT:
        enabler.add_input(event, now);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        if (!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
          int isdown = (event.type == SDL_MOUSEBUTTONDOWN);
          if (event.button.button == SDL_BUTTON_LEFT) {
            enabler.mouse_lbut = isdown;
            enabler.mouse_lbut_down = isdown;
            if (!isdown)
              enabler.mouse_lbut_lift = 0;
          } else if (event.button.button == SDL_BUTTON_RIGHT) {
            enabler.mouse_rbut = isdown;
            enabler.mouse_rbut_down = isdown;
            if (!isdown)
              enabler.mouse_rbut_lift = 0;
          } else
            enabler.add_input(event, now);
        }
        break;
      case SDL_MOUSEMOTION:
        // Deal with the mouse hiding bit
        mouse_lastused = now;
        if(init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
          // turn on mouse picture
          // enabler.set_tile(gps.tex_pos[TEXTURE_MOUSE], TEXTURE_MOUSE,enabler.mouse_x, enabler.mouse_y);
        } else {
          SDL_ShowCursor(SDL_ENABLE);
        }
        break;
      case SDL_ACTIVEEVENT:
        enabler.clear_input();
        if (event.active.state & SDL_APPACTIVE) {
          if (event.active.gain) {
            enabler.flag|=ENABLERFLAG_RENDER;
            gps.force_full_display_count++;
          }
        }
        break;
      case SDL_VIDEOEXPOSE:
        gps.force_full_display_count++;
        enabler.flag|=ENABLERFLAG_RENDER;
        break;
      case SDL_VIDEORESIZE:
        if (is_fullscreen())
          errorlog << "Caught resize event in fullscreen??\n";
        else {
          gamelog << "Resizing window to " << event.resize.w << "x" << event.resize.h << endl << flush;
          renderer->resize(event.resize.w, event.resize.h);
        }
        break;
      } // switch (event.type)
    } //while have event

    // Update mouse state
    if (!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
      int mouse_x = -1, mouse_y = -1, mouse_state;
      // Check whether the renderer considers this valid input or not, and write it to gps
      if ((SDL_GetAppState() & SDL_APPMOUSEFOCUS) &&
          renderer->get_mouse_coords(mouse_x, mouse_y)) {
        mouse_state = 1;
      } else {
        mouse_state = 0;
      }
      if (mouse_x != gps.mouse_x || mouse_y != gps.mouse_y ||
          mouse_state != enabler.tracking_on) {
        // Pause rendering loop and update values
        if (!paused_loop) {
          pause_async_loop();
          paused_loop = true;
        }
        enabler.tracking_on = mouse_state;
        gps.mouse_x = mouse_x;
        gps.mouse_y = mouse_y;
      }
    }

    if (paused_loop)
      unpause_async_loop();

    do_frame();
#if !defined(NO_FMOD)
    // Call FMOD::System.update(). Manages a bunch of sound stuff.
    musicsound.update();
#endif
  }
}

int enablerst::loop(string cmdline) {
  command_line = cmdline;

  // Call DF's initialization routine
  if (!beginroutine())
    exit(EXIT_FAILURE);
  
  // Allocate a renderer
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
#ifdef CURSES
    renderer = new renderer_curses();
#else
    report_error("PRINT_MODE","TEXT not supported on windows");
    exit(EXIT_FAILURE);
#endif
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_2D)) {
    renderer = new renderer_2d();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER)) {
    renderer = new renderer_accum_buffer();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER)) {
    renderer = new renderer_framebuffer();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT)) {
    if (init.display.partial_print_count)
      renderer = new renderer_partial();
    else
      renderer = new renderer_once();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_VBO)) {
    renderer = new renderer_vbo();
  } else {
    renderer = new renderer_opengl();
  }

  // At this point we should have a window that is setup to render DF.
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
#ifdef CURSES
    eventLoop_ncurses();
#endif
  } else {
    SDL_EnableUNICODE(1);
    eventLoop_SDL();
  }

  endroutine();

  // Clean up graphical resources
  delete renderer;
}

void enablerst::override_grid_size(int x, int y) {
  if (SDL_ThreadID() != renderer_threadid) {
    // Ask the renderer to do it
    async_msg m(async_msg::push_resize);
    m.x = x; m.y = y;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    // We are the renderer; do it.
    overridden_grid_sizes.push(make_pair<int,int>(init.display.grid_x,init.display.grid_y));
    renderer->grid_resize(x, y);
  }
}

void enablerst::release_grid_size() {
  if (SDL_ThreadID() != renderer_threadid) {
    async_frombox.write(async_msg(async_msg::pop_resize));
    async_fromcomplete.read();
  } else {
    if (!overridden_grid_sizes.size()) return;
    pair<int,int> sz = overridden_grid_sizes.top();
    overridden_grid_sizes.pop();
    renderer->grid_resize(sz.first, sz.second);
  }
}

void enablerst::zoom_display(zoom_commands command) {
  async_zoom.write(command);
}

int enablerst::calculate_fps() { return calculated_fps; }
int enablerst::calculate_gfps() { return calculated_gfps; }

void enablerst::do_update_fps(queue<int> &q, int &sum, int &last, int &calc) {
  while (q.size() && sum > 10000) {
    sum -= q.front();
    q.pop();
  }
  const int now = SDL_GetTicks();
  const int interval = now - last;
  q.push(interval);
  sum += interval;
  last = now;
  if (sum)
    calc = q.size() * 1000 / sum;
}

void enablerst::update_fps() {
  do_update_fps(frame_timings, frame_sum, frame_last, calculated_fps);
}

void enablerst::update_gfps() {
  do_update_fps(gframe_timings, gframe_sum, gframe_last, calculated_gfps);
}

void enablerst::set_fps(int fps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_fps);
    m.fps = fps;
    async_paused = true;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (fps == 0)
      fps = 1048576;
    this->fps = fps;
    fps_per_gfps = fps / gfps;
    struct async_cmd cmd;
    cmd.cmd = async_cmd::set_fps;
    cmd.val = fps;
    async_tobox.write(cmd);
    async_tobox.write(async_cmd(async_cmd::start));
  }
}

void enablerst::set_gfps(int gfps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_gfps);
    m.fps = gfps;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (gfps == 0)
      gfps = 50;
    this->gfps = gfps;
    fps_per_gfps = fps / gfps;
  }
}

int call_loop(void *dummy) {
  enabler.async_loop();
  return 0;
}

int main (int argc, char* argv[]) {
#if !defined(__APPLE__) && defined(unix)
  bool ok = gtk_init_check(&argc, &argv);
  if (!ok) {
    puts("Display initialization failed, DF will crash if asked to use a messagebox.");
  }
#endif

  // Initialise minimal SDL subsystems.
  int retval = SDL_Init(SDL_INIT_TIMER);
  // Report failure?
  if (retval != 0) {
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }
  enabler.renderer_threadid = SDL_ThreadID();

  // Spawn simulation thread
  SDL_CreateThread(call_loop, NULL);

  init.begin(); // Load init.txt settings
  
#if !defined(__APPLE__) && defined(unix)
  if (!ok && !init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    puts("Display not found and PRINT_MODE not set to TEXT, aborting.");
    exit(EXIT_FAILURE);
  }
#endif

  // Initialize video, if we /use/ video
  retval = SDL_InitSubSystem(init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) ? 0 : SDL_INIT_VIDEO);
  if (retval != 0) {
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }
  
#ifdef linux
  if (!init.media.flag.has_flag(INIT_MEDIA_FLAG_SOUND_OFF)) {
    // Initialize OpenAL
    if (!musicsound.initsound()) {
      puts("Initializing OpenAL failed, no sound will be played");
      init.media.flag.add_flag(INIT_MEDIA_FLAG_SOUND_OFF);
    }
  }
#endif

#ifdef WIN32
  // Attempt to get as good a timer as possible
  int ms = 1;
  while (timeBeginPeriod(ms) != TIMERROR_NOERROR) ms++;
#endif

  // Load keyboard map
  keybinding_init();
  enabler.load_keybindings("data/init/interface.txt");

  string cmdLine;
  for (int i = 1; i < argc; ++i) { 
    char *option = argv[i];
    cmdLine += option;
    cmdLine += " ";
  }
  int result = enabler.loop(cmdLine);

  SDL_Quit();

#ifdef CURSES
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT))
    endwin();
#endif
  
#ifdef WIN32
  timeEndPeriod(ms);
#endif
  
  return result;
}

char get_slot_and_addbit_uchar(unsigned char &addbit,long &slot,long checkflag,long slotnum)
{
  if(checkflag<0)return 0;

  //FIND PROPER SLOT
  slot=checkflag>>3;
  if(slot>=slotnum)return 0;

  //FIND PROPER BIT IN THAT SLOT
  addbit=1<<(checkflag%8);

  return 1;
}
