#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <map>
using std::string;

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
		long screentexpos[MAX_GRID_X][MAX_GRID_Y];
		char screentexpos_addcolor[MAX_GRID_X][MAX_GRID_Y];
		unsigned char screentexpos_grayscale[MAX_GRID_X][MAX_GRID_Y];
		unsigned char screentexpos_cf[MAX_GRID_X][MAX_GRID_Y];
		unsigned char screentexpos_cbr[MAX_GRID_X][MAX_GRID_Y];
		unsigned char screen[MAX_GRID_X][MAX_GRID_Y][4];
		long clipx[2],clipy[2];
		long tex_pos[TEXTURENUM];

		long rect_id;

		LARGE_INTEGER print_time[100];
		long print_index;
		char display_frames;

		short force_full_display_count;

		char original_rect;




		graphicst()
			{
			print_index=0;
			display_frames=0;
			rect_id=-1;
			force_full_display_count=4;
			original_rect=1;
			}

		void locate(long y,long x);
		void changecolor(short f,short b,char bright);
		void addchar(unsigned char c,char advance=1);
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
					screen[x][y][1]=0;
					screen[x][y][2]=7;
					screen[x][y][3]=0;
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
