#include "platform.h"
#include <cerrno>
#include <string>
#include <cstring>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <queue>

extern "C" {
#include <zlib.h>
#include <assert.h>
#ifndef WIN32
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <signal.h>
#endif
}
#include "svector.h"
#include "random.h"

using std::string;

#include "basics.h"

#include "g_basics.h"

extern "C" {
#ifdef __APPLE__
# include "osx_messagebox.h"
#elif defined(unix)
# include <gtk/gtk.h>
#endif
}

#include "endian.h"
#include "files.h"

#include "enabler.h"
enablerst enabler;
#include "graphics.h"

#include "init.h"
extern initst init;

#ifndef NO_FMOD
// For musicsound.update();
#include "music_and_sound_g.h"
extern musicsoundst musicsound;
#endif
extern graphicst gps;

// Function prototypes.
char beginroutine(void);
char mainloop(void);
void endroutine(void);
void ne_toggle_fullscreen(void);

static int loopvar = 1;

static int glerrorcount = 0;

// GL error macro
#ifdef DEBUG
# define printGLError() do { GLenum err; do { err = glGetError(); if (err && glerrorcount < 40) { printf("GL error: 0x%x in %s:%d\n", err, __FILE__ , __LINE__); glerrorcount++; } } while(err); } while(0);
# define deputs(str) puts(str)
#else
# define printGLError()
# define deputs(str)
#endif


#ifndef MAX
	#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
	#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Yes, the profusion of global variables below is a bad thing. The reason they
// exist is because there is only one, global gridrectst, but it likes to
// pretend otherwise, and zooming is currently a global action. In other words,
// bad architecture.

// There are two different kinds of zoom.
// Grid zoom: The game may use a smaller or larger grid than the "natural" size for the window.
// Viewport zoom: The window may show just part of the grid.
// Grid zoom is calculated from zoom_steps.
static int zoom_steps = 0;
static double grid_zoom = 1.0, viewport_zoom = 1.0;
// Further, there's a distinction between forced grid zoom and requested zoom.
static double grid_zoom_req = 1.0;
// Furthermore, there's the center of the viewport to consider. Origins range from 0 to 1.
static double viewport_x = 0, viewport_y = 0;
// Whether zoom commands affect grid_zoom or viewport_zoom
static bool zoom_grid = true;
// A list of zoom commands to execute at the end of this frame
static std::queue<enum zoom_commands> zoom_command_buffer;
// A general "buffers dirty, don't render this frame" flag
static bool skip_gframe = true;

//Used during image export
double old_grid_zoom_req;
bool old_zoom_grid;

// GL viewport used for rendering, for mouse calculations
static int origin_x, origin_y, size_x, size_y;

// Translates a window position to a grid location. Returns -1 for
// window locations outside the grid.
pair<int,int> window_to_grid(int x, int y) {
  // Translate to grid coordinates
  if (x < origin_x || y < origin_y || x >= origin_x + size_x || y >= origin_y + size_y)
    return std::make_pair(-1,-1);
  double rel_x = (x - origin_x) / (double)size_x;
  double rel_y = (y - origin_y) / (double)size_y;
  return std::make_pair(rel_x * init.display.grid_x,
                        rel_y * init.display.grid_y);
}


static void resize_grid(int width, int height, bool resizing) {
  skip_gframe = true;
  // The grid must fit inside the window. We ensure this by zooming
  // out if it'd otherwise become smaller than 80x25, or in if it'd
  // become larger than 200x200.
  if (enabler.create_full_screen) {
    width = enabler.window.init.width;
    height = enabler.window.init.height;
  }

  // Figure out how large a grid we're being asked for.
  const int font_w = enabler.create_full_screen ? init.font.large_font_dispx : init.font.small_font_dispx;
  const int font_h = enabler.create_full_screen ? init.font.large_font_dispy : init.font.small_font_dispy;

  const int desired_grid_x = width / font_w / grid_zoom_req;
  const int desired_grid_y = height / font_h / grid_zoom_req;
#ifdef DEBUG
  printf("Asked for %dx%d grid\n", desired_grid_x, desired_grid_y);
#endif
  int new_grid_x = MAX(MIN(desired_grid_x,MAX_GRID_X),80),
      new_grid_y = MAX(MIN(desired_grid_y,MAX_GRID_Y),25);
  double min_zoom = 0, max_zoom = 1000;
  min_zoom = MAX(min_zoom, (double)width / (font_w * MAX_GRID_X));
  min_zoom = MAX(min_zoom, (double)height / (font_h * MAX_GRID_Y));
  max_zoom = MIN(max_zoom, (double)width / (font_w * 80));
  max_zoom = MIN(max_zoom, (double)height / (font_h * 25));

  if (max_zoom < min_zoom) {
    puts("I can't handle a window like this. Get a grip.");
    return;
  }

  grid_zoom = grid_zoom_req;
  if (grid_zoom < min_zoom)
    grid_zoom = min_zoom;
  if (grid_zoom > max_zoom)
    grid_zoom = max_zoom;

#ifdef DEBUG
  printf("Setting to %dx%d, grid %dx%d, zoom %f\n", width, height, new_grid_x,new_grid_y,grid_zoom);
#endif
  if (enabler.create_full_screen) {
    init.display.large_grid_x = new_grid_x;
    init.display.large_grid_y = new_grid_y;
  } else {
    init.display.small_grid_x = new_grid_x;
    init.display.small_grid_y = new_grid_y;
    enabler.desired_windowed_width = enabler.window_width = width;
    enabler.desired_windowed_height = enabler.window_height = height;
  }
  gridrectst *rect; int i=0;
  // FIXME: THere's just one gridrect, and it's time DF accepted this.
  while (rect = enabler.get_gridrect(i)) {
    i++;
    rect->allocate(new_grid_x, new_grid_y);
  }
  // Reset GL and SDL for the new window size
 if (resizing)
   enabler.reset_gl();
 ne_toggle_fullscreen();
}

static void reset_window() {
  if (enabler.create_full_screen)
    resize_grid(-1,-1, false);
  else
    resize_grid(enabler.desired_windowed_width, enabler.desired_windowed_height, false);
}

static void resize_window(int width, int height) {
  resize_grid(width, height, true);
}

void zoom_display(enum zoom_commands command) {
  zoom_command_buffer.push(command);
}

// Delayed because it's /called/ delayed, not because it delays
static void zoom_display_delayed(enum zoom_commands command) {
  const int font_w = enabler.create_full_screen ? init.font.large_font_dispx : init.font.small_font_dispx;
  const int font_h = enabler.create_full_screen ? init.font.large_font_dispy : init.font.small_font_dispy;
  const double zoom_factor = init.input.zoom_factor;
  const int old_zoom_steps = zoom_steps;
  const double old_req = grid_zoom_req, old_grid_zoom = grid_zoom;

  switch (command) {
  case zoom_toggle_gridzoom:
    deputs("Toggling zoom_grid");
    zoom_grid = !zoom_grid;
    // Todo
    break;
  case zoom_reset:
    deputs("Resetting zoom");
    grid_zoom_req = 1.0;
    zoom_steps = 0;
    reset_window();
    break;
  case zoom_in:
    if (zoom_grid) {
      grid_zoom_req *= zoom_factor; zoom_steps++;
      if (0 == zoom_steps) // Avoid perturbations
        grid_zoom_req = 1.0;
      reset_window();
      if (grid_zoom == old_grid_zoom && grid_zoom < grid_zoom_req) { // Can't zoom in this direction right now
        grid_zoom_req = old_req;
        zoom_steps = old_zoom_steps;
      }
    } else {
      viewport_zoom *= zoom_factor;
      gps.force_full_display_count++;
#ifdef DEBUG
      printf("Viewport_zoom = %f\n", viewport_zoom);
#endif
    }
    break;
  case zoom_out:
    if (zoom_grid) {
      grid_zoom_req /= zoom_factor;
      zoom_steps--;
      if (0 == zoom_steps)
        grid_zoom_req = 1.0;
      reset_window();
      if (grid_zoom == old_grid_zoom && grid_zoom > grid_zoom_req) {
        grid_zoom_req = old_req;
        zoom_steps = old_zoom_steps;
      }
    } else {
      viewport_zoom = MAX(1.0, viewport_zoom / zoom_factor);
      gps.force_full_display_count++;
#ifdef DEBUG
      printf("Viewport_zoom = %f\n", viewport_zoom);
#endif
    }
    break;
  default:
    puts("invalid zoom command");
  }
}

