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
#ifndef __GUY_HPP__
#define __GUY_HPP__
#include "guy-fwd.hpp"

#include "screen-fwd.hpp"
#include "stats.hpp"
#include "walker-fwd.hpp"

// Holds attributes for characters.
// Used to store character data in SaveData's team_list.
// Used as walker::myguy in game for various attribute dependent effects for
// walkers who are on the player team.

class Guy
{
public:
    Guy();
    Guy(Sint32 whatfamily);

    Sint32 query_heart_value(); // How much are we worth?
    Walker *create_walker(VideoScreen *myscreen);
    Walker *create_and_add_walker(VideoScreen *myscreen);
    void upgrade_to_level(Sint16 level, bool set_xp=true);
    void set_level_number(Sint16 level);
    void update_derived_stats(Walker *w);

    Sint16 get_level() const;
    float get_hp_bonus() const;
    float get_mp_bonus() const;
    float get_damage_bonus() const;
    float get_armor_bonus() const;
    float get_speed_bonus() const;
    float get_fire_frequency_bonus() const;

    std::string name; // Must be 12 characters or less
    Uint8 family; // Our family
    Sint16 strength;
    Sint16 dexterity;
    Sint16 constitution;
    Sint16 intelligence;
    Sint16 armor;
    Uint32 exp;
    Sint16 kills; // Version 3+
    Sint32 level_kills; // Version 3+

    // This will not be exact after changing damage to floating point,
    // but binary serialization of floats is messy.
    Sint32 total_damage; // Version 4+
    Sint32 total_hits; // Version 4+
    Sint32 total_shots; // Version 4+
    Sint16 teamnum; // Version 5+

    // Stats for the last battle
    float scen_damage;
    Sint16 scen_kills;
    float scen_damage_taken;
    float scen_min_hp;
    Sint16 scen_shots;
    Sint16 scen_hits;

    // An ID for comparing old guys with their duplicated counterparts
    // after battle
    Sint32 id;

protected:
    Sint16 level;
};

#endif
