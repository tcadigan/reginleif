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
#include "options.hpp"

#include "io.hpp"

#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9

#define KEY_FILE "keyprefs.dat"

// Zardus: These were originally static chars but are now ints
// Now define the arrays with their default values

Sint32 key1[] = {
    // Movements
    SDLK_w, SDLK_e, SDLK_d, SDLK_c,
    SDLK_x, SDLK_z, SDLK_a, SDLK_q,
    // Fire and special
    SDLK_LCTRL, SDLK_LALT,
    // Switch guys
    SDLK_TAB,
    // Change special
    SDLK_1,
    // Yell
    SDLK_s,
    // Shifter
    SDLK_LSHIFT,
    // Options menu
    SDLK_2,
    // Cheat key
    SDLK_F5
};

Sint32 key2[] = {
    // Movements
    SDLK_KP8, SDLK_KP9, SDLK_KP6, SDLK_KP3,
    SDLK_KP2, SDLK_KP1, SDLK_KP4, SDLK_KP7,
    // Fire and special
    SDLK_KP0, SDLK_KP_ENTER,
    // Switch guys
    SDLK_KP_PLUS,
    // Change special
    SDLK_KP_MINUS,
    // Yell
    SDLK_KP5,
    // Shifter
    SDLK_KP_PERIOD,
    // Options menu
    SDLK_KP_MULTIPLY,
    // Cheat key
    SDLK_F8
};

Sint32 key3[] = {
    // Movements
    SDLK_i, SDLK_o, SDLK_l, SDLK_PERIOD,
    SDLK_COMMA, SDLK_m, SDLK_j, SDLK_u,
    // Fire and special
    SDLK_SPACE, SDLK_SEMICOLON,
    // Switch guys
    SDLK_BACKSPACE,
    // Yell
    SDLK_7,
    // Shifter
    SDLK_RSHIFT,
    // Options menu
    SDLK_8,
    // Cheat key
    SDLK_F7
};

Sint32 key4[] = {
    // Movements
    SDLK_t, SDLK_y, SDLK_h, SDLK_n,
    SDLK_b, SDLK_v, SDLK_f, SDLK_r,
    // Fire and special
    SDLK_5, SDLK_6,
    // Switch guys
    SDLK_EQUALS,
    // Change special
    SDLK_3,
    // Yell
    SDLK_g,
    // Shifter
    SDLK_MINUS,
    // Options menu
    SDLK_4,
    // Cheat key
    SDLK_F6
};

// This only exists so we can use the array constructor for our
// prefs object (grumble grumble)
Sint32 *normalkeys[] = { key1, key2, key3, key4 };

Options::Options()
{
    Sint32 i;
    SDL_RWops *infile;

    // Allocate our normal keys
    memcpy(allkeys, *normalkeys, 64 * sizeof(Sint32));

    // Set up preference defaults
    for (i = 0; i < 4; ++i) {
        // Display hp/sp bars and numbers
        prefs[i][PREF_LIFE] = PREF_LIFE_BOTH;
        // Dispaly score/exp info
        prefs[i][PREF_SCORE] = PREF_SCORE_ON;
        // Start at full screen
        prefs[i][PREF_VIEW] = PREF_VIEW_FULL;
        // Default to no joystick
        prefs[i][PREF_JOY] = PREF_NO_JOY;
        prefs[i][PREF_RADAR] = PREF_RADAR_ON;
        prefs[i][PREF_FOES] = PREF_FOES_ON;
        prefs[i][PREF_GAMMA] = 0;
        // No button behind text
        prefs[i][PREF_OVERLAY] = PREF_OVERLAY_OFF;
    }

    infile = open_read_file(KEY_FILE);

    // Failed to read
    if (!infile) {
        return;
    }

    // Read the blobs of data...
    for (i = 0; i < 4; ++i) {
        SDL_RWread(infile, allkeys[i], 16 * sizeof(Sint32), 1);
        SDL_RWread(infile, prefs[i], 10, 1);
    }

    SDL_RWclose(infile);
}

Options::~Options()
{
}

// It DOESN'T actually LOAD (tee hee), it only quries the prefs object...
// but the stupid view objects don't know that...don't tell them!
bool Options::load(Sint16 mynum, Uint8 *prefs, Sint32 *mykeys)
{
    Sint16 prefnum = mynum;

    // Yes, we are ACTUALLY COPYING the data
    if (prefs != &prefs[prefnum]) {
        memcpy(prefs, &prefs[prefnum], 10);
    }

    if (mykeys != allkeys[prefnum]) {
        memcpy(mykeys, allkeys[prefnum], 16 * sizeof(Sint32));
    }

    return 1;
}

// This time, we actually DO access the file since the bloke playing the game
// might decide to quit or turn off the computer at any time and then wonder
// later, "Where'd my prefs go! Bly'me!"
bool Options::save(Sint16 mynum, Uint8 *vprefs, Sint32 *mykeys)
{
    short prefnum = mynum;
    Sint32 i;
    SDL_RWops *outfile;

    // Yes, we are ACTUALLY COPYING the data
    memcpy(&prefs[prefnum], vprefs, 10);
    memcpy(allkeys[prefnum], mykeys, 16 * sizeof(Sint32));

    outfile = open_write_file(KEY_FILE);

    // Failed to write
    if (!outfile) {
        return false;
    }

    // Write the blobs of data...
    for (i = 0; i < 4; ++i) {
        SDL_RWwrite(outfile, allkeys[i], 16 * sizeof(Sint32), 1);
        SDL_RWwrite(outfile, prefs[i], 10, 1);
    }

    SDL_RWclose(outfile);

    return true;
}
