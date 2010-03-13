#ifdef __APPLE__
# include "osx_messagebox.h"
#elif defined(unix)
# include <gtk/gtk.h>
#endif

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

#ifdef CURSES
# include "renderer_curses.cpp"
#endif

// The frame timer probably doesn't absolutely need to use globals,
// but this code *works*. I'm not touching it.
#ifdef unix
// sig_atomic_t is guaranteed to be atomic.
static volatile sig_atomic_t gframes_outstanding;
#else
// int *should* be, but is not guaranteed. OTOH, on windows it is.
// Also, if the very rare race condition does somehow trigger, and mess
// things up (which it shouldn't), the next check will clamp it anyway.
static volatile signed int gframes_outstanding;
#endif
static double frames_outstanding;

static Uint32 timer_tick(Uint32 interval, void *param) {
  SDL_cond *c = (SDL_cond*)param;
  gframes_outstanding++;
  const int fps = enabler.get_fps(),
    gfps = enabler.get_gfps();
  double fps_per_gfps = (double)fps / (double)gfps;

  frames_outstanding += fps_per_gfps;
  SDL_CondSignal(c); // Wake up the sleeper if necessary
  return interval;
}

enablerst::enablerst() {
  fullscreen = false;
  sync = NULL;
  renderer = NULL;
  timer_cond = NULL;
  dummy_mutex = NULL;
  calculated_fps = calculated_gfps = frame_sum = gframe_sum = frame_last = gframe_last = 0;
  fps = 100; gfps = 20;
}

void enablerst::do_frame() {
  const double fps_per_gfps = (double)fps / (double)gfps;
  // Clamp max number of outstanding frames/gframes to a sane number
  // Yes, this is a race condition, but the comparison and assignment are
  // individually atomic so worst-case it's clamped to 5 when it "should" be 6
  // or something. No problem there.
  if (gframes_outstanding > 5) gframes_outstanding = 5;
  if (frames_outstanding > fps_per_gfps*2+1) frames_outstanding = fps_per_gfps*2+1;
  //ENABLERFLAG_MAXFPS can lead to negative values, at least for frames_outstanding
  if (gframes_outstanding < -5) gframes_outstanding = -5;
  if (frames_outstanding < -20) frames_outstanding = -20;

  // Run the main loop, if appropriate.
  if (frames_outstanding >= 1 || (flag & ENABLERFLAG_MAXFPS)) {
    // puts("loop");
    frames_outstanding -= 1;
    update_fps();
    if (mainloop())
      loopvar = 0;
  }
  
  // Render one graphical frame, if appropriate.
  if ((flag & ENABLERFLAG_RENDER) && gframes_outstanding > 0 &&
      (!sync || glClientWaitSync(sync, 0, 0) == GL_ALREADY_SIGNALED)) {
    if (sync) {
      glDeleteSync(sync);
      sync = NULL;
    }
    render_things(); // Call UI renderers in DF proper
    render();        // ..then put them on-screen
    update_gfps();
    flag &= ~ENABLERFLAG_RENDER; // Mark this rendering as complete.
  } else {
    gframes_outstanding = 0;
  }

  // if (flag & ENABLERFLAG_MAXFPS)
  //   puts("max");
  // if (frames_outstanding > 0)
  //   std::cout << "frames: " << frames_outstanding << '\n';
  
  // Sleep if appropriate
  // printf("f: %g, g: %d\n", frames_outstanding, gframes_outstanding);
  if (gframes_outstanding <= 0 &&
      frames_outstanding <= 0) {
    // Sleep until the timer thread signals us
    if(!(flag & ENABLERFLAG_MAXFPS))SDL_CondWait(timer_cond, dummy_mutex);
  }
}

int enablerst::loop(string cmdline) {
  command_line = cmdline;

  // Call DF's initialization routine
  if (!beginroutine())
    exit(EXIT_FAILURE);

  // Initialize an interval timer for gframes, with
  // a semaphore that the main loop can sleep on.
  SDL_cond *c = SDL_CreateCond();
  SDL_TimerID timer = SDL_AddTimer(1000 / gfps, timer_tick, c);
  timer_cond = c;
  // We need this to keep SDL happy
  dummy_mutex = SDL_CreateMutex();
  SDL_mutexP(dummy_mutex);
  frames_outstanding = 0;
  
  // Allocate a renderer
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    renderer = new renderer_curses();
  } else {
    puts("No supported renderer selected!");
    abort();
  }

  // At this point we should have a window that is setup to render DF.
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
#ifdef CURSES
    eventLoop_ncurses();
#endif
  } else {
    // textures.upload_textures();
    SDL_EnableUNICODE(1);
    // eventLoop_SDL(window);
  }

  endroutine();

  // Clean up graphical resources
  delete renderer;
}

void enablerst::override_grid_size(int x, int y) {
  overridden_grid_sizes.push(make_pair<int,int>(init.display.grid_x,init.display.grid_y));
  init.display.grid_x = x;
  init.display.grid_y = y;
}

void enablerst::release_grid_size() {
  if (!overridden_grid_sizes.size()) return;
  pair<int,int> sz = overridden_grid_sizes.top();
  overridden_grid_sizes.pop();

  init.display.grid_x = sz.first;
  init.display.grid_y = sz.second;
}

void enablerst::zoom_display(zoom_commands command) {
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
  this->fps = fps;
}

void enablerst::set_gfps(int gfps) {
  this->gfps = gfps;
}

// Reports an error to the user, using a MessageBox and stderr.
static void report_error(const char *error_preface, const char *error_message)
{
  char *buf = NULL;
  // +4 = +colon +space +newline +nul
  buf = new char[strlen(error_preface) + strlen(error_message) + 4];
  sprintf(buf, "%s: %s\n", error_preface, error_message);
  MessageBox(NULL, buf, "SDL Error", MB_OK);
  fprintf(stderr, "%s", buf);
  delete [] buf;
}


int main (int argc, char* argv[]) {
#if !defined(__APPLE__) && defined(unix)
  bool ok = gtk_init_check(&argc, &argv);
  if (!ok) {
    puts("Display initialization failed, DF will crash if asked to use a messagebox.");
  }
#endif

  init.begin(); // Load init.txt settings
#if !defined(__APPLE__) && defined(unix)
  if (!ok && !init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    puts("Display not found and PRINT_MODE not set to TEXT, aborting.");
    exit(EXIT_FAILURE);
  }
#endif
  
#ifdef linux
  if (!init.media.flag.has_flag(INIT_MEDIA_FLAG_SOUND_OFF)) {
    // Initialize OpenAL
    if (!musicsound.initsound()) {
      puts("Initializing OpenAL failed, no sound will be played");
      init.media.flag.add_flag(INIT_MEDIA_FLAG_SOUND_OFF);
    }
  }
#endif

  // Initialise relevant SDL subsystems.
  int retval = SDL_Init(SDL_INIT_TIMER | (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) ? 0 : SDL_INIT_VIDEO));
  // Report failure?
  if (retval != 0) {
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }

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
