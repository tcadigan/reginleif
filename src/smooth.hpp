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
#ifndef __SMOOTH_HPP__
#define __SMOOTH_HPP__

//
// smooth.hpp
//

#include <SDL2/SDL.h>

#include "pixie_data.hpp"

// Used for deciding cases
#define TO_UP 1
#define TO_RIGHT 2
#define TO_DOWN 4
#define TO_LEFT 8
#define TO_AROUND 15

// These are the "genre" defines...
#define TYPE_GRASS 1
#define TYPE_WATER 2
#define TYPE_TREES 3
#define TYPE_DIRT 4
#define TYPE_COBBLE 5
#define TYPE_GRASS_DARK 6
#define TYPE_DIRT_DARK 7
#define TYPE_WALL 8
#define TYPE_CARPET 9
#define TYPE_GRASS_LIGHT 10
#define TYPE_UNKNOWN 50

class smoother
{
public:
    smoother();

    void reset();

    // Set out target grid to smooth...
    void set_target(PixieData const &data);

    // Smoothes entire target grid
    Sint32 smooth();

    // Smooth at x, y; returns changed or note
    Sint32 smooth(Sint32 x, Sint32 y);

    // Return target type, i.e. PIX_GRASS1
    Sint32 query_x_y(Sint32 x, Sint32 y);

    // Returns target genre, i.e. TYPE_GRASS
    Sint32 query_genre_x_y(Sint32 x, Sint32 y);

protected:
    // Returns 0-15 of 4 surroundings
    Sint32 surrounds(Sint32 x, Sint32 y, Sint32 whatgenre);

    // Set grid location to what value
    void set_x_y(Sint32 x, Sint32 y, Sint32 whatvalue);

    // Our grid to change
    Uint8 *mygrid;

    // Dimensions of our grid...
    Sint32 maxx;
    Sint32 maxy;
};

#endif
