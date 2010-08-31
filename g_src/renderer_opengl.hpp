#include "renderer.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/circular_buffer.hpp>

using boost::circular_buffer;
using boost::scoped_ptr;

class catalog_fitter {
public:
  struct position {
    int x, y;
  };  
private:
  struct tile {
    int x, y; // Location of this tile
    int w, h; // Size of this tile
    int free_w, free_h; // The largest free space we might possibly have. Could be smaller.
    scoped_ptr<tile> c1, c2; // Children; NULL if we haven't split this tile yet

    bool fit(int req_w, int req_h, position &out) {
      // First, check whether we can possibly fit this.
      if (req_w > free_w || req_h > free_h) return false;
      // We might. Okay, do we have child nodes yet?
      if (!c1 && !c2) {
        // Nope, which means it *will* fit. Check whether it'll fit exactly.
        if (req_w == w && req_h == h) {
          // Yep. We'll mark this tile down as "used", and write the position.
          free_w = free_h = 0;
          out.x = x; out.y = y;
          return true;
        }
        // No, there's extra space. Okay, we'll split this tile.
        if (w > req_w) {
          // Vertically.
          c1.reset(new tile(x, y, req_w, h));
          c2.reset(new tile(x + req_w, y, w - req_w, h));
        } else {
          // No, horizontally.
          c1.reset(new tile(x, y, w, req_h));
          c2.reset(new tile(x, y + req_h, w, h - req_h));
        }
      }
      // Check if a child node will accept the tile
      if ((c1 && c1->fit(req_w, req_h, out)) ||
          (c2 && c2->fit(req_w, req_h, out))) {
        // One of the child nodes accepted. We don't care who, but rewrite free_* accordingly.
        free_w = MAX(c1->free_w, c2->free_w);
        free_h = MAX(c1->free_h, c2->free_h);
        return true;
      }
      // Neither child accepts it. Whoops, free_* gave a false positive.
      return false;
    }

    // Construct a tile based on parameters
    tile(int x, int y, int w, int h) {
      this->x = x; this->y = y;
      this->w = free_w = w;
      this->h = free_h = h;
    }

    // Construct a tile wrapping an inner one. We double one of its dimensions.
    tile(scoped_ptr<tile> &inner) {
      // To avoid having to rewrite every node under inner, we leave it at top left.
      x = 0; y = 0;
      if (inner->w < inner->h) {
        // Double width, split vertically
        w = inner->w * 2;
        h = inner->h;
        c2.reset(new tile(inner->w, 0, inner->w, inner->h));
      } else {
        // Double height, split horizontally
        w = inner->w;
        h = inner->h * 2;
        c2.reset(new tile(0, inner->h, inner->w, inner->h));
      }
      c1.swap(inner);
      free_w = MAX(c1->free_w, c2->free_w);
      free_h = MAX(c1->free_h, c2->free_h);
    }
  };
  scoped_ptr<tile> top;
public:
  position fit(int w, int h) {
    position ret;
    while (!top->fit(w, h, ret)) {
      // Got to extend the catalog.
      top.reset(new tile(top));
    }
    // It finally fit somewhere.
    return ret;
  }
  catalog_fitter() {
    top.reset(new tile(0, 0, 256, 256));
  }
};

class renderer_opengl : public renderer_sdl, public renderer_cpu {
protected:
  struct interleaved_attributes {
    GLfloat fg_red, fg_green, fg_blue, fg_alpha;
    GLfloat bg_red, bg_green, bg_blue, bg_alpha;
    GLfloat tex_u, tex_v;
  };
  struct catalog_position {
    GLfloat s, t, p, q;
  };
  // Generated once per (grid) resize
  vector<GLfloat> vertices;
  GLuint texture_catalog;
  vector<catalog_position> texture_pos;
  // Rewritten every frame
  vector<interleaved_attributes> attributes;

