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
//
// util.hpp
//
// Random helper functions
//

#include "util.hpp"

#include <cstdio>
#include <cstring> // buffers: For strlen
#include <ctime>
#include <string>
#include <sys/stat.h>

Uint32 start_time = 0;
Uint32 reset_value = 0;

void Log(char const *format, ...)
{
    va_list args;
    va_start(args, format);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, format, args);
    va_end(args);
}

void reset_timer()
{
    reset_value = SDL_GetTicks();
}

Sint32 query_timer()
{
    /*
     * Zardus: Why 13.6? With DOS timing, you had to divide 1,193,180 by the
     *         desired frequency that would return ticks/second. Gladiator
     *         used to use a frequency of 65536/4 ticks per hour, or
     *         1193180/16383 = 72.3 ticks per second. This translates into
     *         13.6 milliseconds/tick
     */
    return static_cast<Sint32>((SDL_GetTicks() - reset_value) / 13.6);
}

Sint32 query_timer_control()
{
    return static_cast<Sint32>(SDL_GetTicks() / 13.6);
}

void time_delay(Sint32 delay)
{
    if (delay < 0) {
        return;
    }

    SDL_Delay(static_cast<Uint32>(delay * 13.6));
}

void lowercase(char *str)
{
    Uint32 i;
    for (i = 0; i < strlen(str); ++i) {
        str[i] = tolower(str[i]);
    }
}

// buffers: add: Another extra routine
void uppercase(char *str)
{
    Uint32 i;
    for (i = 0; i < strlen(str); ++i) {
        str[i] = toupper(str[i]);
    }
}

// kari: Yet two extra
void lowercase(std::string &str)
{
    for (auto&& itr : str) {
        itr = tolower(itr);
    }
}

void uppercase(std::string &str)
{
    for (auto&& itr : str) {
        itr = toupper(itr);
    }
}
