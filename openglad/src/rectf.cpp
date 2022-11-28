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
#include "rectf.hpp"

Rectf::Rectf()
    : x(0)
    , y(0)
    , w(0)
    , h(0)
{
}

Rectf::Rectf(float x, float y, float w, float h)
    : x(x)
    , y(y)
    , w(w)
    , h(h)
{
}

bool Rectf::contains(float x, float y) const
{
    if ((w >= 0.0f) && (h >= 0.0f)) {
        return ((this->x <= x)
                && ((this->x + w) >= x)
                && (this->y <= y)
                && ((this->y + h) >= y));
    }

    if ((w < 0.0f) && (h < 0.0f)) {
        return (((this->x + w) <= x)
                && (this->x >= x)
                && ((this->y + h) <= y)
                && (this->y >= y));
    }

    if (w < 0.0f) {
        return (((this->x + w) <= x)
                && (this->x >= x)
                && (this->y <= y)
                && ((this->y + h) >= y));
    }

    return ((this->x < x)
            && ((this->x + w) >= x)
            && ((this->y + h) <= y)
            && (this->y >= y));
}
