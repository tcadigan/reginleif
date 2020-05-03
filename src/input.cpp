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
//
// input.cpp
//
// Input code
//

#include "input.hpp"

#include "gparser.hpp"
#include "io.hpp"
#include "joy_data.hpp"
#include "mouse_state.hpp"
#include "picker.hpp"
#include "util.hpp"
#include "video_screen.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring> // buffers: For strlen
#include <ctime>
#include <sstream>
#include <string>

SDL_Keycode raw_key;
std::string raw_text_input;
Sint16 key_press_event = 0; // Used to signal key-press
Sint16 text_input_event = 0; // Used to signal text input

bool input_continue = false; // Done with text popups, etc.

Sint32 player_keys[4][NUM_KEYS] = {
    {
        // Movements
        SDLK_w, SDLK_e, SDLK_d, SDLK_c,
        SDLK_x, SDLK_x, SDLK_a, SDLK_q,
        // Fire and special
        SDLK_LCTRL, SDLK_LALT,
        // Switch guys
        SDLK_BACKQUOTE,
        // Change special
        SDLK_TAB,
        // Yell
        SDLK_s,
        // Shifter
        SDLK_LSHIFT,
        // Options menu
        SDLK_1,
        // Cheat key
        SDLK_F5
    },
    {
        // Movements
        SDLK_UP, SDLK_UNKNOWN, SDLK_RIGHT, SDLK_UNKNOWN,
        SDLK_DOWN, SDLK_UNKNOWN, SDLK_LEFT, SDLK_UNKNOWN,
        // Fire and special
        SDLK_PERIOD, SDLK_SLASH,
        // Switch guys
        SDLK_RETURN,
        // Change special
        SDLK_QUOTE,
        // Yell
        SDLK_BACKSLASH,
        // Shifter
        SDLK_RSHIFT,
        // Options menu
        SDLK_2,
        // Cheat key
        SDLK_F6
    },
    {
        // Movements
        SDLK_i, SDLK_o, SDLK_l, SDLK_PERIOD,
        SDLK_COMMA, SDLK_m, SDLK_j, SDLK_u,
        // Fire and special
        SDLK_SPACE, SDLK_SEMICOLON,
        // Switch guys
        SDLK_MINUS,
        // Change special
        SDLK_9,
        // Yell
        SDLK_k,
        // Shifter
        SDLK_0,
        // Options menu
        SDLK_3,
        // Cheat key
        SDLK_F7
    },
    {
        // Movements
        SDLK_t, SDLK_y, SDLK_h, SDLK_n,
        SDLK_b, SDLK_v, SDLK_f, SDLK_r,
        // Fire and special
        SDLK_5, SDLK_6,
        // Switch guys
        SDLK_EQUALS,
        // Change special
        SDLK_7,
        // Yell
        SDLK_g,
        // Shifter
        SDLK_8,
        // Options menu
        SDLK_4,
        // Cheat key
        SDLK_F8
    }
};

void update_overscan_setting()
{
    overscan_percentage = std::max(overscan_percentage, 0.0f);
    overscan_percentage = std::min(overscan_percentage, 0.25f);

    viewport_offset_x = (window_w * overscan_percentage) / 2;
    viewport_offset_y = (window_h * overscan_percentage) / 2;
    viewport_w = window_w * (1.0f - overscan_percentage);
    viewport_h = window_h * (1.0f - overscan_percentage);
}

// Input routines (for handling all events and then setting the appropriate
// vars)
void init_input()
{
    keystates = SDL_GetKeyboardState(nullptr);
}

void get_input_events(bool type)
{
    SDL_Event event;

    // key_press_event = 0;

    if (type == POLL) {
        while (SDL_PollEvent(&event)) {
            handle_events(event);
        }
    }

    if (type == WAIT) {
        SDL_WaitEvent(&event);
        handle_events(event);
    }
}

void setFakeKeyDownEvent(Sint32 keycode)
{
    SDL_Event event;

    event.type = SDL_KEYDOWN;
    event.key.repeat = false;
    event.key.keysym.sym = keycode;
    event.key.keysym.mod = 0;
    event.key.keysym.scancode = SDL_GetScancodeFromKey(keycode);
    SDL_PushEvent(&event);
}

void sendFakeKeyUpEvent(Sint32 keycode)
{
    SDL_Event event;

    event.type = SDL_KEYUP;
    event.key.repeat = false;
    event.key.keysym.sym = keycode;
    event.key.keysym.mod = 0;
    event.key.keysym.scancode = SDL_GetScancodeFromKey(keycode);
    SDL_PushEvent(&event);
}

