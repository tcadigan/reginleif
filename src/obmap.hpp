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
#ifndef __OBMAP_HPP__
#define __OBMAP_HPP__

// Definition of OBMAP class

#include <list>
#include <map>

#include "base.hpp"

class obmap
{
public:
    obmap();
    ~obmap();
    Sint16 query_list(walker *ob, Sint16 x, Sint16 y);
    Sint16 remove(walker *ob); // this goes in walker's destructor
    Sint16 add(walker *ob, Sint16 x, Sint16 y); // This goes in walker's constructor
    Sint16 move(walker *ob, Sint16 x, Sint16 y); // This goes in walker's setxy
    std::list<walker *> &obmap_get_list(Sint16 x, Sint16 y); // Returns the list at x,y for fnf

    size_t size() const;
    void draw();

    Sint16 obmapres;
    std::map<std::pair<Sint16, Sint16>, std::list<walker *>> pos_to_walker;
    std::map<walker *, std::list<std::pair<Sint16, Sint16>>> walker_to_pos;

private:
    Sint16 hash(Sint16 y);
    Sint16 unhash(Sint16 y);
};

#endif
