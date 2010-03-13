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
#include <cassert>

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

static unsigned char *screen_old = NULL; // For partial printing
static long *screentexpos_old = NULL;
static char *screentexpos_addcolor_old = NULL;
static unsigned char *screentexpos_grayscale_old = NULL;
static unsigned char *screentexpos_cf_old = NULL;
static unsigned char *screentexpos_cbr_old = NULL;

void graphicst::unallocate() {
  if (pbo_mapped == -1) { // Delete local-memory things
    if (!screen) return;
    delete[] screen; screen = NULL;
    delete[] screentexpos;
    delete[] screentexpos_addcolor;
    delete[] screentexpos_grayscale;
    delete[] screentexpos_cf;
    delete[] screentexpos_cbr;
    
    delete[] screen_old;
    delete[] screentexpos_old;
    delete[] screentexpos_addcolor_old;
    delete[] screentexpos_grayscale_old;
    delete[] screentexpos_cf_old;
    delete[] screentexpos_cbr_old;
  } else {
    // Unmap PBOs, then free them
    if (screen) {
      glBindBufferARB(GL_TEXTURE_BUFFER_ARB, shader_pbo[pbo_mapped]);
      glUnmapBufferARB(GL_TEXTURE_BUFFER_ARB); // The others are offsets into the same buffer
      screen = NULL;
      glBindBufferARB(GL_TEXTURE_BUFFER_ARB, 0);
    }
    glDeleteBuffersARB(2, shader_pbo);
    // And the TBO
    glDeleteTextures(1, &shader_tbo);
  }
}

// Add, then increment to the (possible) PBO alignment requirement
static void align(size_t &sz, off_t inc) {
  sz += inc;
  while (sz%64) sz++; // So.. tired.. FIXME.
}

void graphicst::swap_pbos() {
  assert (pbo_mapped != -1);
  if (screen) {
    // Unmap old PBO
    glBindBufferARB(GL_TEXTURE_BUFFER_ARB, shader_pbo[pbo_mapped]);
    glUnmapBufferARB(GL_TEXTURE_BUFFER_ARB); // The others are offsets into the same buffer
    screen = NULL;
    // And attach it
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER_ARB, shader_tbo);
    glTexBufferARB(GL_TEXTURE_BUFFER_ARB, GL_ALPHA8UI_EXT, shader_pbo[pbo_mapped]);
    printGLError();
  }
  pbo_mapped = !pbo_mapped;
  glBindBufferARB(GL_TEXTURE_BUFFER_ARB, shader_pbo[pbo_mapped]);
  printGLError();
  if (GL_ARB_map_buffer_range) {
    screen = (unsigned char*)glMapBufferRange(GL_TEXTURE_BUFFER_ARB, 0, shader_pbo_sz,
                                              // GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
                                              GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  } else {
    glBufferDataARB(GL_TEXTURE_BUFFER_ARB, shader_pbo_sz, NULL, GL_STREAM_DRAW);
    screen = (unsigned char*)glMapBufferARB(GL_TEXTURE_BUFFER_ARB, GL_WRITE_ONLY);
  }
  assert(screen);
  screentexpos = (long*)(screen + offset_texpos);
  screentexpos_addcolor = (char*)(screen + offset_addcolor);
  screentexpos_grayscale = screen + offset_grayscale;
  screentexpos_cf = screen + offset_cf;
  screentexpos_cbr = screen + offset_cbr;
  printGLError();
  glBindBufferARB(GL_TEXTURE_BUFFER_ARB, 0);
  printGLError();
}

