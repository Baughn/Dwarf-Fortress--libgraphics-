class renderer_2d : public renderer {
protected:
  SDL_Surface *screen;
  map<texture_fullid, SDL_Surface*> tile_cache;
  int dispx, dispy, dimx, dimy;

  SDL_Surface *tile_cache_lookup(texture_fullid &id) {
    map<texture_fullid, SDL_Surface*>::iterator it = tile_cache.find(id);
    if (it != tile_cache.end()) {
      return it->second;
    } else {
      // Create the colorized texture
      SDL_Surface *tex   = enabler.textures.get_texture_data(id.texpos);
      SDL_Surface *color;
      bool use_hw        = init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DHW);
      if (use_hw) {
        color = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                     tex->w, tex->h,
                                     tex->format->BitsPerPixel,
                                     tex->format->Rmask,
                                     tex->format->Bmask,
                                     tex->format->Gmask,
                                     0);
        if (!color) {
          use_hw = false;
          MessageBox (NULL, "Unable to create texture in video memory. Performance will suffer. Use 2DSW, or don't use graphical tiles.", "Out of VRAM", MB_OK | MB_ICONEXCLAMATION);
        }
      }
      if (!use_hw) {
        color = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                     tex->w, tex->h,
                                     tex->format->BitsPerPixel,
                                     tex->format->Rmask,
                                     tex->format->Bmask,
                                     tex->format->Gmask,
                                     0);
        if (!color) {
          MessageBox (NULL, "Unable to create texture!", "Fatal error", MB_OK | MB_ICONEXCLAMATION);
          abort();
        }
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
          float alpha = pixel_src[3] / 255;
          for (int c = 0; c < 3; c++) {
            float fg = color_fg[c] / 255.0, bg = color_bg[c] / 255.0, tex = pixel_src[c] / 255.0;
            pixel_dst[c] = ((alpha * (tex * fg)) + ((1 - alpha) * bg)) * 255;
          }
        }
      }
      
      SDL_UnlockSurface(color);
      SDL_UnlockSurface(tex);
      
      // Convert to display format
      SDL_DisplayFormat(color);
      // Insert and return
      tile_cache[id] = color;
      return color;
    }
  }
  
  virtual bool init_video(int w, int h) {
    // Get ourselves a 2D SDL window
    Uint32 flags = init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DHW) ? SDL_HWSURFACE : SDL_SWSURFACE;
    flags |= init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DASYNC) ? SDL_ASYNCBLIT : 0;

    // Set it up for windowed or fullscreen, depending.
    if (enabler.is_fullscreen()) { 
      flags |= SDL_FULLSCREEN;
    } else {
      if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE))
        flags |= SDL_RESIZABLE;
    }

    // (Re)create the window
    screen = SDL_SetVideoMode(w, h, 32, flags);
    if (screen == NULL) cout << "INIT FAILED!" << endl;

    return screen != NULL;
  }
  
public:
  void update_tile(int x, int y) {
    // Read tiles from gps, create cached texture
    struct texture_fullid id = screen_to_texid(x, y);
    SDL_Surface *tex = tile_cache_lookup(id);
    // Figure out where to blit
    SDL_Rect dst;
    dst.x = tex->w * x;
    dst.y = tex->h * y;
    // And blit.
    SDL_BlitSurface(tex, NULL, screen, &dst);
  }

  void update_all() {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    for (int x = 0; x < gps.dimx; x++)
      for (int y = 0; y < gps.dimy; y++)
        update_tile(x, y);
  }

  virtual void render() {
    SDL_Flip(screen);
  }

  ~renderer_2d() {
    for (map<texture_fullid,SDL_Surface*>::iterator it = tile_cache.begin();
         it != tile_cache.end();
         ++it) {
      SDL_FreeSurface(it->second);
    }
  }

  virtual void init_sdl() {
    // Disable key repeat
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

  renderer_2d() {
    init_sdl();
  }

  void grid_resize(int w, int h) {
    dimx = w; dimy = h;
    // Only reallocate the grid if it actually changes
    if (init.display.grid_x != dimx || init.display.grid_y != dimy)
      gps_allocate(dimx, dimy);
    // But always force a full display cycle
    gps.force_full_display_count = 1;
    enabler.flag |= ENABLERFLAG_RENDER;    
  }
  
  void resize(int w, int h) {
    // We've gotten resized.. first step is to reinitialize video
    cout << "New window size: " << w << "x" << h << endl;
    init_video(w, h);
    dispx = enabler.is_fullscreen() ?
      init.font.large_font_dispx :
      init.font.small_font_dispx;
    dispy = enabler.is_fullscreen() ?
      init.font.large_font_dispy :
      init.font.small_font_dispy;
    cout << "Font size: " << dispx << "x" << dispy << endl;
    // If grid size is currently overridden, we don't change it
    if (enabler.overridden_grid_sizes.size() == 0) {
      // (Re)calculate grid-size
      dimx = MIN(MAX(w / dispx, MIN_GRID_X), MAX_GRID_X);
      dimy = MIN(MAX(h / dispy, MIN_GRID_Y), MAX_GRID_Y);
      cout << "Resizing grid to " << dimx << "x" << dimy << endl << endl;
      grid_resize(dimx, dimy);
    }
  }

  void set_fullscreen() {
    resize(init.display.desired_fullscreen_width,
           init.display.desired_fullscreen_height);
  }

  bool get_mouse_coords(int &x, int &y) {
    // Origin is always 0,0. Simple.
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (mouse_x < 0 || mouse_x >= dispx*dimx ||
        mouse_y < 0 || mouse_y >= dispy*dimy)
      return false;
    x = mouse_x / dispx;
    y = mouse_y / dispy;
    return true;
  }
};

class renderer_offscreen : private renderer_2d {
  virtual bool init_video(int w, int h) {
    // Create an offscreen buffer
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0);
    assert(screen);
    return true;
  }
  virtual void init_sdl() {}
public:
  ~renderer_offscreen() {
    SDL_FreeSurface(screen);
  }
  // Create an offscreen renderer of a given grid-size
  renderer_offscreen(int grid_x, int grid_y) {
    dispx = enabler.is_fullscreen() ?
      init.font.large_font_dispx :
      init.font.small_font_dispx;
    dispy = enabler.is_fullscreen() ?
      init.font.large_font_dispy :
      init.font.small_font_dispy;
    dimx = grid_x;
    dimy = grid_y;
    init_video(dispx * grid_x, dispy * grid_y);
  }
  // Slurp the entire gps content into the renderer at some given offset
  void update_all(int offset_x, int offset_y) {
    for (int x = 0; x < gps.dimx; x++) {
      for (int y = 0; y < gps.dimy; y++) {
        // Read tiles from gps, create cached texture
        struct texture_fullid id = screen_to_texid(x, y);
        SDL_Surface *tex = tile_cache_lookup(id);
        // Figure out where to blit
        SDL_Rect dst;
        dst.x = tex->w * (x+offset_x);
        dst.y = tex->h * (y+offset_y);
        // And blit.
        SDL_BlitSurface(tex, NULL, screen, &dst);
      }
    }
  }
  // Save the image to some file
  void save_to_file(const string &file) {
    // TODO: Support png, etc.
    SDL_SaveBMP(screen, file.c_str());
  }
};
