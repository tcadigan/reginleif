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
// pixien.cpp

/*
 * Changelog
 *     buffers: 07/31/02:
 *         * Include cleanup
 */
#include "pixien.hpp"

/*
 * *****************************************************
 * PixieN -- N-frame pixie
 * It is identical to the PIXIE class except it handles
 * multiple frames and switch frames before a put.
 * *****************************************************
 */

PixieN::PixieN(PixieData const &data)
    : Pixie(data)
{
    facings = data.data;
    frames = data.frames;
    frame = 0;
}

PixieN::PixieN(PixieData const &data, Sint32 doaccel)
    : Pixie(data)
{
    facings = data.data;
    frames = data.frames;
    frame = 0;

    if (doaccel) {
        init_sdl_surface();
    }
}

PixieN::~PixieN()
{
    bmp = nullptr;
    facings = nullptr;
    frames = 0;
    frame = 0;
}

// Changes the frame number and poshorts the BMP data poshorter to
// the correct frame's data
Sint16 PixieN::set_frame(Sint16 framenum)
{
    if ((framenum < 0) || (framenum >= frames)) {
        return 0;
    }

    bmp = facings + (framenum * size);
    frame = framenum;

    return 1;
}

Sint16 PixieN::next_frame()
{
    Sint16 frame_num = set_frame(frame % frames);
    ++frame;

    return frame_num;
}

Sint16 PixieN::query_frame()
{
    return frame;
}
