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

#include <SDL2/SDL.h>

// Zardus: Keys is a sys var (apparently) so we'll use allkeys
Sint32 allkeys[4][16];

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

/*
 * This is a child object of all viewscreens. It is use to save and load all
 * prefs because each player has their own prefs. WE ASSUME 4 PLAYERS ALWAYS.
 */
class Options
{
public:
    Options();
    ~Options();

    bool load(Sint16 mynum, Uint8 *prefs, Sint32 *mykeys);
    bool save(Sint16 mynum, Uint8 *vprefs, Sint32 *mykeys);

protected:
    Uint8 prefs[4][10];
    Uint8 keys[4][16];
};

#endif
