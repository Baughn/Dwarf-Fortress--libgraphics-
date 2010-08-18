#include "enabler.h"
#include "renderer.hpp"
#include "init.h"

bool renderer_sdl::init_video(int w, int h) {
  // Get ourselves a 2D SDL window
  Uint32 flags = 0;
  if (init_opengl) {
    flags |= SDL_OPENGL | SDL_HWSURFACE;
  } else {
    flags |= init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DHW) ? SDL_HWSURFACE : SDL_SWSURFACE;
    flags |= init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DASYNC) ? SDL_ASYNCBLIT : 0;
    }
  
  // Set it up for windowed or fullscreen, depending.
  if (enabler.is_fullscreen()) { 
    flags |= SDL_FULLSCREEN;
  } else {
    if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE))
      flags |= SDL_RESIZABLE;
  }

  if (init_opengl) {
    // Setup OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, init.window.flag.has_flag(INIT_WINDOW_FLAG_VSYNC_ON));
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,
                        init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER) ? 0 : 1);
  }

  // (Re)create the window
  SDL_Surface *screen = SDL_SetVideoMode(w, h, 32, flags);
  if (screen == NULL) cout << "INIT FAILED!" << endl;

  if (init_opengl) {
    // Test double-buffering status
    int test;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &test);
    if (test != ((init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER)) ? 0 : 1)) {
      if (enabler.is_fullscreen());
      //errorlog << "Requested single-buffering not available\n" << flush;
      else
        report_error("OpenGL", "Requested single-buffering not available");
    }
      
    // (Re)initialize GLEW. Technically only needs to be done once on
    // linux, but on windows forgetting will cause crashes.
    glewInit();
      
    // Set the viewport and clear
    glViewport(0, 0, screen->w, screen->h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
    
  return screen != NULL;
}

renderer_sdl::renderer_sdl(bool init_opengl) {
  this->init_opengl = init_opengl;
  preferred_zoom = current_zoom = 0;
    
  // Disable key repeat (we do our own)
  SDL_EnableKeyRepeat(0, 0);
  // Set window title/icon.
  SDL_WM_SetCaption(GAME_TITLE_STRING, NULL);
  SDL_Surface *icon = IMG_Load("data/art/icon.png");
  if (icon != NULL) {
    SDL_WM_SetIcon(icon, NULL);
    // The icon's surface doesn't get used past this point.
    SDL_FreeSurface(icon); 
  }
    
  // Find the current desktop resolution if fullscreen resolution is auto
  if (init.display.desired_fullscreen_width  == 0 ||
      init.display.desired_fullscreen_height == 0) {
    const struct SDL_VideoInfo *info = SDL_GetVideoInfo();
    init.display.desired_fullscreen_width = info->current_w;
    init.display.desired_fullscreen_height = info->current_h;
  }

  // Initialize our window
  bool worked = init_video(enabler.is_fullscreen() ?
                           init.display.desired_fullscreen_width :
                           init.display.desired_windowed_width,
                           enabler.is_fullscreen() ?
                           init.display.desired_fullscreen_height :
                           init.display.desired_windowed_height);

  // Fallback to windowed mode if fullscreen fails
  if (!worked && enabler.is_fullscreen()) {
    enabler.fullscreen = false;
    report_error("SDL initialization failure, trying windowed mode", SDL_GetError());
    worked = init_video(init.display.desired_windowed_width,
                        init.display.desired_windowed_height);
  }
  // Quit if windowed fails
  if (!worked) {
    report_error("SDL initialization failure", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void renderer_sdl::resize(int w, int h) {
  cout << "New window size: " << w << " " << h << endl;
  // Re-initialize video
  init_video(w,h);
  if (!enabler.overridden_grid_sizes.size()) {
    // Recalculate the grid
    gridx = w / enabler.tileset.dispx;
    gridy = h / enabler.tileset.dispy;
    dispx = enabler.tileset.dispx;
    dispy = enabler.tileset.dispy;
    originx = originy = 0;
    // And done.
    grid_resize(gridx, gridy);
  }
}

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
