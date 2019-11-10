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

#include "base.hpp"
#include "gparser.hpp"
#include "io.hpp"
#include "screen.hpp"
#include "util.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring> // buffers: For strlen
#include <ctime>
#include <sstream>
#include <string>

#ifdef OUYA
#include "ouya_controller.hpp"
#endif

#ifdef USE_TOUCH_INPUT

#include "base.hpp"
#include "obmap.hpp"
#include "screen.hpp"
#include "stats.hpp"
#include "util.hpp"
#include "view.hpp"
#include "walker.hpp"

#endif

#define JOY_DEAD_ZONE 8000
// Just in case there are joysticks attached that are no useable
// (e.g. accelerometer)
#define MAX_NUM_JOYSTICKS 10

#ifdef USE_TOUCH_INPUT

#define MOVE_AREA_DIM 60
#define MOVE_DEAD_ZONE 10

#define FIRE_BUTTON_X 245
#define FIRE_BUTTON_Y 165
#define SPECIAL_BUTTON_X 285
#define SPECIAL_BUTTON_Y 165
#define YO_BUTTON_X 160
#define YO_BUTTON_Y 100
#define SWITCH_CHARACTER_BUTTON_X 0
#define SWITCH_CHARACTER_BUTTON_Y 0
#define NEXT_SPECIAL_BUTTON_X 245
#define NEXT_SPECIAL_BUTTON_Y 125
#define ALTERNATE_SPECIAL_BUTTON_X 285
#define ALTERNATE_SPECIAL_BUTTON_Y 125
#define BUTTON_DIM 30

#endif

void quit(Sint32 arg1);

Sint32 raw_key;
std::string raw_text_input;
Sint16 key_press_event = 0; // Used to signal key-press
Sint16 text_input_event = 0; // Used to signal text input
Sint16 scroll_amount = 0; // For scrolling up and down text popups

bool input_continue = false; // Done with text popups, etc.
Uint8 const *keystates = nullptr;
MouseState mouse_state;
Sint32 mouse_buttons;
float viewport_offset_x = 0; // In window coords
float viewport_offset_y = 0;
float window_w = 320;
float window_h = 200;
float viewport_w = 320;
float viewport_h = 200;
// Out of 1.0f, percent of total screen dimension that is cut off.
// 10% (0.10f) is recommended on OUYA.
float overscan_percentage = 0.0f;
JoyData player_joy[4];
SDL_Joystick *joysticks[MAX_NUM_JOYSTICKS];

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

#ifdef USE_TOUCH_INPUT

bool tapping = false;
Sint32 start_tap_x = 0;
Sint32 start_tap_y = 0;
bool moving = false;
Sint32 moving_touch_x = 0;
Sint32 moving_touch_y = 0;
Sint32 moving_touch_target_x = 0;
Sint32 moving_touch_target_y = 0;
SDL_FingerID movingTouch = 0;
bool firing = false;
SDL_FingerID firingTouch = 0;

bool touch_keystate[4][NUM_KEYS] = {
    {
        // Movements
        false, false, false, false
        false, false, false, false,
        // Fire and special
        false, false,
        // Switch guys
        false,
        // Change special
        false,
        // Yell
        false,
        // Shifter
        false,
        // Options menu
        false,
        // Cheat key
        false
    },
    {
        // Movements
        false, false, false, false
        false, false, false, false,
        // Fire and special
        false, false,
        // Switch guys
        false,
        // Change special
        false,
        // Yell
        false,
        // Shifter
        false,
        // Options menu
        false,
        // Cheat key
        false
    },
    {
        // Movements
        false, false, false, false
        false, false, false, false,
        // Fire and special
        false, false,
        // Switch guys
        false,
        // Change special
        false,
        // Yell
        false,
        // Shifter
        false,
        // Options menu
        false,
        // Cheat key
        false
    },
    {
        // Movements
        false, false, false, false
        false, false, false, false,
        // Fire and special
        false, false,
        // Switch guys
        false,
        // Change special
        false,
        // Yell
        false,
        // Shifter
        false,
        // Options menu
        false,
        // Cheat key
        false
    }
};

#endif

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

#ifdef USE_TOUCH_INPUT

