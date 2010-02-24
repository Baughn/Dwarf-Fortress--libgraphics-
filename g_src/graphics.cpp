#include "platform.h"
#include <string.h>
#include <math.h>
#include <iosfwd>
#include <iostream>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <zlib.h>

#include "svector.h"

#include "random.h"

#include "endian.h"

#include "files.h"

#include "textlines.h"

#include "enabler.h"

#include "find_files.h"

#include "basics.h"

#include "g_basics.h"

#include "texture_handler.h"

#include "graphics.h"

#include "music_and_sound_g.h"

#include "init.h"

#include "interface.h"

#include "curses.h"

using namespace std;


#pragma comment( lib, "opengl32.lib" )			// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )				// Search For GLu32.lib While Linking

extern enablerst enabler;
extern texture_handlerst texture;
extern graphicst gps;
extern interfacest gview;

void process_object_lines(textlinesst &lines,string &chktype);


static std::map<std::pair<int,int>,int> color_pairs; // For curses. MOVEME.
static unsigned char screen_old[MAX_GRID_X][MAX_GRID_Y][4]; // For partial printing.
static long screentexpos_old[MAX_GRID_X][MAX_GRID_Y];
static char screentexpos_addcolor_old[MAX_GRID_X][MAX_GRID_Y];
static unsigned char screentexpos_grayscale_old[MAX_GRID_X][MAX_GRID_Y];
static unsigned char screentexpos_cf_old[MAX_GRID_X][MAX_GRID_Y];
static unsigned char screentexpos_cbr_old[MAX_GRID_X][MAX_GRID_Y];


void graphicst::addcoloredst(const char *str,const char *colorstr)
{
	int s;
	for(s=0;s<strlen(str)&&screenx<init.display.grid_x;s++)
          {
            if(screenx<0)
              {
                s-=screenx;
                screenx=0;
                if(s>=strlen(str))break;
              }
            
            changecolor((colorstr[s] & 7),((colorstr[s] & 56))>>3,((colorstr[s] & 64))>>6);
            addchar(str[s]);
          }
}

void graphicst::addst(const string &str)
{
	int s;
	for(s=0;s<str.length()&&screenx<init.display.grid_x;s++)
		{
		if(screenx<0)
			{
			s-=screenx;
			screenx=0;
			if(s>=str.length())break;
			}

		addchar(str[s]);
		}
}

void graphicst::addst(const char *str)
{
	int s;
	for(s=0;s<strlen(str)&&screenx<init.display.grid_x;s++)
		{
		if(screenx<0)
			{
			s-=screenx;
			screenx=0;
			if(s>=strlen(str))break;
			}

		addchar(str[s]);
		}
}

void graphicst::erasescreen_clip()
{
	changecolor(0,0,0);
	short x2,y2;
	for(x2=clipx[0];x2<=clipx[1];x2++)
		{
		for(y2=clipy[0];y2<=clipy[1];y2++)
			{
			locate(y2,x2);
			addchar(' ');
			}
		}
}

void graphicst::erasescreen_rect(int x1, int x2, int y1, int y2)
{ 
  changecolor(0,0,0);
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      locate(y, x);
      addchar(' ');
    }
  }
}

void graphicst::erasescreen()
{
	changecolor(0,0,0);
	short x2,y2;
	for(x2=0;x2<init.display.grid_x;x2++)
		{
		for(y2=0;y2<init.display.grid_y;y2++)
			{
			locate(y2,x2);
			addchar(' ');
			}
		}

	gridrectst *rect=enabler.get_gridrect(rect_id);
	if(rect!=NULL)rect->trinum=0;
}

