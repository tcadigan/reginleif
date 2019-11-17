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
#ifndef __LEVEL_PICKER_HPP__
#define __LEVEL_PICKER_HPP__

#include <SDL2/SDL.h>

#include "level_data.hpp"
#include "screen-fwd.hpp"

#include <list>

Sint32 pick_level(VideoScreen *screenp, Sint32 default_level, bool enable_delete=false);
void getLevelStats(LevelData &level_data, Sint32 *max_enemy_level,
                   float *average_enemy_level, Sint32 *num_enemies,
                   float *difficulty, std::list<Sint32> &exits);

#endif
