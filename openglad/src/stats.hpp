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
#ifndef __STATS_HPP__
#define __STATS_HPP__

// Defition of STATS class

//
// Include file for the stats object
//

#include <list>
#include <string>

#include <SDL2/SDL.h>

#include "base.hpp"
#include "command.hpp"

// These are for the bit flags
enum BitFlagEnum : Sint32 {
    BIT_FLYING = 1, // Fly over water, trees
    BIT_SWIMMING = 2, // Move over water
    BIT_ANIMATE = 4, // Animate even when not moving
    BIT_INVINCIBLE = 8, // Can't be harmed
    BIT_NO_RANGED = 16, // No ranged attack
    BIT_IMMORTAL = 32, // For weapons that don't die when they hit
    BIT_NO_COLLIDE = 64, // Fly through walkers
    BIT_PHANTOM = 128, // Use phantomputbuffer instead of walkerputbuffer
    BIT_NAMED = 256, // Has a name (will have outline,
    BIT_FORESTWALK = 512, // Can walk through forests
    BIT_MAGICAL = 1024, // Generally for magical weapons
    BIT_FIRE = 2048, // For any flame weapons
    BIT_ETHEREAL = 4096, // Fly "through" walls
    BIT_LAST = 8192
};

// Other special effects, etc.
#define FAERIE_FREEZE_TIME 40

// Class statistics, for (guess what?) controlling stats, etc....
class Statistics
{
public:
    Statistics(Sint16 order, Sint16 family);
    ~Statistics();

    Sint16 try_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2);
    Sint16 try_command(Sint16 whatcommand, Sint16 iterations);
    void set_command(Sint16 whatcommand, Sint16 iterations);
    void set_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2);
    void add_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2);
    void force_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2);
    bool has_commands();
    Sint16 query_bit_flags(Sint32 myvalue);
    void clear_bit_flags();
    // Sets a single flag
    void set_bit_flags(Sint32 someflag, Sint16 newvalue);
    // Is our right blocked?

    // For NPCs normally...
    std::string name; // Must be 12 characters or less
    Uint8 old_order;
    Uint8 old_family;
    Uint32 last_distance;
    // Distances (to foe) are used for AI walking
    Sint32 current_distance;
    // Holds (currently) 32 bit flags
    Sint32 bit_flags;
    Sint16 delete_me;

    float hitpoints;
    float max_hitpoints;
    float magicpoints;
    float max_magicpoints;

    Sint32 max_heal_delay;
    Sint32 current_heal_delay;
    Sint32 max_magic_delay;
    Sint32 current_magic_delay;
    // Magic points we regain each round
    Sint32 magic_per_round;
    // hp we regain each round
    Sint32 heal_per_round;
    // Reduces damage against us
    float armor;

    Uint16 level;
    // Use for paralyzing...
    Sint16 frozen_delay;
    // Cost of our special ability
    Uint16 special_cost[NUM_SPECIALS];
    // Cost of our weapon
    Sint16 weapon_cost;
    std::list<Command> commands;

private:
    // Parameters to command
    // Sint16 com1;
    // Sint16 com2;
    // Number of rounds we've spent right walking
};

#endif
