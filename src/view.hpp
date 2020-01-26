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

#include "view-fwd.hpp"

#include "level_data.hpp"
#include "options.hpp"
#include "radar.hpp"

// Max of 5 lines, currently
#define MAX_MESSAGES 5

// Viewscreen related defines
enum PrefOptionsEnum : Uint8 {
    PREF_LIFE = 0,
    PREF_SCORE = 1,
    PREF_VIEW = 2,
    PREF_JOY = 3,
    PREF_RADAR = 4,
    PREF_FOES = 5,
    PREF_GAMMA = 6,
    PREF_OVERLAY = 7,
    PREF_MAX = 8 // == 1 + highest pref...
};

enum PrefLifeEnum : Uint8 {
    PREF_LIFE_TEXT = 0,
    PREF_LIFE_BARS = 1,
    PREF_LIFE_BOTH = 2,
    PREF_LIFE_SMALL = 3,
    PREF_LIFE_OFF = 4
};

enum PrefScoreEnum : Uint8 {
    PREF_SCORE_OFF = 0,
    PREF_SCORE_ON = 1
};

enum PrefViewEnum : Uint8 {
    PREF_VIEW_FULL = 0,
    PREF_VIEW_PANELS = 1,
    PREF_VIEW_1 = 2,
    PREF_VIEW_2 = 3,
    PREF_VIEW_3 = 4
};

enum PrefJoystickEnum: Uint8 {
    PREF_NO_JOY = 0,
    PREF_USE_JOY = 1
};

enum PrefRadarEnum: Uint8 {
    PREF_RADAR_OFF = 0,
    PREF_RADAR_ON = 1
};

enum PrefFoesEnum : Uint8 {
    PREF_FOES_OFF = 0,
    PREF_FOES_ON = 1
};

enum PrefOverlayEnum : Uint8 {
    PREF_OVERLAY_OFF = 0,
    PREF_OVERLAY_ON = 1
};

class ViewScreen
{
public:
    ViewScreen(Sint16 x, Sint16 y, Sint16 length, Sint16 height, Sint16 whatnum);
    ~ViewScreen();

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

    friend class PixieN;
    friend class Pixie;
    friend class Text;
    friend class Walker;

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
    Walker *control;
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
    Radar *myradar;
    // Has the radar been started yet?
    Sint16 radarstart;

protected:
    Options *prefsob;
    Sint16 size;
    Sint16 xview;
    Sint16 yview;
    Uint8 *bmp;
    Uint8 *oldbmp;
};

#endif
