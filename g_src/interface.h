#ifndef INTERFACE_H
#define INTERFACE_H

#include <set>
#include "ViewBase.h"
#include "keybindings.h"

struct cursesmovie_headerst
{
	long dimx,dimy;
	long delayrate;
};

class viewscreen_movieplayerst : viewscreenst
{
	public:
		static viewscreen_movieplayerst *create(char pushtype,viewscreenst *scr=NULL);
		virtual void help();
		virtual void feed(std::set<InterfaceKey> &events);
		virtual void logic();
		virtual void render();
		virtual char movies_okay(){return 0;}
		void clearfilelist();
		void force_play(string &file);

		virtual char is_option_screen(){if(is_playing)return 2;else return 0;}

	protected:
		char saving;
		char loading;
		char editing;
		char text_mode;
		unsigned char editing_char;
		long editing_copy_from;
		short editing_screenf;
		short editing_screenb;
		short editing_screenbright;
		long editing_selected_sound;
		char editing_menu;
		string savename;
		string force_file;
		char is_playing;
		char is_forced_play;
		char quit_if_no_play;
		long maxmoviepos;
		long end_frame_pos;

		long selfile;
		svector<char *> filelist;

		viewscreen_movieplayerst();
		virtual ~viewscreen_movieplayerst(){clearfilelist();};
};

enum InterfacePushType
{
	INTERFACE_PUSH_AS_PARENT,
	INTERFACE_PUSH_AS_CHILD,
	INTERFACE_PUSH_AT_BACK,
	INTERFACE_PUSH_AT_FRONT,
	INTERFACE_PUSHNUM
};

#define INTERFACEFLAG_RETAIN_NONZERO_INPUT BIT1

#define MOVIEBUFFSIZE 800000
#define COMPMOVIEBUFFSIZE 1000000

class interfacest : public interfacekeyst
{
	friend class viewscreen_movieplayerst;

	public:
		viewscreenst view;
		unsigned long flag;

		long shutdown_interface_tickcount;
		long shutdown_interface_for_ms;

		char loop();
		void remove_to_first();
		void removescreen(viewscreenst *scr);
		void addscreen(viewscreenst *scr,char pushtype,viewscreenst *relate);
		char is_supermovie_on()
			{
			return supermovie_on;
			}

		void print_interface_token(InterfaceKey key);


		interfacest();
		~interfacest();

	protected:
		char supermovie_on;
		long supermovie_pos;
		long supermovie_delayrate;
		long supermovie_delaystep;
		stringvectst supermovie_sound;
#ifndef NO_FMOD
		long supermovie_sound_time[200][SOUND_CHANNELNUM];
#endif
		unsigned char supermoviebuffer[MOVIEBUFFSIZE];
		unsigned char supermoviebuffer_comp[COMPMOVIEBUFFSIZE];
		long currentblocksize;
		long nextfilepos;
		char first_movie_write;
		string movie_file;

		void insertscreen_as_parent(viewscreenst *scr,viewscreenst *child);
		void insertscreen_as_child(viewscreenst *scr,viewscreenst *parent);
		void insertscreen_at_back(viewscreenst *scr);
		void insertscreen_at_front(viewscreenst *scr);
		void handlemovie(char flushall);
		void finish_movie();
		void use_movie_input();

		long write_movie_chunk();
		void read_movie_chunk(long &maxmoviepos,char &is_playing);
};

#define SCROLLING_NOSELECT BIT1
#define SCROLLING_NO_WRAP BIT2
#define SCROLLING_REVERSE BIT3
void finishscrolling(long &selection,long min,long max,long jump,unsigned long flag,char littlekey);
char standardscrolling(std::set<InterfaceKey> &events,short &selection,long min,long max,long jump,unsigned long flag=0);
char standardscrolling(std::set<InterfaceKey> &events,long &selection,long min,long max,long jump,unsigned long flag=0);
char secondaryscrolling(std::set<InterfaceKey> &events,short &scroll,long min,long max,long jump,unsigned long flag=0);
char secondaryscrolling(std::set<InterfaceKey> &events,long &scroll,long min,long max,long jump,unsigned long flag=0);
#define STRINGENTRY_LETTERS BIT1
#define STRINGENTRY_SPACE BIT2
#define STRINGENTRY_NUMBERS BIT3
#define STRINGENTRY_CAPS BIT4
#define STRINGENTRY_SYMBOLS BIT5
char standardstringentry(std::set<InterfaceKey> &events,char *str,int maxlen,unsigned long flag);
char standardstringentry(std::set<InterfaceKey> &events,string &str,int maxlen,unsigned long flag);

void drawborder(const char *str,char style=0,const char *colorstr=NULL);

#endif