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
#include "view-fwd.hpp"

class Radar
{
public:
    Radar(ViewScreen *myview, VideoScreen *myscreen, Sint16 whatnum);
    ~Radar();
    Sint16 draw();
    Sint16 draw(LevelData *data);
    Sint16 on_screen();
    Sint16 on_screen(Sint16 whatx, Sint16 whaty, Sint16 hor, Sint16 ver);
    Sint16 refresh();

    // Slow function to update radar/map info
    void update();
    void update(LevelData *data);
    void start();
    void start(LevelData *data);

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

    VideoScreen *screenp;
    ViewScreen *viewscreen;
    Uint8 *bmp;
    Uint8 *oldbmp;
    bool force_lower_position;
    Sint16 xview;
    Sint16 yview;

protected:
    // What is my viewscreen related number?
    Sint16 mynum;
    // Uint8 *buffer;
    Uint8 size;
};

#endif
