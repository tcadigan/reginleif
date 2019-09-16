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
#ifndef __TREASURE_HPP__
#define __TREASURE_HPP__

// Definition of TREASURE class

#include "walker-fwd.hpp"

class Treasure : public Walker
{
public:
    Treasure(PixieData const &data);
    virtual ~Treasure();

    Sint16 act();
    // Called upon destruction
    // Sint16 death();
    Sint16 eat_me(Walker *eater);
    Walker *find_teleport_target();
    void set_direct_frame(Sint16 whatframe);

    Uint8 query_order()
    {
        return ORDER_TREASURE;
    }
};

#endif
