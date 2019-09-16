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

#include "gloader-fwd.hpp"

#include "pixien.hpp"
#include "pixie_data.hpp"
#include "screen-fwd.hpp"
#include "walker-fwd.hpp"

class Loader
{
public:
    Loader();
    virtual ~Loader(void);
    Walker *create_walker(Uint8 order, Uint8 family, Screen *screenp, bool cache_weapon = true);
    void set_derived_stats(Walker *w, Uint8 order, Uint8 family);
    PixieN *create_pixieN(Uint8 order, Uint8 family);
    Walker *set_walker(Walker *ob, Uint8 order, Uint8 family);
    PixieData *graphics;
    Uint8 ***animations;
    float *stepsize;
    Sint32 *lineofsight;

    float hitpoints[200]; // Hack for now
    Uint8 *act_types;
    float *damage;
    float *fire_frequency;
};

#endif
