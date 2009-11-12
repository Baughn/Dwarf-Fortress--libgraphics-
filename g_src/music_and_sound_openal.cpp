#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "music_and_sound_openal.h"
#include "music_and_sound_v.h"

#define ABORT(str) do { printf("%s: line %d: %s\n", __FILE__, __LINE__, str); abort(); } while(0);

using namespace std;

#define alPrintErrors() do { alPrintErrors_(__FILE__,__LINE__); } while(0);

static void alPrintErrors_(const char* file, int line) {
  ALenum err;
  while ((err = alGetError()) != AL_NO_ERROR) {
    printf("At %s: %d: ", file, line);
    switch (err) {
    case AL_INVALID_NAME: puts("AL_INVALID_NAME detected"); break;
    case AL_INVALID_ENUM: puts("AL_INVALID_ENUM detected"); break;
    case AL_INVALID_VALUE: puts("AL_INVALID_VALUE detected"); break;
    case AL_INVALID_OPERATION: puts("AL_INVALID_OPERATION detected"); break;
    case AL_OUT_OF_MEMORY: puts("AL_OUT_OF_MEMORY detected"); break;
    }
  }
}

bool musicsoundst::initsound() {
  if (functional) return true;
  
  // Find out what devices we have available
  const char *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
  const char *firstdevice = devices;
  puts("Sound devices available:");
  while (*devices) {
    puts(devices);
    devices += strlen(devices) + 1;
  }
  printf("Picking %s. If your desired device was missing, make sure you have the appropriate 32-bit libraries installed. If you wanted a different device, configure ~/.openalrc appropriately.\n",
         firstdevice);

  // Create the context
  device = alcOpenDevice(firstdevice);
  if (!device)
    return false;

  const ALCint attrlist[] = { ALC_FREQUENCY, SOUND_FREQUENCY,
                              ALC_MONO_SOURCES, 0,
                              ALC_STEREO_SOURCES, SOUND_CHANNELNUM };
  context = alcCreateContext(device, attrlist);
  if (context) {
    puts("Perfect OpenAL context attributes GET");
    goto done;
  }
  context = alcCreateContext(device, NULL);
  if (context) {
    puts("Using OpenAL in compatibility mode");
    goto done;
  }
  return false;

 done:
  if (ALC_FALSE == alcMakeContextCurrent(context)) {
    puts("alcMakeContextCurrent failed");
    return false;
  }
  functional = true;
  return true;
}

// int main() {
//   musicsound.initsound();
//   string str = "data/sound/song_title.ogg";
//   musicsound.set_song(str, 14);
//   musicsound.startbackgroundmusic(14);
//   sleep(9999);
//   exit(1);
// }

void musicsoundst::set_song(string &filename, int slot) {
  if (!functional) return;

  // printf("%s requested in %d\n", filename.c_str(), slot);
  if (!buffers.count(filename)) {
    // Song not already loaded. Load it.
    SF_INFO sfinfo;
    sfinfo.format = 0;
    SNDFILE *sf = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!sf) {
      printf("%s not found, sound not loaded\n", filename.c_str());
      goto end;
    } 
    short *buffer = new short[sfinfo.channels * sfinfo.frames];
    sf_count_t frames_read = sf_readf_short(sf, buffer, sfinfo.frames);
    if (frames_read != sfinfo.frames)
      printf("%s: %d frames requested, %d frames read. Corrupted file?\n",
             filename.c_str(), (int)sfinfo.frames, (int)frames_read);
    sf_close(sf);
    // Construct openal buffer and load this
    ALuint albuf;
    alGenBuffers(1, &albuf);
    if (!alIsBuffer(albuf)) {
      puts("Constructing OpenAL buffer mysteriously failed!");
      goto end;
    }
    ALenum format;
    switch (sfinfo.channels) {
    case 1: format = AL_FORMAT_MONO16;
      break;
    case 2: format = AL_FORMAT_STEREO16;
      break;
    default:
      printf("%s: Unexpected number of channels: %d\n",
             filename.c_str(), (int)sfinfo.channels);
      goto end;
    }
    alBufferData(albuf, format, (ALvoid*)buffer,
                 sfinfo.channels * sfinfo.frames * 2, sfinfo.samplerate);
    alPrintErrors();
    delete[] buffer;

    // Create a source for this song
    ALuint source;
    alGenSources(1, &source);
    if (!alIsSource(source)) {
      puts("Constructing OpenAL source mysteriously failed!");
      goto end;
    }
    alSourceQueueBuffers(source, 1, &albuf);
    
    buffers[filename] = albuf;
    sources[filename] = source;
  }

  // Store the requested song in the requested slot.
  // Say, should this alter the playing song if that slot is already playing?
  slot_buffer[slot] = buffers[filename];
  slot_source[slot] = sources[filename];

  end:
    alPrintErrors();
}

void musicsoundst::set_master_volume(long newvol) {
  if (!functional) return;
  alListenerf(AL_GAIN, newvol / 255.0f);
}

void musicsoundst::playsound(int slot) {
  if (!functional) return;
  // printf("%d requested\n", slot);
  if (!slot_source.count(slot)) {
    printf("Slot %d requested, but no song loaded\n", slot);
    return;
  }
  if (background_slot == slot) {
    puts("playsound called on background song, background song cancelled!?");
    background_slot = -1;
  }
  alSourcei(slot_source[slot], AL_LOOPING, AL_FALSE);
  alSourcePlay(slot_source[slot]);
  alPrintErrors();
}

void musicsoundst::startbackgroundmusic(int slot) {
  if (!functional) return;

  if (!slot_source.count(slot)) {
    printf("Slot %d requested, but no song loaded\n", slot);
    return;
  }

  if (background_slot == slot)
    return; // Verily, it is already playing
  stop_sound(background_slot);
  background_slot = slot;
  // printf("%d backgrounded\n", slot);

  alSourcei(slot_source[slot], AL_LOOPING, AL_TRUE);
  alSourcePlay(slot_source[slot]);
  alPrintErrors();
}

void musicsoundst::stopbackgroundmusic() {
  if (!functional) return;
  if (background_slot == -1) return;

  alSourceStop(slot_source[background_slot]);
}

void musicsoundst::stop_sound() {
  if (!functional) return;
  // Stop all playing sounds. Does this include background music?
  std::map<std::string,ALuint>::iterator it;
  for (it = sources.begin(); it != sources.end(); ++it)
    alSourceStop(it->second);
}

void musicsoundst::stop_sound(int slot) {
  if (!functional) return;
  if (slot_source.count(slot) == 0) return;
  ALuint source = slot_source[slot];
  alSourceStop(source);
}

void musicsoundst::deinitsound() {
  std::map<std::string,ALuint>::iterator it;
  // Free all sources
  for (it = sources.begin(); it != sources.end(); ++it) {
    ALuint source = it->second;
    alDeleteSources(1, &source);
  }
  // Free all sample memory
  for (it = buffers.begin(); it != buffers.end(); ++it) {
    ALuint buffer = it->second;
    alDeleteBuffers(1, &buffer);
  }
  // Deinit OpenAL
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);

  functional=false;
}

// Deprecated stuff below

void musicsoundst::set_sound(string &filename, int slot, int pan, int priority) {
  if (!functional) return;
  set_song(filename, slot);
}

void musicsoundst::playsound(int s, int channel) {
  if (!functional) return;
  playsound(s);
}
