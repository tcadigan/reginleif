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

// buffers: PORT: Don't need this anymore
// #include "detect.hpp"

// buffers: PORT: Don't need this anymore
// #include "smix.hpp"

#define SOUND_BOW 0
#define SOUND_CLANG 1
#define SOUND_DIE1 2
#define SOUND_BLAST 3
#define SOUND_SPARKLE 4
#define SOUND_TELEPORT 5
#define SOUND_YO 6
#define SOUND_BOLT 7
#define SOUND_HEAL 8
#define SOUND_CHARGE 9
#define SOUND_FWIP 10
#define SOUND_EXPLODE 11
#define SOUND_DIE2 12 // Registered only
#define SOUND_ROAR 13 // Orc, reg
#define SOUND_MONEY 14 // reg
#define SOUND_EAT 15 // reg

#define NUMSOUNDS 16 // For now, let's use ALL sounds, regardless

class soundob
{
public:
    soundob();
    soundob(bool silent);
    ~soundob();

    Sint32 init();
    void shutdown();
    void play_sound(Sint16 whichsound);
    void set_sound_volume(Sint32);
    void load_sound(Mix_Chunk **audio, Uint8 *file);
    void free_sound(Mix_Chunk **sound);

    Uint8 query_volume();

    // Toggle sound on/off
    Uint8 set_sound(bool silent);
    void load_sound(SDL_AudioSpec, Uint8 *);
    Uint8 set_volume(Uint8 volumelevel);

    // Our list of sounds
    Uint8 soundlist[NUMSOUNDS][40];

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