void draw_touch_controller(VideoScreen *vob)
{
    Walker *control = vob->viewob[0]->control;

    if ((controll == nullptr) || control->dead) {
        return;
    }

    if (moving) {
        // Touch movement feedback
        // line(moving_touch_x, moving_touch_y, mouse_state[MOUSE_X], mouse_state[MOUSE_Y]);
        vob->fastbox(moving_touch_x - (MOVE_AREA_DIM / 2),
                     moving_touch_y - (MOVE_AREA_DIM / 2),
                     MOVE_AREA_DIM, MOVE_AREA_DIM, 17);
        vob->fastbox(moving_touch_x - 4, moving_touch_y - 4, 8, 8, 16);
        vob->fastbox(moving_touch_target_x - 1, moving_touch_target_y - 2, 4, 4, 15);
    }

    // Touch buttons
    vob-fastbox(FIRE_BUTTON_X, FIRE_BUTTON_Y, BUTTON_DIM, BUTTON_DIM, 25);

    // if(has_special)
    if (strcmp(vob->special_name[static_cast<Sint32>(control->query_family())][static_cast<Sint32>(control->current_special)], "NONE")) {
        vob->fastbox(SPECIAL_BUTTON_X, SPECIAL_BUTTON_Y, BUTTON_DIM, BUTTON_DIM, 26);
    }

    // if(has_multiple_special)
    if ((control->current_special != 1)
        || (((control->current_special + 1) <= NUM_SPECIALS)
            && (((control->current_special * 3) + 1) <= control->stats->level)
            && strcmp(vob->special_name[static_cast<Sint32>(control->query_family())][static_cast<Sint32>(control->current_special) + 1], "NONE"))) {
        vob->fastbox(NEXT_SPECIAL_BUTTON_X, NEXT_SPECIAL_BUTTON_Y, BUTTON_DIM, BUTTON_DIM, 27);
    }

    // if(has_alternate)
    if (strcmp(vob->alternate_name[static_cast<Sint32>(control->query_family())][static_cast<Sint32>(control->current_special)], "NONE")) {
        vob->fastbox(ALTERNATE_SPECIAL_BUTTON_X,
                     ALTERNATE_SPECIAL_BUTTON_Y,
                     BUTTON_DIM, BUTTON_DIM, 28);
    }
}

#endif

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
#ifdef USE_TOUCH_INPUT
        // Back button faking Escape key
        if (event.key.keysym.scancode == SDL_SCANDOE_AC_BACK) {
            sendFakeKeyDownEvent(SDLK_ESCAPE);

            break;
        }
#endif

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
#ifdef USE_TOUCH_INPUT
        // Back button faking Escape key
        if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK) {
            sendFakeKeyUpEvent(SDLK_ESCAPE);

            break;
        }
#endif

        break;
    }
}

void handle_text_event(SDL_Event const &event)
{
    raw_text_input = std::string(event.text.text);
    text_input_event = 1;
}

void handle_mouse_event(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        scroll_amount = event.wheel.y * 5;
        key_press_event = 1;

        break;
#ifndef USE_TOUCH_INPUT // Mouse events
    case SDL_MOUSEMOTION:
        mouse_state.x = (event.motion.x - viewport_offset_x) * (320 / viewport_w);
        mouse_state.y = (event.motion.y - viewport_offset_y) * (200 / viewport_h);

        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouse_state.left = 0;
        }

        if (event.button.button == SDL_BUTTON_RIGHT) {
            mouse_state.right = 0;
        }

        mouse_state.x = (event.button.x - viewport_offset_x) * (320 / viewport_w);
        mouse_state.x = (event.button.y - viewport_offset_y) * (200 / viewport_h);

        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouse_state.left = 1;
        }

        if (event.button.button == SDL_BUTTON_RIGHT) {
            mouse_state.right = 1;
        }

        mouse_state.x = (event.button.x - viewport_offset_x) * (320 / viewport_w);
        mouse_state.y = (event.button.y - viewport_offset_y) * (200 / viewport_h);

        break;