static void eventLoop(GL_Window window)
{
 // Initialize the zoom
 reset_window();
  
 SDL_Event event;
 SDL_Surface *screen = NULL;
 Uint32 mouse_lastused = 0;
 SDL_ShowCursor(SDL_DISABLE);

 KeyUnion NewInput;
 KeyUnion NewUnicode;
 NewInput.Value=0;
 NewInput.flags=KEY_KEYSYMFLAG;
 NewUnicode.Value=0;
 NewUnicode.flags=KEY_UNICODEFLAG;
 while (loopvar) {
  enabler.now = SDL_GetTicks();
  // Handle buffered zoom events
  while (!zoom_command_buffer.empty()) {
    zoom_display_delayed(zoom_command_buffer.front());
    zoom_command_buffer.pop();
  }
  // Handle SDL events
  while (SDL_PollEvent(&event)) {
   switch (event.type) {
    case SDL_QUIT:
     enabler.add_input(INTERFACEEVENT_QUIT,0);
     NewInput.flags=KEY_KEYSYMFLAG;
    break;
    case SDL_MOUSEBUTTONDOWN:
     if(!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
//      enabler.add_input(INTERFACEEVENT_MOUSE_DOWN,0);
      NewInput.symbol=event.button.button;
      switch (NewInput.symbol) {
       case SDL_BUTTON_LEFT:
        enabler.mouse_lbut = 1;
        enabler.mouse_lbut_down = 1;
       break;
       case SDL_BUTTON_RIGHT:
        enabler.mouse_rbut = 1;
        enabler.mouse_rbut_down = 1;
        break;
      default:
        if (NewInput.symbol<NUM_MOUSE_BUTTONS) {
         NewInput.symbol+=KEY_MOUSEDOWN;
         enabler.add_input(NewInput.Value,0);
        }
        break;
      }
     }
    break;
    case SDL_MOUSEBUTTONUP:
     if(!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
//      enabler.add_input(INTERFACEEVENT_MOUSE_UP,0);
      NewInput.symbol=event.button.button;
      switch (NewInput.symbol) {
       case SDL_BUTTON_LEFT:
        enabler.mouse_lbut = 0;
        enabler.mouse_lbut_down = 0;
        enabler.mouse_lbut_lift = 1;
       break;
       case SDL_BUTTON_RIGHT:
        enabler.mouse_rbut = 0;
        enabler.mouse_rbut_down = 0;
        enabler.mouse_rbut_lift = 1;
       break;
       default:
        if (NewInput.symbol<NUM_MOUSE_BUTTONS) {
         NewInput.symbol+=KEY_MOUSEUP;
         enabler.add_input(NewInput.Value,0);
        }
       break;
      }
     }
    break;
    case SDL_KEYDOWN:
     // Disable mouse if it's been long enough
     if (mouse_lastused + 5000 < enabler.now) {
       if(init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
//       hide the mouse picture
//       enabler.set_tile(0, TEXTURE_MOUSE, enabler.mouse_x, enabler.mouse_y);
       }
       SDL_ShowCursor(SDL_DISABLE);
     }
     NewInput.symbol=event.key.keysym.sym;
     //do nothing when all we got is a modifier key
     if (!enabler.is_modkey(NewInput.symbol)) {
      if (NewInput.symbol==SDLK_F12) {
       zoom_display(zoom_reset);
       break;
      }
      NewUnicode.symbol=event.key.keysym.unicode;
      enabler.add_input(NewInput.Value,NewUnicode.Value);
     }
     /* Debian _somehow_ managed to patch SDL 1.2 so that the 'lock'
     * keys don't generate a modifier. This can be fixed by setting
     * an environmental variable that is supposed to _cause_ this
     * behaviour. This also effects Ubuntu (as of 8.04.1).
     */
    break;
    case SDL_ACTIVEEVENT:
     if (event.active.state & SDL_APPACTIVE) {
      if (event.active.gain) {
        gps.force_full_display_count++;
        std::cout << "Gained focus\n";
      } else {
       enabler.clear_input();
       // TODO: Disable rendering when nobody would see it anyway
       // Or maybe pause?
      }
     }
    break;
    case SDL_VIDEOEXPOSE:   gps.force_full_display_count++; break;
    case SDL_MOUSEMOTION:
     // Is the mouse over the screen surface?
     if(!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
       if (event.motion.x >= origin_x && event.motion.x < origin_x + size_x &&
           event.motion.y >= origin_y && event.motion.y < origin_y + size_y) {
         // Store last position
         enabler.oldmouse_x = enabler.mouse_x;
         enabler.oldmouse_y = enabler.mouse_y;
         enabler.tracking_on = 1;
         // Set viewport_x/y as appropriate, and fixup mouse position for zoom
         // We use only the central 60% of the window for setting viewport origin.
         if (!zoom_grid) {
           double mouse_x = (double)event.motion.x / size_x,
             mouse_y = (double)event.motion.y / size_y;
           double percentage = 0.60;
           mouse_x /= percentage;
           mouse_y /= percentage;
           mouse_x -= (1-percentage)/2;
           mouse_y -= (1-percentage)/2;
           mouse_x = MIN(MAX(mouse_x,0),1);
           mouse_y = MIN(MAX(mouse_y,0),1);
           double new_viewport_x = mouse_x, new_viewport_y = mouse_y;
           if (new_viewport_x != viewport_x || new_viewport_y != viewport_y) {
             viewport_x = new_viewport_x;
             viewport_y = new_viewport_y;
             gps.force_full_display_count++;
           }
           double visible = 1/viewport_zoom,
             invisible = 1 - visible;
           double visible_w = enabler.window_width * visible,
             visible_h = enabler.window_height * visible;
           enabler.mouse_x = ((double)event.motion.x / enabler.window_width) * visible_w + (invisible*viewport_x*enabler.window_width);
           enabler.mouse_y = ((double)event.motion.y / enabler.window_height) * visible_h + (invisible*viewport_y*enabler.window_height);
         } else {
           enabler.mouse_x = event.motion.x;
           enabler.mouse_y = event.motion.y;
         }
         mouse_lastused = enabler.now;
         if(init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
           //        turn on mouse picture
           //        enabler.set_tile(gps.tex_pos[TEXTURE_MOUSE], TEXTURE_MOUSE,enabler.mouse_x, enabler.mouse_y);
           SDL_ShowCursor(SDL_DISABLE);
         } else SDL_ShowCursor(SDL_ENABLE);
       } else {
         enabler.oldmouse_x = -1;
         enabler.oldmouse_y = -1;
         enabler.mouse_x = -1;
         enabler.mouse_y = -1;
         enabler.mouse_lbut = 0;
         enabler.mouse_rbut = 0;
         enabler.mouse_lbut_lift = 0;
         enabler.mouse_rbut_lift = 0;
         enabler.tracking_on = 0;
       }
     } //init mouse on
     break;
   case SDL_VIDEORESIZE:
     resize_window(event.resize.w, event.resize.h);
     break;
   } // switch (event.type)
  } //while have event

  enabler.do_frame();
#if !defined(NO_FMOD)
  // Call FMOD::System.update(). Manages a bunch of sound stuff.
  musicsound.update();
#endif
 }
}

#ifdef unix
// sig_atomi_c is guaranteed to be atomic.
static volatile sig_atomic_t gframes_outstanding;
#else
// int *should* be, but is not guaranteed. OTOH, on windows it is.
// Also, if the very rare race condition does somehow trigger, and mess
// things up (which it shouldn't), the next check will clamp it anyway.
static volatile signed int gframes_outstanding;
#endif

static Uint32 timer_tick(Uint32 interval, void *param) {
  SDL_cond *c = (SDL_cond*)param;
  SDL_CondSignal(c); // Wake up the sleeper if necessary
  gframes_outstanding++;
  double fps_per_gfps;
  if(enabler.flag & ENABLERFLAG_MAXFPS)
	{
	fps_per_gfps = ((double)enabler.g_qprate.QuadPart)/((double)enabler.main_qprate.QuadPart);
	}
  else fps_per_gfps = ((double)enabler.g_qprate.QuadPart)/((double)init.display.dwarf_frame_rate.QuadPart);

  enabler.frames_outstanding += fps_per_gfps;
  return interval;
}

int enablerst::loop(void)
{
  // Fill Out Window
  ZeroMemory (&window, sizeof (GL_Window));               // Make Sure Memory Is Zeroed
  window.init.title			= (char*)GAME_TITLE_STRING; // Window Title
  window.init.bitsPerPixel	= 32;                       // Bits Per Pixel
  window.init.isFullScreen	= TRUE;                     // Fullscreen? (Set To TRUE)

  is_program_looping = TRUE;
  if (!beginroutine())
    {
      is_program_looping = FALSE;
    }
  else
    {
      QueryPerformanceCounter(&qpc);
      QueryPerformanceCounter(&qpc2);
      QueryPerformanceCounter(&g_qpc);
		
      // Initialize an interval timer for gframes, with
      // a semaphore that the main loop can sleep on.
      SDL_cond *c = SDL_CreateCond();
      LARGE_INTEGER rate;
      QueryPerformanceFrequency(&rate);
      SDL_TimerID timer = SDL_AddTimer(g_qprate.QuadPart / (rate.QuadPart / 1000),
				       timer_tick, c);
      timer_cond = c;
      // We need this to keep SDL happy
      dummy_mutex = SDL_CreateMutex();
      SDL_mutexP(dummy_mutex);
      frames_outstanding = 0;

      current_render_count = 0;
      secondary_render_count = 0;
		
      window.init.isFullScreen = create_full_screen;
      if (!create_window_GL(&window))
	{
	  MessageBox (NULL, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
	  is_program_looping = FALSE;
	}
      else
	{
	  // At this point we should have a window that is setup to render OpenGL.
	  textures.upload_textures();
      keystate=SDL_GetKeyState(0);
      SDL_EnableUNICODE(1);
	  eventLoop(window);
	  textures.remove_uploaded_textures();

	  // Destroy the active window.
	  destroy_window_GL(&window);	
	}
      // Remove the timer and variable
      SDL_RemoveTimer(timer);
      SDL_DestroyCond(c);
      SDL_DestroyMutex(dummy_mutex);
    }
	
  endroutine();

  return 0;
}


void enablerst::do_frame()
{
  double fps_per_gfps = ((double)g_qprate.QuadPart)/((double)qprate.QuadPart);

  // Clamp max number of outstanding frames/gframes to a sane number
  // Yes, this is a race condition, but the comparison and assignment are
  // individually atomic so worst-case it's clamped to 5 when it "should" be 6
  // or something. No problem there.
  if (gframes_outstanding > 5) gframes_outstanding = 5;
  if (frames_outstanding > fps_per_gfps*2+1) frames_outstanding = fps_per_gfps*2+1;
  //ENABLERFLAG_MAXFPS can lead to negative values, at least for frames_outstanding
  if (gframes_outstanding < -5) gframes_outstanding = -5;
  if (frames_outstanding < -20) frames_outstanding = -20;

  // Hack: Because various enabler values are set by the main loop, we can't
  // run render-setup before the main loop if we just zoomed or they have
  // otherwise become uninitialized.
  bool do_render=false;
  if (gframes_outstanding > 0 && (flag & ENABLERFLAG_RENDER)) {
    do_render = true;
    // Initiate graphics rendering, if appropriate
    if (!skip_gframe)
      render(window, setup);
    // Mark this rendering as complete. It isn't really, but we'll complete it later.
    flag &= ~ENABLERFLAG_RENDER;
  }
  
  // Run the main loop if appropriate
  if (frames_outstanding > 0||(flag & ENABLERFLAG_MAXFPS)) {
    frames_outstanding -= 1;
    if (mainloop())
      is_program_looping = FALSE;
  }

  // Finish rendering graphics, if appropriate
  if (do_render && !skip_gframe) {
    render(window, complete);
    current_render_count++;
    secondary_render_count++;
    gframes_outstanding--;
  }
  skip_gframe = false;

  // Quit. Toady: Any reason not to set loopvar directly?
  if (!is_program_looping)
    loopvar = 0;

  // Sleep if appropriate
//   printf("f: %g, g: %d\n", frames_outstanding, gframes_outstanding);
  if (gframes_outstanding <= 0 &&
      frames_outstanding <= 0) {
    // Sleep until the timer thread signals us
    if(!(flag & ENABLERFLAG_MAXFPS))SDL_CondWait(timer_cond, dummy_mutex);
  }
}

void enablerst::render(GL_Window &window, enum render_phase phase)
{
  render_things(phase);
  if (phase == complete)
    SDL_GL_SwapBuffers();
}

void enablerst::terminate_application(GL_Window* window)
{
  // TODO	-- NOTHING ACTUALLY USING THIS?
  is_program_looping = FALSE;					// Stop Looping Of The Program
}

void enablerst::toggle_fullscreen(GL_Window* window)
{
  enabler.create_full_screen = !enabler.create_full_screen;
	

  reset_gl(window);
  render(*window, setup);
  render(*window, complete);
  reset_window();
}

void enablerst::reset_gl(GL_Window* window) {
  textures.remove_uploaded_textures();

  
  destroy_window_GL(window);
  window->init.isFullScreen = create_full_screen;
  if (!create_window_GL(window))
    {
      MessageBox (NULL, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
      is_program_looping = FALSE;
      return;
    }

  textures.upload_textures();
  // ne_toggle_fullscreen();
}

void enablerst::reshape_GL(int width,int height)				// Reshape The Window When It's Moved Or Resized
{
  glDisable(GL_ALPHA_TEST); glDisable(GL_BLEND); glDisable(GL_DEPTH_TEST);
  glDisable(GL_DITHER); glDisable(GL_FOG); glDisable(GL_LIGHTING);
  glDisable(GL_LOGIC_OP); glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);glShadeModel(GL_FLAT);
  glDisable(GL_TEXTURE_2D); glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1); glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1); glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1); glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1); glPixelTransferi(GL_ALPHA_BIAS, 0);

  window_width=width;
  window_height=height;

  glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));		// Reset The Current Viewport
  glMatrixMode (GL_PROJECTION);								// Select The Projection Matrix
  glLoadIdentity ();											// Reset The Projection Matrix
  gluOrtho2D(0.0, width, height, 0.0);
  glMatrixMode (GL_MODELVIEW);								// Select The Modelview Matrix
  glLoadIdentity ();											// Reset The Modelview Matrix
}

char enablerst::change_screen_resolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
  // TODO
  return false;
}

// Reports an error to the user, using a MessageBox and stderr.
void report_error(const char *error_preface, const char *error_message)
{
  char *buf = NULL;
  // +4 = +colon +space +newline +nul
  buf = new char[strlen(error_preface) + strlen(error_message) + 4];
  sprintf(buf, "%s: %s\n", error_preface, error_message);
  MessageBox(NULL, buf, "SDL Error", MB_OK);
  fprintf(stderr, "%s", buf);
  delete [] buf;
}