void graphicst::display()
{
  gridrectst *rect=enabler.get_gridrect(rect_id);

  if(init.display.flag.has_flag(INIT_DISPLAY_FLAG_BLACK_SPACE))rect->black_space=1;
  else rect->black_space=0;

  long localbufsize=rect->dimx*rect->dimy;
  long d=0;

  if(enabler.create_full_screen)
    {
      rect->dispx=init.font.large_font_dispx;
      rect->dispy=init.font.large_font_dispy;
    }
  else
    {
      rect->dispx=init.font.small_font_dispx;
      rect->dispy=init.font.small_font_dispy;
    }

  long *c_buffer_texpos;
  float *c_buffer_r;
  float *c_buffer_g;
  float *c_buffer_b;
  float *c_buffer_br;
  float *c_buffer_bg;
  float *c_buffer_bb;

  int x2,y2;
  for (x2=0; x2<init.display.grid_x; x2++) {
    for (y2=0; y2<init.display.grid_y; y2++,d++) {
      if (d>=localbufsize) break;
      // Partial printing (and color-conversion): Big-ass if.
      if (screen[x2][y2][0] == screen_old[x2][y2][0] &&
          screen[x2][y2][1] == screen_old[x2][y2][1] &&
          screen[x2][y2][2] == screen_old[x2][y2][2] &&
          screen[x2][y2][3] == screen_old[x2][y2][3] &&
          screentexpos[x2][y2] == screentexpos_old[x2][y2] &&
          screentexpos_addcolor[x2][y2] == screentexpos_addcolor_old[x2][y2] &&
          screentexpos_grayscale[x2][y2] == screentexpos_grayscale_old[x2][y2] &&
          screentexpos_cf[x2][y2] == screentexpos_cf_old[x2][y2] &&
          screentexpos_cbr[x2][y2] == screentexpos_cbr_old[x2][y2] &&
          !force_full_display_count) { // Nothing's changed
      } else {
        // Okay, the buffer has changed. First update the old-buffer.
        screen_old[x2][y2][0] = screen[x2][y2][0];
        screen_old[x2][y2][1] = screen[x2][y2][1];
        screen_old[x2][y2][2] = screen[x2][y2][2];
        screen_old[x2][y2][3] = screen[x2][y2][3];
        screentexpos_old[x2][y2] = screentexpos[x2][y2];
        screentexpos_addcolor_old[x2][y2] = screentexpos_addcolor[x2][y2];
        screentexpos_grayscale_old[x2][y2] = screentexpos_grayscale[x2][y2];
        screentexpos_cf_old[x2][y2] = screentexpos_cf[x2][y2];
        screentexpos_cbr_old[x2][y2] = screentexpos_cbr[x2][y2];
        // Now, then. We need to convert the buffer contents to texture positions and RGB colors..

        c_buffer_texpos = &(rect->buffer_texpos[d]);
        c_buffer_r = &(rect->buffer_r[d]);
        c_buffer_g = &(rect->buffer_g[d]);
        c_buffer_b = &(rect->buffer_b[d]);
        c_buffer_br = &(rect->buffer_br[d]);
        c_buffer_bg = &(rect->buffer_bg[d]);
        c_buffer_bb = &(rect->buffer_bb[d]);

        // Mark this tile as "drawing required".
        if (rect->s_buffer_count[d] < 0) // If 0 or above, it's already in.
          rect->s_buffer_tiles.push_front(d);
        rect->s_buffer_count[d] = init.display.partial_print_count;
        
        //CONVERT COLORS
        convert_to_rgb(*c_buffer_br,*c_buffer_bg,*c_buffer_bb,screen[x2][y2][2],0);
        
        if (screentexpos[x2][y2]!=0) {
          if(screentexpos_grayscale[x2][y2]) {
            convert_to_rgb(*c_buffer_r,*c_buffer_g,*c_buffer_b,screentexpos_cf[x2][y2],screentexpos_cbr[x2][y2]);
          } else if (screentexpos_addcolor[x2][y2]) {
            convert_to_rgb(*c_buffer_r,*c_buffer_g,*c_buffer_b,screen[x2][y2][1],screen[x2][y2][3]);
          } else {
            *c_buffer_r=1;
            *c_buffer_g=1;
            *c_buffer_b=1;
          }
          *c_buffer_texpos=screentexpos[x2][y2];
        } else {
          convert_to_rgb(*c_buffer_r,*c_buffer_g,*c_buffer_b,screen[x2][y2][1],screen[x2][y2][3]);
          if(enabler.create_full_screen)*c_buffer_texpos=init.font.large_font_texpos[screen[x2][y2][0]];
          else *c_buffer_texpos=init.font.small_font_texpos[screen[x2][y2][0]];
        }
      }
    }
  }
}

#ifdef CURSES
// Map from DF color to ncurses color
static int ncurses_map_color(int color) {
  if (color < 0) abort();
  switch (color) {
  case 0: return 0;
  case 1: return 4;
  case 2: return 2;
  case 3: return 6;
  case 4: return 1;
  case 5: return 5;
  case 6: return 3;
  case 7: return 7;
  default: return ncurses_map_color(color - 7);
  }
}

