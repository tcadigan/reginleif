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
#ifndef __TEXT_HPP__
#define __TEXT_HPP__

// Defintion of TEXT class

#include "pixie_data.hpp"
#include "screen-fwd.hpp"
#include "view-fwd.hpp"

class Text
{
public:
    Text(Uint8 const *filename);
    ~Text();
    // Returns width, in pixels
    Sint16 query_width(Uint8 const *string);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string, Uint8 color);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 color, Uint8 const *formatted_string, ...);
    Sint16 write_xy_shadow(Sint16 x, Sint16 y, Uint8 color, Uint8 const *formatted_string, ...);
    Sint16 write_xy_center(Sint16 x, Sint16 y, Uint8 color, Uint8 const *formatted_string, ...);
    Sint16 write_xy_center_shadow(Sint16 x, Sint16 y, Uint8 color, Uint8 const *formatted_string, ...);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string, Sint16 to_buffer);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string, Uint8 color, Sint16 to_buffer);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string, ViewScreen *whereto);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 const *string, Uint8 color, ViewScreen *whereto);
    Sint16 write_y(Sint16 y, Uint8 const *string);
    Sint16 write_y(Sint16 y, Uint8 const *string, Uint8 color);
    Sint16 write_y(Sint16 y, Uint8 const *string, Sint16 to_buffer);
    Sint16 write_y(Sint16 y, Uint8 const *string, Uint8 color, Sint16 to_buffer);
    Sint16 write_y(Sint16 y, Uint8 const *string, ViewScreen *whereto);
    Sint16 write_y(Sint16 y, Uint8 const *string, Uint8 color, ViewScreen *whereto);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color);
    Sint16 write_char_xy_alpha(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Uint8 alpha);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Sint16 to_buffer);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Sint16 to_buffer);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter, ViewScreen *whereto);
    Sint16 write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, ViewScreen *whereto);
    Uint8 *input_string(Sint16 x, Sint16 y, Sint16 maxlength, Uint8 const *begin);
    Uint8 *input_string(Sint16 x, Sint16 y, Sint16 maxlength, Uint8 const *begin, Uint8 forecolor, Uint8 backcolor);
    Uint8 *input_string_ex(Sint16 x, Sint16 y, Sint16 maxlength, Uint8 const *message, Uint8 const *begin);
    Uint8 *input_string_ex(Sint16 x, Sint16 y, Sint16 maxlength, Uint8 const *message, Uint8 const *begin, Uint8 forecolor, Uint8 backcolor);

    friend class VirtualButton;

    PixieData letters;
    Sint16 sizex;
    Sint16 sizey;
};

#endif