// This function creates our OpenGL window.
char enablerst::create_window_GL(GL_Window* window)
{
  int retval = -1;
  Uint32 flags = 0;
  SDL_Surface *screen = NULL;
	
  // Set up SDL to give us an OpenGL context.
  flags |= SDL_HWSURFACE;
  flags |= SDL_OPENGL;
  // Find the current desktop resolution if fullscreen resolution is auto
  if (desired_fullscreen_width == 0 || desired_fullscreen_height == 0) {
    const struct SDL_VideoInfo *info = SDL_GetVideoInfo();
    desired_fullscreen_width = info->current_w;
    desired_fullscreen_height = info->current_h;
  }
  // Set it up for windowed or fullscreen, depending on what they asked for.
  if (window->init.isFullScreen) { 
    flags |= SDL_FULLSCREEN;
    window->init.width = desired_fullscreen_width;
    window->init.height = desired_fullscreen_height;
  } else {
    flags |= SDL_RESIZABLE;
    window->init.width = desired_windowed_width;
    window->init.height = desired_windowed_height;
  }
	
  // Disable key repeat and set window title.
  SDL_EnableKeyRepeat(0, 0);
  SDL_WM_SetCaption(window->init.title, NULL);

  SDL_Surface *icon = IMG_Load("icon.png");
  if (icon != NULL) {
    SDL_WM_SetIcon(icon, NULL);
    // The icon's surface doesn't get used passed this point.
    SDL_FreeSurface(icon); 
  }

  // Ask for an SDL_Surface (and so an OpenGL context)
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER))
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
 else
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
// Toady: Is this actually useful for anything? You could save some memory by uncommenting this line.
//   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  // WARNING: If you ask for acceleration, ATI cards will refuse to give you any
  // It's the default anyway, so it's not realy needed.
//   SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  screen = SDL_SetVideoMode(window->init.width, window->init.height, window->init.bitsPerPixel, flags);
  glewInit();
  // Make sure we actually got what we asked for
  int test=0;
  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &test);
  if (test != ((init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER)) ? 0 : 1))
    std::cerr << "SDL_GL_DOUBLEBUFFER failed\n";
//   SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &test);
//   // Report failure.
//   if (test != 1) {
//     report_error("Failed to switch to hardware-accelerated opengl mode", "");
//     destroy_window_GL(window);
//     return false;
//   }
  if (screen == NULL) {
    report_error("SDL initialization failure", SDL_GetError());
    destroy_window_GL(window);
    return false;
  }
	
  reshape_GL(window->init.width, window->init.height);
  glClear(GL_COLOR_BUFFER_BIT);
	
  // Initialize the gridrects
  for (int i=0; i<gridrect.size(); i++)
    gridrect[i]->init_gl();
  return true;
}

// Destroy The OpenGL Window & Release Resources
char enablerst::destroy_window_GL(GL_Window* window)
{
  // Uninitialize the gridrects
  for (int i=0; i<gridrect.size(); i++)
    gridrect[i]->uninit_gl();
  // There's no need to destroy the window; the call to SDL_SetVideoMode
  // will recreate it as needed.
  return true;
}

enablerst::enablerst()
{
  buffer_draw=0;
  center_x=0;center_y=0;

  tracking_on=0;
  oldmouse_x=-1;oldmouse_y=-1;mouse_x=-1;mouse_y=-1;
  mouse_lbut=0;mouse_rbut=0;
  mouse_lbut_lift=0;mouse_rbut_lift=0;

  set_color(1,1,1);

  desired_windowed_width=640;
  desired_windowed_height=300;
  desired_fullscreen_width=640;
  desired_fullscreen_height=300;

  ccolor[0][0]=0;
  ccolor[0][1]=0;
  ccolor[0][2]=0;
  ccolor[1][0]=0;
  ccolor[1][1]=0;
  ccolor[1][2]=0.5f;
  ccolor[2][0]=0;
  ccolor[2][1]=0.5f;
  ccolor[2][2]=0;
  ccolor[3][0]=0;
  ccolor[3][1]=0.5f;
  ccolor[3][2]=0.5f;
  ccolor[4][0]=0.5f;
  ccolor[4][1]=0;
  ccolor[4][2]=0;
  ccolor[5][0]=0.5f;
  ccolor[5][1]=0;
  ccolor[5][2]=0.5f;
  ccolor[6][0]=0.5f;
  ccolor[6][1]=0.5f;
  ccolor[6][2]=0;
  ccolor[7][0]=0.75f;
  ccolor[7][1]=0.75f;
  ccolor[7][2]=0.75f;
  ccolor[8][0]=0.5f;
  ccolor[8][1]=0.5f;
  ccolor[8][2]=0.5f;
  ccolor[9][0]=0;
  ccolor[9][1]=0;
  ccolor[9][2]=1.0f;
  ccolor[10][0]=0;
  ccolor[10][1]=1.0f;
  ccolor[10][2]=0;
  ccolor[11][0]=0;
  ccolor[11][1]=1.0f;
  ccolor[11][2]=1.0f;
  ccolor[12][0]=1.0f;
  ccolor[12][1]=0;
  ccolor[12][2]=0;
  ccolor[13][0]=1.0f;
  ccolor[13][1]=0;
  ccolor[13][2]=1.0f;
  ccolor[14][0]=1.0f;
  ccolor[14][1]=1.0f;
  ccolor[14][2]=0;
  ccolor[15][0]=1.0f;
  ccolor[15][1]=1.0f;
  ccolor[15][2]=1.0f;

  next_tile_slot=-1;

  next_gridrect_id=1;
  flag=0;
  QueryPerformanceFrequency(&qpfr);
  main_qprate.QuadPart = qpfr.QuadPart / 100;
  qprate = main_qprate;
  g_qprate = main_qprate;

  tile_vertices = tile_texcoords = NULL;
}


