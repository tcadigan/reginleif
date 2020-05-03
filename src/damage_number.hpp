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
#ifndef __DAMAGE_NUMBER_HPP__
#define __DAMAGE_NUMBER_HPP__

#include <SDL2/SDL.h>

// TODO: Move this to screen class so it doesn't get overlapped
//       by other walkers drawing
class DamageNumber
{
public:
    DamageNumber(float x, float y, float value, Uint8 color);

    void draw(Sint16 topx, Sint16 topy, Sint16 xloc, Sint16 yloc);

    float x;
    float y;
    float t;
    float value;
    Uint8 color;
};

#endif
