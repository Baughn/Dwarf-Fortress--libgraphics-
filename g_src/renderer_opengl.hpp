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

  virtual void draw() {
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
  }

  void render() {
    draw();
    // Flip the screen
    if (!enabler.sync && init.display.flag.has_flag(INIT_DISPLAY_FLAG_ARB_SYNC))
      enabler.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);    
    SDL_GL_SwapBuffers();
  }
};

struct renderer_once : public renderer_opengl {
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

  virtual void draw() {
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
    fill = 0;
  }
};

class renderer_accum_buffer : public renderer_once {
public:
  void draw() {
    // Copy the previous frame's buffer back in
    glAccum(GL_RETURN, 1);
    renderer_once::draw();
    // Store the screen contents back to the buffer
    glAccum(GL_LOAD, 1);
  }
};



class renderer_framebuffer : public renderer_once {
  GLuint framebuffer, fb_texture;
  
  void grid_resize(int w, int h) {
    if (fb_texture) {
      glDeleteTextures(1, &fb_texture);
      fb_texture = 0;
    }
    if (framebuffer) {
      glDeleteFramebuffersEXT(1, &framebuffer);
      framebuffer = 0;
    }

    renderer_once::grid_resize(w, h);

    glGenFramebuffersEXT(1, &framebuffer);
    // Allocate FBO texture memory
    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    // Bind texture to FBO
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, fb_texture, 0);
    // And reset to the texture catalog
    glBindTexture(GL_TEXTURE_2D, texture_catalog);
    printGLError();
  }

  void draw() {
    // Bind the framebuffer
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
    // Draw to framebuffer
    renderer_once::draw();
    // Draw the framebuffer to screen
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer);
    glBlitFramebufferEXT(0,0, SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
                         0,0, SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
                         GL_COLOR_BUFFER_BIT, GL_NEAREST);
    printGLError();
  }
};

class renderer_vbo : public renderer_opengl {
  GLuint vbo; // Vertexes only

  void realloc_arrays(int w, int h) {
    if (vbo)
      glDeleteBuffersARB(1, &vbo);
    renderer_opengl::realloc_arrays(w, h);
    glGenBuffersARB(1, &vbo);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * sizeof(GLfloat),
                    &vertices[0], GL_STATIC_DRAW_ARB);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
};
