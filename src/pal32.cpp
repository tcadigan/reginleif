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
#include <cstdio>
#include <string>

Uint8 temppal[768]; // For loadingg, setting, etc.
Uint8 gammapal[768]; // For gamma-correction

// buffers: PORT: We need a palette to store the current palette
Uint8 curpal[768];

Uint8 our_pal_lookup(Sint32 index);

// load_and_set_palette
//
// Loads palette from file FILENAME, stores palette info in NEWPALETTE, and
// set the current palette to this.
Sint16 load_and_set_palette(std::string const &filename, Uint8 *newpalette)
{
    Sint16 i;

    // buffers: Don't need this file stuff since we use our_pal_lookup instead
    /*
     * FILE *infile = fopen(filename, "rb");
     * if (infile == nullptr) {
     *     Log("Error in reading palette file %s\n", filename);
     *
     *     return 0;
     * }
     *
     * if ((fread(temppal, 1, 768, infile) != 768) || ferror(infile)) {
     *     Log("Error: Corrupt palette file %s!\n", filename);
     *
     *     return 0;
     * }
     *
     * fclose(infile);
     */

    // Copy back the palette info...
    for (i = 0; i < 768; ++i) {
        // newpalette[i] = temppal[i];
        newpalette[i] = our_pal_lookup(i);
    }

    // set_palette(temppal);
    set_palette(newpalette);

    return 1;
}

// save_palette
// Save the dos palette so we can restore it when done
Sint16 save_palette(Uint8 *whatpalette)
{
    // buffers: PORT: We don't have a palette to save :P
    return 0;
}

// load_palette
// Loads palette from file FILENAME into NEWPALETTE
Sint16 load_palette(std::string const &filename, Uint8 *newpalette)
{
    Sint16 i;

    // buffers: We don't need this anymore since we use our_pal_lookup() now
    /*
     * // Open for read
     * FILE *infile = fopen(filename, "rb");
     * if (infile == nullptr) {
     *     Log("Error in reading palette file %s\n", filename);
     *
     *     return 0;
     * }
     *
     * if ((fread(temppal, 1, 768, infile) != 768) || ferror(infile)) {
     *     Log("Error: Corrupt palette file %s!\n", filename);
     *
     *     return 0;
     * }
     *
     * fclose(infile);
     */

    // Copy back the palette info...
    for (i = 0; i < 768; ++i) {
        // newpalette[i] = temppal[i];
        newpalette[i] = our_pal_lookup(i);
    }

    return 1;
}

// set_palette
// Sets the current palette to NEWPALETTE
Sint16 set_palette(Uint8 *newpalette)
{
    Sint16 i;

    // Copy over the palette info...
    for (i = 0; i < 768; ++i) {
        curpal[i] = newpalette[i];
    }

    return 1;
}

// adjust_palette
// Performs gamma correction (ligthening/darkening) on whichpal based on a
// positive or negative amount; displays new palette, but does NOT affect
// whichpal
void adjust_palette(Uint8 *whichpal, Sint16 amount)
{
    Sint16 i;
    Sint16 tempcol;
    Sint16 multiple = static_cast<Sint16>(amount * 10);

    // Coppy whichpal to temppal for setting...
    for (i = 0; i < 768; ++i) {
        tempcol = whichpal[i];

        // Now modify the current color bit based on 'amount'
        // Convert the 'amount' to = (x * 10%) + x; i.e., 2 = (20% + 2) increase
        tempcol = static_cast<Sint16>(((tempcol * (100 + multiple)) / 100) + amount);

        tempcol = std::max(tempcol, static_cast<Sint16>(0));
        tempcol = std::min(tempcol, static_cast<Sint16>(63));

        // Now set the current palette index to modified bit value
        curpal[i] = static_cast<Uint8>(tempcol);
    }
}

// cycle_palette
// Cycle and display newpalette
void cycle_palette(Uint8 *newpalette, Sint16 start, Sint16 end, Sint16 shift)
{
    Sint16 i;
    Sint16 length = static_cast<Sint16>(end - start);
    Sint16 newval;
    Sint16 colorspot;

    // Copy over the palette info...
    for (i = 0; i < 768; i += 3) {
        colorspot = static_cast<Sint16>(i / 3);

        if ((colorspot >= start) && (colorspot <= end)) {
            newval = static_cast<Sint16>(colorspot - shift);

            if (newval < start) {
                newval += length;
            }

            newval *= 3;
            temppal[i] = newpalette[newval];
            temppal[i + 1] = newpalette[newval + 1];
            temppal[i + 2] = newpalette[newval + 2];
        } else {
            temppal[i] = newpalette[i];
            temppal[i + 1] = newpalette[i + 1];
            temppal[i + 2] = newpalette[i + 2];
        }
    }

    // Return the modified palette
    for (i = 0; i < 768; ++i) {
        newpalette[i] = temppal[i];
        // buffers: Since this is supposed to load the pal too, we copy it
        //          over to ourpal.
        curpal[i] = temppal[i];
    }
}

void query_palette_reg(Uint8 index, Sint32 *red, Sint32 *green, Sint32 *blue)
{
    Sint32 tred;
    Sint32 tgreen;
    Sint32 tblue;

    tred = static_cast<Sint32>(curpal[index * 3]);
    tgreen = static_cast<Sint32>(curpal[(index * 3) + 1]);
    tblue = static_cast<Sint32>(curpal[(index * 3) + 2]);

    *red = tred;
    *green = tgreen;
    *blue = tblue;
}

