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
#ifndef __PAL32_HPP__
#define __PAL32_HPP__

//
// H file for palette.cpp
//

// #include <cstdio>
// The above is included in palette.cpp now

#include <SDL2/SDL.h>

// Load/set palette from disk
Sint16 load_and_set_palette(SDL_Color *newpalette);

// Load palette from disk
Sint16 load_palette(SDL_Color *newpalette);

// Set palette
Sint16 set_palette(SDL_Color *newpalette);

// Gamma correction??
void adjust_palette(SDL_Color *whichpal, Sint16 amount);

// Color cycling
void cycle_palette(SDL_Color *newpalette, Sint16 start, Sint16 end, Sint16 shift);

SDL_Color query_palette_reg(Uint8 index);
void set_palette_reg(Uint8 index, Uint8 red, Uint8 green, Uint8 blue);
Sint16 save_palette(Uint8 *whatpalette);

#endif
