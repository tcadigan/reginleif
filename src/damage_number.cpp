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
#include "damage_number.hpp"

#include "video_screen.hpp"

#include <sstream>

DamageNumber::DamageNumber(float x, float y, float value, Uint8 color)
    : x(x)
    , y(y)
    , t(1.0f)
    , value(value)
    , color(color)
{
}

void DamageNumber::draw(Sint16 topx, Sint16 topy, Sint16 xloc, Sint16 yloc)
{
    Sint32 xscreen = static_cast<Sint32>((x - topx) + xloc);
    Sint32 yscreen = static_cast<Sint32>((y - topy) + yloc);

    Uint8 alpha = 0;

    if (t >= 255) {
        alpha = 255;
    } else if (t >= 0) {
        alpha = t * 255;
    }

    std::stringstream buf;
    if (value != 0) {
        buf << value;
    }

    myscreen->text_normal.write_xy_center_alpha(xscreen, yscreen, color, alpha, buf);
}