  virtual void realloc_arrays(int w, int h) {
    // The vertex array doesn't change per-frame, so we rebuild it here.
    vertices.clear();
    vertices.reserve(w*h*6*2);
    const float screenw = SDL_GetVideoSurface()->w;
    const float screenh = SDL_GetVideoSurface()->h;
    for (int x=0; x < w; x++) {
      for (int y=0; y < h; y++) {
        auto xpos = [&](int x){return float(x * dispx + originx) / screenw * 2 - 1; };
        auto ypos = [&](int y){return float(y * dispy + originy) / screenh * -2 + 1; };
        vertices.push_back(xpos(x));   // Top left
        vertices.push_back(ypos(y));
        vertices.push_back(xpos(x+1)); // Top right
        vertices.push_back(ypos(y));
        vertices.push_back(xpos(x));   // Bottom left
        vertices.push_back(ypos(y+1));
        vertices.push_back(xpos(x));   // Bottom left
        vertices.push_back(ypos(y+1));
        vertices.push_back(xpos(x+1)); // Top right
        vertices.push_back(ypos(y));
        vertices.push_back(xpos(x+1)); // Bottom right
        vertices.push_back(ypos(y+1));
      }
    }
    glVertexPointer(2, GL_FLOAT, 0, &vertices[0]);
    // The attribute array does change per-frame. We just allocate space.
    attributes.resize(w*h*6);
  }
public:
  renderer_opengl() : renderer_sdl(true) {
    texture_catalog = 0;
  }
  void grid_resize(int w, int h) {
    gps_allocate(w,h);
    // Reset GL parameters
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    realloc_arrays(w,h);
    // Load textures. All the tile textures are the same size, so building the catalog is
    // dead simple. Also, we always make a square catalog.
    int size = sqrt(dispx * dispy * 256);
    while ((size / dispx) * (size / dispy) < 256) size++;
    if (!GL_ARB_texture_non_power_of_two) {
      // This won't work. Try upsizing to a power of two..
      // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
      size--;
      size |= size >> 1;
      size |= size >> 2;
      size |= size >> 4;
      size |= size >> 8;
      size |= size >> 16;
      size++;
    }
    cout << "Using " << size << "x" << size << " texture catalog\n";
    const int sizex = (size / dispx) * dispx,
      sizey = (size / dispy) * dispy;
    SDL_Surface *catalog = SDL_CreateRGBSurface(SDL_SWSURFACE, size, size, 32,
                                                0xff000000,
                                                0x00ff0000,
                                                0x0000ff00,
                                                0x000000ff
                                                // 0x000000ff,
                                                // 0x0000ff00,
                                                // 0x00ff0000,
                                                // 0xff000000
                                                );
                                                
    texture_pos.resize(256);
    for (int i = 0; i < 256; i++) {
      SDL_Surface *tile = enabler.tileset.get_texture(i, dispx, dispy);
      SDL_Rect dst;
      dst.x = (i * dispx) % sizex;
      dst.y = (i * dispx) / sizex * dispy;
      SDL_SetAlpha(tile, 0, 0);
      SDL_BlitSurface(tile, NULL, catalog, &dst);
      SDL_FreeSurface(tile);
      texture_pos[i] = { float(dst.x) / float(size),
                         1 - float(dst.y) / float(size),
                         float(dst.x + dispx) / float(size),
                         1 - float(dst.y + dispy) / float(size) };
    }
    // Throw the catalog at opengl
    glEnable(GL_TEXTURE_2D);
    if (texture_catalog)
      glDeleteTextures(1, &texture_catalog);
    glGenTextures(1, &texture_catalog);
    glBindTexture(GL_TEXTURE_2D, texture_catalog);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    SDL_LockSurface(catalog);
    vector<unsigned char> data;
    data.reserve(catalog->w*catalog->h*4);
    int pitch = catalog->pitch;
    for (int y = catalog->h - 1; y >= 0; y--) {
      for (int x = 0; x < catalog->w; x++) {
        unsigned int pixel = ((unsigned int*)((unsigned char*)catalog->pixels + y*pitch))[x];
        data.push_back((pixel & catalog->format->Rmask) >> catalog->format->Rshift);
        data.push_back((pixel & catalog->format->Gmask) >> catalog->format->Gshift);
        data.push_back((pixel & catalog->format->Bmask) >> catalog->format->Bshift);
        unsigned char alpha = ((pixel & catalog->format->Amask) >> catalog->format->Ashift);
        data.push_back(alpha);
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, &data[0]);
    // FIXME: Check for errors
    SDL_UnlockSurface(catalog);
    SDL_FreeSurface(catalog);
    printGLError();
  }
  // out[0] through out[5] must all be valid.
  void write_attributes(interleaved_attributes * __restrict__ outs, const texture_fullid &id) {
    for (int i = 0; i < 6; i++) {
      interleaved_attributes &out = outs[i];
      out.fg_red   = id.r;
      out.fg_green = id.g;
      out.fg_blue  = id.b;
      out.fg_alpha = 1;
      
      out.bg_red   = id.br;
      out.bg_green = id.bg;
      out.bg_blue  = id.bb;
      out.bg_alpha = 1;

      if (id.texpos < 256) {
        const catalog_position &pos = texture_pos[id.texpos];
        switch (i) {
        case 0:
          out.tex_u = pos.s;
          out.tex_v = pos.t;
          break;
        case 1:
        case 4:
          out.tex_u = pos.p;
          out.tex_v = pos.t;
          break;
        case 2:
        case 3:
          out.tex_u = pos.s;
          out.tex_v = pos.q;
          break;
        case 5:
          out.tex_u = pos.p;
          out.tex_v = pos.q;
          break;
        }
      } else {
        out.tex_u = 0;
        out.tex_v = 0;
      }
    }
  }
  void update_tile(int x, int y) {
    texture_fullid id = screen_to_texid(x, y);
    const int tile = (x * gps.dimy + y) * 6;
    write_attributes(&attributes[tile], id);
  }
  void render() {
    const int vertex_count = gps.dimx * gps.dimy * 6;
    // Draw the background
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glColorPointer(4, GL_FLOAT, sizeof(interleaved_attributes),
                   &(attributes[0].bg_red));
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    printGLError();
    // Draw the foreground, colors and textures both
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexCoordPointer(2, GL_FLOAT, sizeof(interleaved_attributes),
                      &(attributes[0].tex_u));
    glColorPointer(4, GL_FLOAT, sizeof(interleaved_attributes),
                   &(attributes[0].fg_red));
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    printGLError();
    // Flip the screen
    if (!enabler.sync && init.display.flag.has_flag(INIT_DISPLAY_FLAG_ARB_SYNC))
      enabler.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);    
    SDL_GL_SwapBuffers();
  }
};

