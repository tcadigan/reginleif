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
#ifndef __JOY_DATA_HPP__
#define __JOY_DATA_HPP__

#include "base.hpp"

#include <SDL2/SDL.h>

#define JOY_DEAD_ZONE 8000
// Just in case there are joysticks attached that are no useable
// (e.g. accelerometer)
#define MAX_NUM_JOYSTICKS 10

class JoyData
{
public:
    JoyData();
    JoyData(Sint32 index);

    void setKeyFromEvent(Sint32 key_enum, SDL_Event const &event);

    bool getState(Sint32 key_enum) const;
    bool getPress(Sint32 key_enum, SDL_Event const &event) const;
    bool getRelease(Sint32 key_enum, SDL_Event const &event) const;
    bool hasButtonSet(Sint32 key_enum) const;

    Sint32 index;
    Sint32 numAxes;
    Sint32 numButtons;
    Sint32 numHats;

    static Sint32 const NONE = 0;
    static Sint32 const BUTTON = 1;
    static Sint32 const POS_AXIS = 2;
    static Sint32 const NEG_AXIS = 3;
    static Sint32 const HAT_UP = 4;
    static Sint32 const HAT_UP_RIGHT = 5;
    static Sint32 const HAT_RIGHT = 6;
    static Sint32 const HAT_DOWN_RIGHT = 7;
    static Sint32 const HAT_DOWN = 8;
    static Sint32 const HAT_DOWN_LEFT = 9;
    static Sint32 const HAT_LEFT = 10;
    static Sint32 const HAT_UP_LEFT = 11;

    Sint32 key_type[NUM_KEYS];
    Sint32 key_index[NUM_KEYS];
};

JoyData player_joy[4];
SDL_Joystick *joysticks[MAX_NUM_JOYSTICKS];

void init_joysticks();
bool isJoystickEvent(SDL_Event const &event);
bool playerHasJoystick(Sint32 player_num);
void disablePlayerJoystick(Sint32 player_num);
void resetJoystick(Sint32 player_num);
void handle_joy_event(SDL_Event const &event);
void handle_joystick_events(SDL_Event const &event);
Sint16 query_joystick_key_press_event();
void clear_joystick_key_press_event();

#endif