void gridrectst::render(enum render_phase phase, bool clear)
{
  if (!gl_initialized) {
    puts("render called without gl being initialized");
    return;
  }

#ifdef DEBUG
  static int frame = 0;
#endif
  switch (phase) {
  case setup:
    {

      if (accum_buffer) {
        // Copy the previous frame's buffer back in
        glAccum(GL_RETURN, 1);
      }
      else if (framebuffer) {
        // Setup a framebuffer for rendering
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
      }

      // Clear the screen as appropriate
      if (clear) {
        glViewport(0,0,enabler.window_width,enabler.window_height);
        glClear(GL_COLOR_BUFFER_BIT | (accum_buffer ? GL_ACCUM_BUFFER_BIT : 0));
      }
      
      // This code looks incredibly slow, but isn't normally used. It should be fine.
      // Toady: Is it still actually in use? Anywhere?
      if(trinum>0)
        {
          glEnable(GL_FOG);
          glFogf(GL_FOG_START,0);
          printGLError();
          glFogf(GL_FOG_END,1000);
          glFogf(GL_FOG_MODE,GL_LINEAR);
          float fcolor[4]={0,0,0,0};
          glFogfv(GL_FOG_COLOR,fcolor);

          glMatrixMode(GL_PROJECTION);
          glPushMatrix();
          glLoadIdentity();
          printGLError();
          gluPerspective(54.0f, enabler.window_width/enabler.window_height, 2.0f, 1000.0f);
          glMatrixMode(GL_MODELVIEW);
          glPushMatrix();
          gluLookAt(115+160.0f*(float)sin(view_angle),-115-160.0f*(float)cos(view_angle),view_z,115,-115,150,0,0,1);

          glEnable(GL_DITHER);
          glShadeModel(GL_SMOOTH);
          glDisable(GL_TEXTURE_2D);
          glEnable(GL_BLEND);
          glEnable(GL_CULL_FACE);
          glEnable(GL_DEPTH_TEST);
          printGLError();

          glBegin(GL_TRIANGLES);
          long t;
          for(t=0;t<trinum;t++)
            {
              glColor4fv(tricol[t]);
              glVertex3fv(tri[t]);
            }
          glEnd();
          printGLError();

          glMatrixMode(GL_PROJECTION);
          glPopMatrix();
          glMatrixMode(GL_MODELVIEW);
          glPopMatrix();
      
        }

      glDisable(GL_DITHER);
      printGLError();
      glShadeModel(GL_FLAT);
      printGLError();
      glDisable(GL_BLEND);
      printGLError();
      glDisable(GL_TEXTURE_2D);
      printGLError();
      glDisable(GL_CULL_FACE);
      printGLError();
      glDisable(GL_DEPTH_TEST);

      printGLError();

      // Build arrays. Vertex array is separated out, as it can be skipped in standard or VBO mode.
      long tex_pos;
      int edge_l=0, edge_r=1,edge_u,edge_d;
      long px,py;
      long d=0;
      GLfloat *ptr_bg_color_w  = &ptr_bg_color[0];
      GLfloat *ptr_fg_color_w = &ptr_fg_color[0];
      GLfloat *ptr_tex_w = &ptr_tex[0];
      const struct gl_texpos *txt = enabler.textures.gl_texpos;

      // Vertex array
      if (framebuffer || accum_buffer || !vertices_initialized || init.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT)) {
        vertices_initialized = true;
        tile_count = 0;
        GLfloat *ptr_vertex_w = &ptr_vertex[0];
        for(px=0;px<dimx;px++,edge_l++,edge_r++)
          {
            edge_u=0;
            edge_d=1;
            for(py=0;py<dimy;py++,d++,edge_u++,edge_d++)
              {
                tex_pos=buffer_texpos[d];
            
                if(tex_pos==-1) { // Check whether the tile is dirty
                  continue; // Not dirty
                } // Don't update dirty buffer here, as this code is not always executed
            
                // Set vertex locations
                *(ptr_vertex_w++) = edge_l; // Upper left
                *(ptr_vertex_w++) = edge_u;
                *(ptr_vertex_w++) = edge_r; // Upper right
                *(ptr_vertex_w++) = edge_u;
                *(ptr_vertex_w++) = edge_r; // Lower right
                *(ptr_vertex_w++) = edge_d;
            
                *(ptr_vertex_w++) = edge_l; // Upper left
                *(ptr_vertex_w++) = edge_u;
                *(ptr_vertex_w++) = edge_r; // Lower right
                *(ptr_vertex_w++) = edge_d;
                *(ptr_vertex_w++) = edge_l; // Lower left
                *(ptr_vertex_w++) = edge_d;

                // One tile down
                ++tile_count;
              }
          }
        // Upload vertex array. This rarely happens.
        if (vbo_refs[0]) {
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[0]); // Vertices
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, tile_count*sizeof(GLfloat)*6*2, &ptr_vertex[0], GL_STATIC_DRAW_ARB);
        }
        
        // Reset variables
        edge_l=0; edge_r=1;
        d=0;
      }

      // Map VBOs for writing
      if (vbo_refs[0]) {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[1]);
        ptr_bg_color_w = (GLfloat*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        assert(ptr_bg_color_w);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[2]);
        ptr_fg_color_w = (GLfloat*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        assert(ptr_fg_color_w);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[3]);
        ptr_tex_w = (GLfloat*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        assert(ptr_tex_w);
      }

      // FG, BG and tex-coord arrays
      for(px=0;px<dimx;px++,edge_l++,edge_r++)
        {
          edge_u=0;
          edge_d=1;
          for(py=0;py<dimy;py++,d++,edge_u++,edge_d++)
            {
              if(trinum>0)
                {
                  if(py>=1&&py<=init.display.grid_y-2&&px>=init.display.grid_x-55&&px<=init.display.grid_x-26)continue;
                }

              tex_pos=buffer_texpos[d];
          
              if(tex_pos==-1) { // Check whether the tile is dirty
                continue; // Not dirty
              } else {
                // Dirty. Update dirty buffer.
                s_buffer_texpos[d]=buffer_texpos[d];
                s_buffer_r[d]=buffer_r[d];
                s_buffer_g[d]=buffer_g[d];
                s_buffer_b[d]=buffer_b[d];
                s_buffer_br[d]=buffer_br[d];
                s_buffer_bg[d]=buffer_bg[d];
                s_buffer_bb[d]=buffer_bb[d];
              }

              // Background colors
              for (int i=0; i<6; i++) {
                // We need one color component for each vertex, even if
                // only one is actually used with flat shading.
                //
                // Toady: I'm sure you can think of useful ways to use smooth shading..
                *(ptr_bg_color_w++) = buffer_br[d];
                *(ptr_bg_color_w++) = buffer_bg[d];
                *(ptr_bg_color_w++) = buffer_bb[d];
                *(ptr_bg_color_w++) = 1.0; // Alpha, but basically padding.
              }
              // Foreground colors
              for (int i=0; i<6; i++) {
                // Same story as for the background.
                *(ptr_fg_color_w++) = buffer_r[d];
                *(ptr_fg_color_w++) = buffer_g[d];
                *(ptr_fg_color_w++) = buffer_b[d];
                *(ptr_fg_color_w++) = 1.0; // Padding. Or alpha. You decide.
              }

              if (tex_pos<0||tex_pos>=enabler.textures.textureCount())
                {
                  //         std::cerr << "Assumptions broken!\n";
                  *(ptr_tex_w++) = 0; // Upper left
                  *(ptr_tex_w++) = 0;
                  *(ptr_tex_w++) = 1; // Upper right
                  *(ptr_tex_w++) = 0;
                  *(ptr_tex_w++) = 1; // Lower right
                  *(ptr_tex_w++) = 1;

                  *(ptr_tex_w++) = 0; // Upper left
                  *(ptr_tex_w++) = 0;
                  *(ptr_tex_w++) = 1; // Lower right
                  *(ptr_tex_w++) = 1;
                  *(ptr_tex_w++) = 0; // Lower left
                  *(ptr_tex_w++) = 1;
                }
              else
                {
                  // Textures
                  *(ptr_tex_w++) = txt[tex_pos].left;   // Upper left
                  *(ptr_tex_w++) = txt[tex_pos].bottom;
                  *(ptr_tex_w++) = txt[tex_pos].right;  // Upper right
                  *(ptr_tex_w++) = txt[tex_pos].bottom;
                  *(ptr_tex_w++) = txt[tex_pos].right;  // Lower right
                  *(ptr_tex_w++) = txt[tex_pos].top;

                  *(ptr_tex_w++) = txt[tex_pos].left;   // Upper left
                  *(ptr_tex_w++) = txt[tex_pos].bottom;
                  *(ptr_tex_w++) = txt[tex_pos].right;  // Lower right
                  *(ptr_tex_w++) = txt[tex_pos].top;
                  *(ptr_tex_w++) = txt[tex_pos].left;   // Lower left
                  *(ptr_tex_w++) = txt[tex_pos].top;
                }
            }
        }

      printGLError();

      // Upload VBO data to the gpu, if appropriate
      if (vbo_refs[0]) {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[1]); // background color
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[2]); // foreground color
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[3]); // texture coordinates
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
      }
      printGLError();

      // Unbind framebuffer
      if (framebuffer)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    } // case setup
    break;
  case complete:
    {
      // Render to framebuffer
      if (framebuffer) {
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
      }
      
      // Setup view matrix
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      
      // Total size of window, ignoring zoom entirely. Only used to calculate other values.
      const float totalsizex=dispx*dimx; // dispx is tile width in pixels, dimx is grid size in tiles
      const float totalsizey=dispy*dimy;
      // Setup a nicer coordinate system
      glTranslatef(-1,1,0);
      glScalef(2,-2,2);
      // Scale the grid so it fits exactly into the GL viewport, making 1 GL unit equal 1 tile
      glScalef(1.0f / dimx, 1.0f / dimy, 1);
      // Figure out what part of the window we actually use for said viewport, to get the aspect ratio and zoom right
      // We always use the entire window is black_space is off.
      origin_x = 0; origin_y = 0; size_x = enabler.window_width; size_y = enabler.window_height;
      if (black_space) { // Otherwise...
        if (totalsizex > enabler.window_width || totalsizey > enabler.window_height) {
          // Insufficient space; we must compress the grid while maintaining aspect ratio.
          const double aspect = totalsizex / totalsizey;
          // Figure out which axis is most compressed, use the entire window in that axis and
          // calculate the other from that.
          const double compressx = totalsizex / enabler.window_width,
            compressy = totalsizey / enabler.window_height;
          if (compressx > compressy) {
            size_y = size_y / (compressx / compressy);
            origin_y = (enabler.window_height - size_y) / 2;
          } else {
            size_x = size_x / (compressy / compressx);
            origin_x = (enabler.window_width - size_x) / 2;
          }
        } else {
          // Sufficient space, so we just stick ourselves in the middle.
          size_x = totalsizex; size_y = totalsizey;
          origin_x = (enabler.window_width - size_x) / 2;
          origin_y = (enabler.window_height - size_y) / 2;
        }
      }
      glViewport(origin_x, origin_y, size_x, size_y);

      
      // If viewport scaling is on.. we viewport-scale.
      if (!zoom_grid) {
        double visible = 1 / viewport_zoom;
        double invisible = 1 - visible;
        double left = invisible * viewport_x,
          top = invisible * viewport_y;
        glScalef(viewport_zoom,viewport_zoom,1);
        glTranslatef(-left*dimx,-top*dimy,0);
      }
      printGLError();

      // Bind the appropriate buffers
      if (vbo_refs[0]) {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[0]);
        glVertexPointer(2, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[1]);
        glColorPointer(4, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[3]);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
      } else {
        glVertexPointer(2, GL_FLOAT, 0, &ptr_vertex[0]);
        glColorPointer(4, GL_FLOAT, 0, &ptr_bg_color[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, &ptr_tex[0]);
      }
        
      
      // Draw the background colors
      glDisable(GL_ALPHA_TEST);
      printGLError();
      glDisable(GL_TEXTURE_2D);
      printGLError();
      glEnableClientState(GL_COLOR_ARRAY);
      printGLError();
      glEnableClientState(GL_VERTEX_ARRAY);
      printGLError();
      // printf("%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
      glDrawArrays(GL_TRIANGLES, 0, tile_count*6);
      printGLError();

      printGLError();

      // Switch out the color pointer
      if (vbo_refs[0]) {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[2]);
        glColorPointer(4, GL_FLOAT, 0, 0);
      } else {
        glColorPointer(4, GL_FLOAT, 0, &ptr_fg_color[0]);
      }
      printGLError();

      // Draw the foreground, textures and color both
      glEnable(GL_ALPHA_TEST);
      printGLError();
      glAlphaFunc(GL_NOTEQUAL, 0);
      printGLError();
      glEnable(GL_TEXTURE_2D);
      printGLError();
      glBindTexture(GL_TEXTURE_2D, enabler.textures.gl_catalog);
      printGLError();
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      printGLError();
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDrawArrays(GL_TRIANGLES, 0, tile_count*6);
      printGLError();

      // Clean up
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      printGLError();
      glDisableClientState(GL_COLOR_ARRAY);
      printGLError();

      glDisable(GL_BLEND);

      glDisable(GL_ALPHA_TEST);
      printGLError();

//       glMatrixMode(GL_MODELVIEW);
//       printGLError();
//       glPopMatrix();
//       printGLError();
#ifdef DEBUG
      if (frame < 5) {
        frame++;
        std::cout << "Frame " << frame << " complete\n";
      }
#endif

      if (accum_buffer) {
        // If we use the accumulation buffer, store the screen contents back to the buffer
        glAccum(GL_LOAD, 1);
      }
      // If we used a framebuffer, render that FBO to the screen
      else if (framebuffer) {
        // Render to screen
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
        glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer);
        // Blit framebuffer
        glBlitFramebufferEXT(0,0,enabler.window_width,enabler.window_height,
                             0,0,enabler.window_width,enabler.window_height,
                             GL_COLOR_BUFFER_BIT, GL_LINEAR);
      }
    } // case complete
    break;
  }
}

void gridrectst::init_gl() {
  static bool shown=false; // It's a bit hacky, but we want to only show the message once
  if (gl_initialized) uninit_gl();
  vertices_initialized = false;
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_VBO) && GLEW_ARB_vertex_buffer_object) {
#ifndef DEBUG
    if (!shown)
#endif
      std::cout << "Using OpenGL output path with buffer objects\n";
    // Allocate memory for the server-side arrays, and test mapping to avoid crashes
    bool ok = true;
    glGenBuffersARB(4, vbo_refs);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, dimx*dimy*6*2*sizeof(GLfloat), NULL, GL_STATIC_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[1]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, dimx*dimy*6*4*sizeof(GLfloat), NULL, GL_STREAM_DRAW_ARB);
    if (!glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB)) ok = false;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[2]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, dimx*dimy*6*4*sizeof(GLfloat), NULL, GL_STREAM_DRAW_ARB);
    if (!glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB)) ok = false;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[3]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, dimx*dimy*6*2*sizeof(GLfloat), NULL, GL_STREAM_DRAW_ARB);
    if (!glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB)) ok = false;
    // Unmap
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[1]);
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[2]);
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_refs[3]);
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    if (!ok) {
      puts("Mapping VBOs failed, falling back to standard mode with client-side arrays");
      glDeleteBuffersARB(4, vbo_refs);
      vbo_refs[0]=0;
    }
  } else {
#ifndef DEBUG
    if (!shown)
#endif
      std::cout << "Using OpenGL output path with client-side arrays";
  }

  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER) && GLEW_EXT_framebuffer_object) {
#ifndef DEBUG
    if (!shown)
#endif
      std::cout << " and off-screen framebuffer\n";
    glGenFramebuffersEXT(1, &framebuffer);
    // Allocate FBO texture memory
    glGenTextures(1, &fb_texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 enabler.window_width, enabler.window_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GLint param = (init.window.flag.has_flag(INIT_WINDOW_FLAG_TEXTURE_LINEAR) ?
                   GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,param);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,param);
    
    // Bind texture to FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, fb_texture, 0);
    // Create and attach a depth buffer
    // glGenRenderbuffersEXT(1, &fb_depth);
    // glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fb_depth);
    // glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, enabler.window_width, enabler.window_height);
    // glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fb_depth);
    if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT) {
      framebuffer_initialized = true;
    } else {
      // glDeleteRenderbuffersEXT(1, &fb_depth);
      glDeleteTextures(1, &fb_texture);
      glDeleteFramebuffersEXT(1, &framebuffer);
      framebuffer = 0; // Disable framebuffer
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#ifndef DEBUG
      if (!shown)
        std::cout << "Error: Invalid framebuffer configuration, FBO disabled; using standard mode. No action required.\n";
#endif
    }
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    framebuffer_initialized = true;
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER)) {
#ifndef DEBUG
    if (!shown)
#endif
      std::cout << " and accumulation buffer\n";
    accum_buffer=true;
  } else {
#ifndef DEBUG
    if (!shown)
#endif
      std::cout << "\n";
    framebuffer = 0;
  }
    
  if (accum_buffer)
    glClearAccum(0,0,0,0);
  // Allocate memory for the client-side arrays
