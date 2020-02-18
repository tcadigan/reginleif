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
#ifndef __SOUNDOB_HPP__
#define __SOUNDOB_HPP__

// soundob.hpp file...

#include <SDL2/SDL_mixer.h>

#include <string>

// buffers: PORT: Don't need this anymore
// #include "detect.hpp"

// buffers: PORT: Don't need this anymore
// #include "smix.hpp"

enum SoundEnum : Uint8 {
    SOUND_BOW = 0,
    SOUND_CLANG = 1,
    SOUND_DIE1 = 2,
    SOUND_BLAST = 3,
    SOUND_SPARKLE = 4,
    SOUND_TELEPORT = 5,
    SOUND_YO = 6,
    SOUND_BOLT = 7,
    SOUND_HEAL = 8,
    SOUND_CHARGE = 9,
    SOUND_FWIP = 10,
    SOUND_EXPLODE = 11,
    SOUND_DIE2 = 12, // Registered only
    SOUND_ROAR = 13, // Orc, reg
    SOUND_MONEY = 14, // reg
    SOUND_EAT = 15, // reg
    NUMSOUNDS = 16 // For now, let's use ALL sounds, regardless
};

class SoundObject
{
public:
    SoundObject();
    SoundObject(bool silent);
    ~SoundObject();

    bool init();
    void shutdown();
    void play_sound(Sint16 whichsound);
    void set_sound_volume(Sint32);
    void load_sound(Mix_Chunk **audio, std::string const &file);
    void free_sound(Mix_Chunk **sound);

    Uint8 query_volume();

    // Toggle sound on/off
    Uint8 set_sound(bool silent);
    void load_sound(SDL_AudioSpec, Uint8 *);
    Uint8 set_volume(Uint8 volumelevel);

    // Our list of sounds
    std::string soundlist[NUMSOUNDS];

    // AudioSpec for loading sounds
    Mix_Chunk *sound[NUMSOUNDS];

    // Card specific information
    Sint32 baseio;
    Sint32 irq;
    Sint32 dma;
    Sint32 dma16;

    // Volume: 0-255
    Sint32 volume;

    // 0 = on, 1 = silent
    Uint8 silence;
};

#endif
