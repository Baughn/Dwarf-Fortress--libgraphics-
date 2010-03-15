// STANDARD
class renderer_opengl : public renderer_sdl {
protected:
  SDL_Surface *screen;
  
  bool init_video(int w, int h) {
    // Get ourselves an opengl-enabled SDL window
    Uint32 flags = SDL_HWSURFACE | SDL_OPENGL;

    // Set it up for windowed or fullscreen, depending.
    if (enabler.is_fullscreen()) { 
      flags |= SDL_FULLSCREEN;
    } else {
      if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE))
        flags |= SDL_RESIZABLE;
    }

    // Setup OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, init.window.flag.has_flag(INIT_WINDOW_FLAG_VSYNC_ON));
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,
                        init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER) ? 0 : 1);

    // (Re)create the window
    screen = SDL_SetVideoMode(w, h, 32, flags);

    if (!screen) return false;

    // Test double-buffering status
    int test;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &test);
    if (test != ((init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER)) ? 0 : 1)) {
      if (enabler.is_fullscreen())
        errorlog << "Requested single-buffering not available\n" << flush;
      else
        report_error("OpenGL", "Requested single-buffering not available");
    }

    // (Re)initialize GLEW. Only needs to be done once on linux, on
    // window forgetting will cause crashes.
    glewInit();

    // Set the viewport
    glViewport(0, 0, w, h);

    return true;
  }

  // Vertexes, foreground color, background color, texture coordinates
  GLfloat *vertexes, *fg, *bg, *tex;

  void write_tile_vertexes(GLfloat x, GLfloat y, GLfloat *vertex) {
    vertex[0]  = x;   // Upper left
    vertex[1]  = y;
    vertex[2]  = x+1; // Upper right
    vertex[3]  = y;
    vertex[4]  = x;   // Lower left
    vertex[5]  = y+1;
    vertex[6]  = x;   // Lower left again (triangle 2)
    vertex[7]  = y+1;
    vertex[8]  = x+1; // Upper right
    vertex[9]  = y;
    vertex[10] = x+1; // Lower right
    vertex[11] = y+1;
  }

  virtual void allocate(int tiles) {
    vertexes = static_cast<GLfloat*>(realloc(vertexes, sizeof(GLfloat) * tiles * 2 * 6));
    assert(vertexes);
    fg = static_cast<GLfloat*>(realloc(fg, sizeof(GLfloat) * tiles * 4 * 6));
    assert(fg);
    bg = static_cast<GLfloat*>(realloc(bg, sizeof(GLfloat) * tiles * 4 * 6));
    assert(bg);
    tex = static_cast<GLfloat*>(realloc(tex, sizeof(GLfloat) * tiles * 2 * 6));
    assert(tex);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexes);
  }
  
  virtual void init_opengl() {
    enabler.textures.upload_textures();
    // Allocate array memory
    allocate(gps.dimx * gps.dimy);
    // Initialize the vertex array
    int tile = 0;
    for (GLfloat x = 0; x < gps.dimx; x++)
      for (GLfloat y = 0; y < gps.dimy; y++, tile++)
        write_tile_vertexes(x, y, vertexes + 6*2*tile);
    // Setup invariant state
    glEnableClientState(GL_COLOR_ARRAY);
    // Set up our coordinate system
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, gps.dimx, gps.dimy, 0);
  }

  virtual void uninit_opengl() {
    enabler.textures.remove_uploaded_textures();
  }

  virtual void draw(int vertex_count) {
    // Render the background colors
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glColorPointer(4, GL_FLOAT, 0, bg);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    // Render the foreground, colors and textures both
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glColorPointer(4, GL_FLOAT, 0, fg);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    
    printGLError();
  }

  void write_tile_arrays(int x, int y, GLfloat *fg, GLfloat *bg, GLfloat *tex) {
    struct texture_fullid id = screen_to_texid(x, y);
    const gl_texpos *txt = enabler.textures.gl_texpos;
    // TODO: Only bother to set the one that's actually read in flat-shading mode
    // And set flat-shading mode.
    for (int i = 0; i < 6; i++) {
      *(fg++) = id.r;
      *(fg++) = id.g;
      *(fg++) = id.b;
      *(fg++) = 1;
      
      *(bg++) = id.br;
      *(bg++) = id.bg;
      *(bg++) = id.bb;
      *(bg++) = 1;
    }
    // Set texture coordinates
    *(tex++) = txt[id.texpos].left;   // Upper left
    *(tex++) = txt[id.texpos].bottom;
    *(tex++) = txt[id.texpos].right;  // Upper right
    *(tex++) = txt[id.texpos].bottom;
    *(tex++) = txt[id.texpos].left;   // Lower left
    *(tex++) = txt[id.texpos].top;

    *(tex++) = txt[id.texpos].left;   // Lower left
    *(tex++) = txt[id.texpos].top;
    *(tex++) = txt[id.texpos].right;  // Upper right
    *(tex++) = txt[id.texpos].bottom;
    *(tex++) = txt[id.texpos].right;  // Lower right
    *(tex++) = txt[id.texpos].top;
  }
  