#ifdef DEBUG
  printf("Room for %d vertices allocated\n", dimx*dimy*6);
#endif
  ptr_vertex.resize(dimx*dimy*6*2);   // dimx*dimy tiles,
  if (!vbo_refs[0]) { // We don't need these in VBO mode
    ptr_fg_color.resize(dimx*dimy*6*4); // six vertices,
    ptr_bg_color.resize(dimx*dimy*6*4+8); // two vertex components or
    ptr_tex.resize(dimx*dimy*6*2+4);      // four colors per vertex
  }
  shown = true;
  gl_initialized = true;
}

void gridrectst::uninit_gl() {
  if (!gl_initialized) return;
  if (vbo_refs[0]) {
    glDeleteBuffersARB(4, vbo_refs);
    vbo_refs[0] = 0;
  }
  if (framebuffer) {
    glDeleteLists(1, fb_draw_list);
    glDeleteRenderbuffersEXT(1, &fb_depth);
    glDeleteTextures(1, &fb_texture);
    glDeleteFramebuffersEXT(1, &framebuffer);
    framebuffer = 0;
    framebuffer_initialized = false;
  }
  gl_initialized = false;
  accum_buffer = false;
}

void convert_to_rgb(float &r,float &g,float &b,char col,char bright)
{
  short ci=col+(bright!=0)*8;
  if(ci>=0&&ci<16)
    {
      r=enabler.ccolor[ci][0];
      g=enabler.ccolor[ci][1];
      b=enabler.ccolor[ci][2];
    }
}

gridrectst *gridrectst::create(long dimx,long dimy)
{
  gridrectst *newrect=new gridrectst(dimx,dimy);
  return newrect;
}

gridrectst::~gridrectst()
{
  clean();
}

gridrectst::gridrectst(long newdimx,long newdimy)
{
  gl_initialized = false;
  vbo_refs[0] = 0;
  accum_buffer = false;
  framebuffer_initialized = false;
  framebuffer = 0;
  vertices_initialized = false;

  allocate(newdimx,newdimy);
  trinum=0;
}

void gridrectst::allocate(long newdimx,long newdimy)
{
  if(buffer_texpos.size() == 0 || dimx!=newdimx || dimy!=newdimy)
    {
      dimx=newdimx;
      dimy=newdimy;
      buffer_texpos.resize(dimx*dimy);
      buffer_r.resize(dimx*dimy);
      buffer_g.resize(dimx*dimy);
      buffer_b.resize(dimx*dimy);
      buffer_br.resize(dimx*dimy);
      buffer_bg.resize(dimx*dimy);
      buffer_bb.resize(dimx*dimy);
      s_buffer_texpos.resize(dimx*dimy);
      s_buffer_r.resize(dimx*dimy);
      s_buffer_g.resize(dimx*dimy);
      s_buffer_b.resize(dimx*dimy);
      s_buffer_br.resize(dimx*dimy);
      s_buffer_bg.resize(dimx*dimy);
      s_buffer_bb.resize(dimx*dimy);
      s_buffer_count.resize(dimx*dimy);
      std::fill(s_buffer_texpos.begin(), s_buffer_texpos.end(), -1);
      std::fill(s_buffer_count.begin(), s_buffer_count.end(), 0);
    }
  // Make sure to reallocate GL buffers if it's running
  if (gl_initialized)
    init_gl();
}

void gridrectst::clean()
{
  allocate(0,0);
}

long enablerst::gridrect_create(long dimx,long dimy)
{
  gridrectst *rect=gridrectst::create(dimx,dimy);
  rect->id=next_gridrect_id;next_gridrect_id++;
  gridrect.push_back(rect);

  return rect->id;
}

gridrectst *enablerst::get_gridrect(long rect_id)
{
  int r;
  for(r=0;r<gridrect.size();r++)
    {
      if(gridrect[r]->id==rect_id)return gridrect[r];
    }

  return NULL;
}

void enablerst::set_color(float r,float g,float b,float a)
{
  if(fade_t==0)
    {
      color_r=r;color_g=g;color_b=b;color_a=a;
    }
  else
    {
      color_r=r*(1.0f-fade_t)+fade_r*fade_t;
      color_g=g*(1.0f-fade_t)+fade_g*fade_t;
      color_b=b*(1.0f-fade_t)+fade_b*fade_t;
      color_a=a*(1.0f-fade_t)+fade_a*fade_t;
    }
}

void enablerst::render_tiles(enum render_phase phase, bool clear)
{
  //SET UP THE VIEW
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-center_x,-center_y,0);

  // Draw gridrects
  // Toady: gridrect.size always seems to be 1. Is there any use in it being an array?
  for(int r=0;r<gridrect.size();r++)
    {
      assert(r < 2);
      gridrect[r]->render(phase, clear);
    }
  // Draw set_tile tiles
  if (phase == complete) {
    glColor3f(1,1,1);
    glEnable(GL_ALPHA_TEST);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, tile_vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, tile_texcoords);
    glDrawArrays(GL_QUADS, 0, tiles.size() * 4);
  }
}

// Toady: Yep, this had a lot more parameters as add_tile. You weren't using them.
void enablerst::set_tile(long tex, int id, int x, int y) {
  if (tex < 0 || textures.get_texture_data(tex) == NULL) {
    tiles.erase(id);
    return;
  }
//   std::cout << tex << " " << id << "\n";
  struct tile new_data;
  new_data.x = x;
  new_data.y = y;
  new_data.tex = tex;
  tiles[id] = new_data;
  // (Re)generate vertex and texture arrays for all tiles
  // FIXME: This is overly expensive. Or it would be, if there was more than one tile
  // at a time. You get the idea, though; just try not to regenerate /everything/
  // /every/ time.
  if (tile_vertices) delete[] tile_vertices;
  if (tile_texcoords) delete[] tile_texcoords;
  tile_vertices = new GLfloat[8*tiles.size()];
  tile_texcoords = new GLfloat[8*tiles.size()];
  GLfloat *tv = tile_vertices, *tt = tile_texcoords;
  for (int i=0; i < tiles.size(); i++) {
    struct tile t = tiles[i];
    SDL_Surface *s = textures.get_texture_data(t.tex);
    *(tv++) = t.x; // NW
    *(tv++) = t.y;
    *(tv++) = t.x + s->w; // NE
    *(tv++) = t.y;
    *(tv++) = t.x + s->w; // SE
    *(tv++) = t.y + s->h;
    *(tv++) = t.x; // SW
    *(tv++) = t.y + s->h;
    *(tt++) = textures.gl_texpos[t.tex].left; // NW
    *(tt++) = textures.gl_texpos[t.tex].top;
    *(tt++) = textures.gl_texpos[t.tex].right; // NE
    *(tt++) = textures.gl_texpos[t.tex].top;
    *(tt++) = textures.gl_texpos[t.tex].right; // SE
    *(tt++) = textures.gl_texpos[t.tex].bottom;
    *(tt++) = textures.gl_texpos[t.tex].left; // SW
    *(tt++) = textures.gl_texpos[t.tex].bottom;
  }
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

void enablerst::enable_fade(float r,float g,float b,float a,float t)
{
  fade_r=r;fade_g=g;fade_b=b;fade_a=a;fade_t=t;
}

void enablerst::disable_fade()
{
  fade_t=0;
}

void enablerst::read_pixels(int x,int y,int width,int height,unsigned char *buffer)
{
  glReadBuffer(GL_BACK);
  glReadPixels(x,y,width,height,GL_RGBA,GL_UNSIGNED_BYTE,buffer);
}

void enablerst::flip_uchar_array(unsigned char *buff,long dimx,long dimy,long bytes_per_pixel,unsigned long flag)
{
  unsigned char swap;
  long x,y,ind,incy,ind2,dimmult;

  if(flag & COPYTEXTUREFLAG_VERFLIP)
    {
      ind=0;
      incy=dimx*bytes_per_pixel;
      dimmult=(dimy-1)*incy;
      for(x=0;x<dimx;x++)
	{
	  ind2=0;
	  for(y=0;y<dimy>>1;y++)
	    {
	      swap=buff[ind+ind2];
	      buff[ind+ind2]=buff[dimmult-ind2+ind];
	      buff[dimmult-ind2+ind]=swap;

	      if(bytes_per_pixel>1)
		{
		  swap=buff[ind+1+ind2];
		  buff[ind+1+ind2]=buff[dimmult-ind2+ind+1];
		  buff[dimmult-ind2+ind+1]=swap;
		}
	      if(bytes_per_pixel>2)
		{
		  swap=buff[ind+2+ind2];
		  buff[ind+2+ind2]=buff[dimmult-ind2+ind+2];
		  buff[dimmult-ind2+ind+2]=swap;
		}
	      if(bytes_per_pixel>3)
		{
		  swap=buff[ind+3+ind2];
		  buff[ind+3+ind2]=buff[dimmult-ind2+ind+3];
		  buff[dimmult-ind2+ind+3]=swap;
		}
	      ind2+=incy;
	    }
	  ind+=bytes_per_pixel;
	}
    }
  if(flag & COPYTEXTUREFLAG_HORFLIP)
    {
      ind=0;
      incy=dimx*bytes_per_pixel;
      dimmult=(dimx-1)*bytes_per_pixel;
      for(y=0;y<dimy;y++)
	{
	  ind2=0;
	  for(x=0;x<dimx>>1;x++)
	    {
	      swap=buff[ind+ind2];
	      buff[ind+ind2]=buff[ind+dimmult-ind2];
	      buff[ind+dimmult-ind2]=swap;

	      if(bytes_per_pixel>1)
		{
		  swap=buff[ind+ind2+1];
		  buff[ind+ind2+1]=buff[ind+dimmult-ind2+1];
		  buff[ind+dimmult-ind2+1]=swap;
		}
	      if(bytes_per_pixel>2)
		{
		  swap=buff[ind+ind2+2];
		  buff[ind+ind2+2]=buff[ind+dimmult-ind2+2];
		  buff[ind+dimmult-ind2+2]=swap;
		}
	      if(bytes_per_pixel>3)
		{
		  swap=buff[ind+ind2+3];
		  buff[ind+ind2+3]=buff[ind+dimmult-ind2+3];
		  buff[ind+dimmult-ind2+3]=swap;
		}
	      ind2+=bytes_per_pixel;
	    }
	  ind+=incy;
	}
    }
}

void text_system_file_infost::initialize_info()
{
  std::ifstream fseed(filename.c_str());
  if(fseed.is_open())
    {
      string str;

      while(std::getline(fseed,str))
	{
	  if(str.length()>0)number++;
	}
    }
  else
    {
      string str;
      str="Error Initializing Text: ";
      str+=filename;
      errorlog_string(str);
    }
  fseed.close();
}

void text_system_file_infost::get_text(text_infost &text)
{
  text.clean();

  if(number==0)return;

  std::ifstream fseed(filename.c_str());
  if(fseed.is_open())
    {
      string str;

      int num=trandom(number);

      //SKIP AHEAD TO THE RIGHT SPOT
      while(num>0)
	{
	  std::getline(fseed,str);
	  num--;
	}

      //PROCESS THE STRING INTO TEXT ELEMENTS
      if(std::getline(fseed,str))
	{
	  int curpos;
	  string nextstr;
	  char doing_long=0;

	  text_info_elementst *newel;
	  long end=str.length();
			
	  while(end>0)
	    {
	      if(isspace(str[end-1]))end--;
	      else break;
	    }
			
	  str.resize(end);

	  for(curpos=0;curpos<end;curpos++)
	    {
	      //HANDLE TOKEN OR ENDING
	      //TWO FILE TOKENS IN A ROW MEANS LONG
	      //ONE MEANS STRING
	      if(str[curpos]==file_token || curpos==end-1)
		{
		  if(str[curpos]!=file_token)nextstr+=str[curpos];

		  //HAVE SOMETHING == SAVE IT
		  if(!nextstr.empty())
		    {
		      if(doing_long)
			{
			  newel=new text_info_element_longst(atoi(nextstr.c_str()));
			  text.element.push_back(newel);
			  doing_long=0;
			}
		      else
			{
			  newel=new text_info_element_stringst(nextstr);
			  text.element.push_back(newel);
			}

		      nextstr.erase();
		    }
		  //STARTING A LONG
		  else
		    {
		      doing_long=1;
		    }
		}
	      //JUST ADD IN ANYTHING ELSE
	      else
		{
		  nextstr+=str[curpos];
		}
	    }
	}
    }
  fseed.close();
}

// void enablerst::antialias(unsigned char *dat,long srcx,long srcy,char border)
// {
//   long pos=3,x,y;
//   for(y=0;y<srcy;y++)
//     {
//       for(x=0;x<srcx;x++,pos+=4)
// 	{
// 	  if(dat[pos]!=0)
// 	    {
// 	      if(x>0&&y>0&&x<srcx-1&&y<srcy-1)
// 		{
// 		  if(dat[pos-4]==0||
// 		     dat[pos+4]==0||
// 		     dat[pos-4*srcx]==0||
// 		     dat[pos+4*srcx]==0)
// 		    {
// 		      dat[pos]=128;
// 		    }
// 		}
// 	      else if(border)dat[pos]=128;
// 	    }
// 	}
//     }
// }

int main (int argc, char* argv[])
{
#if !defined(__APPLE__) && defined(unix)
  gtk_init(&argc, &argv);
#endif

#ifdef linux
  // Initialize OpenAL
  if (!musicsound.initsound())
    puts("Initializing OpenAL failed, no sound will be played");
#endif
  
  // Initialise relevant SDL subsystems.
  int retval = SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
  // Turn on keyboard translation, from raw keycodes to letters
  //SDL_EnableUNICODE(1);
  // Report failure.
  if (retval != 0) {
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }

  string cmdLine;
  for (int i = 1; i < argc; ++i) { 
    char *option = argv[i];
    cmdLine += option;
    cmdLine += " ";
  }
  enabler.command_line = cmdLine;
	
  int result = enabler.loop();

  SDL_Quit();
  
  return result;
}

// -------------------------

#ifndef WIN32
BOOL CreateDirectory(const char* pathname, void*)
{
  if (mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
    if (errno != EEXIST) {
      std::string emsg = "mkdir(";
      emsg.append(pathname);
      emsg.append(") failed");
      perror(emsg.c_str());
    }
    return FALSE;
  } else {
    return TRUE;
  }
}

BOOL DeleteFile(const char* filename)
{
  return !unlink(filename);
}

void ZeroMemory(void* dest, int len)
{
  memset(dest, 0, len);
}

/* Returns milliseconds since 1970
 * Wraps every 24 days (assuming 32-bit signed dwords)
 */
DWORD GetTickCount()
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

// Fills performanceCount with microseconds passed since 1970
// Wraps in twenty-nine thousand years or so
BOOL QueryPerformanceCounter(LARGE_INTEGER* performanceCount)
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  performanceCount->QuadPart = ((long long)tp.tv_sec * 1000000) + tp.tv_usec;
  return TRUE;
}

