#ifndef SHADERS_HPP
#define SHADERS_HPP

struct shaderst {
  // Shader objects
  GLuint v, g, f, p;
  
  void ensource(GLuint shader, const string &file) {
    renderer_sdl *renderer = dynamic_cast<renderer_sdl*>(enabler.renderer);
    SDL_Surface *screen = SDL_GetVideoSurface();
    printGLError();
    cout << "Opening shader " << file << endl;
    ifstream s(file);
    vector<const GLchar*> lines;
    list<string> slurp;
    while (s.good()) {
      string line;
      getline(s, line);
      if (line == "// Inserted by DF:") {
        ostringstream s;
        // Geometry
        s << "#define GRID_X " << gps.dimx << endl;
        s << "#define GRID_Y " << gps.dimy << endl;
        s << "#define DISP_X " << renderer->dispx << endl;
        s << "#define DISP_Y " << renderer->dispy << endl;
        s << "#define DISP_X_SCR " << 2 / double(gps.dimx) << endl;
        s << "#define DISP_Y_SCR " << -2 / double(gps.dimy) << endl;
        // Color palette
        s << "const vec4 colors[] = vec4[16](";
        for (int i=0; i<16; i++) {
          float r = enabler.ccolor[i][0];
          float g = enabler.ccolor[i][1];
          float b = enabler.ccolor[i][2];
          s << "vec4(" << r << "," << g << "," << b << ",1.0)";
          if (i < 15)
            s << ", ";
          else
            s << ");" << endl;
        }
        slurp.push_back(s.str());
      } else {
        slurp.push_back(line);
      }
    }
    for (auto it = slurp.begin(); it != slurp.end(); ++it) {
      GLchar *cline = new GLchar[it->size() + 2];
      memcpy(cline, &(*it)[0], it->size());
      cline[it->size()] = '\n';
      cline[it->size()+1] = 0;
      lines.push_back(cline);
    }
    glShaderSource(shader, lines.size(), &lines[0], NULL);
    glCompileShader(shader);
    printGLError();
    for (auto it = lines.begin(); it != lines.end(); ++it) {
      delete[] *it;
    }
  }
      
  void load() {
    if (p) unload();
    
    // Create, load, compile and link shader
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    g = glCreateShader(GL_GEOMETRY_SHADER);
    p = glCreateProgram();

    glAttachShader(p, v);
    glAttachShader(p, f);
    glAttachShader(p, g);

    ensource(v, enabler.tileset.vertex_shader);
    ensource(f, enabler.tileset.fragment_shader);
    ensource(g, enabler.tileset.geometry_shader);

    glLinkProgram(p);

    // (Re)create the log
    ostringstream s;
    GLchar buffer[65535];
    GLsizei length;

    glGetProgramInfoLog(p, 65535, &length, buffer);
    buffer[length] = 0;
    s << buffer << endl;

    logstr = s.str();
    if (!valid()) {
      cout << "Shader compilation failed:" << endl << logstr;
      exit(EXIT_FAILURE);
    }
    glUseProgram(p);
  }

  void unload() {
    if (p) {
      glDeleteShader(v);
      glDeleteShader(f);
      glDeleteShader(g);
      glDeleteProgram(p);
      glUseProgram(0);
      p = 0;
    }
  }
  
  shaderst() {
    v = f = p = 0;
    GLint max;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
    cout << "Max " << max << " vertex attribute arrays" << endl;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max);
    cout << "Max " << max << " texture image units" << endl;
  }
  ~shaderst() { unload(); }
  void reload() { load(); }
  bool valid() {
    GLint ok;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    return ok == GL_TRUE;
  }
  // A log of the last load operation
  string logstr;
  // To get uniform locations
  GLint uniform(const char *name) {
    GLint i = glGetUniformLocation(p, name);
    // if (i == -1)
    //   cout << "Uniform " << name << " not found!" << endl;
    return i;
  }
  // And attribs
  GLint attrib(const char *name) {
    GLint i = glGetAttribLocation(p, name);
    if (i == -1)
      cout << "Attribute " << name << " not found!" << endl;
    return i;
  }
};

#endif
