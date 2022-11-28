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
#ifndef __MOUSE_STATE_HPP__
#define __MOUSE_STATE_HPP__

#include <SDL2/SDL.h>

#define MOUSE_LEFT 2
#define MOUSE_RIGHT 3

class MouseState
{
public:
    float x;
    float y;
    bool left;
    bool right;

    bool in(SDL_Rect const &r) const {
        return ((r.x <= x) && (x < (r.x + r.w)) && (r.y < y) && (y < (r.y + r.h)));
    }
};

MouseState &query_mouse();
MouseState &query_mouse_no_poll();
void grab_mouse();
void release_mouse();
void handle_mouse_event(SDL_Event const &event);
void handle_mouse_events(SDL_Event const &event);

Sint16 query_mouse_key_press_event();
void clear_mouse_key_press_event();

#endif
