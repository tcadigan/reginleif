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
#ifndef __PIXIEN_HPP__
#define __PIXIEN_HPP__

// Definition of PIXIEN class

#include "pixie.hpp"

class PixieN : public Pixie
{
public:
    PixieN(PixieData const &data);
    PixieN(PixieData const &data, Sint32 doaccel);
    virtual ~PixieN();

    Sint16 set_frame(Sint16 framenum);
    Sint16 query_frame();
    Sint16 next_frame();

protected:
    // Total frames
    Sint16 frames;

    // Current frame
    Sint16 frame;

    Uint8 *facings;
};

extern PixieN *backdrops[5];

#endif
