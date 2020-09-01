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
#ifndef __LOADER_HPP__
#define __LOADER_HPP__

// Definition of LOADER class

#include "base.hpp"
#include "pixien.hpp"
#include "pixie_data.hpp"

#define SIZE_ORDERS 7

class Loader
{
public:
    Loader();
    virtual ~Loader(void);

    PixieN *create_pixieN(Uint8 order, Uint8 family);
    PixieData get_graphics(Uint8 order, Uint8 family);
    float get_hitpoints(Uint8 order, Uint8 family);
    float get_stepsize(Uint8 order, Uint8 family);
    float get_damage(Uint8 order, Uint8 family);
    float get_fire_frequency(Uint8 order, Uint8 family);
    Sint32 get_lineofsight(Uint8 order, Uint8 family);
    ActEnum get_act_type(Uint8 order, Uint8 family);
    Sint8 **get_animation(Uint8 order, Uint8 family);

    // Highest PIX element that has hitpoints
    float hitpoints[(NUM_FAMILIES * ORDER_FX) + FAMILY_BOOMERANG + 1];
    PixieData graphics[SIZE_ORDERS * NUM_FAMILIES];
    Sint8 **animations[SIZE_ORDERS * NUM_FAMILIES];
    float stepsizes[SIZE_ORDERS * NUM_FAMILIES];
    Sint32 lineofsight[SIZE_ORDERS * NUM_FAMILIES];
    ActEnum act_types[SIZE_ORDERS * NUM_FAMILIES];
    float damage[SIZE_ORDERS * NUM_FAMILIES];
    float fire_frequency[SIZE_ORDERS * NUM_FAMILIES];
};

#endif