class renderer_once : public renderer_opengl {
  struct interleaved_attributes {
    GLfloat x, y;
    GLfloat fg_red, fg_green, fg_blue, fg_alpha;
    GLfloat bg_red, bg_green, bg_blue, bg_alpha;
    GLfloat tex_u, tex_v;
  };

  vector<interleaved_attributes> attributes;
  int fill;

  void realloc_arrays(int w, int h) {
    // We reserve sufficient space for a worst-case render. Memory's cheap.
    attributes.resize(w*h*6);
    fill = 0;
  }

  void update_tile(int x, int y) {
    texture_fullid id = screen_to_texid(x, y);
    int screenw = SDL_GetVideoSurface()->w;
    int screenh = SDL_GetVideoSurface()->h;
    auto xpos = [&](int x){return float(x * dispx + originx) / screenw * 2 - 1; };
    auto ypos = [&](int y){return float(y * dispy + originy) / screenh * -2 + 1; };
    for (int i = 0; i < 6; i++) {
      interleaved_attributes &out = attributes[fill++];
      out.fg_red   = id.r;
      out.fg_green = id.g;
      out.fg_blue  = id.b;
      out.fg_alpha = 1;
      
      out.bg_red   = id.br;
      out.bg_green = id.bg;
      out.bg_blue  = id.bb;
      out.bg_alpha = 1;

      if (id.texpos < 256) {
        const catalog_position &pos = texture_pos[id.texpos];
        switch (i) {
        case 0:
          out.tex_u = pos.s;
          out.tex_v = pos.t;
          out.x = xpos(x);
          out.y = ypos(y);
          break;
        case 1:
        case 4:
          out.tex_u = pos.p;
          out.tex_v = pos.t;
          out.x = xpos(x+1);
          out.y = ypos(y);
          break;
        case 2:
        case 3:
          out.tex_u = pos.s;
          out.tex_v = pos.q;
          out.x = xpos(x);
          out.y = ypos(y+1);
          break;
        case 5:
          out.tex_u = pos.p;
          out.tex_v = pos.q;
          out.x = xpos(x+1);
          out.y = ypos(y+1);
          break;
        }
      } else {
        out.tex_u = 0;
        out.tex_v = 0;
      }
    }
  }

