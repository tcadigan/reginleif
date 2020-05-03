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

//
// Palette.cpp file -- Holds (what else?) palette routines
//
// Created: 1995-02-05
//

/*
 * ChangeLog
 *     buffers: 08/14/02: * Added our_pal_lookup func so we don't need our.pal
 *     Zardus: 08/20/02: Added return 0 for save_pallete so VC++ is happy
 */

#include "pal32.hpp"

#include <algorithm>

// buffers: PORT: We need a palette to store the current palette
SDL_Color curpal[256];
SDL_Color temppal[256]; // For loading, setting, etc.

static constexpr SDL_Color const data[256] = {
    SDL_Color{0, 0, 0, 255}, SDL_Color{32, 32, 32, 255},
    SDL_Color{64, 64, 64, 255}, SDL_Color{96, 96, 96, 255},
    SDL_Color{128, 128, 128, 255}, SDL_Color{160, 160, 160, 255},
    SDL_Color{192, 192, 192, 255}, SDL_Color{224, 224, 224, 255},
    SDL_Color{4, 4, 4, 255}, SDL_Color{36, 36, 36, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{100, 100, 100, 255},
    SDL_Color{132, 132, 132, 255}, SDL_Color{164, 164, 164, 255},
    SDL_Color{196, 196, 196, 255}, SDL_Color{228, 228, 228, 255},
    SDL_Color{0, 0, 0, 255}, SDL_Color{60, 60, 60, 255},
    SDL_Color{72, 72, 72, 255}, SDL_Color{84, 84, 84, 255},
    SDL_Color{96, 96, 96, 255}, SDL_Color{108, 108, 108, 255},
    SDL_Color{120, 120, 120, 255}, SDL_Color{132, 132, 132, 255},
    SDL_Color{144, 144, 144, 255}, SDL_Color{156, 156, 156, 255},
    SDL_Color{168, 168, 168, 255}, SDL_Color{180, 180, 180, 255},
    SDL_Color{192, 192, 192, 255}, SDL_Color{204, 204, 204, 255},
    SDL_Color{216, 216, 216, 255}, SDL_Color{228, 228, 228, 255},
    SDL_Color{228, 64, 64, 255}, SDL_Color{216, 72, 72, 255},
    SDL_Color{204, 80, 80, 255}, SDL_Color{192, 88, 88, 255},
    SDL_Color{180, 96, 96, 255}, SDL_Color{168, 104, 104, 255},
    SDL_Color{156, 112, 112, 255}, SDL_Color{144, 120, 120, 255},
    SDL_Color{228, 0, 0, 255}, SDL_Color{208, 0, 0, 255},
    SDL_Color{188, 0, 0, 255}, SDL_Color{168, 0, 0, 255},
    SDL_Color{148, 0, 0, 255}, SDL_Color{128, 0, 0, 255},
    SDL_Color{108, 0, 0, 255}, SDL_Color{88, 0, 0, 255},
    SDL_Color{64, 228, 64, 255}, SDL_Color{72, 216, 72, 255},
    SDL_Color{80, 204, 80, 255}, SDL_Color{88, 192, 88, 255},
    SDL_Color{96, 180, 96, 255}, SDL_Color{104, 168, 104, 255},
    SDL_Color{112, 156, 112, 255}, SDL_Color{120, 144, 120, 255},
    SDL_Color{0, 228, 0, 255}, SDL_Color{0, 208, 0, 255},
    SDL_Color{0, 188, 0, 255}, SDL_Color{0, 168, 0, 255},
    SDL_Color{0, 148, 0, 255}, SDL_Color{0, 128, 0, 255},
    SDL_Color{0, 108, 0, 255}, SDL_Color{0, 88, 0, 255},
    SDL_Color{64, 64, 228, 255}, SDL_Color{72, 72, 216, 255},
    SDL_Color{80, 80, 204, 255}, SDL_Color{88, 88, 192, 255},
    SDL_Color{96, 96, 180, 255}, SDL_Color{104, 104, 168, 255},
    SDL_Color{112, 112, 156, 255}, SDL_Color{120, 120, 144, 255},
    SDL_Color{0, 0, 228, 255}, SDL_Color{0, 0, 208, 255},
    SDL_Color{0, 0, 188, 255}, SDL_Color{0, 0, 168, 255},
    SDL_Color{0, 0, 148, 255}, SDL_Color{0, 0, 128, 255},
    SDL_Color{0, 0, 108, 255}, SDL_Color{0, 0, 88, 255},
    SDL_Color{228, 228, 64, 255}, SDL_Color{216, 216, 72, 255},
    SDL_Color{204, 204, 80, 255}, SDL_Color{192, 192, 88, 255},
    SDL_Color{180, 180, 96, 255}, SDL_Color{168, 168, 104, 255},
    SDL_Color{156, 156, 112, 255}, SDL_Color{144, 144, 120, 255},
    SDL_Color{228, 228, 0, 255}, SDL_Color{208, 208, 0, 255},
    SDL_Color{188, 188, 0, 255}, SDL_Color{168, 168, 0, 255},
    SDL_Color{148, 148, 0, 255}, SDL_Color{128, 128, 0, 255},
    SDL_Color{108, 108, 0, 255}, SDL_Color{88, 88, 0, 255},
    SDL_Color{228, 64, 228, 255}, SDL_Color{216, 72, 216, 255},
    SDL_Color{204, 80, 204, 255}, SDL_Color{192, 88, 192, 255},
    SDL_Color{180, 96, 180, 255}, SDL_Color{168, 104, 168, 255},
    SDL_Color{156, 112, 156, 255}, SDL_Color{144, 120, 144, 255},
    SDL_Color{228, 0, 228, 255}, SDL_Color{208, 0, 208, 255},
    SDL_Color{188, 0, 188, 255}, SDL_Color{168, 0, 168, 255},
    SDL_Color{148, 0, 148, 255}, SDL_Color{128, 0, 128, 255},
    SDL_Color{108, 0, 108, 255}, SDL_Color{88, 0, 88, 255},
    SDL_Color{64, 228, 228, 255}, SDL_Color{72, 216, 216, 255},
    SDL_Color{80, 204, 204, 255}, SDL_Color{88, 192, 192, 255},
    SDL_Color{96, 180, 180, 255}, SDL_Color{104, 168, 168, 255},
    SDL_Color{112, 156, 156, 255}, SDL_Color{120, 144, 144, 255},
    SDL_Color{0, 228, 228, 255}, SDL_Color{0, 208, 208, 255},
    SDL_Color{0, 188, 188, 255}, SDL_Color{0, 168, 168, 255},
    SDL_Color{0, 148, 148, 255}, SDL_Color{0, 128, 128, 255},
    SDL_Color{0, 108, 108, 255}, SDL_Color{0, 88, 88, 255},
    SDL_Color{228, 164, 100, 255}, SDL_Color{208, 144, 80, 255},
    SDL_Color{188, 124, 60, 255}, SDL_Color{168, 104, 40, 255},
    SDL_Color{148, 84, 20, 255}, SDL_Color{128, 64, 0, 255},
    SDL_Color{108, 44, 0, 255}, SDL_Color{88, 24, 0, 255},
    SDL_Color{200, 160, 120, 255}, SDL_Color{180, 140, 100, 255},
    SDL_Color{160, 120, 80, 255}, SDL_Color{140, 100, 60, 255},
    SDL_Color{120, 80, 40, 255}, SDL_Color{100, 60, 20, 255},
    SDL_Color{80, 40, 0, 255}, SDL_Color{60, 20, 0, 255},
    SDL_Color{228, 100, 164, 255}, SDL_Color{208, 80, 144, 255},
    SDL_Color{188, 60, 124, 255}, SDL_Color{168, 40, 104, 255},
    SDL_Color{148, 20, 84, 255}, SDL_Color{128, 0, 64, 255},
    SDL_Color{108, 0, 44, 255}, SDL_Color{88, 0, 24, 255},
    SDL_Color{200, 120, 160, 255}, SDL_Color{180, 100, 140, 255},
    SDL_Color{160, 80, 120, 255}, SDL_Color{140, 60, 100, 255},
    SDL_Color{120, 40, 80, 255}, SDL_Color{100, 20, 60, 255},
    SDL_Color{80, 0, 40, 255}, SDL_Color{60, 0, 20, 255},
    SDL_Color{0, 72, 24, 255}, SDL_Color{0, 64, 24, 255},
    SDL_Color{0, 52, 20, 255}, SDL_Color{0, 44, 20, 255},
    SDL_Color{0, 32, 12, 255}, SDL_Color{0, 24, 8, 255},
    SDL_Color{0, 12, 4, 255}, SDL_Color{0, 8, 0, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 100, 228, 255}, SDL_Color{144, 80, 208, 255},
    SDL_Color{124, 60, 188, 255}, SDL_Color{104, 40, 168, 255},
    SDL_Color{84, 20, 148, 255}, SDL_Color{64, 0, 128, 255},
    SDL_Color{44, 0, 108, 255}, SDL_Color{24, 0, 88, 255},
    SDL_Color{160, 120, 200, 255}, SDL_Color{140, 100, 180, 255},
    SDL_Color{120, 80, 160, 255}, SDL_Color{100, 60, 140, 255},
    SDL_Color{80, 40, 120, 255}, SDL_Color{60, 20, 100, 255},
    SDL_Color{40, 0, 80, 255}, SDL_Color{20, 0, 60, 255},
    SDL_Color{100, 164, 228, 255}, SDL_Color{92, 156, 220, 255},
    SDL_Color{84, 148, 212, 255}, SDL_Color{76, 140, 204, 255},
    SDL_Color{68, 132, 196, 255}, SDL_Color{60, 124, 188, 255},
    SDL_Color{52, 116, 180, 255}, SDL_Color{44, 108, 172, 255},
    SDL_Color{36, 100, 164, 255}, SDL_Color{28, 92, 156, 255},
    SDL_Color{20, 84, 148, 255}, SDL_Color{12, 76, 140, 255},
    SDL_Color{4, 68, 132, 255}, SDL_Color{0, 60, 124, 255},
    SDL_Color{0, 52, 116, 255}, SDL_Color{0, 44, 108, 255},
    SDL_Color{228, 60, 0, 255}, SDL_Color{228, 84, 0, 255},
    SDL_Color{228, 108, 0, 255}, SDL_Color{228, 132, 0, 255},
    SDL_Color{228, 156, 0, 255}, SDL_Color{228, 180, 0, 255},
    SDL_Color{228, 204, 0, 255}, SDL_Color{228, 228, 0, 255},
    SDL_Color{228, 60, 0, 255}, SDL_Color{228, 84, 0, 255},
    SDL_Color{228, 108, 0, 255}, SDL_Color{228, 132, 0, 255},
    SDL_Color{228, 156, 0, 255}, SDL_Color{228, 180, 0, 255},
    SDL_Color{228, 204, 0, 255}, SDL_Color{228, 228, 0, 255},
    SDL_Color{228, 148, 124, 255}, SDL_Color{204, 132, 108, 255},
    SDL_Color{188, 112, 96, 255}, SDL_Color{172, 96, 80, 255},
    SDL_Color{224, 140, 92, 255}, SDL_Color{208, 128, 96, 255},
    SDL_Color{192, 120, 88, 255}, SDL_Color{176, 108, 76, 255},
    SDL_Color{112, 72, 72, 255}, SDL_Color{120, 80, 80, 255},
    SDL_Color{128, 88, 88, 255}, SDL_Color{136, 96, 96, 255},
    SDL_Color{144, 104, 104, 255}, SDL_Color{152, 112, 112, 255},
    SDL_Color{160, 120, 120, 255}, SDL_Color{168, 128, 128, 255},
};

// load_and_set_palette
//
// Loads palette from file FILENAME, stores palette info in NEWPALETTE, and
// set the current palette to this.
Sint16 load_and_set_palette(SDL_Color *newpalette)
{
    // Copy back the palette info...
    for (Sint16 i = 0; i < 256; ++i) {
        newpalette[i] = data[i];
    }

    set_palette(newpalette);

    return 1;
}

// load_palette
// Loads palette from file FILENAME into NEWPALETTE
Sint16 load_palette(SDL_Color *newpalette)
{
    // Copy back the palette info...
    for (Sint16 i = 0; i < 256; ++i) {
        newpalette[i] = data[i];
    }

    return 1;
}

// set_palette
// Sets the current palette to NEWPALETTE
Sint16 set_palette(SDL_Color *newpalette)
{
    // Copy over the palette info...
    for (Sint16 i = 0; i < 256; ++i) {
        curpal[i] = newpalette[i];
    }

    return 1;
}

// adjust_palette
// Performs gamma correction (ligthening/darkening) on whichpal based on a
// positive or negative amount; displays new palette, but does NOT affect
// whichpal
void adjust_palette(SDL_Color *whichpal, Sint16 amount)
{
    SDL_Color tempcol;
    Sint16 multiple = amount * 10;

    // Coppy whichpal to temppal for setting...
    for (Sint16 i = 0; i < 256; ++i) {
        tempcol = whichpal[i];

        // Now modify the current color bit based on 'amount'
        // Convert the 'amount' to = (x * 10%) + x; i.e., 2 = (20% + 2) increase
        tempcol.r = ((tempcol.r * (100 + multiple)) / 100) + amount;
        tempcol.g = ((tempcol.g * (100 + multiple)) / 100) + amount;
        tempcol.b = ((tempcol.b * (100 + multiple)) / 100) + amount;

        tempcol.r = std::min(static_cast<Uint8>(63),
                             std::min(static_cast<Uint8>(0), tempcol.r));
        tempcol.g = std::min(static_cast<Uint8>(63),
                             std::min(static_cast<Uint8>(0), tempcol.g));
        tempcol.b = std::min(static_cast<Uint8>(63),
                             std::min(static_cast<Uint8>(0), tempcol.b));

        // Now set the current palette index to modified bit value
        curpal[i] = tempcol;
    }
}

// cycle_palette
// Cycle and display newpalette
void cycle_palette(SDL_Color *newpalette, Sint16 start, Sint16 end, Sint16 shift)
{
    Sint16 length = end - start;
    Sint16 newval;

    // Copy over the palette info...
    for (Sint16 i = 0; i < 256; ++i) {
        if ((i >= start) && (i <= end)) {
            newval = i - shift;

            if (newval < start) {
                newval += length;
            }
        } else {
            newval = i;
        }

        temppal[i] = newpalette[newval];
    }

    // Return the modified palette
    for (Sint16 i = 0; i < 256; ++i) {
        newpalette[i] = temppal[i];
        // buffers: Since this is supposed to load the pal too, we copy it
        //          over to ourpal.
        curpal[i] = temppal[i];
    }
}

SDL_Color query_palette_reg(Uint8 index)
{
    return curpal[index];
}

void set_palette_reg(Uint8 index, Uint8 red, Uint8 green, Uint8 blue)
{
    curpal[index] = SDL_Color{red, green, blue, 255};
}