void graphicst::allocate(int x, int y) {
  dimx = x; dimy = y;
  init.display.grid_x = x;
  init.display.grid_y = y;
  setclipping(0, x-1, 0, y-1);
  force_full_display_count++;
#ifdef DEBUG
  cout << "GPS alloc: " << x << " " << y << endl;
#endif
  unallocate();
  
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_SHADER)) {
    // Create PBOs.
    glGenBuffersARB(2, shader_pbo);
    size_t bufsz = 0;
    align(bufsz, x*y*4 * sizeof *screen);
    offset_texpos = bufsz;
    align(bufsz, x*y * sizeof *screentexpos);
    offset_addcolor = bufsz;
    align(bufsz, x*y * sizeof *screentexpos_addcolor);
    offset_grayscale = bufsz;
    align(bufsz, x*y * sizeof *screentexpos_grayscale);
    offset_cf = bufsz;
    align(bufsz, x*y * sizeof *screentexpos_cf);
    offset_cbr = bufsz;
    align(bufsz, x*y * sizeof *screentexpos_cbr);
    shader_pbo_sz = bufsz;
    for (int i=0; i < 2; i++) {
      glBindBufferARB(GL_TEXTURE_BUFFER_ARB, shader_pbo[i]);
      glBufferDataARB(GL_TEXTURE_BUFFER_ARB, bufsz, NULL, GL_STREAM_DRAW);
      pbo_mapped = i;
    }
    screen = NULL;
    // Create the texture buffer object
    glGenTextures(1, &shader_tbo);
    glBindTexture(GL_TEXTURE_BUFFER_ARB, shader_tbo);
    // Initialize the renderer
    swap_pbos();
  } else {
    pbo_mapped = -1;
    screen = new unsigned char[screen, x*y*4];
    memset(screen, 0, x*y*4);
    screentexpos = new long[screentexpos, x*y];
    memset(screentexpos, 0, x*y*4);
    screentexpos_addcolor = new char[screentexpos_addcolor, x*y];
    memset(screentexpos_addcolor, 0, x*y);
    screentexpos_grayscale = new unsigned char[screentexpos_grayscale, x*y];
    memset(screentexpos_grayscale, 0, x*y);
    screentexpos_cf = new unsigned char[screentexpos_cf, x*y];
    memset(screentexpos_cf, 0, x*y);
    screentexpos_cbr = new unsigned char[screentexpos_cbr, x*y];
    memset(screentexpos_cbr, 0, x*y);
    
    screen_old = new unsigned char[screen_old, x*y*4];
    memset(screen_old, 0, x*y*4);
    screentexpos_old = new long[screentexpos_old, x*y];
    memset(screentexpos_old, 0, x*y*4);
    screentexpos_addcolor_old = new char[screentexpos_addcolor_old, x*y];
    memset(screentexpos_addcolor_old, 0, x*y);
    screentexpos_grayscale_old = new unsigned char[screentexpos_grayscale_old, x*y];
    memset(screentexpos_grayscale_old, 0, x*y);
    screentexpos_cf_old = new unsigned char[screentexpos_cf_old, x*y];
    memset(screentexpos_cf_old, 0, x*y);
    screentexpos_cbr_old = new unsigned char[screentexpos_cbr_old, x*y];
    memset(screentexpos_cbr_old, 0, x*y);
  }
}

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
        memset(screen, 0, dimx*dimy*4);
}