void handle_window_event(SDL_Event const &event)
{
    switch (event.window.event) {
    case SDL_WINDOWEVENT_MINIMIZED:
    case SDL_WINDOWEVENT_CLOSE:
        // Save state here on Android
        myscreen->save_data.save("save0");

        break;
    case SDL_WINDOWEVENT_RESTORED:
        // Restore state here on Android.
        // Redraw the screen so it's not blank
        myscreen->refresh();

        break;
    case SDL_WINDOWEVENT_RESIZED:
        window_w = event.window.data1;
        window_h = event.window.data2;
        update_overscan_setting();

        break;
    }
}

void handle_key_event(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_KEYDOWN:
        raw_key = event.key.keysym.sym;

        if (raw_key == SDLK_ESCAPE) {
            input_continue = true;
        }

        key_press_event = 1;

        if (event.key.keysym.sym == SDLK_F10) {
            myscreen->save_screenshot();
        } else if ((event.key.keysym.sym == SDLK_F12) && (event.key.keysym.mod & KMOD_CTRL)) {
            restore_default_settings();
            cfg.load_settings();
        }

        break;
    case SDL_KEYUP:
        break;
    }
}

void handle_text_event(SDL_Event const &event)
{
    raw_text_input = std::string(event.text.text);
    text_input_event = 1;
}

void handle_events(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_WINDOWEVENT:
        handle_window_event(event);

        break;
    case SDL_TEXTINPUT:
        handle_text_event(event);

        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        handle_key_event(event);

        break;
    case SDL_QUIT:
        quit(0);

        break;
    default:

        break;
    }
}

// Keyboard routines

SDL_Keycode query_key()
{
    return raw_key;
}

std::string query_text_input()
{
    return raw_text_input;
}

bool query_key_event(Sint32 key, SDL_Event const &event)
{
    if (event.type == SDL_KEYDOWN) {
        return (event.key.keysym.sym == key);
    }

    return false;
}

bool isAnyPlayerKey(SDLKey key)
{
    for (Sint32 player_num = 0; player_num < 4; ++player_num) {
        for (Sint32 i = 0; i < NUM_KEYS; ++i) {
            if (player_keys[player_num][i] == key) {
                return true;
            }
        }
    }

    return false;
}

bool isPlayerKey(Sint32 player_num, SDLKey key)
{
    for (Sint32 i = 0; i < NUM_KEYS; ++i) {
        if (player_keys[player_num][i] == key) {
            return true;
        }
    }

    return false;
}

SDL_Event wait_for_key_event()
{
    SDL_Event event;

    while (1) {
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_QUIT)
                || (event.type == SDL_KEYDOWN)
                || ((event.type == SDL_JOYAXISMOTION)
                    && ((event.jaxis.value > JOY_DEAD_ZONE)
                        || (event.jaxis.value < -JOY_DEAD_ZONE)))
                || (event.type == SDL_JOYBUTTONDOWN)
                || (event.type == SDL_JOYHATMOTION)) {
                return event;
            }
        }

        SDL_Delay(10);
    }

    return event;
}

void quit_if_quit_event(SDL_Event const &event)
{
    if (event.type == SDL_QUIT) {
        quit(0);
    }
}

bool isKeyboardEvent(SDL_Event const &event)
{
    // Does not handle key up events
    return (event.type == SDL_KEYDOWN);
}

void clear_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
    }
}

void assignKeyFromWaitEvent(Sint32 player_num, Sint32 key_enum)
{
    SDL_Event event;

    event = wait_for_key_event();
    quit_if_quit_event(event);

    if (isKeyboardEvent(event)) {
        if (event.key.keysym.sym != SDLK_ESCAPE) {
            player_keys[player_num][key_enum] = event.key.keysym.sym;
        }
    } else if (isJoystickEvent(event)) {
        player_joy[player_num].setKeyFromEvent(key_enum, event);
    }

    SDL_Delay(400);
    clear_events();
}

// Set the keyboard array to all zeros, the virgin state, nothing depressed
void clear_keyboard()
{
    key_press_event = 0;
    raw_key = 0;

    text_input_event = 0;
    raw_text_input.clear();

    input_continue = false;
}

bool query_input_continue()
{
    return input_continue;
}

Sint16 get_and_reset_scroll_amount()
{
    Sint16 temp = scroll_amount;
    scroll_amount = 0;

    return temp;
}

