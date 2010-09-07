#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <map>
#include <cassert>
#include <iostream>
using std::string;

#include "GL/glew.h"
#include "g_basics.h"
#include "platform.h"
#include "basics.h"
#include "endian.h"

enum Texture
{
	TEXTURE_MOUSE,
	TEXTURENUM
};

class graphicst
{
  int lookup_pair(std::pair<int,int> color);
  long calculate_old_fps();
 public:
  long screenx,screeny;
  char screenf,screenb;

  // Four bytes per tile. In machine order, but if that were LE:
  // Bytes 0-2: Tile index
  // Byte  3, bits 0-3: Foreground color
  //          bits 4-7: Background color
  // After that, the entire grid is in Y-major column order.
  unsigned int *screen;

  void resize(int x, int y);
                                
  long clipx[2],clipy[2];
  long tex_pos[TEXTURENUM];

  long rect_id;

  long print_index;
  char display_frames;

  short force_full_display_count;

  char original_rect;

  int dimx, dimy;

  graphicst()
    {
      print_index=0;
      display_frames=0;
      rect_id=-1;
      force_full_display_count=4;
      original_rect=1;

      screen = NULL;
    }

  void locate(long y,long x)
  {
    // No point in clamping here, addchar clamps too.
    screenx=x;
    screeny=y;
  }
  void changecolor(short f,short b,char bright=0)
  {
    screenf = f + bright * 8;
    screenb = b;
  }
  void addchar(unsigned char c,char advance=1)
  {
    unsigned int *s = screen + screenx*dimy + screeny;
    if (s >= screen && s < screen_limit) {
      *s = c | ((screenf & 0xf) << 24) | ((screenb & 0xf) << 28);
    }
    screenx += advance;
  }
  void addchar(unsigned int x, unsigned int y, unsigned char c,
               unsigned char f, unsigned char b, unsigned char bright) {
    f += bright*8;
    unsigned int *s = screen + x*dimy + y;
    if (s >= screen && s < screen_limit) {
      *s = c | ((f & 0xf) << 24) | ((b & 0xf) << 28);
    }
  }
  void addcoloredst(const char *str,const char *colorstr);
  // With justify_left, the set location is treated as the left edge of the text.
  // With justify_right, the right edge.
  // With justify_center.. well, guess what? The center.
  void addst(const string &str, justification just = justify_left);
  void addst(const char *str, justification just = justify_left);
  void erasescreen_clip();
  void erasescreen();
  void erasescreen_rect(int x1, int x2, int y1, int y2);
  void setclipping(long x1,long x2,long y1,long y2);

  void add_tile(long texp,char addcolor);
  void add_tile_grayscale(long texp,char cf,char cbr);

  void prepare_graphics(string &src_dir);

  void gray_out_rect(long sx,long ex,long sy,long ey)
  {
    int x,y;
    for(x=sx;x<=ex;x++)
      {
        for(y=sy;y<=ey;y++)
          {
            int t = x*dimy + y;
            screen[t] = (screen[t] & 0x00ffffff) | (0x07 << 24);
          }
      }
  }
  void dim_colors(long x,long y,char dim);

  void rain_color_square(long x,long y);
  void snow_color_square(long x,long y);
  void color_square(long x,long y,unsigned char f,unsigned char b,unsigned char br);

  long border_start_x(){return 1;}
  long border_start_y(){return 1;}
  long border_end_x(){return 78;}
  long border_end_y(){return 23;}
  long text_width(){return 1;}
  long text_height(){return 1;}
  long window_element_height(long minus,char border)
  {
    long height=25;
    if(border)height-=2;
    height-=text_height()*minus;
    return height;
  }

  int mouse_x, mouse_y;
  void get_mouse_text_coords(int32_t &mx, int32_t &my);
  void draw_border(int x1, int x2, int y1, int y2);

  // Instead of doing costly bounds-checking calculations, we cache the end
  // of the array..
  unsigned int *screen_limit;
};

extern graphicst gps;
// From graphics.cpp
void render_things();

// Locates some area of the screen with free space for writing
class gps_locator {
  int y, last_x;
public:
  gps_locator(int y, int x) {
    this->y = y;
    last_x = x;
  }
  bool is_free(int x) {
    unsigned int c = gps.screen[x*gps.dimy + y] & 0x00ffffff;
    switch (c) {
    case 0:
    case 20:
    case 176:
    case 177:
    case 178:
    case 219:
    case 254:
    case 255:
      return true;
    default:
      return false;
    }
  }
  void operator()(int sz) {
    // First, check if our cached slot will still do
    bool ok = true;
    for (int x = last_x; x < last_x + sz; x++)
      if (!is_free(x)) {
        ok = false;
        break;
      }
    if (ok) {
      // Yeah, okay
      gps.locate(y, last_x);
    } else {
      // Not so okay. Find a new spot.
      int run = 0, x = 0;
      for (; x < gps.dimx; x++) {
        if (is_free(x))
          run++;
        else run = 0;
        if (run > sz + 2) { // We pad things a bit for cleanliness.
          ok = true;
          x -= sz + 1;
          break;
        }
      }
      if (ok) {
        // Found a new spot.
        last_x = x;
        gps.locate(y, x);
      } else {
        // Damn it.
        gps.locate(y, last_x);
      }
    }
  }
};

#endif