void graphicst::display()
{
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_SHADER)) {
    force_full_display_count = 0;
    return; // The rest is done on the GPU in shader mode.
  }

  long d=0;

  long *c_buffer_texpos;
  float *c_buffer_r;
  float *c_buffer_g;
  float *c_buffer_b;
  float *c_buffer_br;
  float *c_buffer_bg;
  float *c_buffer_bb;

  int x2,y2;
  if (force_full_display_count) {
    memcpy(screen_old, screen, dimx*dimy*4*sizeof *screen);
    // TODO: Don't bother with this in non-graphical mode
    memcpy(screentexpos_old, screentexpos, dimx*dimy*sizeof *screentexpos);
    memcpy(screentexpos_addcolor_old, screentexpos_addcolor, dimx*dimy*sizeof *screentexpos_addcolor);
    memcpy(screentexpos_grayscale_old, screentexpos_grayscale, dimx*dimy*sizeof *screentexpos_grayscale);
    memcpy(screentexpos_cf_old, screentexpos_cf, dimx*dimy*sizeof *screentexpos_cf);
    memcpy(screentexpos_cbr_old, screentexpos_cbr, dimx*dimy*sizeof *screentexpos_cbr);
    // Update the entire screen
    enabler.update_all();
  } else {
    for (x2=0; x2<init.display.grid_x; x2++) {
      for (y2=0; y2<init.display.grid_y; y2++,d++) {
        // Partial printing (and color-conversion): Big-ass if.
        if (screen[x2*dimy*4 + y2*4 + 0] == screen_old[x2*dimy*4 + y2*4 + 0] &&
            screen[x2*dimy*4 + y2*4 + 1] == screen_old[x2*dimy*4 + y2*4 + 1] &&
            screen[x2*dimy*4 + y2*4 + 2] == screen_old[x2*dimy*4 + y2*4 + 2] &&
            screen[x2*dimy*4 + y2*4 + 3] == screen_old[x2*dimy*4 + y2*4 + 3] &&
            // TODO: Don't bother with this in non-graphical mode
            screentexpos[x2*dimy + y2] == screentexpos_old[x2*dimy + y2] &&
            screentexpos_addcolor[x2*dimy + y2] == screentexpos_addcolor_old[x2*dimy + y2] &&
            screentexpos_grayscale[x2*dimy + y2] == screentexpos_grayscale_old[x2*dimy + y2] &&
            screentexpos_cf[x2*dimy + y2] == screentexpos_cf_old[x2*dimy + y2] &&
            screentexpos_cbr[x2*dimy + y2] == screentexpos_cbr_old[x2*dimy + y2])
          {
            // Nothing's changed, this clause deliberately empty
          } else {
          // Okay, the buffer has changed. First update the old-buffer.
          screen_old[x2*dimy*4 + y2*4 + 0] = screen[x2*dimy*4 + y2*4 + 0];
          screen_old[x2*dimy*4 + y2*4 + 1] = screen[x2*dimy*4 + y2*4 + 1];
          screen_old[x2*dimy*4 + y2*4 + 2] = screen[x2*dimy*4 + y2*4 + 2];
          screen_old[x2*dimy*4 + y2*4 + 3] = screen[x2*dimy*4 + y2*4 + 3];
          // TODO: Skip this bit in non-graphics mode.
          screentexpos_old[x2*dimy + y2] = screentexpos[x2*dimy + y2];
          screentexpos_addcolor_old[x2*dimy + y2] = screentexpos_addcolor[x2*dimy + y2];
          screentexpos_grayscale_old[x2*dimy + y2] = screentexpos_grayscale[x2*dimy + y2];
          screentexpos_cf_old[x2*dimy + y2] = screentexpos_cf[x2*dimy + y2];
          screentexpos_cbr_old[x2*dimy + y2] = screentexpos_cbr[x2*dimy + y2];
          
          // Then just inform enabler.
          enabler.update_tile(x2, y2);
        }
      }
    }
  }
  if (force_full_display_count > 0) force_full_display_count--;
}

void graphicst::renewscreen()
{
  if (display_frames) {
    ostringstream fps;
    fps << "FPS: " << setw(3) << enabler.calculate_fps() << " (" << enabler.calculate_gfps() << ")";
    changecolor(7,3,1);
    locate(0,init.display.grid_x / 2 + 11);
    addst(fps.str());
  }
  
  display();
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
				switch(screen[x*dimy*4 + y*4 + 2])
					{
					case 4:
					case 5:
					case 6:
						screen[x*dimy*4 + y*4 + 2]=1;
						break;
					case 2:
					case 7:
						screen[x*dimy*4 + y*4 + 2]=3;
						break;
					}
				switch(screen[x*dimy*4 + y*4 + 1])
					{
					case 4:
					case 5:
					case 6:
						screen[x*dimy*4 + y*4 + 1]=1;
						break;
					case 2:
					case 7:
						screen[x*dimy*4 + y*4 + 1]=3;
						break;
					}
				if(screen[x*dimy*4 + y*4 + 1]==screen[x*dimy*4 + y*4 + 2])screen[x*dimy*4 + y*4 + 1]=0;
				screen[x*dimy*4 + y*4 + 3]=0;
				if(screen[x*dimy*4 + y*4 + 1]==0&&screen[x*dimy*4 + y*4 + 2]==0&&screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 3]=1;
				break;
			case 3:
				switch(screen[x*dimy*4 + y*4 + 2])
					{
					case 4:
					case 5:
						screen[x*dimy*4 + y*4 + 2]=6;
						break;
					case 2:
					case 7:
						screen[x*dimy*4 + y*4 + 2]=3;
						break;
					}
				switch(screen[x*dimy*4 + y*4 + 1])
					{
					case 1:
						screen[x*dimy*4 + y*4 + 3]=0;
						break;
					case 4:
					case 5:
						screen[x*dimy*4 + y*4 + 1]=6;
						break;
					case 2:
						screen[x*dimy*4 + y*4 + 1]=3;
						break;
					case 7:
						screen[x*dimy*4 + y*4 + 1]=3;
						break;
					}
				if(screen[x*dimy*4 + y*4 + 1]!=7)screen[x*dimy*4 + y*4 + 3]=0;
				if(screen[x*dimy*4 + y*4 + 1]==screen[x*dimy*4 + y*4 + 2]&&
					screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 1]=0;
				if(screen[x*dimy*4 + y*4 + 1]==0&&screen[x*dimy*4 + y*4 + 2]==0&&screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 3]=1;
				break;
			case 2:
				switch(screen[x*dimy*4 + y*4 + 2])
					{
					case 4:
					case 5:
						screen[x*dimy*4 + y*4 + 2]=6;
						break;
					}
				switch(screen[x*dimy*4 + y*4 + 1])
					{
					case 4:
					case 5:
						screen[x*dimy*4 + y*4 + 1]=6;
						break;
					}
				if(screen[x*dimy*4 + y*4 + 1]!=7)screen[x*dimy*4 + y*4 + 3]=0;
				if(screen[x*dimy*4 + y*4 + 1]==screen[x*dimy*4 + y*4 + 2]&&
					screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 1]=0;
				if(screen[x*dimy*4 + y*4 + 1]==0&&screen[x*dimy*4 + y*4 + 2]==0&&screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 3]=1;
				break;
			case 1:
				if(screen[x*dimy*4 + y*4 + 1]!=7)screen[x*dimy*4 + y*4 + 3]=0;
				if(screen[x*dimy*4 + y*4 + 1]==screen[x*dimy*4 + y*4 + 2]&&
					screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 1]=0;
				if(screen[x*dimy*4 + y*4 + 1]==0&&screen[x*dimy*4 + y*4 + 2]==0&&screen[x*dimy*4 + y*4 + 3]==0)screen[x*dimy*4 + y*4 + 3]=1;
				break;
			}
		}
}