  void render() {
    // Draw the background
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glVertexPointer(2, GL_FLOAT, sizeof(interleaved_attributes),
                    &(attributes[0].x));
    glColorPointer(4, GL_FLOAT, sizeof(interleaved_attributes),
                   &(attributes[0].bg_red));
    glDrawArrays(GL_TRIANGLES, 0, fill);
    printGLError();
    // Draw the foreground, colors and textures both
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexCoordPointer(2, GL_FLOAT, sizeof(interleaved_attributes),
                      &(attributes[0].tex_u));
    glColorPointer(4, GL_FLOAT, sizeof(interleaved_attributes),
                   &(attributes[0].fg_red));
    glDrawArrays(GL_TRIANGLES, 0, fill);
    printGLError();
    // Flip the screen
    if (!enabler.sync && init.display.flag.has_flag(INIT_DISPLAY_FLAG_ARB_SYNC))
      enabler.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);    
    SDL_GL_SwapBuffers();
    // Done.
    fill = 0;
  }
};



// // Specialization for PARTIAL:0
// class renderer_once : public renderer_opengl {
//   int tile_count;
  
// protected:
//   void update_tile(int x, int y) {
//     write_tile_vertexes(x, y, vertexes + tile_count * 6 * 2);
//     write_tile_arrays(x, y,
//                       fg + tile_count * 6 * 4,
//                       bg + tile_count * 6 * 4,
//                       tex + tile_count * 6 * 2);
//     tile_count++;
//   }

//   void draw(int dummy) {
//     renderer_opengl::draw(tile_count*6);
//     tile_count = 0;
//   }

// public:
//   renderer_once() {
//     tile_count = 0;
//   }
// };

// // PARTIAL:N
// class renderer_partial : public renderer_opengl {
//   int buffersz;
//   list<int> erasz; // Previous eras
//   int current_erasz; // And the current one
//   int sum_erasz;
//   int head, tail; // First unused tile, first used tile respectively
//   int redraw_count; // Number of eras to max out at

//   void update_tile(int x, int y) {
//     write_tile_vertexes(x, y, vertexes + head * 6 * 2);
//     write_tile_arrays(x, y,
//                       fg + head * 6 * 4,
//                       bg + head * 6 * 4,
//                       tex + head * 6 * 2);
//     head = (head + 1) % buffersz;
//     current_erasz++; sum_erasz++;
//     if (head == tail) {
//       //gamelog << "Expanding partial-printing buffer" << endl;
//       // Buffer is full, expand it.
//       renderer_opengl::allocate(buffersz * 2);
//       // Move the tail to the end of the newly allocated space
//       tail += buffersz;
//       memmove(vertexes + tail * 6 * 2, vertexes + head * 6 * 2, sizeof(GLfloat) * 6 * 2 * (buffersz - head));
//       memmove(fg + tail * 6 * 4, fg + head * 6 * 4, sizeof(GLfloat) * 6 * 4 * (buffersz - head));
//       memmove(bg + tail * 6 * 4, fg + head * 6 * 4, sizeof(GLfloat) * 6 * 4 * (buffersz - head));
//       memmove(tex + tail * 6 * 2, fg + head * 6 * 2, sizeof(GLfloat) * 6 * 2 * (buffersz - head));
//       // And finish.
//       buffersz *= 2;
//     }
//   }

//   void allocate(int tile_count) {
//     assert(false);
//   }
  
//   virtual void reshape_gl() {
//     // TODO: This function is duplicate code w/base class reshape_gl
//     // Setup invariant state
//     glEnableClientState(GL_COLOR_ARRAY);
//     /// Set up our coordinate system
//     if (forced_steps + zoom_steps == 0 &&
//         init.display.flag.has_flag(INIT_DISPLAY_FLAG_BLACK_SPACE)) {
//       size_x = gps.dimx * dispx;
//       size_y = gps.dimy * dispy;
//       off_x = (screen->w - size_x) / 2;
//       off_y = (screen->h - size_y) / 2;
//     } else {
//       // If we're zooming (or just not using black space), we use the
//       // entire window.
//       size_x = screen->w;
//       size_y = screen->h;
//       off_x = off_y = 0;
//     }
//     glViewport(off_x, off_y, size_x, size_y);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluOrtho2D(0, gps.dimx, gps.dimy, 0);
//   }

