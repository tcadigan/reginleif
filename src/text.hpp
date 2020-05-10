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

#include <sstream>
#include <string>

#include "pixie_data.hpp"

class Text
{
public:
    Text(std::filesystem::path const &filename);
    ~Text();
    // Returns width, in pixels
    Sint16 query_width(std::string const &string);
    bool write_xy(Sint16 x, Sint16 y, std::string const &string);
    bool write_xy(Sint16 x, Sint16 y, Uint8 color, std::string const &string);
    Sint16 write_xy(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream);
    Sint16 write_xy_shadow(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream);
    bool write_xy_center(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream);
    bool write_xy_center_alpha(Sint16, Sint16, Uint8 color, Uint8 alpha, std::stringstream const &stream);
    bool write_xy_center_shadow(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream);
    bool write_xy(Sint16 x, Sint16 y, std::string const &string, Sint16 to_buffer);
    Sint16 write_xy(Sint16 x, Sint16 y, std::string const &string, Uint8 color, Sint16 to_buffer);
    bool write_xy(Sint16 x, Sint16 y, std::string const &string, Uint8 color, Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy);
    Sint16 write_y(Sint16 y, std::string const &string, Uint8 color, Sint16 to_buffer);
    Sint16 write_y(Sint16 y, std::string const &string, Uint8 color, Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy);
    bool write_char_xy(Sint16 x, Sint16 y, Uint8 letter);
    bool write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color);
    bool write_char_xy_alpha(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Uint8 alpha);
    bool write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Sint16 to_buffer);
    std::string input_string(Sint16 x, Sint16 y, Uint32 maxlength, std::string const &begin, Uint8 forecolor, Uint8 backcolor);
    std::string input_string_ex(Sint16 x, Sint16 y, Uint32 maxlength, std::string const &message, std::string const &begin, Uint8 forecolor, Uint8 backcolor);

    PixieData letters;
    Sint16 sizex;
    Sint16 sizey;

private:
    PixieData set_letters(std::filesystem::path const &filename);
};

#endif
