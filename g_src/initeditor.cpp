#include <iostream>

#include "initeditor.hpp"
#include "init.h"
#include "graphics.h"
#include "ViewBase.h"
#include "interface.h"

using namespace std;
using namespace widgets;

enum setting_name {
  // Sound
  sound_toggle, volume, sound_sel_end,
  // Graphics
  intro, windowed, windowedx, windowedy, fullscreenx, fullscreeny, font, graphics_toggle, graphics_sel_end,
  // Graphics (advanced)
  resizable, black_space, print_mode, texture_param, single_buffer, arb_sync, vsync, graphics_a_sel_end,
  // FPS
  fps_toggle, fps_cap, g_fps_cap, fps_sel_end,
  // Input
  mouse_toggle, mouse_picture, zoom_speed, key_hold, key_repeat, key_accel_start, key_accel_limit,
  recenter_shutdown, input_sel_end,
  // Misc
  topmost, misc_sel_end,
};

class editor {
  textbox *helptext;
protected:
  void render_help(int x1, int x2, int y1, int y2) {
    helptext->render(x1, x2, y1, y2);
  }
public:
  editor(const string &helptext) {
    this->helptext = new textbox(helptext, false);
  }
  ~editor() {
    delete helptext;
  }
  virtual void feed(set<InterfaceKey> &input) = 0;
  virtual void render(int x1, int x2, int y1, int y2) = 0;
  virtual void apply() = 0;
};

class editor_toggle : public editor {
  void feed(set<InterfaceKey> &input) {
  }
  void render(int x1, int x2, int y1, int y2) {
  }
  void apply() {
  }
};

struct setting {
  string initkey;
  class editor *editor;
};

static class setting_map : public map<setting_name, setting> {
  void insert(setting_name s, const setting &v) { (*this)[s] = v; }
public:
  setting_map() {
    insert(sound_toggle, {"SOUND", new editor_toggle("Master sound toggle")});
  }
};
  


// static class name_map : public map<setting,string> {
//   void insert(setting s, const string &v) { (*this)[s] = v; }
// public:
//   name_map() {
//     insert(sound_toggle,"SOUND");
//     insert(volume,"VOLUME");
    
//     insert(intro,"INTRO");
//     insert(windowed,"WINDOWED");
//     insert(windowedx,"WINDOWEDX");
//     insert(windowedy,"WINDOWEDY");
//     insert(fullscreenx,"FULLSCREENX");
//     insert(fullscreeny,"FULLSCREENY");
//     insert(font,"FONT");
//     insert(graphics_toggle,"GRAPHICS");

//     insert(resizable,"RESIZABLE");
//     insert(black_space,"BLACK_SPACE");
//     insert(print_mode,"PRINT_MODE");
//     insert(texture_param,"TEXTURE_PARAM");
//     insert(single_buffer,"SINGLE_BUFFER");
//     insert(arb_sync,"ARB_SYNC");
//     insert(vsync,"VSYNC");

//     insert(fps_toggle,"FPS");
//     insert(fps_cap,"FPS_CAP");
//     insert(g_fps_cap,"G_FPS_CAP");

//     insert(mouse_toggle,"MOUSE");
//     insert(mouse_picture,"MOUSE_PICTURE");
//     insert(zoom_speed,"ZOOM_SPEED");
//     insert(key_hold,"KEY_HOLD_MS");
//     insert(key_repeat,"KEY_REPEAT_MS");
//     insert(key_accel_start,"KEY_REPEAT_ACCEL_START");
//     insert(key_accel_limit,"KEY_REPEAT_ACCEL_LIMIT");
//     insert(recenter_shutdown,"RECENTER_INTERFACE_SHUTDOWN_MS");
    
//     insert(topmost,"TOPMOST");
//   }
// } name;

// static class help_map : public map<setting,string> {
//   void insert(setting s, const string &v) { (*this)[s] = v; }
// public:
//   help_map() {
//     insert(sound_toggle, "Change this to turn sound off");
//     insert(volume, "The sound volume runs from 0 (off) to 255 (full).");
    
