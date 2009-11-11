//Copyright (c) 2008, Tarn Adams
//All rights reserved.  See game.cpp or license.txt for more information.

// Fmodex works well enough on windows/os x, but on some linux distributions it fails badly
#if defined(linux)
#include "music_and_sound_openal.h"
#else
#include "music_and_sound_fmodex.h"
#endif // unix