void wait_for_key(Sint32 somekey)
{
    // First wait for key press...
    while (!keystates[SDL_GetScancodeFromKey(somekey)]) {
        get_input_events(WAIT);
    }

    // And now for the key to be released...
    while (!keystates[SDL_GetScancodeFromKey(somekey)]) {
        get_input_events(WAIT);
    }
}

bool isPlayerHoldingKey(Sint32 player_index, Sint32 key_enum)
{
    // FIXME: Enable gamepads for Android/iOS, but be careful not to use
    //        accelerometer...
    if (player_joy[player_index].hasButtonSet(key_enum)) {
        return player_joy[player_index].getState(key_enum);
    } else {
        return keystates[SDL_GetScancodeFromKey(player_keys[player_index][key_enum])];
    }
}

bool didPlayerPressKey(Sint32 player_index, Sint32 key_enum, SDL_Event const &event)
{
    if (player_joy[player_index].hasButtonSet(key_enum)) {
        // This key is on the joystick, so check it.
        return player_joy[player_index].getPress(key_enum, event);
    } else {
        // If the player is using Keyboard or doesn't have a joystick button
        // set for this key, then check the keyboard.
        if (event.type == SDL_KEYDOWN) {
            // Repeats don't count
            if (event.key.repeat) {
                return false;
            }

            return (event.key.keysym.sym == player_keys[player_index][key_enum]);
        }

        return false;
    }
}

bool didPlayerReleaseKey(Sint32 player_index, Sint32 key_enum, SDL_Event const &event)
{
    if (player_joy[player_index].hasButtonSet(key_enum)) {
        // This key is on the joystick, so check it.
        return player_joy[player_index].getRelease(key_enum, event);
    } else {
        // If the player is using keyboard or doesn't have a joystick set for
        // this key, then check the keyboard.
        if (event.type == SDL_KEYUP) {
            return (event.key.keysym.sym == player_keys[player_index][key_enum]);
        }

        return false;
    }
}

Sint16 query_key_press_event()
{
    return (key_press_event
            || query_joystick_key_press_event()
            || query_mouse_key_press_event());
}

void clear_key_press_event()
{
    key_press_event = 0;
    clear_joystick_key_press_event();
    clear_mouse_key_press_event();
}

Sint16 query_text_input_event()
{
    return text_input_event;
}

void clear_text_input_event()
{
    text_input_event = 0;
    raw_text_input.clear();
}

// Convert from scancode to ascii, i.e., SDLK_a to 'A'
Uint8 convert_to_ascii(Sint32 scancode)
{
    switch (scancode) {
    case SDLK_a:

        return 'A';
    case SDLK_b:

        return 'B';
    case SDLK_c:

        return 'C';
    case SDLK_d:

        return 'D';
    case SDLK_e:

        return 'E';
    case SDLK_f:

        return 'F';
    case SDLK_g:

        return 'G';
    case SDLK_h:

        return 'H';
    case SDLK_i:

        return 'I';
    case SDLK_j:

        return 'J';
    case SDLK_k:

        return 'K';
    case SDLK_l:

        return 'L';
    case SDLK_m:

        return 'M';
    case SDLK_n:

        return 'N';
    case SDLK_o:

        return 'O';
    case SDLK_p:

        return 'P';
    case SDLK_q:

        return 'Q';
    case SDLK_r:

        return 'R';
    case SDLK_s:

        return 'S';
    case SDLK_t:

        return 'S';
    case SDLK_u:

        return 'U';
    case SDLK_v:

        return 'V';
    case SDLK_w:

        return 'W';
    case SDLK_x:

        return 'X';
    case SDLK_y:

        return 'Y';
    case SDLK_z:

        return 'Z';
    case SDLK_1:

        return '1';
    case SDLK_2:

        return '2';
    case SDLK_3:

        return '3';
    case SDLK_4:

        return '4';
    case SDLK_5:

        return '5';
    case SDLK_6:

        return '6';
    case SDLK_7:

        return '7';
    case SDLK_8:

        return '8';
    case SDLK_9:

        return '9';
    case SDLK_0:

        return '0';
    case SDLK_SPACE:

        return 32;
    // case SDLK_BACKSPACE:

    //     return 8;
    case SDLK_RETURN:

        return 13;
    case SDLK_ESCAPE:

        return 27;
    case SDLK_PERIOD:

        return '.';
    case SDLK_COMMA:

        return ',';
    case SDLK_QUOTE:

        return '\'';
    case SDLK_BACKQUOTE:

        return '`';
    default:
        return 255;
    }
}