void graphicst::rain_color_square(long x,long y)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x*dimy*4 + y*4 + 1]=1;
		screen[x*dimy*4 + y*4 + 2]=0;
		screen[x*dimy*4 + y*4 + 3]=1;
		}
}

void graphicst::snow_color_square(long x,long y)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x*dimy*4 + y*4 + 1]=7;
		screen[x*dimy*4 + y*4 + 2]=0;
		screen[x*dimy*4 + y*4 + 3]=1;
		}
}

void graphicst::color_square(long x,long y,unsigned char f,unsigned char b,unsigned char br)
{
	if(x>=clipx[0]&&x<=clipx[1]&&
		y>=clipy[0]&&y<=clipy[1])
		{
		screen[x*dimy*4 + y*4 + 1]=f;
		screen[x*dimy*4 + y*4 + 2]=b;
		screen[x*dimy*4 + y*4 + 3]=br;
		}
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
		screentexpos[screenx*dimy + screeny]=texp;
		screentexpos_addcolor[screenx*dimy + screeny]=addcolor;
		screentexpos_grayscale[screenx*dimy + screeny]=0;
		}
}

void graphicst::add_tile_grayscale(long texp,char cf,char cbr)
{
	if(screenx>=clipx[0]&&screenx<=clipx[1]&&
		screeny>=clipy[0]&&screeny<=clipy[1])
		{
		screentexpos[screenx*dimy + screeny]=texp;
		screentexpos_addcolor[screenx*dimy + screeny]=0;
		screentexpos_grayscale[screenx*dimy + screeny]=1;
		screentexpos_cf[screenx*dimy + screeny]=cf;
		screentexpos_cbr[screenx*dimy + screeny]=cbr;
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

void render_things()
{
  //GRAB CURRENT SCREEN AT THE END OF THE LIST
  viewscreenst *currentscreen=&gview.view;
  while(currentscreen->child!=NULL)currentscreen=currentscreen->child;
  
  //NO INTERFACE LEFT, LEAVE
  if(currentscreen==&gview.view)return;
  
  currentscreen->render();
  // HACK: Render REC when recording macros. Definitely want this screen-specific. Or do we?
  if (enabler.is_recording()) {
    gps.locate(0, 20);
    gps.changecolor(4,1,1);
    gps.addst("REC");
  }
  gps.renewscreen();
  
  // Draw everything to screen
  enabler.render();
}