//     insert(intro, "Change this to skip the wonderful intro movies.");
//     insert(windowed, "This lets you set the starting windowed/fullscreen setting.  Can be YES, NO or PROMPT.");
//     insert(windowedx, "Horizontal size for windowed mode. If set below 256 it specifies the grid size instead, with a minimum of 80.");
//     insert(windowedy, "Vertical size for windowed mode. If set below 256 it specifies the grid size instead, with a minimum of 25.");
//     insert(fullscreenx, "Horizontal fullscreen resolution. 0 = auto, but you can change this if you wish.");
//     insert(fullscreeny, "Vertical fullscreen resolution. 0 = auto, but you can change this if you wish.");
//     insert(font, "Basic font. Changing to the 800x600 font might make you happier.");
//     insert(graphics_toggle, "If on, use graphical tiles (when installed)");

//     insert(resizable, "You may disable window resizing if you wish.");
//     insert(black_space, "If this is set to NO, tiles will be stretched to fit the screen if there is a resolution mismatch. If this is set to YES, the tiles will not be stretched, but rather the game view will be centralized, surrounded by black space.  Tiles that are too large will always be compressed rather than running off the screen.");
//     insert(print_mode, "Video function used to draw graphics on screen");
//     insert(texture_param, "Which resizing function to use (OpenGL only)");
//     insert(single_buffer, "If on, use a single-buffered video mode");
//     insert(arb_sync, "If on, attempt to avoid overloading weak GPUs");
//     insert(vsync, "If on, synchronize graphical refresh to screen");

//     insert(fps_toggle, "Display FPS counter");
//     insert(fps_cap, "Maximum simulation speed, in hertz");
//     insert(g_fps_cap, "Maximum graphical refresh speed, in hertz");

//     insert(mouse_toggle, "If off, disable mouse support");
//     insert(mouse_picture, "If on, show a nice picture instead of the hardware mouse. Laggy.");
//     insert(zoom_speed, "Grid units to zoom per mouse-wheel click");
//     insert(key_hold, "Number of milliseconds to wait for \"slow\" repeating commands to repeat the first time");
//     insert(key_repeat, "Number of milliseconds between repeats that don't fall in the above category");
//     insert(key_accel_start, "Number of keypresses before repeats start to accelerate");
//     insert(key_accel_limit, "Maximum key-press acceleration");
//     insert(recenter_shutdown, "Number of milliseconds before the game accepts keyboard input after recentering on an event in dwarf mode");
    
//     insert(topmost, "If on, keep DF on top of other windows");
//   }
// } help;

// template<typename T>
// static void tostr(ostringstream &oss, T t) { oss << t; }

// template<>
// void tostr<bool>(ostringstream &oss, bool b) { oss << (b ? "YES" : "NO"); }

// template<>
// void tostr<InitDisplayWindow>(ostringstream &oss, InitDisplayWindow w) {
//   switch (w) {
//   case INIT_DISPLAY_WINDOW_TRUE: oss << "YES"; break;
//   case INIT_DISPLAY_WINDOW_FALSE: oss << "NO"; break;
//   case INIT_DISPLAY_WINDOW_PROMPT: oss << "PROMPT"; break;
//   }
// }

// template<typename T>
// static void opt(ostringstream &oss, setting s, T val) {
//   oss << help[s] << endl;
//   oss << "[" << name[s] << ":";
//   tostr(oss, val);
//   oss << "]" << endl << endl;
// }



