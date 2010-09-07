#include "enabler.h"
#include "renderer_2d.hpp"

void renderer_2d::render() {
  SDL_Flip(surface);
}

void renderer_2d::grid_resize(int w, int h) {
  gps_allocate(w, h);
}

SDL_Surface *renderer_2d::tile_cache_lookup(texture_fullid &id) {
  map<texture_fullid, SDL_Surface*>::iterator it = tile_cache.find(id);
  if (it != tile_cache.end()) {
    return it->second;
  } else {
    // Create the colorized texture
    SDL_Surface *tex   = enabler.tileset.get_texture(id.texpos, dispx, dispy);
    SDL_Surface *color;
    color = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                 tex->w, tex->h,
                                 tex->format->BitsPerPixel,
                                 tex->format->Rmask,
                                 tex->format->Gmask,
                                 tex->format->Bmask,
                                 0);
    if (!color) {
      MessageBox (NULL, "Unable to create texture!", "Fatal error", MB_OK | MB_ICONEXCLAMATION);
      abort();
    }
    
    // Fill it
    Uint32 color_fgi = SDL_MapRGB(color->format, id.r*255, id.g*255, id.b*255);
    Uint8 *color_fg = (Uint8*) &color_fgi;
    Uint32 color_bgi = SDL_MapRGB(color->format, id.br*255, id.bg*255, id.bb*255);
    Uint8 *color_bg = (Uint8*) &color_bgi;
    SDL_LockSurface(tex);
    SDL_LockSurface(color);
    
    Uint8 *pixel_src, *pixel_dst;
    for (int y = 0; y < tex->h; y++) {
      pixel_src = ((Uint8*)tex->pixels) + (y * tex->pitch);
      pixel_dst = ((Uint8*)color->pixels) + (y * color->pitch);
      for (int x = 0; x < tex->w; x++, pixel_src+=4, pixel_dst+=4) {
        float alpha = pixel_src[3] / 255.0;
        for (int c = 0; c < 3; c++) {
          float fg = color_fg[c] / 255.0, bg = color_bg[c] / 255.0, tex = pixel_src[c] / 255.0;
          pixel_dst[c] = ((alpha * (tex * fg)) + ((1 - alpha) * bg)) * 255;
        }
      }
    }
    
    SDL_UnlockSurface(color);
    SDL_UnlockSurface(tex);
    
    // Convert to display format
    SDL_Surface *disp = SDL_DisplayFormat(color);
    SDL_FreeSurface(color);
    SDL_FreeSurface(tex);
    // Insert and return
    tile_cache[id] = disp;
    return disp;
  }
}

void renderer_2d::update_tile(int x, int y) {
  // Figure out where to blit
  SDL_Rect dst;
  dst.x = dispx * x + originx;
  dst.y = dispy * y + originy;
  // Read tiles from gps, create cached texture
  texture_fullid id = screen_to_texid(x, y);
  SDL_Surface *tex;
  tex = tile_cache_lookup(id);
  // And blit.
  SDL_BlitSurface(tex, NULL, surface, &dst);
}