//   void draw_arrays(GLfloat *vertexes, GLfloat *fg, GLfloat *bg, GLfloat *tex, int tile_count) {
//     // Set vertex pointer
//     glVertexPointer(2, GL_FLOAT, 0, vertexes);
//     // Render the background colors
//     glDisable(GL_TEXTURE_2D);
//     glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//     glDisable(GL_BLEND);
//     glDisable(GL_ALPHA_TEST);
//     glColorPointer(4, GL_FLOAT, 0, bg);
//     glDrawArrays(GL_TRIANGLES, 0, tile_count * 6);
//     // Render the foreground, colors and textures both
//     glEnable(GL_ALPHA_TEST);
//     glAlphaFunc(GL_NOTEQUAL, 0);
//     glEnable(GL_TEXTURE_2D);
//     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glColorPointer(4, GL_FLOAT, 0, fg);
//     glTexCoordPointer(2, GL_FLOAT, 0, tex);
//     glDrawArrays(GL_TRIANGLES, 0, tile_count * 6);
//   }

//   void draw(int dummy) {
//     if (tail > head) {
//       // We're straddling the end of the array, so have to do this in two steps
//       draw_arrays(vertexes + tail * 6 * 2,
//                   fg + tail * 6 * 4,
//                   bg + tail * 6 * 4,
//                   tex + tail * 6 * 2,
//                   buffersz - tail);
//       draw_arrays(vertexes, fg, bg, tex, head-1);
//     } else {
//       draw_arrays(vertexes + tail * 6 * 2,
//                   fg + tail * 6 * 4,
//                   bg + tail * 6 * 4,
//                   tex + tail * 6 * 2,
//                   sum_erasz);
//     }
    
//     printGLError();
//     erasz.push_back(current_erasz); current_erasz = 0;
//     if (erasz.size() == redraw_count) {
//       // Right, time to retire the oldest era.
//       tail = (tail + erasz.front()) % buffersz;
//       sum_erasz -= erasz.front();
//       erasz.pop_front();
//     }
//   }
  
// public:
//   renderer_partial() {
//     redraw_count = init.display.partial_print_count;
//     buffersz = 2048;
//     renderer_opengl::allocate(buffersz);
//     current_erasz = head = tail = sum_erasz = 0;
//   }
// };

// class renderer_accum_buffer : public renderer_once {
//   void draw(int vertex_count) {
//     // Copy the previous frame's buffer back in
//     glAccum(GL_RETURN, 1);
//     renderer_once::draw(vertex_count);
//     // Store the screen contents back to the buffer
//     glAccum(GL_LOAD, 1);
//   }
// };

// class renderer_framebuffer : public renderer_once {
//   GLuint framebuffer, fb_texture;
  
//   void init_opengl() {
//     glGenFramebuffersEXT(1, &framebuffer);
//     // Allocate FBO texture memory
//     glGenTextures(1, &fb_texture);
//     glBindTexture(GL_TEXTURE_2D, fb_texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
//                  screen->w, screen->h,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
//     // Bind texture to FBO
//     glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
//     glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
//                               GL_TEXTURE_2D, fb_texture, 0);
//     renderer_once::init_opengl();
//   }

//   void uninit_opengl() {
//     renderer_once::uninit_opengl();
//     glDeleteTextures(1, &fb_texture);
//     glDeleteFramebuffersEXT(1, &framebuffer);
//   }

//   void draw(int vertex_count) {
//     // Bind the framebuffer
//     glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
//     glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
//     // Draw
//     renderer_once::draw(vertex_count);
//     // Draw the framebuffer to screen
//     glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
//     glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer);
//     glBlitFramebufferEXT(0,0, screen->w, screen->h,
//                          0,0, screen->w, screen->h,
//                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
//     printGLError();
//   }
// };

// class renderer_vbo : public renderer_opengl {
//   GLuint vbo; // Vertexes only

//   void init_opengl() {
//     renderer_opengl::init_opengl();
//     glGenBuffersARB(1, &vbo);
//     glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
//     glBufferDataARB(GL_ARRAY_BUFFER_ARB, gps.dimx*gps.dimy*6*2*sizeof(GLfloat), vertexes, GL_STATIC_DRAW_ARB);
//     glVertexPointer(2, GL_FLOAT, 0, 0);
//     glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
//   }
  
//   void uninit_opengl() {
//     glDeleteBuffersARB(1, &vbo);
//     renderer_opengl::uninit_opengl();
//   }
// };