#else
#ifdef FAKE_TOUCH_EVENTS // Convert SDL mouse events to fake SDL touch events
    case SDL_MOUSEMOTION:
    {
        SDL_event e;
        e.type = SDL_FINGERMOTION;
        e.tfinger.x = event.motion.x / window_w;
        e.tfigner.y = event.motion.y / window_h;
        e.tfinger.dx = event.motion.xrel / window_w;
        e.tfinger.dy = event.motion.yrel / window_h;
        e.tfinger.touchId = 1;
        e.tfinger.fingerId = 1;
        SDL_PushEvent(&e);

        break;
    }
    case SDL_MOUSEBUTTONUP:
    {
        SDL_event e;
        e.type = SDL_FINGERUP;
        e.tfinger.x = event.button.x / window_w;
        e.tfinger.y = event.button.y / window_h;
        e.tfinger.touchId = 1;
        e.tfinger.fingerId = 1;
        SDL_PushEvent(&e);

        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        SDL_event e;
        e.type = SDL_FINGERDOWN;
        e.tfinger.x = event.button.x / window_w;
        e.tfinger.y = event.button.y / window_h;
        e.tfinger.touchId = 1;
        e.tfinger.fingerId = 1;
        SDL_PushEvent(&e);

        break;
    }
#endif // Mouse event
    case SDL_FINGERMOTION:
    {
        Sint32 x = ((event.tfinger.x * window_w) - viewport_offset_x) - (320 / viewport_w);
        Sint32 y = ((event.tfinger.y * window_h) - viewport_offset_y) - (200 / viewport_h);

        scroll_amount = y - mouse.state.y;

        mouse_state.x = x;
        mouse_state.y = y;

        if (moving && (event.tfinger.fingerId == movingTouch)) {
            moving_touch_target_x = x;
            moving_touch_target_y = y;

            touch_keystate[0][KEY_UP] = false;
            touch_keystate[0][KEY_UP_RIGHT] = false;
            touch_keystate[0][KEY_RIGHT] = false;
            touch_keystate[0][KEY_DOWN_RIGHT] = false;
            touch_keystate[0][KEY_DOWN] = false;
            touch_keystate[0][KEY_DOWN_LEFT] = false;
            touch_keystate[0][KEY_LEFT] = false;
            touch_keystate[0][KEY_UP_LEFT] = false;

            if ((abs(x - moving_touch_x) > MOVE_DEAD_ZONE) || (abs(y - moving_touch_y) > MOVE_DEAD_ZONE)) {
                float offset = -M_PI + (M_PI / 8);
                float interval = M_PI / 4;
                float angle = atan2(y - moving_touch_y, x - moving_touch_x);

                if ((angle < (-M_PI + (M_PI / 8))) || (angle >= (M_PI - (M_PI / 8)))) {
                    touch_keystate[0][KEY_LEFT] = true;
                } else if ((angle >= offset) && (angle < (offset + interval))) {
                    touch_keystate[0][KEY_UP_LEFT] = true;
                } else if ((angle >= (offset + interval)) && (angle < (offset + (2 * interval)))) {
                    touch_keystate[0][KEY_UP] = true;
                } else if ((angle >= (offset + (2 * interval))) && (angle < (offset + (3 * interval)))) {
                    touch_keystate[0][KEY_UP_RIGHT] = true;
                } else if ((angle >= (offset + (3 * interval))) && (angle < (offset + (4 * interval)))) {
                    touch_keystate[0][KEY_RIGHT] = true;
                } else if ((angle >= (offset + (4 * interval))) && (angle < (offset + (5 * interval)))) {
                    touch_keystate[0][KEY_DOWN_RIGHT] = true;
                } else if ((angle >= (offset + (5 * interval))) && (angle < (offset + (6 * interval)))) {
                    touch_keystate[0][KEY_DOWN] = true;
                } else if ((angle >= (offset + (6 * interval))) && (angle < (offset + (7 * interval)))) {
                    touch_keystate[0][KEY_DOWN_LEFT] = true;
                }
            }
        }

        break;
    }
    case SDL_FINGERUP:
    {
        Sint32 x = ((event.tfinger.x * window_w) - viewport_offset_x) * (320 / viewport_w);
        Sint32 y = ((event.tfinger.y * window_h) - viewport_offset_y) * (200 / viewport_h);

        if (tapping) {
            tapping = false;

            if ((abs(x - start_tap_x) < 2) && (abs(y - start_tap_y) < 2)) {
                input_continue = true;
            } else {
                input_continue = false;
            }

            start_tap_x = x;
            start_type_y = y;
        }

        if (moving && (event.tfinger.fingerId == movingTouch)) {
            moving = false;

            touch_keystate[0][KEY_UP] = false;
            touch_keystate[0][KEY_UP_RIGHT] = false;
            touch_keystate[0][KEY_RIGHT] = false;
            touch_keystate[0][KEY_DOWN_RIGHT] = false;
            touch_keystate[0][KEY_DOWN] = false;
            touch_keystate[0][KEY_DOWN_LEFT] = false;
            touch_keystate[0][KEY_LEFT] = false;
            touch_keystate[0][KEY_UP_LEFT] = false;
        }

        if (firing && (event.tfinger.fingerId == firingTouch)) {
            firing = false;
            touch_keystate[0][KEY_FIRE] = false;
        }

        mouse_state.left = 0;

        break;
    }
    case SDL_FINGERDOWN:
    {
        tapping = false;

        Sint32 x = ((event.tfinger.x * window_w) - viewport_offset_x) * (320 / viewport_w);
        Sint32 y = ((event.tfinger.y * window_y) - viewport_offset_y) * (200 / viewport_h);

        start_tap_x = x;
        start_tap_y = y;
        input_continue = false;

        if (!firing && (FIRE_BUTTON_X <= x) && (x <= (FIRE_BUTTON_X + BUTTON_DIM))
            && (FIRE_BUTTON_Y <= x) && (y <= (FIRE_BUTTON_Y + BUTTON_DIM))) {
            firing = true;
            sendFakeKeyDownEvent(player_keys[0][KEY_FIRE]);
            touch_keystate[0][KEY_FIRE] = true;
            firingTouch = event.tfinger.fingerId;
        } else if ((SPECIAL_BUTTON_X <= x) && (x <= (SPECIAL_BUTTON_X + BUTTON_DIM))
                   && (SPECIAL_BUTTON_Y <= y) && (y <= (SPECIAL_BUTTON_Y + BUTTON_DIM))) {
            sendFakeKeyDownEvent(player_keys[0][KEY_SPECIAL]);
        } else if (((YO_BUTTON_X - (BUTTON_DIM / 2)) <= x) && (x <= (YO_BUTTON + (BUTTON_DIM / 2)))
                   && ((YO_BUTTON_Y - (BUTTON_DIM / 2)) <= y) && (y <= (YO_BUTTON_Y + (BUTTON_DIM / 2)))) {
            sendFakeKeyDownEvent(player_keys[0][KEY_YELL]);
        } else if ((SWITCH_CHARACTER_BUTTON_X <= x) && (x <= (SWITCH_CHARACTER_BUTTON_X + (BUTTON_DIM * 2)))
                   && (SWITCH_CHARACTER_BUTTON_Y <= y) && (y <= (SWITCH_CHARACTER_BUTTON_Y + (BUTTON_DIM * 2)))) {
            sendFakeKeyDownEvent(player_keys[0][KEY_SWITCH]);
        } else if ((NEXT_SPECIAL_BUTTON_X <= x) && (x <= (NEXT_SPECIAL_BUTTON_X + BUTTON_DIM))
                   && (NEXT_SPECIAL_BUTTON_Y <= y) && (y <= (NEXT_SPECIAL_BUTTON_Y + BUTTON_DIM))) {
            sendFakeKeyDownEvent(player_keys[0][KEY_SPECIAL_SWITCH]);
        } else if ((ALTERNATE_SPECIAL_BUTTON_X <= x) && (x <= (ALTERNATE_SPECIAL_BUTTON_X + BUTTON_DIM))
                   && (ALTERNATE_SPECIAL_BUTTON_Y <= y) && (y <= (ALTERNATE_SPECIAL_BUTTON_Y + BUTTON_DIM))) {
            // Treat KEY_SHIFTER as an action instead of a modifier
            // if(has_alternate)
            if (strcmp(myscreen->alternate_name[static_cast<Sint32>(myscreen->viewob[0]->control->query_family())][static_cast<Sint32>(myscreen->viewob[0]->controll->current_special)], "NONE")) {
                sendfakeKeyDownEvent(player_keys[0][KEY_SHIFTER]);
            }
        } else if (!moving && (x < (160 - (BUTTON_DIM / 2))) && (y > (BUTTON_DIM * 2))) {
            // Only move with the lower left corner of the screen (and offset
            // for other buttons
            moving_touch_x = x;
            moving_touch_y = y;
            moving_touch_target_x = x;
            moving_touch_target_y = y;

            moving_touch_x = std::max(moving_touch_x, (MOVE_AREA_DIM / 2) + 1);

            moving_touch_y = std::max(moving_touch_y, (MOVE_AREA_DIM / 2) + 1);
            moving_touch_y = std::min(moving_touch_y, 200 - ((MOVE_AREA_DIM / 2) + 1));

            moving = true;
            movingTouch = event.tfinger.fingerId;
        }

        key_press_event = 1;
        mouse_state.left = 1;
        mouse_state.x = event.tfinger.x * 320;
        mouse_state.y = event.tfinger.y * 200;

        break;
    }