BOOL QueryPerformanceFrequency(LARGE_INTEGER* performanceCount)
{
  /* A constant, 10^6, as we give microseconds since 1970 in
   * QueryPerformanceCounter. */
  performanceCount->QuadPart = 1000000;
  
  return TRUE;
}
#endif

// http://msdn.microsoft.com/en-us/library/ms646301.aspx 
/*
SHORT Enabler_GetKeyState(int virtKey)
{
  Uint8 *keystate = SDL_GetKeyState(NULL);
  SDLMod keymod = SDL_GetModState();
	
  switch (virtKey) {
  case VK_LSHIFT:
  case VK_RSHIFT:
  case VK_SHIFT:
    if (keymod & KMOD_SHIFT) {
      return (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) ? 0x8001 : 0x0001;
    } else {
      return (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) ? 0x8000 : 0x0000;
    }
    break;
  case VK_LCONTROL:
  case VK_RCONTROL:
  case VK_CONTROL:
    if (keymod & KMOD_CTRL) {
      return (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) ? 0x8001 : 0x0001;
    } else {
      return (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) ? 0x8000 : 0x0000;
    }
    break;
  case VK_CAPITAL:
    if (keymod & KMOD_CAPS) {
      return (keystate[SDLK_CAPSLOCK]) ? 0x8001 : 0x0001;
    } else {
      return (keystate[SDLK_CAPSLOCK]) ? 0x8000 : 0x0000;
    }
    break;
  case VK_LMENU:
  case VK_RMENU:
  case VK_MENU:
    if (keymod & KMOD_ALT) {
      return (keystate[SDLK_LALT] || keystate[SDLK_RALT]) ? 0x8001 : 0x0001;
    } else {
      return (keystate[SDLK_LALT] || keystate[SDLK_RALT]) ? 0x8000 : 0x0000;
    }
    break;
  case VK_NUMLOCK:
    if (keymod & KMOD_NUM) {
      return (keystate[SDLK_NUMLOCK]) ? 0x8001 : 0x0001;
    } else {
      return (keystate[SDLK_NUMLOCK]) ? 0x8000 : 0x0000;
    }
    break;
  default:
    if (virtKey <= SDLK_FIRST || virtKey >= SDLK_LAST) {
      return 0x0000;
    }
    if (keystate[virtKey]) {
      return 0x8000;
    }
    else return 0x0000;
  }
}

int Enabler_ShowCursor(BOOL show)
{
  static int displayCount = 0;
	
  if(show) {
    displayCount += 1;
    if (displayCount == 0) {
      SDL_ShowCursor(SDL_ENABLE);
    }
  } else {
    if (displayCount == 0) {
      SDL_ShowCursor(SDL_DISABLE);
    }
    displayCount -= 1;
  }
	
  return displayCount;
}
*/

char* itoa(int value, char* result, int base)
{
  // check that the base is valid
  if (base < 2 || base > 16) { *result = 0; return result; }
	
  char* out = result;
  int quot = value;
	
  do
    {
      *out = "0123456789abcdef"[ /*std::*/abs(quot % base) ];
      ++out;
      quot /= base;
    }
  while (quot);
	
  if (value < 0) *out++ = '-';
	
  std::reverse(result, out);
  *out = 0;
  return result;
}

#ifndef WIN32
int MessageBox(HWND *dummy, const char *text, const char *caption, UINT type)
{
  bool toggle_screen = false;
  int ret = IDOK;
  if (enabler.create_full_screen) {
    enabler.toggle_fullscreen();
    toggle_screen = true;
  }
# ifdef __APPLE__ // Cocoa code
  if (type & MB_OK) {
    CocoaAlertPanel(caption, text, "OK", NULL, NULL);
  } else if (type & MB_YESNO) {
    ret = CocoaAlertPanel(caption, text, "Yes", "No", NULL);
    ret = (ret == 0 ? IDNO : IDYES);
  }
# else // GTK code
  GtkWidget *dialog = gtk_message_dialog_new(NULL,
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     type & MB_YESNO ?
					     GTK_MESSAGE_QUESTION :
					     GTK_MESSAGE_ERROR,
					     type & MB_YESNO ?
					     GTK_BUTTONS_YES_NO :
					     GTK_BUTTONS_OK,
					     "%s", text);
  gtk_window_set_position((GtkWindow*)dialog, GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_title((GtkWindow*)dialog, caption);
  gint dialog_ret = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  while (gtk_events_pending())
    gtk_main_iteration();
				       
  if (type & MB_YESNO) {
    switch (dialog_ret) {
    default:
    case GTK_RESPONSE_DELETE_EVENT:
    case GTK_RESPONSE_NO:
      ret = IDNO;
      break;
    case GTK_RESPONSE_YES:
      ret = IDYES;
      break;
    }
  }
# endif
	
  if (toggle_screen) {
    enabler.toggle_fullscreen();
  }
	
  return ret;
}
#endif

// Check whether a particular texture can be sized to some size,
// assuming in RGBA 32-bit format
bool testTextureSize(GLuint texnum, int w, int h) {
  GLint gpu_width;
  glBindTexture(GL_TEXTURE_2D, texnum);
      printGLError();
  glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      printGLError();
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &gpu_width);
      printGLError();

  if (gpu_width == w) return true;
  return false;
}

// Used to sort textures
struct vsize_pos {
  int h, w;
  SDL_Surface *s;
  long pos;
  // Assigned texture-catalog coordinates
  int x, y;

  bool operator< (struct vsize_pos y) const {
    // sort produces an ascending order. We want descending. Don't argue.
    if (h > y.h) return true;
    return false;
  }
};

