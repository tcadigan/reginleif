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
#ifndef __BROWSER_ENTRY_HPP__
#define __BROWSER_ENTRY_HPP__

#include "level_data.hpp"
#include "radar.hpp"
#include "screen-fwd.hpp"

#include <SDL2/SDL.h>

#include <list>
#include <string>

class BrowserEntry
{
public:
    LevelData level_data;
    SDL_Rect mapAreas;
    Radar myradar;
    std::string level_name;
    Sint32 max_enemy_level;
    float average_enemy_level;
    Sint32 num_enemies;
    float difficulty;
    std::list<Sint32> exits;
    std::string scentext[80]; // Array to hold scenario information
    Uint8 scentextlines; // How many lines of text in scenario info

    BrowserEntry(VideoScreen *screenp, Sint32 index, Sint32 scen_num);
    ~BrowserEntry();

    void updateIndex(Sint32 index);
    void draw(VideoScreen *screenp);
};

#endif
