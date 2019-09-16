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
#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

#include "view-fwd.hpp"

/*
 * This is a child object of all viewscreens. It is use to save and load all
 * prefs because each player has their own prefs. WE ASSUME 4 PLAYERS ALWAYS.
 */
class Options
{
public:
    Options();
    ~Options();

    Sint16 load(ViewScreen *viewp);
    Sint16 save(ViewScreen *viewp);

protected:
    Uint8 prefs[4][10];
    Uint8 keys[4][16];
};

#endif