int graphicst::lookup_pair(pair<int,int> color) {
  map<pair<int,int>,int>::iterator it = color_pairs.find(color);
  if (it != color_pairs.end()) return it->second;
  // We don't already have it. Generate a new pair if possible.
  if (color_pairs.size() < COLOR_PAIRS - 1) {
    const short pair = color_pairs.size() + 1;
    init_pair(pair, ncurses_map_color(color.first), ncurses_map_color(color.second));
    color_pairs[color] = pair;
    return pair;
  }
  // We don't have it, and there's no space for more. Panic!
  endwin();
  puts("Ran out of space for color pairs! Ask Baughn to implement a fallback!");
  exit(EXIT_FAILURE);
}

// see: http://dwarffortresswiki.net/index.php/Character_table
static int charmap[256] = {
  0x0000, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
  0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
  /* 0x20 */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2302,
  /* 0x80 */
  0xC7, 0xFC, 0xE9, 0xE2, 0xE4, 0xE0, 0xE5, 0xE7,
    0xEA, 0xEB, 0xE8, 0xEF, 0xEE, 0xEC, 0xC4, 0xC5,
  0xC9, 0xE6, 0xC6, 0xF4, 0xF6, 0xF2, 0xFB, 0xF9,
    0xFF, 0xD6, 0xDC, 0xA2, 0xA3, 0xA5, 0x20A7, 0x192,
  0xE1, 0xED, 0xF3, 0xFA, 0xF1, 0xD1, 0xAA, 0xBA,
    0xBF, 0x2310, 0xAC, 0xBD, 0xBC, 0xA1, 0xAB, 0xBB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
    0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
    0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
  0x3B1, 0xDF/*yay*/, 0x393, 0x3C0, 0x3A3, 0x3C3, 0xB5, 0x3C4,
    0x3A6, 0x398, 0x3A9, 0x3B4, 0x221E, 0x3C6, 0x35B, 0x2229,
  0x2261, 0xB1, 0x2265, 0x2264, 0x2320, 0x2321, 0xF7, 0x2248,
    0xB0, 0x2219, 0xB7, 0x221A, 0x207F, 0xB2, 0x25A0, 0xA0
};
#endif

void graphicst::renewscreen()
{
  if(display_frames && !enabler.doing_buffer_draw())
    {
      ostringstream fps;
      fps << "FPS: " << setw(3) << enabler.calculate_fps() << " (" << enabler.calculate_gfps() << ")";
      changecolor(7,3,1);
      locate(0,init.display.grid_x * 2 / 3);
      addst(fps.str());
    }

#ifdef CURSES
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {    
    int x2,y2;
    if (force_full_display_count > 0) {
      force_full_display_count--;
      for (x2=0;x2<init.display.grid_x;x2++) {
          for(y2=0;y2<init.display.grid_y;y2++) {
            const int ch   = screen[x2][y2][0];
            const int fg   = screen[x2][y2][1];
            const int bg   = screen[x2][y2][2];
            const int bold = screen[x2][y2][3];
            screen_old[x2][y2][0] = ch;
            screen_old[x2][y2][1] = fg;
            screen_old[x2][y2][2] = bg;
            screen_old[x2][y2][3] = bold;
            const string utf8 = encode_utf8(charmap[ch] ? charmap[ch] : ch);
            const int pair = lookup_pair(make_pair<int,int>(fg,bg));
            attron(COLOR_PAIR(pair));
            if (bold) attron(A_BOLD);
            mvprintw(y2, x2, "%s", utf8.c_str());
            if (bold) attroff(A_BOLD);
          }
      }
    } else {
      for (x2=0;x2<init.display.grid_x;x2++) {
        for(y2=0;y2<init.display.grid_y;y2++) {
          const int ch   = screen[x2][y2][0];
          const int fg   = screen[x2][y2][1];
          const int bg   = screen[x2][y2][2];
          const int bold = screen[x2][y2][3];
          if (screen_old[x2][y2][0] != ch ||
              screen_old[x2][y2][1] != fg ||
              screen_old[x2][y2][2] != bg ||
              screen_old[x2][y2][3] != bold) {
            screen_old[x2][y2][0] = ch;
            screen_old[x2][y2][1] = fg;
            screen_old[x2][y2][2] = bg;
            screen_old[x2][y2][3] = bold;
            const string utf8 = encode_utf8(charmap[ch] ? charmap[ch] : ch);
            const int pair = lookup_pair(make_pair<int,int>(fg,bg));
            attron(COLOR_PAIR(pair));
            if (bold) attron(A_BOLD);
            mvprintw(y2, x2, "%s", utf8.c_str());
            if (bold) attroff(A_BOLD);
          }
        }
      }
    }
  } else
#endif
    {
    display();
    enabler.flag|=ENABLERFLAG_RENDER;
  }
}