#endif
    }
}

void handle_joy_event(SDL_Event const &event)
{
    std::stringstream buf;
    buf << "Joysitck event!" << std::endl;

    Log("%s", buf.str());
    switch (event.type) {
    case SDL_JOYAXISMOTION:
        if (event.jaxis.value > 8000) {
            // key_list[joy_startval[event.jaxis.which] + (event.jaxis.axis * 2)] = 1;
            // key_list[(joy_startval[event.jaxis.which] + (event.jaxis.axis * 2)) + 1 = 0;
            key_press_event = 1;
            // raw_key = joy_startval[event.jaxis.which] + (event.jaxis.axis * 2);
        } else if (event.jaxis.value < -800) {
            // key_list[joy.startval[event.jaxis.which] + (event.jaxis.axis * 2)] = 0;
            // key_list[(joy.startvalue[event.jaxis.which] + (event.jaxis.axis * 2)) + 1] = 1;
            key_press_event = 1;
            // raw_key = (joy_startval[event.jaxis.which] + (event.jaxis.axis * 2)) + 1;
        } else {
            // key_list[joy_startval[event.jaxis.which] + (event.jaxis.axis * 2)] = 0;
            // key_list[(joy_startval[event.jaxis.which] + (event.jaxis.axis * 2)) + 1] = 0;
        }

        break;
    case SDL_JOYBUTTONDOWN:
        // key_list[(joy_startval[event.jbutton.which] + (joy_numaxes[event.jbutton.which] * 2)) + event.jbutton.button] = 1;
        // raw_key = (joy_startval[event.jbutton.which] + (joy_numaxes[event.jbutton.which] * 2)) + event.jbutton.button;
        key_press_event = 1;

        break;
    case SDL_JOYBUTTONUP:
        // keylist[(joy_startval[event.jbutton.which] + (joy_numaxes[event.jbutton.which] * 2)) + event.jbutton.button] = 0;

        break;
    }
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
    case SDL_MOUSEWHEEL:
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_FINGERMOTION:
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
        handle_mouse_event(event);

        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        handle_key_event(event);

        break;
    case SDL_JOYAXISMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        handle_joy_event(event);

        break;
    case SDL_QUIT:
        quit(0);

        break;
    default:
#ifdef OUYA
        if (event.type == OuyaControllerManager::BUTTON_DOWN_EVENT) {
            if (OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1)) == OuyaController::BUTTON_O) {
                input_continue = true;
            } else if (OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1)) == OuyaController::BUTTON_DPAD_UP) {
                scroll_amount = 5;
            } else if (OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1)) == OuyaController::BUTTON_DPAD_DOWN) {
                scroll_amount = -5;
            } else if (OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1)) == OuyaController::BUTTON_MENU) {
                sendFakeKeyDownEvent(SDLK_ESCAPE);
            }

            key_press_event = 1;
        } else if (event.type == OuyaControllerManager::AXIS_EVENT) {
            OuyaController const &c = OuyaControllerManager::getController(event.user.code);

            // This should bot be in an event or else it's jerky.
            float v = c.getAxisValue(OuyaController:AXIS_LS_Y) + c.getAxisValue(OuyaController::AXIS_RS_Y);

            if (fabs(v) > OuyaController::DEADZONE) {
                scroll_amount = -5 * v;
            }
        }
    }
