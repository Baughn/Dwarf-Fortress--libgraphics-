#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
using std::cout;
using std::endl;

class renderer_minimal {
 public:
  virtual void swap_arrays() = 0;
  virtual void prepare() {};
  virtual void display() = 0;
  virtual void render() = 0;
  // Notification: Window size has changed.
  virtual void resize(int w, int h) = 0;
  // Request: Resize the grid. Window stays the same size.
  virtual void grid_resize(int w, int h) = 0;
  virtual void set_fullscreen() {} // Should read from enabler.is_fullscreen()
  virtual void zoom(zoom_commands cmd) {};
  virtual bool get_mouse_coords(int &x, int &y) = 0;
};


class renderer_cpu : virtual public renderer_minimal {
 protected:
  unsigned int *screen;
  // For partial printing:
  unsigned int *screen_old;
  void gps_allocate(int x, int y);
  texture_fullid screen_to_texid(int x, int y);
 public:
  virtual void update_tile(int x, int y) = 0;
  void update_all() {
    for (int x = 0; x < gps.dimx; x++)
      for (int y = 0; y < gps.dimy; y++)
        update_tile(x, y);
  }
  virtual void swap_arrays();
  virtual void display();
  renderer_cpu() {
    screen = NULL;
    screen_old = NULL;
  }
};

// Common sdl-mode setup
class renderer_sdl : virtual public renderer_minimal {
  bool init_opengl;
  bool init_video(int w, int h);
public:
  renderer_sdl(bool init_opengl);
  virtual void resize(int w, int h);
  virtual void zoom(zoom_commands cmd);
  virtual bool get_mouse_coords(int &x, int &y) {
  }
  // Current screen geometry
  int preferred_zoom, current_zoom;   // Zoom requests (in pixels bias to the tileset's smallest dimension)
  int dispx, dispy; // Tile size
  int originx, originy; // Origin of the top-left-most tile
};


class renderer_2d : public renderer_sdl, public renderer_cpu {
  SDL_Surface *surface;
  map<texture_fullid, SDL_Surface*> tile_cache;
  SDL_Surface *tile_cache_lookup(texture_fullid &id);
public:
  renderer_2d() : renderer_sdl(false) {
    surface = SDL_GetVideoSurface();
  }
  virtual void resize(int w, int h) {
    int prev_dispx = dispx, prev_dispy = dispy;
    renderer_sdl::resize(w, h);
    surface = SDL_GetVideoSurface();
    if (prev_dispx != dispx || prev_dispy != dispy) {
      for (auto it = tile_cache.begin(); it != tile_cache.end(); ++it)
        SDL_FreeSurface(it->second);
      tile_cache.clear();
    }
  }
  void render();
  void grid_resize(int w, int h);
  void update_tile(int x, int y);
};


#endif
