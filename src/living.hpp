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
#ifndef __LIVING_HPP__
#define __LIVING_HPP__

// Definition of LIVING class

#include "base.hpp"
#include "walker.hpp"

class living : public walker
{
public:
    living(PixieData const &data);
    virtual ~living();
    Sint16 act();
    Sint16 check_special(); // Determine if we should do special...
    Sint16 collide(walker *ob);
    Sint16 do_action(); // Perform orverriding action
    walker *do_summon(Uint8 whatfamily, Uint16 lifetime);
    Sint16 facing(Sint16 x, Sint16 y);
    void set_difficulty(Uint32 whatlevel);
    Sint16 shove(walker *target, Sint16 x, Sint16 y);

    Uint8 query_order()
    {
        return ORDER_LIVING;
    }

    virtual bool walk(float x, float y);
protected:

    Sint16 act_random();
};

#endif
