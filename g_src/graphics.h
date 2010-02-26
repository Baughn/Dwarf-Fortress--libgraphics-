#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <map>
using std::string;

#include "GL/glew.h"
#include "g_basics.h"
#include "platform.h"

enum Texture
{
	TEXTURE_MOUSE,
	TEXTURENUM
};

class graphicst
{
  int lookup_pair(std::pair<int,int> color);
	public:
		long screenx,screeny;
		short screenf,screenb;
		char screenbright;
		long *screentexpos;
		char *screentexpos_addcolor;
		unsigned char *screentexpos_grayscale;
		unsigned char *screentexpos_cf;
		unsigned char *screentexpos_cbr;
		unsigned char *screen;

                // Shader-mode stuff
                int pbo_mapped; // -1 if PBOs not in use, otherwise an index into shader_pbo
                // PBO layout (we use a single PBO for all buffers):
                // screen, then screentexpos, then _addcolor, then _grayscale, then _cf, then _cbr
                // All 64-byte aligned.
                GLuint shader_pbo[2]; // We alternate between writing into and rendering these PBOs
                size_t shader_pbo_sz;
                off_t offset_texpos, offset_addcolor, offset_grayscale, offset_cf, offset_cbr;
                GLuint shader_tbo; // Texture buffer object
                void swap_pbos();
                
		long clipx[2],clipy[2];
		long tex_pos[TEXTURENUM];

		long rect_id;

		LARGE_INTEGER print_time[100];
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

                        screentexpos = NULL;
                        screentexpos_addcolor = NULL;
                        screentexpos_grayscale = NULL;
                        screentexpos_cf = NULL;
                        screentexpos_cbr = NULL;
                        screen = NULL;
                        pbo_mapped = -1;

                        allocate(MAX_GRID_X, MAX_GRID_Y);
                        }

                void allocate(int x, int y);
                void unallocate();
                
                void locate(long y,long x)
                {
                  // No point in clamping here, addchar clamps too.
                  screenx=x;
                  screeny=y;
                }
                void changecolor(short f,short b,char bright)
                {
                  screenf=f;
                  screenb=b;
                  screenbright=bright;
                }
                void addchar(unsigned char c,char advance=1)
                {
                  if(screenx>=clipx[0]&&screenx<=clipx[1]&&
                     screeny>=clipy[0]&&screeny<=clipy[1])
                    {
                      screen[screenx*dimy*4 + screeny*4 + 0]=c;
                      screen[screenx*dimy*4 + screeny*4 + 1]=screenf;
                      screen[screenx*dimy*4 + screeny*4 + 2]=screenb;
                      screen[screenx*dimy*4 + screeny*4 + 3]=screenbright;
                      
                      screentexpos[screenx*dimy + screeny]=0;
                    }
                  if(advance)screenx++;
                }
		void addcoloredst(const char *str,const char *colorstr);
		void addst(const string &str);
		void addst(const char *str);
		void erasescreen_clip();
		void erasescreen();
                void erasescreen_rect(int x1, int x2, int y1, int y2);
		void renewscreen();
		void setclipping(long x1,long x2,long y1,long y2);

		void add_tile(long texp,char addcolor);
		void add_tile_grayscale(long texp,char cf,char cbr);

		void display();

		void prepare_graphics();

		void prepare_rect(char n_orig);

		void gray_out_rect(long sx,long ex,long sy,long ey)
                {
                  long x,y;
                  for(x=sx;x<=ex;x++)
                    {
                      for(y=sy;y<=ey;y++)
                        {
                          screen[x*dimy*4 + y*4 + 1]=0;
                          screen[x*dimy*4 + y*4 + 2]=7;
                          screen[x*dimy*4 + y*4 + 3]=0;
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

		void get_mouse_text_coords(long &mx,long &my);
                void draw_border(int x1, int x2, int y1, int y2);
};

extern graphicst gps;

#endif
