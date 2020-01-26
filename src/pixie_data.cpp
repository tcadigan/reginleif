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
#include "pixie_data.hpp"

PixieData::PixieData()
    : frames(0)
    , w(0)
    , h(0)
    , data(nullptr)
{}

PixieData::PixieData(Uint8 frames, Uint8 w, Uint8 h, Uint8 *data)
    : frames(frames)
    , w(w)
    , h(h)
    , data(data)
{}

bool PixieData::valid() const
{
    return ((data != nullptr) && (frames != 0) && (w != 0) && (h != 0));
}

void PixieData::clear()
{
    frames = 0;
    w = 0;
    h = 0;
    data = nullptr;
}

void PixieData::free()
{
    frames = 0;
    w = 0;
    h = 0;
    delete[] data;
    data = nullptr;
}
