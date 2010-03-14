
class renderer_opengl : public renderer_sdl {
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

  void write_tile(GLfloat x, GLfloat y, GLfloat *vertex) {
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
  
  void init_opengl() {
    enabler.textures.upload_textures();
    // Allocate array memory
    vertexes = new GLfloat[gps.dimx * gps.dimy * 2 * 6];
    fg = new GLfloat[gps.dimx * gps.dimy * 4 * 6];
    bg = new GLfloat[gps.dimx * gps.dimy * 4 * 6];
    tex = new GLfloat[gps.dimx * gps.dimy * 2 * 6];
    // Initialize the vertex array
    int tile = 0;
    for (GLfloat x = 0; x < gps.dimx; x++)
      for (GLfloat y = 0; y < gps.dimy; y++, tile++)
        write_tile(x, y, vertexes + 6*2*tile);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertexes);
    // Setup invariant state
    glEnableClientState(GL_COLOR_ARRAY);
    // Set up our coordinate system
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, gps.dimx, gps.dimy, 0);
  }

  void uninit_opengl() {
    enabler.textures.remove_uploaded_textures();
    delete[] vertexes;
    delete[] fg;
    delete[] bg;
    delete[] tex;
  }
  
public:
  void update_tile(int x, int y) {
    struct texture_fullid id = screen_to_texid(x, y);
    const int tile = x*gps.dimy + y;
    // Update the arrays
    GLfloat *fg  = this->fg + tile * 4 * 6;
    GLfloat *bg  = this->bg + tile * 4 * 6;
    GLfloat *tex = this->tex + tile * 2 * 6;
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

  void update_all() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int x = 0; x < gps.dimx; x++)
      for (int y = 0; y < gps.dimy; y++)
        update_tile(x, y);
  }

  void render() {
    // Render the background colors
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glColorPointer(4, GL_FLOAT, 0, bg);
    glDrawArrays(GL_TRIANGLES, 0, gps.dimx*gps.dimy*6);
    // Render the foreground, colors and textures both
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glColorPointer(4, GL_FLOAT, 0, fg);
    glDrawArrays(GL_TRIANGLES, 0, gps.dimx*gps.dimy*6);

    
    printGLError();
    SDL_GL_SwapBuffers();
  }

  renderer_opengl() {
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
