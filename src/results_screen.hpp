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
#ifndef __RESULTS_SCREEN_HPP__
#define __RESULTS_SCREEN_HPP__

#include <SDL2/SDL.h>

#include <map>

#include "guy.hpp"
#include "walker.hpp"

// When no change to the guys has happened
bool results_screen(Sint32 ending, Sint32 nextlevel);
bool results_screen(Sint32 ending, Sint32 nextlevel, std::map<Sint32, Guy *> &before, std::map<Sint32, Walker *> &after);
Uint32 get_time_bonus(Sint32 playernum);

#endif