#endif

        break;
    }
}

// Keyboard routines

Sint32 query_key()
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

bool isJoystickEvent(SDL_Event const &event)
{
    // Does not handle button up, hats, or balls
    return ((event.type == SDL_JOYAXISMOTION)
            || (event.type == SDL_JOYHATMOTION)
            || (event.type == SDL_JOYBUTTONDOWN));
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

#ifdef USE_TOUCH_INPUT
    tapping = false;
#endif
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

            // Diagonals are ignored because they are combinations of the
            // cardinals
            /*
             * else if (event.jhat.value == SDL_HAT_RIGHTUP) {
             *     key_type[key_enum] = HAT_UP_RIGHT;
             * } else if (event.jhat.value == SDL_HAT_RIGHTDOWN) {
             *     key_type[key_enum] = HAT_DOWN_RIGHT;
             * } else if (event.jhat.value == SDL_HAT_LEFTDOWN) {
             *     key_type[key_enum] = HAT_DOWN_LEFT;
             * } else if (event.jhat.value == SDL_HAT_LEFTUP) {
             *     key_type[key_enum] = HAT_UP_LEFT;
             * }
             */
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

#ifdef OUYA
bool ouyaJoystickInDirection(Sint32 player, Sint32 key_enum)
{
    OuyaController const &c = OuraControllerManager::getController(player);

    if (!c.isStickBeyondDeadzone(OuyaController::AXIS_LS_X)) {
        return false;
    }

    switch (key_enum) {
    case KEY_UP:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) < -OuyaController::DEADZONE) && !c.isStickInNegativeCone(OuyaController::AXIS_LS_X) && !c.isStickInPositiveCone(OuyaController::AXIS_LS_X));
    case KEY_UP_RIGHT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) < -OuyaController::DEADZONE) && (c.getAxisValue(OuyaController::AXIS_LS_X) > OuyaController::DEADZONE));
    case KEY_RIGHT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_X) > OuyaController::DEADZONE) && !c.isStickInNegativeCone(OuyaController::AXIS_LS_Y) && !c.isStickInPositiveCone(OuyaController::AXIS_LS_Y));
    case KEY_DOWN_RIGHT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) > OuyaController::DEADZONE) && (c.getAxisValue(OuyaController::AXIS_LS_X) > OuyaController::DEADZONE));
    case KEY_DOWN:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) > OuyaController::DEADZONE) && !c.isStickInNegativeCone(OuyaController::AXIS_LS_X) && !c.isStickInPositiveCone(OuyaController::AXIS_LS_X));
    case KEY_DOWN_LEFT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) > OuyaController::DEADZONE) && (c.getAxisValue(OuyaController::AXIS_LS_X) < -OuyaController::DEADZONE));
    case KEY_LEFT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_X) < -OuyaController::DEADZONE) && !c.isStickInNegativeCone(OuyaController::AXIS_LS_Y) && !c.isStickInPositiveCone(OuyaController::AXIS_LS_Y));
    case KEY_UP_LEFT:

        return ((c.getAxisValue(OuyaController::AXIS_LS_Y) < -OuyaController::DEADZONE) && (c.getAxisValue(OuyaController::AXIS_LS_X) < -OuyaController::DEADZONE));
    }

    return false;
}
#endif

