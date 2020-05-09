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
#include "joy_data.hpp"

#include "input.hpp"
#include "util.hpp"

#include <sstream>

JoyData player_joy[4];
SDL_Joystick *joysticks[MAX_NUM_JOYSTICKS];

JoyData::JoyData()
    : index(-1)
    , numAxes(0)
    , numButtons(0)
    , numHats(0)
{
}

JoyData::JoyData(Sint32 index)
    : index(-1)
    , numAxes(0)
    , numButtons(0)
    , numHats(0)
{
    SDL_Joystick *js = joysticks[index];

    if (js == nullptr) {
        return;
    }

    this->index = index;
    numAxes = SDL_JoystickNumAxes(js);
    numButtons = SDL_JoystickNumButtons(js);
    numHats = SDL_JoystickNumHats(js);

    // Clear all keys for this joystick
    for (Sint32 i = 0; i < NUM_KEYS; ++i) {
        key_type[i] = NONE;
        key_index[i] = 0;
    }

    // Default movement
    // Prefer two axes
    if (numAxes > 1) {
        key_type[KEY_RIGHT] = POS_AXIS;
        key_index[KEY_RIGHT] = 0;
        key_type[KEY_LEFT] = NEG_AXIS;
        key_index[KEY_LEFT] = 0;

        key_type[KEY_UP] = NEG_AXIS;
        key_index[KEY_UP] = 1;
        key_type[KEY_DOWN] = POS_AXIS;
        key_index[KEY_DOWN] = 1;
    } else if (numHats > 0) {
        // But a single hat is okay otherwise
        // indices default to hat 0
        key_type[KEY_UP] = HAT_UP;
        key_type[KEY_UP_RIGHT] = HAT_UP_RIGHT;
        key_type[KEY_RIGHT] = HAT_RIGHT;
        key_type[KEY_DOWN_RIGHT] = HAT_DOWN_RIGHT;
        key_type[KEY_DOWN] = HAT_DOWN;
        key_type[KEY_DOWN_LEFT] = HAT_DOWN_LEFT;
        key_type[KEY_LEFT] = HAT_LEFT;
        key_type[KEY_UP_LEFT] = HAT_UP_LEFT;
    }

    // Default actions
    if (numButtons > 0) {
        key_type[KEY_FIRE] = BUTTON;
        key_index[KEY_FIRE] = 0;
    }

    if (numButtons > 1) {
        key_type[KEY_SPECIAL] = BUTTON;
        key_index[KEY_SPECIAL] = 1;
    }

    if (numButtons > 2) {
        key_type[KEY_SPECIAL_SWITCH] = BUTTON;
        key_index[KEY_SPECIAL_SWITCH] = 2;
    }

    if (numButtons > 3) {
        key_type[KEY_YELL] = BUTTON;
        key_index[KEY_YELL] = 3;
    }

    if (numButtons > 4) {
        key_type[KEY_SHIFTER] = BUTTON;
        key_index[KEY_SHIFTER] = 4;
    }

    if (numButtons > 5) {
        key_type[KEY_SWITCH] = BUTTON;
        key_index[KEY_SWITCH] = 5;
    }
}

