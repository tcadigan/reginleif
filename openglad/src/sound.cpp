/*
 * Copyright (C) 1995-2002 FSGames. Ported by Sean Ford and Yan Shosh
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
// Sound object

/*
 * Changelog:
 *     buffers: 08/07/02: * Moved SDL_OpenAudio to after the silence chech in init()
 *     buffers: 08/16/02: * Sound works now.
 */

#include "soundob.hpp"

#include "io.hpp"
#include "util.hpp"

#include <filesystem>

SoundObject::SoundObject()
{
    // Do stuff
    // Default is sound ON
    silence = 0;

    for (Sint32 i = 0; i < NUMSOUNDS; ++i) {
        sound[i] = nullptr;
    }

    init();
}

// This version of the constructor will set "silence" to the value of the toggle
// before init-ing, so that if we don't want sound, we won't load them into
// memory.
SoundObject::SoundObject(bool silent)
{
    silence = silent;

    for (Uint32 i = 0; i < NUMSOUNDS; ++i) {
        sound[i] = nullptr;
    }

    // init will do nothing if silent is set
    init();
}

SoundObject::~SoundObject()
{
    shutdown();
}

bool SoundObject::init()
{
    Sint32 i;

    // Free any existing sounds
    for (Sint32 i = 0; i < NUMSOUNDS; ++i) {
        Mix_FreeChunk(sound[i]);
        sound[i] = nullptr;
    }

    // Do we have sounds on?
    if (silence) {
        return false;
    }

    Sint32 sample_rate = 22050;
    Sint32 sample_format = AUDIO_S16;
    Sint32 sample_buffer_size = 1024;
    bool stereo = true;

    if (Mix_OpenAudio(sample_rate, sample_format, stereo ? 2 : 1, sample_buffer_size) == -1) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ERROR: Mix_OpenAudio: %s\n", Mix_GetError());
        exit(0);
    }

    Mix_AllocateChannels(8);

    // Init the sounds...
    soundlist[SOUND_BOW] = "twang.wav";
    soundlist[SOUND_CLANG] = "clang.wav";
    soundlist[SOUND_DIE1] = "die1.wav";
    soundlist[SOUND_BLAST] = "blast1.wav";
    soundlist[SOUND_SPARKLE] = "faerie1.wav";
    soundlist[SOUND_TELEPORT] = "teleport.wav";
    soundlist[SOUND_YO] = "yo.wav";
    soundlist[SOUND_BOLT] = "bolt1.wav";
    soundlist[SOUND_HEAL] = "heal1.wav";
    soundlist[SOUND_CHARGE] = "charge.wav";
    soundlist[SOUND_FWIP] = "fwip.wav";
    soundlist[SOUND_EXPLODE] = "explode1.wav";
    soundlist[SOUND_DIE2] = "die2.wav"; // Registered only
    soundlist[SOUND_ROAR] = "roar.wav"; // Registered only
    soundlist[SOUND_MONEY] = "money.wav"; // Registered only
    soundlist[SOUND_EAT] = "eat.wav"; // Registered only

    for (i = 0; i < NUMSOUNDS; ++i) {
#ifdef SOUND_OB
        Log("loading sound %d: %s\n", i, soundlist[i]);
#endif

        load_sound(&sound[i], soundlist[i]);
    }

    // Set volume (default is loudest)
    volume = MIX_MAX_VOLUME;

#ifdef SOUND_DB
    Log("Done with sound initialization\n");
#endif

    return true;
}

void SoundObject::load_sound(Mix_Chunk **audio, std::string const &file)
{
    SDL_RWops *rw = open_read_file(std::filesystem::path("sound/" + file));
    *audio = Mix_LoadWAV_RW(rw, 0);

    if (*audio == nullptr) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ERROR: Mix_LoadWAV: %s\n", Mix_GetError());

        exit(0);
    }

    SDL_RWclose(rw);

    Mix_VolumeChunk(*audio, MIX_MAX_VOLUME / 2);
}

void SoundObject::free_sound(Mix_Chunk **sound)
{
    Mix_FreeChunk(*sound);
}

void SoundObject::shutdown()
{
    Sint32 i;

    if (silence) {
        return;
    }

    for (i = 0; i < NUMSOUNDS; ++i) {
        if (sound[i] != nullptr) {
            free_sound(&sound[i]);
        }
    }

    Mix_CloseAudio();
}

void SoundObject::play_sound(Sint16 whichnum)
{
    // If silent mode set, do nothing here
    if (silence) {
        return;
    }

    Mix_PlayChannel(-1, sound[whichnum], 0);
}

Uint8 SoundObject::query_volume()
{
    return volume;
}

Uint8 SoundObject::set_volume(Uint8 volumelevel)
{
    volume = volumelevel;

    return volume;
}

// Used to turn sound on or off
Uint8 SoundObject::set_sound(bool silent)
{
    // Are we already set this way?
    if (silence == silent) {
        return silence;
    }

    silence = silent;
    init();

    return silence;
}
