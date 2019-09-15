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
#ifndef __EFFECT_HPP__
#define __EFFECT_HPP__

// Definition of Effect class

#include "base.hpp"
#include "obmap.hpp"
#include "screen.hpp"
#include "stats.hpp"
#include "walker.hpp"
#include "guy.hpp"

class effect : public walker
{
public:
    effect(PixiData const &data);
    virtual ~effect();
    Sint16 act();
    Sint16 animate();
    Sint16 death(); // Called on desctruction

    Sint16 query_order()
    {
        return ORDER_FX;
    }
};

#endif
