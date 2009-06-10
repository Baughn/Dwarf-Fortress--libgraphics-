//Copyright (c) 2008, Tarn Adams
//All rights reserved.  See game.cpp or license.txt for more information.
#if 1
#include "music_and_sound_fmodex.h"
#else
// Deprecated and possibly bit-rotten code below

#define SOUND_CHANNELNUM 16

#ifndef NO_FMOD

#include <fmod.h>
#include <fmod_errors.h>

#define MAXSONGNUM 1000
#define MAXSOUNDNUM 1000

class musicsoundst
{
	public:
		int SoftChannelNumber;

		musicsoundst()
			{
			doing_forced=0;
			int s;
			for(s=0;s<MAXSONGNUM;s++)mod[s]=NULL;
			for(s=0;s<MAXSOUNDNUM;s++)samp[s]=NULL;
			}
		~musicsoundst()
			{
			deinitsound();
			}

		void startbackgroundmusic(int song);
		void stopbackgroundmusic();
		void playsound(int s,int channel=-1);
		void playsound(int s,int min_channel,int max_channel,int force_channel);
		void initsound();
		void deinitsound();
		void forcebackgroundmusic(int song,unsigned long time);
		void set_song(string &filename,int slot);
		void set_sound(string &filename,int slot,int pan=-1,int priority=0);
		void set_sound_params(int slot,int p1,int vol,int pan,int priority);
		void stop_sound(int channel=FSOUND_ALL);
		void set_master_volume(long newvol);

	private:
		int song;
		char musicactive;
		char soundpriority;
		int soundplaying;

		char doing_forced;
		unsigned long forcesongtime;
		unsigned long forcesongstart;

		char on;

		FSOUND_SAMPLE *mod[MAXSONGNUM];
		FSOUND_SAMPLE *samp[MAXSOUNDNUM];
};
#endif // NO_FMOD
#endif // unix