// Texture catalog implementation
void textures::upload_textures() {
  if (uploaded) return; // Don't bother
  glEnable(GL_TEXTURE_2D);
  printGLError();
  glGenTextures(1, &gl_catalog);
  printGLError();

  // First, sort the textures by vertical size. We'Ll want to place the large
  // ones first.
  // Since we mustn't alter the raws array, first thing is to create a new one.
  // We pretend textures are one pixel larger than they actually are in either
  // direction, to avoid border scuffles when interpolating.
  std::vector<vsize_pos> ordered;
  long pos = 0;
  for (std::vector<SDL_Surface *>::iterator it = raws.begin();
       it != raws.end(); ++it) {
    if (*it) {
      vsize_pos item;
      item.h = (*it)->h+2;
      item.w = (*it)->w+2;
      item.s = *it;
      item.pos = pos;
      ordered.push_back(item);
    }
    pos++;
  }
  sort(ordered.begin(), ordered.end());

  /* Tiling algorithm:
  **
  ** Given a particular texture width, we pack tiles from largest to smallest
  ** by reserving rows for tiles with a particular height or lower.
  ** This does lead to space wastage when a row has, say, one 32x32 tile and
  ** fifteen 16x16 tiles, but generally not very much.
  **
  ** Possible improvement: Allow for multiple rows of smaller tiles inside
  ** a row that's at least twice as high as the smaller tiles are.
   */

  // Set the initial width to the minimum possible
  int catalog_width = 0;
  for (int i=0; i < ordered.size(); i++)
    if (catalog_width < ordered[i].w) catalog_width = ordered[i].w;
  const int width_increment = 4; // For speed, not that it matters.
  int catalog_height;
  // Figure out what the optimal texture width is
  // This may not be actually be an approximately square texture, but for the
  // moment that's what we're aiming for. On GPUs without the NPOT extension,
  // rectangular textures may actulaly use less video memory.
  // However, a square one is less likely to run into dimensional limits.
  for(;;) {
    int catalog_x = 0;
    int catalog_y = 0;
    int row_height = ordered[0].h;
    catalog_height = row_height;
    for (int pos = 0; pos < ordered.size(); pos++) {
      // Check whether we must start a new row
      if (catalog_x + ordered[pos].w > catalog_width) {
	catalog_x = 0;
	catalog_y = catalog_height;
	row_height = ordered[pos].h;
	catalog_height += row_height;
      }
      // Tentatively install tile at catalog_x, catalog_y
      ordered[pos].x = catalog_x;
      ordered[pos].y = catalog_y;
      // Goto next tile
      catalog_x += ordered[pos].w;
    }
    // If we didn't just cross "square", increment width and try again.
    if (catalog_height > catalog_width)
      catalog_width += width_increment;
    else
      break; // Otherwise we're done.
   }

#ifdef DEBUG
  std::cout << "Ideal catalog size: " << catalog_width << "x" << catalog_height << "\n";
#endif
  
  // Check whether the GPU supports non-power-of-two textures
  bool npot = false;
  if (GLEW_ARB_texture_rectangle && GLEW_ARB_texture_non_power_of_two)
    npot=true;
  
  if (!npot) {
    // Use a power-of-two texture catalog
    int newx = 1, newy = 1;
    while (newx < catalog_width) newx *= 2;
    while (newy < catalog_height) newy *= 2;
    catalog_width = newx;
    catalog_height = newy;
    std::cout << "GPU does not support non-power-of-two textures, using " << catalog_width << "x" << catalog_height << " catalog.\n";
  }
  // Check whether the GPU will allow a texture of that size
  if (!testTextureSize(gl_catalog, catalog_width, catalog_height)) {
    MessageBox(NULL,"GPU unable to accomodate texture catalog. Retry without graphical tiles, update your drivers, or better yet update your GPU.", "GL error", MB_OK);
    exit(EXIT_FAILURE);
  }

  // Guess it will. Well, then, actually upload it
  glBindTexture(GL_TEXTURE_2D, gl_catalog);
      printGLError();
  char *zeroes = new char[catalog_width*catalog_height*4];
  memset(zeroes,0,sizeof(char)*catalog_width*catalog_height*4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, catalog_width, catalog_height, 0, GL_RGBA,
	       GL_UNSIGNED_BYTE, zeroes);
  delete[] zeroes;
      printGLError();
  glBindTexture(GL_TEXTURE_2D, gl_catalog);
      printGLError();
  GLint param = (init.window.flag.has_flag(INIT_WINDOW_FLAG_TEXTURE_LINEAR) ?
    GL_LINEAR : GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,param);
       printGLError();
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,param);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      printGLError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      printGLError();
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      printGLError();
  // Performance isn't important here. Let's make sure there are no alignment issues.
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
      printGLError();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      printGLError();
  // While storing the positions to gl_texpos.
  if (gl_texpos) delete[] gl_texpos;
  gl_texpos = new struct gl_texpos[raws.size()];
  for (int pos = 0; pos < ordered.size(); pos++) {
    long raws_pos = ordered[pos].pos;
    SDL_Surface *s = ordered[pos].s;
    SDL_PixelFormat *f = s->format;
    SDL_LockSurface(s);
    // Make /real/ sure we get the GL format right.
    unsigned char *pixels = new unsigned char[ordered[pos].w * ordered[pos].h * 4];
    // Recall, ordered[pos].w is 2 larger than s->w because of the border.
    for (int bx=0; bx < ordered[pos].w; bx++) {
      int x = bx - 1;
      if (x == -1) x++;
      if (x == s->w) x--;
      for (int by=0; by < ordered[pos].h; by++) {
        int y = by - 1;
        if (y == -1) y++;
        if (y == s->h) y--;
        // GL textures are loaded upside-down, Y=0 at the bottom
        unsigned char *pixel_dst = &pixels[(ordered[pos].h - by - 1)*ordered[pos].w*4 + bx*4];
        unsigned char *pixel_src = &((unsigned char*)s->pixels)[y*s->w*4 + x*4];
        assert (pixel_dst < pixels + ordered[pos].w * ordered[pos].h * 4);
        assert (pixel_src < (unsigned char*)s->pixels + s->w * s->h * 4);
        // We convert all textures to RGBA format at load-time, further below
        for (int i=0; i<4; i++) {
          pixel_dst[i] = pixel_src[i];
        }
      }
    }
    // Right. Upload the texture to the catalog.
    SDL_UnlockSurface(s);
    glBindTexture(GL_TEXTURE_2D, gl_catalog);
    printGLError();
    glTexSubImage2D(GL_TEXTURE_2D, 0, ordered[pos].x, ordered[pos].y, ordered[pos].w, ordered[pos].h,
		    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    delete[] pixels;
    printGLError();
    // Compute texture coordinates and store to gl_texpos for later output.
    // To make sure the right pixel is chosen when texturing, we must
    // pick coordinates that place us in the middle of the pixel we want.
    //
    // There's no real reason to use double instead of floats here, but
    // no reason not to either, and it just might help with precision.
    //
    // There's a one-pixel border around each tile, so we offset by 1
    gl_texpos[raws_pos].left   = ((double)ordered[pos].x+1)      / (double)catalog_width;
    gl_texpos[raws_pos].right  = ((double)ordered[pos].x+1+s->w) / (double)catalog_width;
    gl_texpos[raws_pos].top    = ((double)ordered[pos].y+1)      / (double)catalog_height;
    gl_texpos[raws_pos].bottom = ((double)ordered[pos].y+1+s->h) / (double)catalog_height;
  }
  // And that's that. Locked, loaded and ready for texturing.
  printGLError();
  uploaded=true;
}

void textures::remove_uploaded_textures() {
  if (!uploaded) return; // Nothing to do
  glDeleteTextures(1, &gl_catalog);
  uploaded=false;
}

SDL_Surface *textures::get_texture_data(long pos) {
  if (raws.size() > pos)
    return raws[pos];
  else {
    std::cerr << "Asked for nonexistent texture data\n";
    return NULL;
  }
}

long textures::clone_texture(long src) {
  long tx;
	
  if (raws.size() > src && raws[src]) {
    SDL_Surface *dst = SDL_ConvertSurface(raws[src], raws[src]->format, SDL_SWSURFACE);
	tx=add_texture(dst);
  }
  else {
  // Okay, we've been asked to clone a nonexistent texture. Riight...
  // Fortunately add_texture doesn't look at the pointer it gets at all.
  std::cerr << "Asked to clone nonexistent texture!\n";
  tx=add_texture(NULL);
  }

  if (uploaded) {
    remove_uploaded_textures();
    upload_textures();
	  }

  return tx;
}

void textures::grayscale_texture(long pos) {
 SDL_Surface *s = get_texture_data(pos);
 if (!s) return;
 SDL_LockSurface(s);
 SDL_PixelFormat *f = s->format;
 Uint32 *pixels = (Uint32*)(s->pixels);
 if (f->BytesPerPixel != 4) {
   std::cerr << "grayscale_texture ran into mysteriously uncanocalized texture\n";
   goto cleanup;
 }
 for (int i=0; i < s->w*s->h; i++) { // For each pixel
   int r = (pixels[i] & f->Rmask) >> f->Rshift;
   int g = (pixels[i] & f->Gmask) >> f->Gshift;
   int b = (pixels[i] & f->Bmask) >> f->Bshift;
   int alpha = (pixels[i] & f->Amask) >> f->Ashift;
int luminosity=(int)((float)r*0.30f+(float)g*0.59f+(float)b*0.11f);
if(luminosity<0)luminosity=0;
if(luminosity>255)luminosity=255;
   pixels[i] = (luminosity << f->Rshift) |
     (luminosity << f->Gshift) |
     (luminosity << f->Bshift) |
     (alpha << f->Ashift);
 }

 cleanup:
 SDL_UnlockSurface(s);

  if (uploaded) {
    remove_uploaded_textures();
    upload_textures();
  }
}

// Converts an arbitrary Surface to something like the display format
// (32-bit RGBA), and converts magenta to transparency if convert_magenta is set
// and the source surface didn't already have an alpha channel.
// It also deletes the source surface.
//
// It uses the same pixel format (RGBA, R at lowest address) regardless of
// hardware.
SDL_Surface *canonicalize_format(SDL_Surface *src, bool convert_magenta) {
  SDL_Surface *tgt;
  SDL_PixelFormat fmt;
  fmt.palette = NULL;
  fmt.BitsPerPixel = 32;
  fmt.BytesPerPixel = 4;
  fmt.Rloss = fmt.Gloss = fmt.Bloss = fmt.Aloss = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  fmt.Rshift = 24; fmt.Gshift = 16; fmt.Bshift = 8; fmt.Ashift = 0;
#else
  fmt.Rshift = 0; fmt.Gshift = 8; fmt.Bshift = 16; fmt.Ashift = 24;
#endif
  fmt.Rmask = 255 << fmt.Rshift;
  fmt.Gmask = 255 << fmt.Gshift;
  fmt.Bmask = 255 << fmt.Bshift;
  fmt.Amask = 255 << fmt.Ashift;
  fmt.colorkey = 0;
  fmt.alpha = 255;

  tgt = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32,
			     fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
  if (src->format->Amask == 0 && convert_magenta) { // No alpha
    SDL_SetColorKey(src, SDL_SRCCOLORKEY,
		    SDL_MapRGB(src->format, 255, 0, 255));
  }
  tgt = SDL_ConvertSurface(src, &fmt, SDL_SWSURFACE);
  SDL_FreeSurface(src);
  return tgt;
}

// Finds or creates a free spot in the texture array, and inserts
// surface in that spot, then returns the location.
long textures::add_texture(SDL_Surface *surface) {
  long sz = raws.size();
  // Look for a free spot
  for (long pos=0; pos < sz; pos++) {
    if (raws[pos] == NULL) {
      raws[pos] = surface;
      return pos;
    }
  }

  // No free spot, make one
  raws.push_back(surface);
  return sz;
}

void textures::load_multi_pdim(const string &filename, long *tex_pos, long dimx,
			       long dimy, bool convert_magenta,
			       long *disp_x, long *disp_y) {
  SDL_Surface *raw = IMG_Load(filename.c_str());
  if (!raw) {
    MessageBox(NULL, ("Not found: " + filename).c_str(), "Tileset not found", MB_OK);
    exit(1);
  }
  SDL_Surface *src = canonicalize_format(raw, convert_magenta);
  SDL_SetAlpha(src, 0,255);
  *disp_x = src->w / dimx;
  *disp_y = src->h / dimy;
  long idx = 0;
  for (int y=0; y < dimy; y++)
    for (int x=0; x < dimx; x++) {
      SDL_Surface *tile = SDL_CreateRGBSurface(SDL_SWSURFACE, *disp_x, *disp_y,
					       32, src->format->Rmask,
					       src->format->Gmask,
					       src->format->Bmask,
					       src->format->Amask);
      SDL_SetAlpha(tile, 0,255);
      SDL_Rect pos_src;
      pos_src.x = *disp_x * x;
      pos_src.y = *disp_y * y;
      pos_src.w =  *disp_x;
      pos_src.h =  *disp_y;
      SDL_BlitSurface(src, &pos_src, tile, NULL);
      tex_pos[idx] = add_texture(tile);
      idx++;
    }
  // Re-upload textures if necessary
  if (uploaded) {
    remove_uploaded_textures();
    upload_textures();
  }
}

long textures::load(const string &filename, bool convert_magenta) {
  SDL_Surface *raw = IMG_Load(filename.c_str());
  if (!raw) {
    MessageBox(NULL, ("Not found: " + filename).c_str(), "Image not found", MB_OK);
    exit(1);
  }
  SDL_Surface *tex = canonicalize_format(raw, convert_magenta);
  long pos = add_texture(tex);
  // Re-upload if necessary
  if (uploaded) {
    remove_uploaded_textures();
    upload_textures();
  }
  return pos;
}

void textures::delete_texture(long pos) {
  // We can't actually resize the array, as
  // (a) it'd be slow, and
  // (b) it'd change all the positions. Bad stuff would happen.
  if (raws[pos]) {
    SDL_FreeSurface(raws[pos]);
    raws[pos] = NULL;
  }
}

void curses_text_boxst::add_paragraph(const string &src,long para_width)
{
	stringvectst sp;
	sp.add_string(src);
	add_paragraph(sp,para_width);
}