public:
  void update_tile(int x, int y) {
    const int tile = x*gps.dimy + y;
    // Update the arrays
    GLfloat *fg  = this->fg + tile * 4 * 6;
    GLfloat *bg  = this->bg + tile * 4 * 6;
    GLfloat *tex = this->tex + tile * 2 * 6;
    write_tile_arrays(x, y, fg, bg, tex);
  }

  void update_all() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int x = 0; x < gps.dimx; x++)
      for (int y = 0; y < gps.dimy; y++)
        update_tile(x, y);
  }
  
  void render() {
    draw(gps.dimx*gps.dimy*6);
    SDL_GL_SwapBuffers();
  }

  renderer_opengl() {
    // Init member variables so realloc'll work
    screen   = NULL;
    vertexes = NULL;
    fg       = NULL;
    bg       = NULL;
    tex      = NULL;
    
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

    // Initialize opengl
    init_opengl();
  }

  void resize(int w, int h) {
    cout << "New window size: " << w << "x" << h << endl;
    // (Re)calculate grid-size
    const int dispx = enabler.is_fullscreen() ?
      init.font.large_font_dispx :
      init.font.small_font_dispx;
    const int dispy = enabler.is_fullscreen() ?
      init.font.large_font_dispy :
      init.font.small_font_dispy;
    cout << "Font size: " << dispx << "x" << dispy << endl;
    const int gridx = MIN(MAX(w / dispx, MIN_GRID_X), MAX_GRID_X);
    const int gridy = MIN(MAX(h / dispy, MIN_GRID_Y), MAX_GRID_Y);
    cout << "Resizing grid to " << gridx << "x" << gridy << endl;
    // Only reallocate the grid if it actually changes
    if (init.display.grid_x != gridx || init.display.grid_y != gridy)
      gps.allocate(gridx, gridy);
    // But always force a full display cycle
    gps.force_full_display_count = 1;
    // Reinitialize the video
    uninit_opengl();
    init_video(w, h);
    init_opengl();
  }

  void set_fullscreen() {
    resize(init.display.desired_fullscreen_width,
           init.display.desired_fullscreen_height);
  }
};

// Specialization for PARTIAL:0
class renderer_once : public renderer_opengl {
  int tile_count;
  
protected:
  void update_tile(int x, int y) {
    write_tile_vertexes(x, y, vertexes + tile_count * 6 * 2);
    write_tile_arrays(x, y,
                      fg + tile_count * 6 * 4,
                      bg + tile_count * 6 * 4,
                      tex + tile_count * 6 * 2);
    tile_count++;
  }

  void draw(int dummy) {
    renderer_opengl::draw(tile_count*6);
    tile_count = 0;
  }

public:
  renderer_once() {
    renderer_opengl::renderer_opengl();
    tile_count = 0;
  }
};

// PARTIAL:N
class renderer_partial : public renderer_opengl {
//   int buffersz;
//   list<int> eras;
//   int era_count;
//   int tile_count;

//   void update_tile(int x, int y) {
//     if (tile_count == buffersz) {
//       // Buffer is full, expand it.
//       buffersz *= 2;
//       allocate(buffersz);
//     }
//     write_tile_vertexes(x, y, vertexes + tile_count * 6 * 2);
//     write_tile_arrays(x, y,
//                       fg + tile_count * 6 * 4,
//                       bg + tile_count * 6 * 4,
//                       tex + tile_count * 6 * 2);
//     tile_count++;
//   }

//   void draw(int dummy) {
//     renderer_opengl::draw(tile_count*6);
//     eras.push_back(tile_count);
//     if (eras.size() == era_count) {
//       // Right, time to retire the oldest era.
//       int count = eras.front();
//       eras.pop_front();
      
    
  
// public:
//   renderer_partial(int eras) {
//     renderer_opengl::renderer_opengl();
//     allocate(gps.dimx * gps.dimy * 2); // Enough for a full update, and then some.
//     buffersz = gps.dimx * gps.dimy * 2;
//     era_count = eras;
//     tile_count = 0;
//   }
public:
  renderer_partial(int a) {
    renderer_opengl::renderer_opengl();
  }
};

class renderer_accum_buffer : public renderer_once {
  void draw(int vertex_count) {
    // Copy the previous frame's buffer back in
    glAccum(GL_RETURN, 1);
    renderer_once::draw(vertex_count);
    // Store the screen contents back to the buffer
    glAccum(GL_LOAD, 1);
  }
};

class renderer_framebuffer : public renderer_once {
  GLuint framebuffer, fb_texture;
  
  void init_opengl() {
    glGenFramebuffersEXT(1, &framebuffer);
    // Allocate FBO texture memory
    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 screen->w, screen->h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    // Bind texture to FBO
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, fb_texture, 0);
    renderer_once::init_opengl();
  }

  void uninit_opengl() {
    renderer_once::uninit_opengl();
    glDeleteTextures(1, &fb_texture);
    glDeleteFramebuffersEXT(1, &framebuffer);
  }

  void draw(int vertex_count) {
    // Bind the framebuffer
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
    // Draw
    renderer_once::draw(vertex_count);
    // Draw the framebuffer to screen
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer);
    glBlitFramebufferEXT(0,0, screen->w, screen->h,
                         0,0, screen->w, screen->h,
                         GL_COLOR_BUFFER_BIT, GL_NEAREST);
    printGLError();
  }
};
