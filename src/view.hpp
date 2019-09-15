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
#ifndef __VIEW_HPP__
#define __VIEW_HPP__

// Definition of VIEWSCREEN class

#include "base.hpp"
#include "level_data.hpp"

// Max of 5 lines, currently
#define MAX_MESSAGES 5

// Viewscreen related defines
#define PREF_LIFE static_cast<Uint8>(0)
#define PREF_LIFE_TEXT static_cast<Uint8>(0)
#define PREF_LIFE_BARS static_cast<Uint8>(1)
#define PREF_LIFE_BOTH static_cast<Uint8>(2)
#define PREF_LIFE_SMALL static_cast<Uint8>(3)
#define PREF_LIFE_OFF static_cast<Uint8>(4)
#define PREF_SCORE static_cast<Uint8>(1)
#define PREF_SCORE_OFF static_cast<Uint8>(0)
#define PREF_SCORE_ON static_cast<Uint8>(1)
#define PREF_VIEW static_cast<Uint8>(2)
#define PREF_VIEW_FULL static_cast<Uint8>(0)
#define PREF_VIEW_PANELS static_cast<Uint8>(1)
#define PREF_VIEW_1 static_cast<Uint8>(2)
#define PREF_VIEW_2 static_cast<Uint8>(3)
#define PREF_VIEW_3 static_cast<Uint8>(4)
#define PREF_JOY static_cast<Uint8>(3)
#define PREF_NO_JOY static_cast<Uint8>(0)
#define PREF_USE_JOY static_cast<Uint8>(1)
#define PREF_RADAR static_cast<Uint8>(4)
#define PREF_RADAR_OFF static_cast<Uint8>(0)
#define PREF_RADAR_ON static_cast<Uint8>(1)
#define PREF_FOES static_cast<Uint8>(5)
#define PREF_FOES_OFF static_cast<Uint8>(0)
#define PREF_FOES_ON static_cast<Uint8>(1)
#define PREF_GAMMA static_cast<Uint8>(6)
#define PREF_OVERLAY static_cast<Uint8>(7)
#define PREF_OVERLAY_OFF static_cast<Uint8>(0)
#define PREF_OVERLAY_ON static_cast<Uint8>(1)

#define PREF_MAX 8 // == 1 + highest pref...

class viewscreen;

/*
 * This is a child object of all viewscreens. It is use to save and load all
 * prefs because each player has their own prefs. WE ASSUME 4 PLAYERS ALWAYS.
 */
class options
{
public:
    options();
    ~options();

    Sint16 load(viewscreen *viewp);
    Sint16 save(viewscreen *viewp);

protected:
    Uint8 prefs[4][10];
    Uint8 keys[4][16];
};

class viewscreen
{
public:
    viewscreen(Sint16 x, Sint16 y, Sint16 length, Sint16 height, Sint16 whatnum);
    ~viewscreen();

    void clear();
    Sint16 draw();
    Sint16 redraw();
    Sint16 redraw(LevelData *data, bool draw_radar=true);
    Sint16 refresh();
    Sint16 input(SDL_Event const &event);
    Sint16 continuous_input();
    void set_display_text(Uint8 const *newtext, Sint16 numcycles);
    // Put the text to the buffer, if there
    void display_text();
    // Cycle text upward
    void shift_text(Sint32 row);
    // Clear all text in buffer
    void clear_text(void);
    // Moved here to fix radar
    Sint16 draw_obs();
    Sint16 draw_obs(LevelData *data);
    void resize(Sint16 x, Sint16 y, Sint16 length, Sint16 height);
    // Set according to preferences...
    void resize(Uint8 whatmode);
    void view_team();
    void view_team(Sint16 left, Sint16 top, Sint16 right, Sint16 bottom);
    // Display the options menu
    void options_menu();
    // Get player keyboard info
    Sint32 set_key_prefs();
    Sint32 change_speed(Sint32 whichway);
    Sint32 change_gamma(Sint32 whichway);

    friend class pixieN;
    friend class pixie;
    friend class text;
    friend class walker;

    // For gamma correction
    Sint32 gamma;

    // Max of 80 wide
    Uint8 textlist[MAX_MESSAGES][80];
    // Cycles to display screen text
    Sint16 textcycles[MAX_MESSAGES];

    // Text to display
    Uint8 infotext[80];
    // Number to id the viewscreen, 0, 1, 2, ...
    Sint16 mynum;
    // Used for player-v-player mode
    Sint16 my_team;
    // Holds the keyboard mapping
    Sint32 *mykeys;
    // The user
    walker *control;
    Sint16 xpos;
    Sint16 ypos;
    Sint16 topx;
    Sint16 topy;
    // Physical screen coords
    Sint16 xloc;
    Sint16 yloc;
    // Screen coords of lower right corner
    Sint16 endx;
    Sint16 endy;
    // User preferences...
    Uint8 prefs[10];
    radar *myradar;
    // Has the radar been started yet?
    Sint16 radatastart;

protected:
    options *prefsob;
    Sint16 size;
    Sint16 xview;
    Sint16 yview;
    Uint8 *bmp;
    Uint8 *oldbmp;
};

#endif;
