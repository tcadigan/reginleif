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

#include "io.hpp"
#include "util.hpp"

#include <iostream>
#include <sstream>

PixieData::PixieData()
{
}

PixieData::PixieData(std::filesystem::path const &filename)
{
    /*
     * Create a file stream and read the image
     * File data in form:
     * <# of frames>  1 byte
     * <x size>       1 byte
     * <y size>       1 byte
     * <pixie data>   x * y * frames bytes
     */

    // Zardus: Try to find file using open_read_file
    SDL_RWops *infile = open_read_file(std::filesystem::path("pix" / filename));
    if (infile == nullptr) {
        infile = open_read_file(std::filesystem::path(filename));
    }

    if (infile == nullptr) {
        std::stringstream buf;
        buf << "Cannot open pixie file pix/" << filename << "!" << std::endl;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", buf.str().c_str());

        exit(5);
    }

    SDL_RWread(infile, &frames, 1, 1);
    SDL_RWread(infile, &w, 1, 1);
    SDL_RWread(infile, &h, 1, 1);

    size_t size = (w * h) * frames;
    data = new Uint8[size];

    // Now read the data in a big chunk
    SDL_RWread(infile, data, 1, size);

    SDL_RWclose(infile);
}

PixieData::PixieData(PixieData const &other)
    : frames(other.frames)
    , w(other.w)
    , h(other.h)
{
    Sint32 len = (w * h) * frames;
    data = new Uint8[len];
    memcpy(data, other.data, len);
}

PixieData::~PixieData()
{
    delete[] data;
}

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
