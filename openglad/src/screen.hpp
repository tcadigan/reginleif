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
#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

#include <SDL2/SDL.h>

#include <string>

typedef enum {
    NoZoom = 0x01,
    SAI = 0x02,
    EAGLE = 0x03,
    DOUBLE = 0x04
} RenderEngine;

class Screen
{
public:
    Screen(RenderEngine engine, Sint32 width, Sint32 height, Sint32 fullscreen);
    ~Screen();

    void SaveBMP(SDL_Surface *screen, std::string const &filename);

    void clear();
    void clear(Sint32 x, Sint32 y, Sint32 w, Sint32 h);
    void swap(Sint32 x, Sint32 y, Sint32 w, Sint32 h);
    void clear_window();

    // How to render the physical screen
    RenderEngine Engine;

    SDL_Window *window;
    SDL_Renderer *renderer;

    // The target for all rendering
    SDL_Surface *render;

    // A texture updated by "render" for normal rendering
    SDL_Texture *render_tex;

    // A buffer for doubling filters (i.e. Sai or Eagle)
    SDL_Surface *render2;

    // A larger texture for the doubled result
    SDL_Texture *render2_tex;
};

extern Screen *E_Screen;

#endif