void graphicst::setclipping(long x1,long x2,long y1,long y2)
{
	if(x1<0)x1=0;
	if(x2>init.display.grid_x-1)x2=init.display.grid_x-1;
	if(y1<0)y1=0;
	if(y2>init.display.grid_y-1)y2=init.display.grid_y-1;

	clipx[0]=x1;
	clipx[1]=x2;
	clipy[0]=y1;
	clipy[1]=y2;
}

void graphicst::dim_colors(long x,long y,char dim)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		switch(dim)
			{
			case 4:
				switch(screen[x][y][2])
					{
					case 4:
					case 5:
					case 6:
						screen[x][y][2]=1;
						break;
					case 2:
					case 7:
						screen[x][y][2]=3;
						break;
					}
				switch(screen[x][y][1])
					{
					case 4:
					case 5:
					case 6:
						screen[x][y][1]=1;
						break;
					case 2:
					case 7:
						screen[x][y][1]=3;
						break;
					}
				if(screen[x][y][1]==screen[x][y][2])screen[x][y][1]=0;
				screen[x][y][3]=0;
				if(screen[x][y][1]==0&&screen[x][y][2]==0&&screen[x][y][3]==0)screen[x][y][3]=1;
				break;
			case 3:
				switch(screen[x][y][2])
					{
					case 4:
					case 5:
						screen[x][y][2]=6;
						break;
					case 2:
					case 7:
						screen[x][y][2]=3;
						break;
					}
				switch(screen[x][y][1])
					{
					case 1:
						screen[x][y][3]=0;
						break;
					case 4:
					case 5:
						screen[x][y][1]=6;
						break;
					case 2:
						screen[x][y][1]=3;
						break;
					case 7:
						screen[x][y][1]=3;
						break;
					}
				if(screen[x][y][1]!=7)screen[x][y][3]=0;
				if(screen[x][y][1]==screen[x][y][2]&&
					screen[x][y][3]==0)screen[x][y][1]=0;
				if(screen[x][y][1]==0&&screen[x][y][2]==0&&screen[x][y][3]==0)screen[x][y][3]=1;
				break;
			case 2:
				switch(screen[x][y][2])
					{
					case 4:
					case 5:
						screen[x][y][2]=6;
						break;
					}
				switch(screen[x][y][1])
					{
					case 4:
					case 5:
						screen[x][y][1]=6;
						break;
					}
				if(screen[x][y][1]!=7)screen[x][y][3]=0;
				if(screen[x][y][1]==screen[x][y][2]&&
					screen[x][y][3]==0)screen[x][y][1]=0;
				if(screen[x][y][1]==0&&screen[x][y][2]==0&&screen[x][y][3]==0)screen[x][y][3]=1;
				break;
			case 1:
				if(screen[x][y][1]!=7)screen[x][y][3]=0;
				if(screen[x][y][1]==screen[x][y][2]&&
					screen[x][y][3]==0)screen[x][y][1]=0;
				if(screen[x][y][1]==0&&screen[x][y][2]==0&&screen[x][y][3]==0)screen[x][y][3]=1;
				break;
			}
		}
}

void graphicst::rain_color_square(long x,long y)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x][y][1]=1;
		screen[x][y][2]=0;
		screen[x][y][3]=1;
		}
}

void graphicst::snow_color_square(long x,long y)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x][y][1]=7;
		screen[x][y][2]=0;
		screen[x][y][3]=1;
		}
}

void graphicst::color_square(long x,long y,unsigned char f,unsigned char b,unsigned char br)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x][y][1]=f;
		screen[x][y][2]=b;
		screen[x][y][3]=br;
		}
}

