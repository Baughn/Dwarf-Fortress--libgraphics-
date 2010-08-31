#include "renderer.hpp"

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

