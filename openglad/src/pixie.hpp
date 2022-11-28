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
#ifndef __PIXIE_HPP__
#define __PIXIE_HPP__

// Definition of PIXIE class

#include "pixie_data.hpp"

class Pixie
{
public:
    Pixie(PixieData const &data);
    Pixie(PixieData const &data, Sint32 doaccel);
    virtual ~Pixie();

    bool setxy(Sint16 x, Sint16 y);
    bool draw(Sint16 topx, Sint16 topy, Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy);
    bool drawMix(Sint16 topx, Sint16 topy, Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy);
    bool put_screen(Sint16 x, Sint16 y);
    void init_sdl_surface(void);
    void set_accel(Sint32 a);
    void set_data(PixieData const &data);
    bool on_screen();

    virtual bool move(Sint16 x, Sint16 y);

    Sint16 sizex;
    Sint16 sizey;
    Sint16 xpos;
    Sint16 ypos;

    // buffers: Is SDL_Surface acceleration on/off, 1/0
    Sint32 accel;

protected:
    Uint16 size;
    Uint8 *bmp;
    Uint8 *oldbmp;

    // buffers: Same data as bmp in a convenient SDL_Surface
    SDL_Surface *bmp_surface;

    // On a specific viewscreen?
    bool on_screen(Sint16 topx, Sint16 topy, Sint16 view_x, Sint16 view_y);
};

#endif
