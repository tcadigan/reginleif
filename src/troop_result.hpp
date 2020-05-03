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
#ifndef __TROOP_RESULT_HPP__
#define __TROOP_RESULT_HPP__

#include <SDL2/SDL.h>

#include <string>
#include <vector>

#include "guy.hpp"
#include "walker.hpp"

class TroopResult
{
public:
    TroopResult(Guy *before, Walker *after);

    std::string get_name();
    std::string get_class_name();
    Uint8 get_family();
    Sint32 get_level();
    bool gained_level();
    bool lost_level();
    std::vector<std::string> get_gained_specials();

    // Tehse are percentages of what you need for the next level.
    float get_xp_base();

    // Could be negative, if a level is lost, it is a percentage of the XP
    // needed for the lost level
    float get_xp_gain();

    Sint32 get_tallies();
    // Percentage of total
    float get_hp() const;
    bool is_dead() const;
    bool is_new() const;

    void draw_guy(Sint32 cx, Sint32 cy, Sint32 frame);

    Guy *before;
    Walker *after;

private:
    void show_guy(Sint32 frames, Guy *myguy, Sint16 centerx, Sint16 centery);
};

#endif
