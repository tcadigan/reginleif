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
#ifndef __RADAR_HPP__
#define __RADAR_HPP__

// Definition of RADAR class

#include "level_data.hpp"
#include "walker.hpp"

#include <SDL2/SDL.h>

class Radar
{
public:
    Radar(Sint16 whatnum);
    ~Radar();
    bool draw(LevelData const &data, Walker *control);
    bool on_screen();
    bool on_screen(Sint16 whatx, Sint16 whaty, Sint16 hor, Sint16 ver);
    bool refresh();

    // Slow function to update radar/map info
    void update(LevelData const &data);
    void start(LevelData const &data,
               Sint16 viewscreen_endx, Sint16 viewscreen_endy,
               Sint16 viewscreen_yloc);

    Sint16 sizex;
    Sint16 sizey;
    Sint16 xpos;
    Sint16 ypos;

    // Where on the screen to display
    Sint16 xloc;
    Sint16 yloc;

    // Zardus: radarx and radary are now class members (instead of temp vars) so that
    //         scen can use them for scroll
    // What actual portion of the map is on the radar (top-left coord)
    Sint16 radarx;
    Sint16 radary;

    Uint8 *bmp;
    Uint8 *oldbmp;
    bool force_lower_position;
    Sint16 xview;
    Sint16 yview;

protected:
    // What is my viewscreen related number?
    Sint16 mynum;
    Uint8 size;
};

#endif