// static class : public viewscreenst {
//   void write_init() {
//     ostringstream oss; //("data/init/init.txt.partial");
//     oss << "WARNING: Do NOT copy over the init.txt from an earlier version of DF.\n"
//       "Always read the file carefully, including the comments.\n"
//       "\n"
//       "In general, you can copy savegames and tilesets from older DF\n"
//       "versions, but you should not copy anything else.  You should not, under any\n"
//       "circumstances, unpack a new DF on top of an older one.\n\n\n";
//     // Sound
//     // opt(oss, "SOUND", !init_pristine.media.flag.has_flag(INIT_MEDIA_FLAG_SOUND_OFF));
//     // opt(oss, "VOLUME", init_pristine.media.volume);
//     // // Graphics
//     // opt(oss, "INTRO", !init_pristine.media.flag.has_flag(INIT_MEDIA_FLAG_INTRO_OFF));
//     // opt(oss, "WINDOWED", init_pristine.display.windowed);
//     // opt(oss, "WINDOWEDX", init_pristine.display.desired_windowed_width);
//     // opt(oss, "WINDOWEDY", init_pristine.display.desired_windowed_height);
//     // opt(oss, "FULLSCREENX", init_pristine.display.desired_fullscreen_width);
//     // opt(oss, "FULLSCREENY", init_pristine.display.desired_fullscreen_height);
//     // opt(oss, "FONT", init_pristine.font.fontfile);
//     // opt(oss, "GRAPHICS", init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS));
//     // // Graphics (advanced)
//     // opt(oss, "RESIZABLE", !init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE));
//     // opt(oss, "BLACK_SPACE", init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_BLACK_SPACE));
//     // oss << ::help["PRINT_MODE"] << endl << "[PRINT_MODE:";
//     // if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_2DASYNC))
//     //   oss << "2DASYNC";
//     // else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_2D)) {
//     //   oss << "2D";
//     //   if (!init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_2DHW))
//     //     oss << "SW";
//     // } else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER))
//     //   oss << "FRAME_BUFFER";
//     // else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER))
//     //   oss << "ACCUM_BUFFER";
//     // else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_VBO))
//     //   oss << "VBO";
//     // else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_SHADER))
//     //   oss << "SHADER";
//     // else if (init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT))
//     //   oss << "PARTIAL:" << init_pristine.display.partial_print_count;
//     // oss << "]" << endl << endl;
//     // opt(oss, "TEXTURE_PARAM", init_pristine.window.flag.has_flag(INIT_WINDOW_FLAG_TEXTURE_LINEAR) ? "LINEAR" : "NEAREST");
//     // opt(oss, "SINGLE_BUFFER", init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER));
//     // opt(oss, "ARB_SYNC", init_pristine.display.flag.has_flag(INIT_DISPLAY_FLAG_ARB_SYNC));
//     // opt(oss, "VSYNC", init_pristine.window.flag.has_flag(INIT_WINDOW_FLAG_VSYNC_ON));
//     // // FPS
//     // opt(oss, "FPS", bool(gps.display_frames));
//     // opt(oss, "FPS_CAP", init_pristine.display.fps);
//     // opt(oss, "G_FPS_CAP", init_pristine.display.g_fps);
//     // // Input
//     // opt(oss, "MOUSE", !init_pristine.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF));
//     // opt(oss, "MOUSE_PICTURE", init_pristine.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE));
//     // opt(oss, "ZOOM_SPEED", init_pristine.input.zoom_speed);
//     // opt(oss, "KEY_HOLD_MS", init_pristine.input.hold_time);
//     // opt(oss, "KEY_REPEAT_MS", init_pristine.input.repeat_time);
//     // opt(oss, "KEY_REPEAT_ACCEL_START", init_pristine.input.repeat_accel_start);
//     // opt(oss, "KEY_REPEAT_ACCEL_LIMIT", init_pristine.input.repeat_accel_limit);
//     // opt(oss, "RECENTER_INTERFACE_SHUTDOWN_MS", init_pristine.input.pause_zoom_no_interface_ms);
//     // // Misc
//     // opt(oss, "TOPMOST", init_pristine.window.flag.has_flag(INIT_WINDOW_FLAG_TOPMOST));
//     // opt(oss, "PRIORITY", "NORMAL"); // FIXME

//         // s.close();
//     cout << oss.str() << endl;
//   }

//   enum { left, right } mode;

//   enum group_sel { sound, graphics, graphics_adv, fps, input, misc, apply, save, quit };
    
// public:
//   void render() {
//     puts("foo");
//     gps.erasescreen();
//     gps.locate(2,14);
//     gps.addst("Hiya");
//   }
//   void logic() {
//     enabler.flag |= ENABLERFLAG_RENDER;
//   }
// } editor;

// void init_editor_screen() {
//   gview.addscreen(&editor, INTERFACE_PUSH_AT_BACK, NULL);
//   enabler.flag |= ENABLERFLAG_RENDER;
//   // editor.write_init();
// }