void set_palette_reg(Uint8 index, Sint32 red, Sint32 green, Sint32 blue)
{
    curpal[index * 3] = red;
    curpal[(index * 3) + 1] = green;
    curpal[(index * 3) + 2] = blue;
}

// buffers: This is the our.pal data in a function. I thought having a separate
//          our.pal file was ugly so I just put it all in this func
Uint8 our_pal_lookup(Sint32 index)
{
    Uint8 data[] = {
         0,  0,  0,  8,  8,  8, 16, 16, 16, 24, 24, 24, 32, 32, 32, 40, 40, 40, 48,
        48, 48, 56, 56, 56,  1,  1,  1,  9,  9,  9, 17, 17, 17, 25, 25, 25, 33, 33,
        33, 41, 41, 41, 49, 49, 49, 57, 57, 57,  0,  0,  0, 15, 15, 15, 18, 18, 18,
        21, 21, 21, 24, 24, 24, 27, 27, 27, 30, 30, 30, 33, 33, 33, 36, 36, 36, 39,
        39, 39, 42, 42, 42, 45, 45, 45, 48, 48, 48, 51, 51, 51, 54, 54, 54, 57, 57,
        57, 57, 16, 16, 54, 18, 18, 51, 20, 20, 48, 22, 22, 45, 24, 24, 42, 26, 26,
        39, 28, 28, 36, 30, 30, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,
         0,  0, 32,  0,  0, 27,  0,  0, 22,  0,  0, 16, 57, 16, 18, 54, 18, 20, 51,
        20, 22, 48, 22, 24, 45, 24, 26, 42, 26, 28, 39, 28, 30, 36, 30,  0, 57,  0,
         0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0,
        22,  0, 16, 16, 57, 18, 18, 54, 20, 20, 51, 22, 22, 48, 24, 24, 45, 26, 26,
        42, 28, 28, 39, 30, 30, 36,  0,  0, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,
         0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0, 22, 57, 57, 16, 54, 54, 18, 51,
        51, 20, 48, 48, 22, 45, 45, 24, 42, 42, 26, 39, 39, 28, 36, 36, 30, 57, 57,
         0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27, 27,  0,
        22, 22,  0, 57, 16, 57, 54, 18, 54, 51, 20, 51, 48, 22, 48, 45, 24, 45, 42,
        26, 42, 39, 28, 39, 36, 30, 36, 57,  0, 57, 52,  0, 52, 47,  0, 47, 42,  0,
        42, 37,  0, 37, 32,  0, 32, 27,  0, 27, 22,  0, 22, 16, 57, 57, 18, 54, 54,
        20, 51, 51, 22, 48, 48, 24, 45, 45, 26, 42, 42, 28, 39, 39, 30, 36, 36,  0,
        57, 57,  0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27,
        27,  0, 22, 22, 57, 41, 25, 52, 36, 20, 47, 31, 15, 42, 26, 10, 37, 21,  5,
        32, 16,  0, 27, 11,  0, 22,  6,  0, 50, 40, 30, 45, 35, 25, 40, 30, 20, 35,
        25, 15, 30, 20, 10, 25, 15,  5, 20, 10,  0, 15,  5,  0, 57, 25, 41, 52, 20,
        36, 47, 15, 31, 42, 10, 26, 37,  5, 21, 32,  0, 16, 27,  0, 11, 22,  0,  6,
        50, 30, 40, 45, 25, 35, 40, 20, 30, 35, 15, 25, 30, 10, 20, 25,  5, 15, 20,
         0, 10, 15,  0,  5,  0, 18,  6,  0, 16,  6,  0, 13,  5,  0, 11,  5,  0,  8,
         3,  0,  6,  2,  0,  3,  1,  0,  2,  0, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 25, 57, 36, 20, 52, 31, 15, 47, 26, 10, 42, 21,
         5, 37, 16,  0, 32, 11,  0, 27,  6,  0, 22, 40, 30, 50, 35, 25, 45, 30, 20,
        40, 25, 15, 35, 20, 10, 30, 15,  5, 25, 10,  0, 20,  5,  0, 15, 25, 41, 57,
        23, 39, 55, 21, 37, 53, 19, 35, 51, 17, 33, 49, 15, 31, 47, 13, 29, 45, 11,
        27, 43,  9, 25, 41,  7, 23, 39,  5, 21, 37,  3, 19, 35,  1, 17, 33,  0, 15,
        31,  0, 13, 29,  0, 11, 27, 57, 15,  0, 57, 21,  0, 57, 27,  0, 57, 33,  0,
        57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 15,  0, 57, 21,  0, 57,
        27,  0, 57, 33,  0, 57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 37,
        31, 51, 33, 27, 47, 28, 24, 43, 24, 20, 56, 35, 23, 52, 32, 24, 48, 30, 22,
        44, 27, 19, 28, 18, 18, 30, 20, 20, 32, 22, 22, 34, 24, 24, 36, 26, 26, 38,
        28, 28, 40, 30, 30, 42, 32, 32
    };

    return data[index];
}
