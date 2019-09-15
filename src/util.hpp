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
#ifndef __UTIL_HPP__
#define __UTIL_HPP__

//
// util.hpp
//
// Misc. helper functions and time
//

#include <ctype.h>
#include <string>

#include <SDL2/SDL.h>

#include "io.hpp"

void Log(Uint8 const *format, ...);

void change_time(Uint32 new_count);

void grab_timer();
void release_timer();
void reset_timer();
Sint32 query_timer();
Sint32 query_timer_control();
void time_delay(Sint32 delay);

// Zardus: add: Lowercase func
void lowercase(Uint8 *c);

// kari: lowercase for std::strings
void lowercase(std::string &s);

// buffers: add: uppercase func
void uppercase(Uint8 *c);

// kari: uppercase for std::strings
void uppercase(std::string &s);

// Zardus: add: set_mult func
void set_mult(Sint32 m);

#endif
