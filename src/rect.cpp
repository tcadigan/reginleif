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
#include "rect.hpp"

Rect::Rect()
    : x(0)
    , y(0)
    , w(0)
    , h(0)
{
}

Rect::Rect(Sint32 x, Sint32 y, Uint32 w, Uint32 h)
    : x(x)
    , y(y)
    , w(w)
    , h(h)
{
}

bool Rect::contains(Sint32 x, Sint32 y) const
{
    return ((this->x <= x)
            && (x < static_cast<Sint32>(this->x + w))
            && (this->y <= y)
            && (y < static_cast<Sint32>(this->y + h)));
}