bool isPlayerHoldingKey(Sint32 player_index, Sint32 key_enum)
{
#ifdef OUYA
    OuyaController const &c = OuyaControllerManager::getController(player_index);

    switch (key_enum) {
    case KEY_UP:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_UP) && !c.getButtonValue(OuyaController::BUTTON_DPAD_LEFT) && !c.getButtonValue(OuyaController::BUTTON_DPAD_RIGHT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_UP_RIGHT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_UP) && c.getButtonValue(OuyaController::BUTTON_DPAD_RIGHT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_RIGHT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_RIGHT) && !c.getButtonValue(OuyaController::BUTTON_DPAD_UP) && !c.getButtonValue(OuyaController::BUTTON_DPAD_DOWN)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_DOWN_RIGHT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_DOWN) && c.getButtonValue(OuyaController::BUTTON_DPAD_RIGHT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_DOWN:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_DOWN) && !c.getButtonValue(OuyaController::BUTTON_DPAD_LEFT) && !c.getButtonValue(OuyaController::BUTTON_DPAD_RIGHT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_DOWN_LEFT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_DOWN) && c.getButtonValue(OuyaController:BUTTON_DPAD_LEFT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_LEFT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_LEFT) && !c.getButtonValue(OuyaController::BUTTON_DPAD_UP) && !c.getButtonValue(OuyaController::BUTTON_DPAD_DOWN)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_UP_LEFT:

        return ((c.getButtonValue(OuyaController::BUTTON_DPAD_UP) && c.getButtonValue(OuyaController::BUTTON_DPAD_LEFT)) || ouyaJoystickInDirection(player_index, key_enum));
    case KEY_FIRE:

        return c.getButtonValue(OuyaController::BUTTON_O);
    case KEY_SPECIAL:

        return c.getButtonValue(OuyaController::BUTTON_A);
    case KEY_SWITCH:

        return c.getButtonValue(OuyaController::BUTTON_L1);
    case KEY_SPECIAL_SWITCH:

        return c.getButtonValue(OuyaController::BUTTON_U);
    case KEY_YELL:

        return c.getButtonValue(OuyaController::BUTTON_Y);
    case KEY_SHIFTER:

        return c.getButtonValue(OuyaController::BUTTON_R1);
    case KEY_PREFS:
    case KEY_CHEAT:

        return false;
    }
