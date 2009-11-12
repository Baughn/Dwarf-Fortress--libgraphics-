#ifndef MUSIC_AND_SOUND_OPENAL_H
#define MUSIC_AND_SOUND_OPENAL_H

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#define SOUND_CHANNELNUM 16

// Preferred mixer frequency. Should be the same as what the ogg files
// use, to avoid resampling.
#define SOUND_FREQUENCY 44100


class musicsoundst
{
 public:
  bool initsound(); // Returns false if it failed
  void update() {}
  void set_master_volume(long newvol);

  void set_song(std::string &filename, int slot);
  void playsound(int slot);

  void startbackgroundmusic(int slot);
  void stopbackgroundmusic();
  void stop_sound();

  // Deprecated:
  void forcebackgroundmusic(int slot, unsigned long time);
  void playsound(int s,int channel);
  void stop_sound(int channel);
  void set_sound(std::string &filename,int slot,int pan=-1,int priority=0);
  void playsound(int s,int min_channel,int max_channel,int force_channel);
  void deinitsound();
  void set_sound_params(int slot,int p1,int vol,int pan,int priority);

  musicsoundst() {
    functional = false;
    background_slot = -1;
  }

 private:
  bool functional;
  ALCdevice *device;
  ALCcontext *context;

  std::map<std::string,ALuint> buffers; // OpenAL buffers
  std::map<std::string,ALuint> sources; // And sources
  std::map<int, ALuint> slot_buffer; // Mappings from DF slots to openal
  std::map<int, ALuint> slot_source;

  int background_slot; // Currently playing background music, or -1
};


#endif