void graphicst::get_mouse_text_coords(long &mx,long &my)
{
  if(enabler.tracking_on)
    {
      pair<int,int> pos = window_to_grid(enabler.mouse_x, enabler.mouse_y);
      if (pos.first != -1 && pos.second != -1) {
        mx = pos.first;
        my = pos.second;
      }
    }
}

void graphicst::prepare_rect(char n_orig)
{
  if(rect_id!=-1) {
    gridrectst *gr=enabler.get_gridrect(rect_id);
    if(gr!=NULL)
		{
		gr->allocate(init.display.grid_x,init.display.grid_y);
		}
    else rect_id=-1;
  }

  if(rect_id==-1) {
    rect_id=enabler.gridrect_create(init.display.grid_x,init.display.grid_y);
  }

  original_rect=n_orig;
  
  setclipping(0,init.display.grid_x-1,0,init.display.grid_y-1);
  
  erasescreen();
  
  force_full_display_count=4;
}

void graphicst::prepare_graphics()
{
	if(!init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS))return;

	enabler.textures.remove_uploaded_textures();

	texture.clean();

	//GET READY TO LOAD
	svector<char *> processfilename;
	long f;
	textlinesst setuplines;
	char str[200];

	//LOAD THE OBJECT FILES UP INTO MEMORY
		//MUST INSURE THAT THEY ARE LOADED IN THE PROPER ORDER, IN CASE THEY REFER TO EACH OTHER
#ifndef WIN32
	find_files_by_pattern_with_exception("raw/graphics/graphics_*",processfilename,"text");
#else
	find_files_by_pattern_with_exception("raw/graphics/graphics_*.*",processfilename,"text");
#endif

	string chktype="GRAPHICS";
	for(f=0;f<processfilename.size();f++)
		{
		strcpy(str,"raw/graphics/");
		strcat(str,processfilename[f]);
		setuplines.load_raw_to_lines(str);
		process_object_lines(setuplines,chktype);

		delete[] processfilename[f];
		}
	processfilename.clear();

	enabler.textures.upload_textures();
}

void graphicst::add_tile(long texp,char addcolor)
{
	if(screenx>=clipx[0]&&screenx<=clipx[1]&&
		screeny>=clipy[0]&&screeny<=clipy[1])
		{
		screentexpos[screenx][screeny]=texp;
		screentexpos_addcolor[screenx][screeny]=addcolor;
		screentexpos_grayscale[screenx][screeny]=0;
		}
}

void graphicst::add_tile_grayscale(long texp,char cf,char cbr)
{
	if(screenx>=clipx[0]&&screenx<=clipx[1]&&
		screeny>=clipy[0]&&screeny<=clipy[1])
		{
		screentexpos[screenx][screeny]=texp;
		screentexpos_addcolor[screenx][screeny]=0;
		screentexpos_grayscale[screenx][screeny]=1;
		screentexpos_cf[screenx][screeny]=cf;
		screentexpos_cbr[screenx][screeny]=cbr;
		}
}

void graphicst::draw_border(int x1, int x2, int y1, int y2) {
  // Upper and lower
  for (int x = x1; x <= x2; x++) {
    locate(y1, x);
    addchar(' ');
    locate(y2, x);
    addchar(' ');
  }
  // Left and right
  for (int y = y1+1; y < y2; y++) {
    locate(y, x1);
    addchar(' ');
    locate(y, x2);
    addchar(' ');
  }
}

void render_things(enum render_phase phase)
{
  bool clear = false;
  if(gps.force_full_display_count>0||!init.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT))
    // NOTE: NEED TO CLEAR NO MATTER WHAT IN CASE THEY ARE DOING BLACKSPACE, ETC.
    {
      clear = true;
    }

  //GRAB CURRENT SCREEN AT THE END OF THE LIST
  viewscreenst *currentscreen=&gview.view;
  while(currentscreen->child!=NULL)currentscreen=currentscreen->child;
  
  //NO INTERFACE LEFT, LEAVE
  if(currentscreen==&gview.view)return;
  
  if (phase == setup) {
    currentscreen->render();
    // HACK: Render REC when recording macros. Definitely want this screen-specific. Or do we?
    if (enabler.is_recording()) {
      gps.locate(0, 20);
      gps.changecolor(4,1,1);
      gps.addst("REC");
    }
    gps.renewscreen();
  }
  
  //DRAW EVERYTHING TO BACK BUFFER
  if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT))
    enabler.render_tiles(phase, clear);
}
