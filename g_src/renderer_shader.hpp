#include "shaders.hpp"

class renderer_shader : public renderer_sdl {
  shaderst shader;
  // Buffer object corresponding to gps.screen
  GLuint screen, tbo;
  GLsizeiptr screen_sz;
  // Attributes
  GLuint attributes;
  // Tileset texture arrays
  vector<GLuint> textures;
  
public:
  renderer_shader() : renderer_sdl(true) {
    screen = tbo = attributes = 0;
  }

  struct interleaved_attributes {
    GLfloat vx, vy;
    GLint x, y;
  } __attribute__((packed));

  void grid_resize(int w, int h) {
    cout << "Resizing grid to " << w << "x" << h << endl;
    gps.resize(w, h);

    // (Re)allocate GL buffers
    if (screen) {
      if (gps.screen)
        glUnmapBuffer(GL_TEXTURE_BUFFER);
      glDeleteBuffers(1, &screen);
    }
    if (tbo)
      glDeleteBuffers(1, &tbo);
    if (attributes)
      glDeleteBuffers(1, &attributes);
    glDeleteTextures(textures.size(), &textures[0]);
    textures.clear();

    // Allocate screen buffer(s)
    glGenBuffers(1, &screen);
    glBindBuffer(GL_TEXTURE_BUFFER, screen);
    screen_sz = w*h*sizeof(unsigned int);
    glBufferData(GL_TEXTURE_BUFFER, screen_sz, NULL, GL_STREAM_DRAW);
    // printGLError();
    gps.screen = (unsigned int*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, screen_sz,
                                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    printGLError();
    assert(gps.screen);
    glGenTextures(1, &tbo);
    printGLError();

    // Load shaders
    shader.reload();

    // Load texture arrays
    auto arrays = enabler.tileset.get_all_textures(dispx, dispy);
    int texture_unit = 1; // We start at 1 because 0 is taken by tile_array.
    for (auto it = arrays.begin(); it != arrays.end(); ++it) {
      string name = it->first;
      // First, make sure our current shader wants this.
      GLint uniform = shader.uniform(name.c_str());
      printGLError();
      if (uniform == -1) {
        cout << "Unwanted tile array " << name << endl;
        continue;
      }
      // And that the array *has* textures.
      if (!it->second.size()) {
        report_error("Empty tile array", name.c_str());
        continue;
      }
      // Good, generate and bind a texture.
      glActiveTexture(GL_TEXTURE0 + texture_unit);
      printGLError();
      glUniform1i(uniform, texture_unit);
      printGLError();
      texture_unit++;
      GLuint tex;
      glGenTextures(1, &tex);
      textures.push_back(tex);
      glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
      // Fill that texture.
      int layers = it->second.size();
      int width  = it->second[0]->w;
      int height = it->second[0]->h;
      vector<unsigned char> data;
      data.reserve(width*height*layers*4);
      for (auto tile = it->second.begin(); tile != it->second.end(); ++tile) {
        SDL_LockSurface(*tile);
        int pitch = (*tile)->pitch;
        for (int y = height-1; y >= 0; y--) {
          for (int x = 0; x < width; x++) {
            for (int c = 0; c < 4; c++) {
              data.push_back(((unsigned char*)(*tile)->pixels)
                             [y * pitch + x*4 + c]);
              // data.push_back(0x77);
            }
          }
        }
        SDL_UnlockSurface(*tile);
        SDL_FreeSurface(*tile);
      }
      glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
      glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, layers, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, &data[0]);
      // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
      printGLError();
    }

    // Construct attribute arrays.
    vector<interleaved_attributes> points;
    points.reserve(w*h*6);
    for (int x=0; x < w; x++) {
      for (int y=0; y < h; y++) {
        // Top left
        points.push_back(interleaved_attributes({float(x) / w * 2 - 1,   float(y) / h * -2 + 1,   x, y}));
        // Top right
        points.push_back(interleaved_attributes({float(x+1) / w * 2 - 1, float(y) / h * -2 + 1,   x, y}));
        // Bottom left
        points.push_back(interleaved_attributes({float(x) / w * 2 - 1,   float(y+1) / h * -2 + 1, x, y}));
        // Bottom left
        points.push_back(interleaved_attributes({float(x) / w * 2 - 1,   float(y+1) / h * -2 + 1, x, y}));
        // Top right
        points.push_back(interleaved_attributes({float(x+1) / w * 2 - 1, float(y) / h * -2 + 1,   x, y}));
        // Bottom right
        points.push_back(interleaved_attributes({float(x+1) / w * 2 - 1, float(y+1) / h * -2 + 1, x, y}));
      }
    }
    // And bind them.
    glGenBuffers(1, &attributes);
    glBindBuffer(GL_ARRAY_BUFFER, attributes);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(interleaved_attributes), &points[0],GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(interleaved_attributes), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_INT, sizeof(interleaved_attributes),
                           (GLvoid*)offsetof(interleaved_attributes,x));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    printGLError();
  }
  
  void swap_arrays() {}
  void prepare() {
    if (!gps.screen) {
      glActiveTexture(GL_TEXTURE0);
      gps.screen = (unsigned int*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, screen_sz,
                                                   GL_MAP_WRITE_BIT //| GL_MAP_READ_BIT);
                                                   | GL_MAP_INVALIDATE_BUFFER_BIT);
      assert(gps.screen);
    }
  }
  void display() {
    // Unmap the screen buffer, then attach it to a texture
    assert(gps.screen);
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_TEXTURE_BUFFER, screen);
    glUnmapBuffer(GL_TEXTURE_BUFFER);
    glBindTexture(GL_TEXTURE_BUFFER, tbo);
    glTexBuffer(GL_TEXTURE_BUFFER,
                GL_R32UI,
                screen);
    printGLError();
    gps.screen = NULL;
  }
  void render() {
    // Draw. First, the tiles.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1f(shader.uniform("time"), float(SDL_GetTicks()) / 1000.0);
    glUniform1i(shader.uniform("tile_array"), 0);

    glDrawArrays(GL_TRIANGLES, 0, gps.dimx * gps.dimy * 6);
    printGLError();
    
    enabler.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);    
    SDL_GL_SwapBuffers();
    printGLError();

    // Finally, map a new screen buffer
    printGLError();
  }
};
