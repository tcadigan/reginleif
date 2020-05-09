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
#include "mouse_state.hpp"

#include "input.hpp"

// Mouse defines
#define MOUSE_RESET 0
#define MOUSE_STATE 3
#define MSTATE 4
#define MOUSE_X 0
#define MOUSE_Y 1

MouseState mouse_state;
Sint32 mouse_buttons;

Sint16 key_press_event = 0;

void handle_mouse_event(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        scroll_amount = event.wheel.y * 5;
        key_press_event = 1;

        break;
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
    }
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
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        handle_events(event);
    }

    return mouse_state;
}

MouseState &query_mouse_no_poll()
{
    return mouse_state;
}

void handle_mouse_events(SDL_Event const &event)
{
    switch (event.type) {
    case SDL_MOUSEWHEEL:
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_FINGERMOTION:
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
        handle_mouse_event(event);

        break;
    default:

        break;
    }
}

Sint16 query_mouse_key_press_event()
{
    return key_press_event;
}

void clear_mouse_key_press_event()
{
    key_press_event = 0;
}