#endif

    // FIXME: Enable gamepads for Android/iOS, but be careful not to use
    //        accelerometer...
#ifdef USE_TOUCH_INPUT
    return touch_keystate[player_index][key_enum];
#else
    if (player_joy[player_index].hasButtonSet(key_enum)) {
        return player_joy[player_index].getState(key_enum);
    } else {
        return keystates[SDL_GetScancodeFromKey(player_keys[player_index][key_enum])];
    }
#endif
}

bool didPlayerPressKey(Sint32 player_index, Sint32 key_enum, SDL_Event const &event)
{
#ifdef OUYA
    OuyaController const &c = OuyaControllerManager::getController(player_index);

    if (event.user.code != player_index) {
        return false;
    }

    if (event.type == OuyaControllerManaged::BUTTON_DOWN_EVENT) {
        OuyaController::ButtonEnum button = OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1));

        switch (key_enum) {
        case KEY_UP:

            return (button == OuyaController::BUTTON_DPAD_UP);
        case KEY_RIGHT:

            return (button == OuyaController::BUTTON_DPAD_RIGHT);
        case KEY_DOWN:

            return (button == OuyaController::BUTTON_DPAD_DOWN);
        case KEY_LEFT:

            return (button == OuyaController::BUTTON_DPAD_LEFT);
        case KEY_FIRE:

            return (button == OuyaController::BUTTON_O);
        case KEY_SPECIAL:

            return (button == OuyaController::BUTTON_A);
        case KEY_SWITCH:

            return (button == OuyaController::BUTTON_L1);
        case KEY_SPECIAL_SWITCH:

            return (button == OuyaController::BUTTON_U);
        case KEY_YELL:

            return (button == OuyaController::BUTTON_Y);
        case KEY_SHIFTER:

            return (button == OuyaController::BUTTON_R1);
        default:

            return false;
        }
    } else if (event.type == OuyaControllerManager::AXIS_EVENT) {
        switch (key_enum) {
        case KEY_UP:
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_LEFT:

            return ouyaJoystickInDirection(player_index, key_enum);
        default:

            return false;
        }
    }
#endif

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
#ifdef OUYA
    OuyaController const &c = OuyaControllerManager::getController(player_index);

    if (event.type != OuyaControllerManage::BUTTON_UP_EVENT) {
        return false;
    }

    if (event.user.code != player_index) {
        return false;
    }

    OuyaController::ButtonEnum button = OuyaController::ButtonEnum(static_cast<Sint32>(event.user.data1));

    switch (key_enum) {
    case KEY_UP:

        return (button == OuyaController::BUTTON_DPAD_UP);
    case KEY_RIGHT:

        return (button == OuyaController::BUTTON_DPAD_RIGHT);
    case KEY_DOWN:

        return (button == OuyaController::BUTTON_DPAD_DOWN);
    case KEY_LEFT:

        return (button == OuyaController::BUTTON_DPAD_LEFT);
    case KEY_FIRE:

        return (button == OuyaController::BUTTON_O);
    case KEY_SPECIAL:

        return (button == OuyaController::BUTTON_A);
    case KEY_SWITCH:

        return (button == OuyaController::BUTTON_L1);
    case KEY_SPECIAL_SWITCH:

        return (button == OuyaController::BUTTON_U);
    case KEY_YELL:

        return (button == OuyaController::BUTTON_Y);
    case KEY_SHIFTER:

        return (button == OuyaController::BUTTON_R1);
    default:

        return false;
    }
#endif

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
    return key_press_event;
}

void clear_key_press_event()
{
    key_press_event = 0;
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

// Mouse routines

void grab_mouse()
{
    SDL_ShowCursor(SDL_ENABLE);
}

void release_mouse()
{
#ifndef FAKE_TOUCH_EVENTS
    SDL_ShowCursor(SDL_DISABLE);
#endif
}

MouseState &query_mouse()
{
    // The mouse_state thing is set using get_input_events, though it should
    // probably get its own function
    get_input_events(POLL);

    return mouse_state;
}

MouseState &query_mouse_no_poll()
{
    return mouse_state;
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
