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
// stats.cpp

#include "stats.hpp" // For bit flags, etc.

#include "guy.hpp"
#include "input.hpp"
#include "util.hpp"
#include "video_screen.hpp"
#include "view.hpp"

#include <cmath>
#include <sstream>

Statistics::Statistics(Sint16 order, Sint16 family)
{
    Sint16 i;

    max_hitpoints = 10;
    hitpoints = max_hitpoints;
    max_magicpoints = 50;
    magicpoints = max_magicpoints;
    level = 1;
    // No default armor
    armor = 0;
    // Default of 50 cycles / hitpoint
    current_heal_delay = 1000;
    max_heal_delay = current_heal_delay;
    current_magic_delay = 1000;
    max_magic_delay = current_magic_delay;
    // Default to not regenerating magic
    magic_per_round = 0;
    // Default to not regenerating hitpoints
    heal_per_round = 0;

    // Clear all the bit flags
    bit_flags = 0;
    // Start out able to move :)
    frozen_delay = 0;

    for (i = 0; i < 5; ++i) {
        // Low default special ability cost in magicpoints
        special_cost[i] = 0;
    }

    weapon_cost = 0;
    delete_me = 0;

    // AI finding routine values...
    current_distance = 15000;
    last_distance = current_distance;

    old_order = order;
    old_family = family;

    // Set to null string
    name[0] = 0;
}

Statistics::~Statistics()
{
    delete_me = 1;
}

void Statistics::add_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2)
{
    if (whatcommand == COMMAND_DIE) {
        delete_me = 1;

        return;
    }

    if (whatcommand == COMMAND_FOLLOW) {
        Log("following\n");
    }

    // Add command to end of list
    commands.emplace_back();

    if (whatcommand == COMMAND_WALK) {
        if (info1 > 1) {
            info1 = 1;
        }

        if (info1 < -1) {
            info1 = -1;
        }

        if (info2 > 1) {
            info2 = 1;
        }

        if (info2 < -1) {
            info2 = -1;
        }

        if (!info1 && info2) {
            info2 = 1;
            info1 = 1;
        }
    }

    commands.back().com1 = info1;
    commands.back().com2 = info2;
    commands.back().commandtype = whatcommand;
    commands.back().commandcount = iterations;
}

void Statistics::force_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2)
{
    commands.emplace_front();

    if (whatcommand == COMMAND_WALK) {
        if (info1 > 1) {
            info1 = 1;
        }

        if (info1 < -1) {
            info1 = -1;
        }

        if (info2 > 1) {
            info2 = 1;
        }

        if (info2 < -1) {
            info2 = -1;
        }

        if (!info1 && !info2) {
            info2 = 1;
            info1 = 1;
        }
    }

    commands.front().com1 = info1;
    commands.front().com2 = info2;
    commands.front().commandtype = whatcommand;
    commands.front().commandcount = iterations;
}

bool Statistics::has_commands()
{
    return !commands.empty();
}

// try_command will only set a command if there is none in the queue
Sint16 Statistics::try_command(Sint16 whatcommand, Sint16 iterations)
{
    if (whatcommand == COMMAND_RANDOM_WALK) {
        return try_command(COMMAND_WALK, iterations, static_cast<Sint16>(getRandomSint32(3) - 1), static_cast<Sint16>(getRandomSint32(3) - 1));
    } else {
        return try_command(whatcommand, iterations, 0, 0);
    }
}

Sint16 Statistics::try_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2)
{
    add_command(whatcommand, iterations, info1, info2);

    return 0;
}

void Statistics::set_command(Sint16 whatcommand, Sint16 iterations)
{
    if (whatcommand == COMMAND_RANDOM_WALK) {
        set_command(COMMAND_WALK, iterations, static_cast<Sint16>(getRandomSint32(3) - 1), static_cast<Sint16>(getRandomSint32(3) - 1));
    } else {
        set_command(whatcommand, iterations, 0, 0);
    }
}

void Statistics::set_command(Sint16 whatcommand, Sint16 iterations, Sint16 info1, Sint16 info2)
{
    if (whatcommand == COMMAND_DIE) {
        Log("BLLLLLLLLLLLLLAAAAAAAAAAHHHHHHH!\n");
    }

    force_command(whatcommand, iterations, info1, info2);
}

Sint16 Statistics::query_bit_flags(Sint32 myvalue)
{
    return static_cast<Sint16>(myvalue & bit_flags);
}

void Statistics::clear_bit_flags()
{
    bit_flags = 0;
}

void Statistics::set_bit_flags(Sint32 someflag, Sint16 newvalue)
{
    if (newvalue) {
        bit_flags |= someflag;
    } else {
        bit_flags &= ~someflag;
    }
}