void curses_text_boxst::add_paragraph(stringvectst &src,long para_width)
{
	bool skip_leading_spaces=false;

	//ADD EACH OF THE STRINGS ON IN TURN
	string curstr;
	long strlength=0;
	long s,pos;
	for(s=0;s<src.str.size();s++)
		{
		//GRAB EACH WORD, AND SEE IF IT FITS, IF NOT START A NEW LINE
		for(pos=0;pos<src.str[s]->dat.size();pos++)
			{
			if(skip_leading_spaces)
				{
				if(src.str[s]->dat[pos]==' ')continue;
				else skip_leading_spaces=false;
				}

			//ADD TO WORD
			curstr+=src.str[s]->dat[pos];

			//IF TOO LONG, CUT BACK TO FIRST SPACE
			if(curstr.length()>para_width)
				{
				long opos=pos;

				long minus=0;
				do
					{
					pos--;
					minus++;
					}while(src.str[s]->dat[pos]!=' '&&pos>0);

				//IF WENT ALL THE WAY BACK, INTRODUCE A SPACE
				if(minus==curstr.size())
					{
					src.str[s]->dat.insert(opos-1," ");
					}
				else
					{
					curstr.resize(curstr.size()-minus);
					text.add_string(curstr);
					skip_leading_spaces=true;
					}
				curstr.erase();
				}
			}
		}

	//FLUSH FINAL BIT
	if(!curstr.empty())text.add_string(curstr);
}

void enablerst::copy_texture_data(unsigned char *dest,long destx,long desty,char destalpha,
			unsigned char *src,long srcx,long srcy,char srcalpha,
			long offx,long offy,
			float rmult,float gmult,float bmult,char use_trans,long *color_data,unsigned long flag)
{
	//GET BUFFERS
	unsigned char *buff1=dest;
	unsigned char *buff2=src;

	if(buff1==NULL||buff2==NULL)return;

	//COMPUTE THE PARAMETERS
	long starty,endy,startx,endx,dstartx,dstarty;
	long dind,dind2,dincx,dincx2,dincy,dincy2;

		//START AND END
	startx=offx;endx=offx+srcx-1;
	starty=offy;endy=offy+srcy-1;

		//BOUND START AND END TO DESTINATION BUFFER
	dstartx=0;dstarty=0;
	if(startx<0){dstartx=startx*-1;startx=0;}
	if(starty<0){dstarty=starty*-1;starty=0;}
	if(endx>=destx)endx=destx-1;
	if(endy>=desty)endy=desty-1;

		//SET INCREMENTS BASED ON ALPHA BIT
	if(destalpha)dincx=4;
	else dincx=3;
	if(srcalpha)dincx2=4;
	else dincx2=3;
	dincy=(destx+(endx-startx+1))*dincx;
	dincy2=(srcx+(endx-startx+1))*dincx2;

		//SET THE INITIAL POSITION IN THE BUFFER
	dind=startx*dincx+(desty-starty-1)*destx*dincx;
	dind2=dstartx*dincx2+(srcy-dstarty-1)*srcx*dincx2;

	//Req1Req1Req1Req1Req1Req1
		//these flip commands can't handle the src over-running off the side of the dest buffer
	if(flag & COPYTEXTUREFLAG_VERFLIP)
		{
		dind2-=(endy-starty)*srcx*dincx2;
		if(!(flag & COPYTEXTUREFLAG_HORFLIP))dincy2=0;
		}

	if(flag & COPYTEXTUREFLAG_HORFLIP)
		{
		dind2+=(endx-startx)*dincx2;
		dincx2*=-1;
		if(!(flag & COPYTEXTUREFLAG_VERFLIP))dincy2=0;
		else dincy2*=-1;
		}

		//SET UP MULTIPLITERS
	long rmult_l=(long)(256.0f*rmult);//THE 256 IS NOT A PROBLEM SINCE IT IS A FLOAT ANYWAY
	long gmult_l=(long)(256.0f*gmult);
	long bmult_l=(long)(256.0f*bmult);

	//DO THE COPY
	long place_r,place_g,place_b,place_dist;
	long dx,dy;
	for(dy=starty;dy<=endy;dy++)
		{
		for(dx=startx;dx<=endx;dx++)
			{
			//RESPECT TRANSPARENCY
			if(use_trans)
				{
				if(srcalpha)
					{
					if(buff2[dind2+3]==0)
						{
						dind+=dincx;
						dind2+=dincx2;
						continue;
						}
					}
				}

			place_r=buff2[dind2];
			place_g=buff2[dind2+1];
			place_b=buff2[dind2+2];

			if(color_data!=NULL)
				{
				short slot=-1;

				if(place_r==place_g&&place_g==place_b&&place_r!=0){slot=COLOR_DATA_WHITE_R;place_dist=place_r;}
				if(place_r!=0&&place_g==0&&place_b==0){slot=COLOR_DATA_RED_R;place_dist=place_r;}
				if(place_g!=0&&place_b==0&&place_r==0){slot=COLOR_DATA_GREEN_R;place_dist=place_g;}
				if(place_b!=0&&place_r==0&&place_g==0){slot=COLOR_DATA_BLUE_R;place_dist=place_b;}
				if(place_r==place_g&&place_b==0&&place_r!=0){slot=COLOR_DATA_YELLOW_R;place_dist=place_r;}
				if(place_r==place_b&&place_g==0&&place_b!=0){slot=COLOR_DATA_MAGENTA_R;place_dist=place_r;}
				if(place_g==place_b&&place_r==0&&place_g!=0){slot=COLOR_DATA_CYAN_R;place_dist=place_g;}

				if(slot!=-1)
					{
					if(color_data[slot]!=-1)
						{
						place_r=(place_dist*color_data[slot])>>8;
						place_g=(place_dist*color_data[slot+1])>>8;
						place_b=(place_dist*color_data[slot+2])>>8;
						}
					}
				}

			//COPY IN DATA
			if(srcalpha)
				{
				if(use_trans&&buff2[dind2+3]!=255)
					{
					place_r=((place_r*(long)buff2[dind2+3]+(long)buff1[dind]*(long)(255-buff2[dind2+3]))>>8);
					place_g=((place_g*(long)buff2[dind2+3]+(long)buff1[dind+1]*(long)(255-buff2[dind2+3]))>>8);
					place_b=((place_b*(long)buff2[dind2+3]+(long)buff1[dind+2]*(long)(255-buff2[dind2+3]))>>8);
					}
				}
			if(destalpha&&srcalpha)
				{
				if(!use_trans||buff2[dind2+3]==255)buff1[dind+3]=buff2[dind2+3];
				}

			if(rmult_l!=256)
				{
				buff1[dind]=(place_r*rmult_l)>>8;
				}
			else
				{
				buff1[dind]=place_r;
				}
			if(gmult_l!=256)
				{
				buff1[dind+1]=(place_g*gmult_l)>>8;
				}
			else
				{
				buff1[dind+1]=place_g;
				}
			if(bmult_l!=256)
				{
				buff1[dind+2]=(place_b*bmult_l)>>8;
				}
			else
				{
				buff1[dind+2]=place_b;
				}

			dind+=dincx;
			dind2+=dincx2;
			}
		dind-=dincy;
		dind2-=dincy2;
		}
}

#define BITMAP_ID 0x4D42

void enablerst::save_texture_data_to_bmp(unsigned char *bitmapImage,long dimx,long dimy,long alpha,string &filename)
{
	//GARBAGE-ASS HACK TO Y-FLIP IMAGE, EASY TO SPEED UP
	long bit=3;
	if(alpha)bit=4;
	unsigned char swp;
	long ind1,ind2;
	long x,y;
	for(y=0;y<dimy/2;y++)
		{
		for(x=0;x<dimx;x++)
			{
			ind1=(x+y*dimx)*bit;
			ind2=(x+(dimy-1-y)*dimx)*bit;

			swp=bitmapImage[ind1];
			bitmapImage[ind1]=bitmapImage[ind2];
			bitmapImage[ind2]=swp;
			ind1++;ind2++;

			swp=bitmapImage[ind1];
			bitmapImage[ind1]=bitmapImage[ind2];
			bitmapImage[ind2]=swp;
			ind1++;ind2++;

			swp=bitmapImage[ind1];
			bitmapImage[ind1]=bitmapImage[ind2];
			bitmapImage[ind2]=swp;

			if(alpha)
				{
				ind1++;ind2++;
				swp=bitmapImage[ind1];
				bitmapImage[ind1]=bitmapImage[ind2];
				bitmapImage[ind2]=swp;
				}
			}
		}

 // There may be endianness issues. At any rate, the hex numbers here specify the memory layout of each pixel.
 SDL_Surface *bmp = SDL_CreateRGBSurfaceFrom(bitmapImage, dimx, dimy,
24 + alpha*8, (3+alpha)*dimx, 0xff, 0xff00, 0xff0000,
0xff000000*alpha);
 SDL_SaveBMP(bmp, filename.c_str());
 SDL_FreeSurface(bmp); // Does not free bitmapImage
 /*
#ifdef WIN32
  FILE *filePtr;
  BITMAPFILEHEADER bitmapFileHeader;
  BITMAPINFOHEADER bitmapInfoHeader;
  unsigned int      imageIdx=0;       // image index counter
  unsigned char     tempRGB;            // swap variable

  // open filename in "write binary" mode
  filePtr = fopen(filename.c_str(), "wb");
  if (filePtr == NULL)
    return;

  bitmapFileHeader.bfReserved1=0;
  bitmapFileHeader.bfReserved2=0;
  bitmapFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
  bitmapFileHeader.bfSize=sizeof(BITMAPFILEHEADER);
  bitmapFileHeader.bfType=BITMAP_ID;

  // write the bitmap file header
  fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

  bitmapInfoHeader.biBitCount=24+alpha*8;
  bitmapInfoHeader.biClrImportant=0;
  bitmapInfoHeader.biClrUsed=0;
  bitmapInfoHeader.biCompression=0;
  bitmapInfoHeader.biWidth=dimx;
  bitmapInfoHeader.biHeight=dimy;
  long bytes=bitmapInfoHeader.biBitCount/8;
  bitmapInfoHeader.biPlanes=1;
  bitmapInfoHeader.biSize=sizeof(BITMAPINFOHEADER);
  bitmapInfoHeader.biSizeImage=bitmapInfoHeader.biHeight*bitmapInfoHeader.biWidth*bytes;
  bitmapInfoHeader.biXPelsPerMeter=1;
  bitmapInfoHeader.biYPelsPerMeter=1;

  // write the bitmap information header
  fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

  // swap the R and B values to get RGB since the bitmap color format is in BGR
  for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx+=bytes)
  {
    tempRGB = bitmapImage[imageIdx];
    bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
    bitmapImage[imageIdx + 2] = tempRGB;
  }

  // read in the bitmap image data
  fwrite(bitmapImage, 1, bitmapInfoHeader.biSizeImage, filePtr);

  // close the file
  fclose(filePtr);
#endif
  */
}

bool enablerst::prep_for_image_export()
{
	old_grid_zoom_req=grid_zoom_req;
	grid_zoom_req=1;
	reset_window();
	if(grid_zoom!=1)
		{
		grid_zoom_req=old_grid_zoom_req;
		reset_window();

		errorlog_string("Image export not possible because of zoom/window settings");
		return false;
		}

	old_zoom_grid=zoom_grid;
	zoom_grid=false;

	return true;
}

void enablerst::post_image_export()
{
	zoom_grid=old_zoom_grid;
	grid_zoom_req=old_grid_zoom_req;
	reset_window();
}