void JoyData::setKeyFromEvent(Sint32 key_enum, SDL_Event const &event)
{
    // Diagonals are ignored because they are combinations of the cardinals
    // Things get really messy when diagonals are assigned
    if ((key_enum == KEY_UP_RIGHT)
        || (key_enum == KEY_UP_LEFT)
        || (key_enum == KEY_DOWN_RIGHT)
        || (key_enum == KEY_DOWN_LEFT)) {
        key_type[key_enum] = NONE;
        key_index[key_enum] = 0;

        return;
    }

    bool gotJoy = false;

    if (event.type == SDL_JOYAXISMOTION) {
        if (event.jaxis.value >= 0) {
            key_type[key_enum] = POS_AXIS;
        } else {
            key_type[key_enum] = NEG_AXIS;
        }

        key_index[key_enum] = event.jaxis.axis;
        // Uses the last joystick pressed
        index = event.jaxis.which;
        gotJoy = true;
    } else if (event.type == SDL_JOYBUTTONDOWN) {
        key_type[key_enum] = BUTTON;
        key_index[key_enum] = event.jbutton.button;
        // Uses the last joystick pressed
        index = event.jbutton.which;
        gotJoy = true;
    }  else if (event.type == SDL_JOYHATMOTION) {
        bool badHat = false;

        if (event.jhat.value == SDL_HAT_UP) {
            key_type[key_enum] = HAT_UP;
        } else if (event.jhat.value == SDL_HAT_RIGHT) {
            key_type[key_enum] = HAT_RIGHT;
        } else if (event.jhat.value == SDL_HAT_DOWN) {
            key_type[key_enum] = HAT_DOWN;
        } else if (event.jhat.value == SDL_HAT_LEFT) {
            key_type[key_enum] = HAT_LEFT;
        } else {
            badHat = true;
        }

        if (!badHat) {
            key_index[key_enum] = event.jhat.hat;
            // Uses the last joystick pressed
            index = event.jhat.which;
            gotJoy = true;
        }
    }

    if (gotJoy) {
        // Take over this joystick
        for (Sint32 i = 0; i < 4; ++i) {
            if ((this != &player_joy[i]) && (player_joy[i].index == index)) {
                player_joy[i].index = -1;
            }
        }
    }
}

bool JoyData::getState(Sint32 key_enum) const
{
    if (index < 0) {
        return false;
    }

    switch (key_type[key_enum]) {
    case POS_AXIS:

        return (SDL_JoystickGetAxis(joysticks[index], key_index[key_enum]) > JOY_DEAD_ZONE);
    case NEG_AXIS:

        return (SDL_JoystickGetAxis(joysticks[index], key_index[key_enum]) < -JOY_DEAD_ZONE);
    case BUTTON:

        return SDL_JoystickGetButton(joysticks[index], key_index[key_enum]);
    case HAT_UP:

        return (SDL_JoystickGetHat(joysticks[index], key_index[key_enum]) & SDL_HAT_UP);
    case HAT_RIGHT:

        return (SDL_JoystickGetHat(joysticks[index], key_index[key_enum]) & SDL_HAT_RIGHT);
    case HAT_DOWN:

        return (SDL_JoystickGetHat(joysticks[index], key_index[key_enum]) & SDL_HAT_DOWN);
    case HAT_LEFT:

        return (SDL_JoystickGetHat(joysticks[index], key_index[key_enum]) & SDL_HAT_LEFT);
    case HAT_UP_RIGHT:
    case HAT_DOWN_RIGHT:
    case HAT_DOWN_LEFT:
    case HAT_UP_LEFT:
    default:
        // Diagonals are ignored because they are combinations of the cardinals
        return false;
    }
}

bool JoyData::getPress(Sint32 key_enum, SDL_Event const &event) const
{
    if (index < 0) {
        return false;
    }

    switch (key_type[key_enum]) {
    case BUTTON:
        if (event.type == SDL_JOYBUTTONDOWN) {
            return ((event.jbutton.which == index) && (event.jbutton.button == key_index[key_enum]));
        }

        break;
    case POS_AXIS:
        if (event.type == SDL_JOYAXISMOTION) {
            return ((event.jaxis.which == index) && (event.jaxis.axis == key_index[key_enum]) && (event.jaxis.value > JOY_DEAD_ZONE));
        }

        break;
    case NEG_AXIS:
        if (event.type == SDL_JOYAXISMOTION) {
            return ((event.jaxis.which == index) && (event.jaxis.axis == key_index[key_enum]) && (event.jaxis.value < -JOY_DEAD_ZONE));
        }

        break;
    case HAT_UP:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_UP));
    case HAT_RIGHT:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_RIGHT));
    case HAT_DOWN:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_DOWN));
    case HAT_LEFT:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_LEFT));
    case HAT_UP_RIGHT:
    case HAT_DOWN_RIGHT:
    case HAT_DOWN_LEFT:
    case HAT_UP_LEFT:
    default:
        // Diagonals are ignored because they are combinations of the cardinals

        return false;
    }

    return false;
}

