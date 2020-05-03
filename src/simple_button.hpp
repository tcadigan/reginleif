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
#ifndef __SIMPLE_BUTTON_HPP__
#define __SIMPLE_BUTTON_HPP__

#include "screen.hpp"

#include <SDL2/SDL.h>

#include <string>

class SimpleButton
{
public:
    SDL_Rect area;
    std::string label;
    bool remove_border;
    bool draw_top_separator;
    Sint32 base_color;
    Sint32 high_color;
    Sint32 shadow_color;
    Sint32 text_color;
    bool centered;

    SimpleButton(std::string const &label, Sint32 x, Sint32 y, Uint32 w, Uint32 h, bool remove_border=false, bool draw_top_separator=false);

    void draw(VideoScreen *myscreen);
    bool contains(Sint32 x, Sint32 y) const;

    void set_colors_normal();
    void set_colors_enabled();
    void set_colors_disabled();
    void set_colors_active();
};


#endif
