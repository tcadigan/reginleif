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
#ifndef __WEAP_HPP__
#define __WEAP_HPP__

// Defintion of WEAP class

#include "walker-fwd.hpp"

class Weap : public Walker
{
public:
    Weap(PixieData const &data);
    virtual ~Weap();

    Sint16 act();
    Sint16 animate();
    // Called on destruction
    Sint16 death();
    Sint16 setxy(Sint16 x, Sint16 y);

    Uint8 query_order()
    {
        return ORDER_WEAPON;
    }

    // Weapons only related variables; use with care
    // Do we bounce?
    Sint32 do_bounce;
};

#endif