bool JoyData::getRelease(Sint32 key_enum, SDL_Event const &event) const
{
    if (index < 0) {
        return false;
    }

    switch (key_type[key_enum]) {
    case BUTTON:
        if (event.type == SDL_JOYBUTTONUP) {
            return ((event.jbutton.which == index) && (event.jbutton.button == key_index[key_enum]));
        }

        break;
    case POS_AXIS:
        if (event.type == SDL_JOYAXISMOTION) {
            return ((event.jaxis.which == index) && (event.jaxis.axis == key_index[key_enum]) && (event.jaxis.value < JOY_DEAD_ZONE));
        }

        break;
    case NEG_AXIS:
        if (event.type == SDL_JOYAXISMOTION) {
            return ((event.jaxis.which == index) && (event.jaxis.axis == key_index[key_enum]) && (event.jaxis.value > - JOY_DEAD_ZONE));
        }

        break;
    case HAT_UP:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_UP));
    case HAT_RIGHT:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_RIGHT));
    case HAT_DOWN:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_DOWN));
    case HAT_LEFT:

        return ((event.jhat.which == index) && (event.jhat.hat == key_index[key_enum]) && (event.jhat.value & SDL_HAT_LEFT));
    case HAT_UP_RIGHT:
    case HAT_DOWN_RIGHT:
    case HAT_DOWN_LEFT:
    case HAT_UP_LEFT:
    default:
        // Diagonals are ignored because they are combinations of the cardinals

        return false;
    }

    return false;
}

bool JoyData::hasButtonSet(Sint32 key_enum) const
{
    return ((index >= 0) && (key_type[key_enum] != NONE));
}

void init_joysticks()
{
    // Set up joysticks
    for (Uint16 i = 0; i < MAX_NUM_JOYSTICKS; ++i) {
        joysticks[i] = nullptr;
    }

    Sint32 numjoy;

    numjoy = SDL_NumJoysticks();

    for (Sint32 i = 0; i < numjoy; ++i) {
        joysticks[i] = SDL_JoystickOpen(i);

        if (joysticks[i] != nullptr) {
            player_joy[i] = JoyData(i);
        }
    }

    SDL_JoystickEventState(SDL_ENABLE);
}

bool playerHasJoystick(Sint32 player_num)
{
    return (player_joy[player_num].index >= 0);
}

void disablePlayerJoystick(Sint32 player_num)
{
    player_joy[player_num].index = -1;
}

void resetJoystick(Sint32 player_num)
{
    // FIXME: SDL2 supports hotplugging, so I don't need to restart the joystick
    //        subsystem

    // Reset joystick subsystem
    if (SDL_WasInit(SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK) {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    // Set up joysticks
    for (Sint32 i = 0; i < MAX_NUM_JOYSTICKS; ++i) {
        joysticks[i] = nullptr;
    }

    Sint32 numjoy = SDL_NumJoysticks();

    for (Sint32 i = 0; i < numjoy; ++i) {
        joysticks[i] = SDL_JoystickOpen(i);

        if (joysticks[i] == nullptr) {
            continue;
        }

        // The joystick indices might hcange here.
        // FIXME: There's a chance that players will not have the joysticks
        //        they expect and so they might have buttons, etc. that are
        //        out of range for the new joystick.
    }

    SDL_JoystickEventState(SDL_ENABLE);

    player_joy[player_num] = JoyData(player_num);
}

void handle_joy_event(SDL_Event const &event)
{
    std::stringstream buf;
    buf << "Joystick event!" << std::endl;

    Log("%s", buf.str().c_str());
    switch (event.type) {
    case SDL_JOYAXISMOTION:
        if ((event.jaxis.value > 8000) || (event.jaxis.value < -800)) {
            key_press_event = 1;
        }

        break;
    case SDL_JOYBUTTONDOWN:
        key_press_event = 1;

        break;
    case SDL_JOYBUTTONUP:

        break;
    }
}

bool isJoystickEvent(SDL_Event const &event)
{
    // Does not handle button up, hats, or balls
    return ((event.type == SDL_JOYAXISMOTION)
            || (event.type == SDL_JOYHATMOTION)
            || (event.type == SDL_JOYBUTTONDOWN));
}

void handle_joystick_events(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_JOYAXISMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        handle_joy_event(event);

        break;
    default:
        break;
    }
}

Sint16 query_joystick_key_press_event()
{
    return key_press_event;
}

void clear_joystick_key_press_event()
{
    key_press_event = 0;
}
