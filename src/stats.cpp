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
#include "screen.hpp"
#include "util.hpp"
#include "view.hpp"
#include "walker.hpp"

#include <cmath>
#include <sstream>

#define CHECK_STEP_SIZE 1 // (controller->stepsize) // was 1
#define PATHING_MIN_DISTANCE 100
// Note that obmap::size() counts dead things too, which don't do pathfinding
#define PATHING_SHORT_CIRCUIT_OBJECT_LIMIT 200

Statistics::Statistics(Walker *someguy)
{
    Sint16 i;

    if (someguy) {
        controller = someguy;
    } else {
        controller = nullptr;
        Log("Made a stats with no controller!\n");
    }

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

    if (controller != nullptr) {
        old_order = controller->query_order();
        old_family = controller->query_family();
    } else {
        old_order = ORDER_LIVING;
        old_family = FAMILY_SOLDIER;
    }

    // Set to null string
    name[0] = 0;
}

Statistics::~Statistics()
{
    controller = nullptr;
    delete_me = 1;
}

void Statistics::clear_command()
{
    commands.clear();
    // Make sure our weapon type is restored to normal...
    controller->current_weapon = controller->default_weapon;
    // Make sure we're back to our real team
    if (controller->real_team_num != 255) {
        controller->team_num = controller->real_team_num;
        controller->real_team_num = 255;
    }

    controller->leader = nullptr;
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

// Do the current command
Sint16 Statistics::do_command()
{
    Sint16 commandtype;
    Sint16 com1;
    Sint16 com2;
    Sint16 i;
    Walker *target;
    Sint16 deltax;
    Sint16 deltay;
    Sint32 distance;
    Sint16 newx = 0;
    Sint16 newy = 0;

#ifdef PROFILING
    Profiler docomprofiler("stats::do_command");
#endif

    // Allow dead controllers for now
    if (!controller) {
        Log("STATS:DO_COM: No controller!\n");
        wait_for_key(KEYSTATE_z);

        return 0;
    }

    // Get next command
    if (commands.empty()) {
        return 0;
    }

    commandtype = commands.front().commandtype;
    com1 = commands.front().com1;
    com2 = commands.front().com2;

    Sint16 result = 1;

    switch (commandtype) {
    case COMMAND_WALK:
        controller->walkstep(com1, com2);

        break;
    case COMMAND_FIRE:
        if (controller->query_order() != ORDER_LIVING) {
            Log("commanding a non-living to fire?");

            break;
        }

        if (!controller->fire_check(com1, com2)) {
            commands.front().commandcount = 0;
            result = 0;

            break;
        }

        controller->init_fire(com1, com2);

        break;
    case COMMAND_DIE:
        // Debugging, not currently used
        if (!controller->dead) {
            Log("Trying to make a living ob dead!\n");
        }

        // Then delete us...
        if (commands.front().commandcount < 2) {
            delete_me = 1;
        }

        break;
    case COMMAND_FOLLOW:
        // Follow the leader
        // If we have foe, don't follow this round
        if (controller->foe) {
            commands.front().commandcount = 0;
            controller->leader = nullptr;
            result = 0;

            break;
        }

        if (!controller->leader) {
            if (myscreen->numviews == 1) {
                controller->leader = myscreen->viewob[0]->control;
            } else {
                if (myscreen->viewob[0]->control->yo_delay) {
                    controller->leader = myscreen->viewob[0]->control;
                } else if (myscreen->viewob[1]->control->yo_delay) {
                    controller->leader = myscreen->viewob[1]->control;
                } else {
                    commands.front().commandcount = 0;
                    controller->leader = nullptr;
                    result = 0;

                    break;
                }
            }
        }

        // Do we have a leader now?
        if (controller->leader) {
            distance = controller->distance_to_ob(controller->leader);

            if (distance < 60) {
                controller->leader = nullptr;
                // Don't get too close
                result = 1;

                break;
            }

            // Total horizontal distance..
            newx = static_cast<Sint16>(controller->leader->xpos - controller->xpos);
            newy = static_cast<Sint16>(controller->leader->ypos - controller->ypos);

            if (abs(newx) > abs(3 * newy)) {
                newy = 0;
            }

            if (abs(newy) > abs(3 * newx)) {
                newx = 0;
            }

            // If it's not 0, then get the normal of it
            if (newx) {
                newx = static_cast<Sint16>(newx / abs(newx));
            }

            if (newy) {
                newy = static_cast<Sint16>(newy / abs(newy));
            }
        } // end of if we had a foe...

        controller->walkstep(newx, newy);

        if (commands.front().commandcount < 2) {
            controller->leader = nullptr;
        }

        break;
    case COMMAND_QUICK_FIRE:
        controller->walkstep(com1, com2);
        controller->fire();

        break;
    case COMMAND_MULTIDO:
        // Lets you do <com1> commands in one round
        for (i = 0; i < com1; ++i) {
            do_command();
        }

        break;
    case COMMAND_RUSH:
        // Fighter's special
        if (controller->query_order() == ORDER_LIVING) {
            controller->walkstep(com1, com2);
            controller->walkstep(com1, com2);
            controller->walkstep(com1, com2);

            // We hit someone
            if (controller->collide_ob) {
                target = controller->collide_ob;
                controller->attack(target);
                target->stats->clear_command();
                // A violent shove...we can't call shove since we
                // made shoe unable to shove enemies
                target->stats->force_command(COMMAND_WALK, 4, com1, com2);
            }
        }

        break;
    case COMMAND_SET_WEAPON:
        // Set weapon to specified type
        controller->current_weapon = com1;

        break;
    case COMMAND_RESET_WEAPON:
        // Reset weapon to default type
        controller->current_weapon = controller->default_weapon;

        break;
    case COMMAND_SEARCH:
        // Use right-hand rule to find foe
        if (controller->foe && !controller->foe->dead) {
            walk_to_foe();
        } else {
            // Stop trying to walk to this foe
            commands.front().commandcount = 0;
        }

        break;
    case COMMAND_RIGHT_WALK:
        // Right-hand walk ONLY
        if (controller->foe) {
            distance = controller->distance_to_ob(controller->foe);

            if ((distance > 120) && (distance < 240)) {
                right_walk();
            } else {
                if (direct_walk()) {
                    right_walk();
                }
            }
        }

        break;
    case COMMAND_ATTACK:
        // Attack a nearby, set foe
        if (!controller->foe || controller->foe->dead) {
            commands.front().commandcount = 0;
            result = 1;

            break;
        }

        // Try to walk toward foe, and/or attack...
        deltax = static_cast<Sint16>(controller->foe->xpos - controller->xpos);
        deltay = static_cast<Sint16>(controller->foe->ypos - controller->ypos);

        if (abs(deltax) > abs(3 * deltay)) {
            deltay = 0;
        }

        if (abs(deltay) > abs(3 * deltax)) {
            deltax = 0;
        }

        if (deltax) {
            deltax /= abs(deltax);
        }

        if (deltay) {
            deltay /= abs(deltay);
        }

        if (!controller->fire_check(deltax, deltay)) {
            controller->walkstep(deltax, deltay);
        } else { // controller->fire_check(deltax, deltay))
            force_command(COMMAND_FIRE, static_cast<Sint16>(getRandomSint32(5)), deltax, deltay);
            controller->init_fire(deltax, deltay);
        }

        break;
    case COMMAND_UNCHARM:
        /*
         * if (commandcount > 1) {
         *     add_command(COMMAND_UNCHARM, commandcount - 1, 0, 0);
         *     commandcount = 1;
         * } else if (controller->real_team_num 1= 255) {
         *     controller->team_num = controller->real_team_num;
         *     controller->real_team_num = 255;
         * }
         */

        // end of uncharm case
        break;
    default:

        break;
    }

    // NOTE: The first command might be a different command than it was before
    //       the switch statement. That would make this code decrement the wrong
    //       command.
    if (!commands.empty()) {
        // Reduce number of times left
        --commands.front().commandcount;

        // Last iteration!
        if (commands.front().commandcount < 1) {
            commands.front().commandcount = 0;
            commands.pop_front();
        }
    }

    return result;
}

// Determines what to do when we're hit by 'who'
// 'controller is our parent walker object
void Statistics::hit_response(Walker *who)
{
    Sint32 distance;
    Sint32 i;
    Sint16 myfamily;
    Sint32 deltax;
    Sint32 deltay;
    // Who is attacking us?
    Walker *foe;
    Sint32 possible_specials[NUM_SPECIALS];
    // For hitpoint 'running away'
    float threshold;
    short howmany;

    if (!who || !controller) {
        return;
    }

    if (who->dead || controller->dead) {
        return;
    }

    if (controller->query_act_type() == ACT_CONTROL) {
        return;
    }

    if (controller->query_order() != ORDER_LIVING) {
        return;
    }

    // set quick-reference values...
    myfamily = controller->query_family();

    if (who->query_order() == ORDER_WEAPON && who->owner) {
        foe = who->owner;
    } else {
        foe = who;
    }

    // Determine which specials we can do (by level and sp)...
    // first initialize to CAN'T
    for (i = 0; i < NUM_SPECIALS; ++i) {
        possible_specials[i] = 0;
    }

    // For all our 'possibles' by level
    for (i = 0; i <= (level + 2) / 3; ++i) {
        if ((i < NUM_SPECIALS) && (magicpoints >= special_cost[i])) {
            // Then we can do it
            possible_specials[i] = 1;
        }
    }

    switch (myfamily) {
    case FAMILY_MAGE:
        // Are we a player's character?
        if (controller->myguy) {
            // Then flee at 60%
            threshold = (3 * max_hitpoints) / 5;
        } else {
            // We're an enemy, so be braver :>
            // Flee at 3/8
            threshold = (3 * max_hitpoints) / 8;
        }

        if ((hitpoints < threshold) && possible_specials[1]) {
            // Clear old command...
            // clear_command();
            // Teleport
            // Teleport to safety
            controller->current_special = 1;
            // TELEPORT, not other
            controller->shifter_down = 0;
            // Force allow us to special
            controller->busy = 0;
            controller->special();
        } else if (controller->foe != foe) {
            // We're hit by a new enemy
            controller->foe = foe;
            foe->foe = controller;
            current_distance = 15000;
            last_distance = current_distance;
        }

        break;
    case FAMILY_ARCHMAGE:
        // Yes, this is a cheat...
        controller->busy = 0;

        // Are we a player's character?
        if (controller->myguy) {
            // Then flee at 60%
            threshold = (3 * max_hitpoints) / 5;
        } else {
            // We're an enemy, so be braver :>
            // Flee at 3/8
            threshold = (3 * max_hitpoints) / 8;
        }

        if ((hitpoints < threshold) && possible_specials[1] && getRandomSint32(3)) {
            // Teleport
            // Teleport to safety
            controller->current_special = 1;
            controller->shifter_down = 0;
            // Force allow us to special
            controller->busy = 0;
            controller->special();
        } else {
            // Find out how may foes are around us, etc...
            // We're hit by a new enemy
            if (controller->foe != foe) {
                controller->foe = foe;
                foe->foe = controller;
                current_distance = 15000;
                last_distance = current_distance;
            }

            myscreen->find_foes_in_range(myscreen->level_data.oblist, 200, &howmany, controller);

            // Foes within range?
            if (howmany) {
                // can we summon illusion?
                if (possible_specials[3]) {
                    controller->current_special = 3;

                    if (controller->special()) {
                        return;
                    }
                } // End of 3rd special

                // Heartburst, chain lightning, etc.
                if (possible_specials[2]) {
                    // 2 or fewer enemies, so lightning...
                    // if (howmany < 3)

                    // 50/50 now
                    if (getRandomSint32(2)) {
                        // Lightning
                        controller->shifter_down = 1;
                        controller->current_special = 2;

                        if (controller->special()) {
                            controller->shifter_down = 0;

                            // Then leave! :)
                            if (magicpoints >= special_cost[1]) {
                                controller->busy = 0;
                                controller->special();
                            }

                            return;
                        }
                    } // End of lightning

                    controller->shifter_down = 0;
                    controller->current_special = 2;

                    if (controller->special()) {
                        // Then leave! :)
                        if (magicpoints >= special_cost[1]) {
                            controller->busy = 0;
                            controller->special();
                        }

                        return;
                    }
                } // End of burst or lightning
            } // End of some foes in range for special attack
        }

        break;
    case FAMILY_ARCHER:
        {    // Stay at range...
            if (!controller->foe || (controller->foe != foe)) {
                controller->foe = foe;
                clear_command();
                current_distance = 15000;
                last_distance = current_distance;
            }

            distance = controller->distance_to_ob(foe);

            // Too close!
            if (distance < 64) {
                deltax = static_cast<Sint16>(controller->xpos - foe->xpos);

                if (deltax) {
                    deltax = static_cast<Sint16>(deltax / abs(deltax));
                }

                deltay = static_cast<Sint16>(controller->ypos - foe->ypos);

                if (deltay) {
                    deltay = static_cast<Sint16>(deltay / abs(deltay));
                }

                // Run away
                force_command(COMMAND_WALK, 8, deltax, deltay);
            } // End of too close check
        } // End of archer case

        break;
    default:
        // Attack our attacker
        // Chance of doing special...
        if (controller->check_special() && !getRandomSint32(3)) {
            controller->special();
        }

        // Are we a player's character?
        if (controller->myguy) {
            // Then flee at 50%
            threshold = (5 * max_hitpoints) / 10;
        } else {
            // We're an enemy, so be braver :>
            // Flee at 5/16
            threshold = (5 * max_hitpoints) / 16;
        }

        // Then yell for help and run...
        if ((hitpoints < threshold) && !controller->yo_delay) {
            yell_for_help(foe);
        } // End of yell for help

        // We're attacked by a new enemy
        if (controller->foe != foe) {
            // Clear old commands...
            clear_command();
            // Attack our attacker
            controller->foe = foe;
            foe->foe = controller;
            current_distance = 15000;
            last_distance = current_distance;
        }

        break;
    }
}

void Statistics::yell_for_help(Walker *foe)
{
    Sint16 howmany;
    Sint32 deltax;
    Sint32 deltay;

    controller->yo_delay += 80;

    // Get AI-controller allies to target my foe
    std::list<Walker *> helplist = myscreen->find_friends_in_range(myscreen->level_data.oblist, 160, &howmany, controller);

    for (auto const &w : helplist) {
        w->leader = controller;

        if (foe != w->foe) {
            w->stats->current_distance = 32000;
            w->stats->last_distance = w->stats->current_distance;
        }

        w->foe = foe;

        /*
         * if (w->query_act_type() != ACT_CONTROL) {
         *      w->stats->force_command(COMMAND_FOLLOW, 80, 0, 0);
         * }
         */
    }

    // Force run in the opposite direction
    deltax = -(foe->xpos - controller->xpos);

    if (deltax) {
        deltax = deltax / abs(deltax);
    }

    deltay = -(foe->ypos - controller->ypos);

    if (deltay) {
        deltay = deltay / abs(deltay);
    }

    // Run away
    force_command(COMMAND_WALK, 16, deltax, deltay);

    // Notify friends of need...
    if (controller->myguy && (controller->team_num == 0)) {
        std::stringstream buf(controller->myguy->name);
        buf << " yells for help!";

        myscreen->do_notify(buf.str(), controller);
    }
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

// Returns whether our right is blocked
bool Statistics::right_blocked()
{
    float xdelta;
    float ydelta;
    float controlx = controller->xpos;
    float controly = controller->ypos;
    float mystep = controller->stepsize;

    mystep = CHECK_STEP_SIZE;

    switch (controller->curdir) {
    case FACE_UP:
        xdelta = mystep;
        ydelta = 0;

        break;
    case FACE_UP_RIGHT:
        xdelta = mystep;
        ydelta = mystep;

        break;
    case FACE_RIGHT:
        xdelta = 0;
        ydelta = mystep;

        break;
    case FACE_DOWN_RIGHT:
        xdelta = -mystep;
        ydelta = mystep;

        break;
    case FACE_DOWN:
        xdelta = -mystep;
        ydelta = 0;

        break;
    case FACE_DOWN_LEFT:
        xdelta = -mystep;
        ydelta = -mystep;

        break;
    case FACE_LEFT:
        xdelta = 0;
        ydelta = -mystep;

        break;
    case FACE_UP_LEFT:
        xdelta = mystep;
        ydelta = -mystep;

        break;
    default:
        xdelta = 0;
        ydelta = 0;

        break;
    }

    return !myscreen->query_passable(controlx + xdelta, controly + ydelta, controller);
}

// Retruns whether our right-forward is blocked
bool Statistics::right_forward_blocked()
{
    float controlx = controller->xpos;
    float controly = controller->ypos;
    float mystep = controller->stepsize;

    mystep = CHECK_STEP_SIZE;

    switch (controller->curdir) {
    case FACE_UP:

        return !myscreen->query_passable(controlx + mystep, controly - mystep, controller);
    case FACE_UP_RIGHT:

        return !myscreen->query_passable(controlx + mystep, controly, controller);
    case FACE_RIGHT:

        return !myscreen->query_passable(controlx + mystep, controly + mystep, controller);
    case FACE_DOWN_RIGHT:

        return !myscreen->query_passable(controlx, controly + mystep, controller);
    case FACE_DOWN:

        return !myscreen->query_passable(controlx - mystep, controly + mystep, controller);
    case FACE_DOWN_LEFT:

        return !myscreen->query_passable(controlx = mystep, controly, controller);
    case FACE_LEFT:

        return !myscreen->query_passable(controlx - mystep, controly - mystep, controller);
    case FACE_UP_LEFT:

        return !myscreen->query_passable(controlx, controly - mystep, controller);
    default:

        break;
    }

    return false;
}

// Returns whether our right-back is blocked
bool Statistics::right_back_blocked()
{
    float controlx = controller->xpos;
    float controly = controller->ypos;
    float mystep = controller->stepsize;

    mystep = CHECK_STEP_SIZE;

    switch (controller->curdir) {
    case FACE_UP:

        return !myscreen->query_passable(controlx + mystep, controly + mystep, controller);
    case FACE_UP_RIGHT:

        return !myscreen->query_passable(controlx, controly + mystep, controller);
    case FACE_RIGHT:

        return !myscreen->query_passable(controlx - mystep, controly + mystep, controller);
    case FACE_DOWN_RIGHT:

        return !myscreen->query_passable(controlx - mystep, controly, controller);
    case FACE_DOWN:

        return !myscreen->query_passable(controlx - mystep, controly - mystep, controller);
    case FACE_DOWN_LEFT:

        return !myscreen->query_passable(controlx, controly - mystep, controller);
    case FACE_LEFT:

        return !myscreen->query_passable(controlx + mystep, controly - mystep, controller);
    case FACE_UP_LEFT:

        return !myscreen->query_passable(controlx + mystep, controly, controller);
    default:

        break;
    }

    return false;
}

// Returns whether our front is blocked
bool Statistics::forward_blocked()
{
    float xdelta;
    float ydelta;
    float controlx = controller->xpos;
    float controly = controller->ypos;
    float mystep = CHECK_STEP_SIZE;

    switch (controller->curdir) {
    case FACE_UP:
        xdelta = 0;
        ydelta = -mystep;

        break;
    case FACE_UP_RIGHT:
        xdelta = mystep;
        ydelta = -mystep;

        break;
    case FACE_RIGHT:
        xdelta = mystep;
        ydelta = 0;

        break;
    case FACE_DOWN_RIGHT:
        xdelta = mystep;
        ydelta = mystep;

        break;
    case FACE_DOWN:
        xdelta = 0;
        ydelta = mystep;

        break;
    case FACE_DOWN_LEFT:
        xdelta = -mystep;
        ydelta = mystep;

        break;
    case FACE_LEFT:
        xdelta = -mystep;
        ydelta = 0;

        break;
    case FACE_UP_LEFT:
        xdelta = -mystep;
        ydelta = -mystep;

        break;
    default:
        xdelta = 0;
        ydelta = 0;

        break;
    }

    return !myscreen->query_passable(controlx + xdelta, controly + ydelta, controller);
}

bool Statistics::right_walk()
{
    float xdelta;
    float ydelta;

    // if (walkrounds > 60) {
    //     if (direct_walk()) {
    //         return -1;
    //     }
    // }
    //
    // ++walkrounds;

    if (right_blocked() || right_forward_blocked()) {
        // Walk forward
        if (!forward_blocked()) {
            xdelta = controller->lastx;
            ydelta = controller->lasty;

            if (abs(xdelta) > abs(3 * ydelta)) {
                ydelta = 0;
            }

            if (abs(ydelta) > abs(3 * xdelta)) {
                xdelta = 0;
            }

            if (xdelta) {
                xdelta /= abs(xdelta);
            }

            if (ydelta) {
                ydelta /= abs(ydelta);
            }

            // return controller->walk();
            return controller->walkstep(xdelta, ydelta);
        } else {
            // Turn left
            controller->enddir = static_cast<Uint8>((controller->enddir + 6) % 8);

            return controller->turn(controller->enddir);
        }
    } else if (forward_blocked()) {
        // Turn left
        controller->enddir = static_cast<Uint8>((controller->enddir + 6) % 8);

        return controller->turn(controller->enddir);
    } else if (right_back_blocked()) {
        // Turn right
        controller->enddir = static_cast<Uint8>((controller->enddir + 1) % 8);

        switch (controller->enddir) {
        case FACE_UP:
            xdelta = 0;
            ydelta = -1;

            break;
        case FACE_UP_RIGHT:
            xdelta = 1;
            ydelta = -1;

            break;
        case FACE_RIGHT:
            xdelta = 1;
            ydelta = 0;

            break;
        case FACE_DOWN_RIGHT:
            xdelta = 1;
            ydelta = 1;

            break;
        case FACE_DOWN:
            xdelta = 0;
            ydelta = 1;

            break;
        case FACE_DOWN_LEFT:
            xdelta = -1;
            ydelta = 1;

            break;
        case FACE_LEFT:
            xdelta = -1;
            ydelta = 0;

            break;
        case FACE_UP_LEFT:
            xdelta = -1;
            ydelta = -1;

            break;
        default:
            xdelta = 0;
            ydelta = 0;

            break;
        }

        add_command(COMMAND_WALK, 1, xdelta, ydelta);
    } else {
        // We can't even walk straight to our foe
        if (!direct_walk()) {
            switch(controller->curdir) {
            case FACE_UP:
                xdelta = 0;
                ydelta = -1;

                break;
            case FACE_UP_RIGHT:
                xdelta = 1;
                ydelta = -1;

                break;
            case FACE_RIGHT:
                xdelta = 1;
                ydelta = 0;

                break;
            case FACE_DOWN_RIGHT:
                xdelta = 1;
                ydelta = 1;

                break;
            case FACE_DOWN:
                xdelta = 0;
                ydelta = 1;

                break;
            case FACE_DOWN_LEFT:
                xdelta = -1;
                ydelta = 1;

                break;
            case FACE_LEFT:
                xdelta = -1;
                ydelta = 0;

                break;
            case FACE_UP_LEFT:
                xdelta = -1;
                ydelta = -1;

                break;
            default:
                xdelta = 0;
                ydelta = 0;

                break;
            }

            // if (controller->spaces_clear() > 6)
            return controller->walkstep(xdelta, ydelta);
        }
    }

    return true;
}

bool Statistics::direct_walk()
{
    Walker *foe = controller->foe;
    float xdest;
    float ydest;
    float xdelta;
    float ydelta;
    float xdeltastep;
    float ydeltastep;
    float controlx = controller->xpos;
    float controly = controller->ypos;
    // Sint16 xdistance;
    // Sint16 ydistance;
    // Uint32 tempdistance;
    // Uint8 olddir = controller->curdir;
    // Sint16 oldlastx = controller->lastx;
    // Sint16 oldlasty = controller->lasty;

    if (!foe) {
        return false;
    }

    xdest = foe->xpos;
    ydest = foe->ypos;

    xdelta = xdest - controller->xpos;
    ydelta = ydest - controller->ypos;

    if (abs(xdelta) > abs(3 * ydelta)) {
        ydelta = 0;
    }

    if (abs(ydelta) > abs(3 * xdelta)) {
        xdelta = 0;
    }

    if (controller->fire_check(xdelta, ydelta)) {
        clear_command();
        controller->turn(controller->facing(xdelta, ydelta));
        add_command(COMMAND_ATTACK, static_cast<Sint16>(30 + getRandomSint32(25)), 0, 0);

        return true;
    }

    if (xdelta) {
        xdelta = xdelta / fabs(xdelta);
    }

    if (ydelta) {
        ydelta = ydelta / fabs(ydelta);
    }

    xdeltastep = xdelta * controller->stepsize;
    ydeltastep = ydelta * controller->stepsize;

    /*
     * Tom's note on 08/03/97: I think these would work better if replaced by
     * some sort of single "if forward-blocked()" check, otherwise I'm not
     * sure if this works regardless of current facing...
     */
    if (!myscreen->query_grid_passable(controlx + xdeltastep, controly + ydeltastep, controller)) {
        if (!myscreen->query_grid_passable(controlx + xdeltastep, controly + 0, controller)) {
            if (!myscreen->query_grid_passable(controlx + 0, controly + ydeltastep, controller)) {
                walkrounds = 0;

                return false;

                // We cannot get there by ANY of the straight line moves which
                // take us directly toward foelist
            } else {
                // y ok
                if (!ydelta) {
                    walkrounds = 0;

                    return false;
                }

                controller->walkstep(0, ydelta);

                return true;

                // Walk in the y direction
            }
            // End if (xd, 0)
        } else {
            // x ok
            if (!xdelta) {
                walkrounds = 0;

                return false;
            }

            controller->walkstep(xdelta, 0);

            return true;

            // Walk in the x direction
        }
    } else {
        // x and y ok
        if (!xdelta && !ydelta) {
            walkrounds = 0;

            return false;
        }

        controller->walkstep(xdelta, ydelta);

        return true;

        // Walk in the x and y direction
    }
}

bool Statistics::walk_to_foe()
{
    Walker *foe = controller->foe;
    float xdest;
    float ydest;
    float xdelta;
    float ydelta;
    Uint32 tempdistance = 9999999L;
    Sint16 howmany;

    // Random just to be sure this gets reset sometime
    if (!foe || !getRandomSint32(300)) {
        current_distance = 15000L;
        last_distance = current_distance;

        return false;
    }

    --controller->path_check_counter;
    // This makes us only check every few rounds, to save processing time
    if (controller->path_check_counter <= 0) {
        controller->path_check_counter = (5 + rand()) % 10;
        controller->path_to_foe.clear();

        xdest = foe->xpos;
        ydest = foe->ypos;

        xdelta = xdest - controller->xpos;
        ydelta = ydest - controller->ypos;

        tempdistance = static_cast<Uint32>(controller->distance_to_ob(foe));
        // Do simpler pathing if the distance is short or if there are too
        // many walker (pathfinding is expensive)
        if ((tempdistance < PATHING_MIN_DISTANCE) || (myscreen->level_data.myobmap->size() > PATHING_SHORT_CIRCUIT_OBJECT_LIMIT)) {
            std::list<Walker *> foelist = myscreen->find_foes_in_range(myscreen->level_data.oblist, PATHING_MIN_DISTANCE, &howmany, controller);

            if (howmany > 0) {
                Walker *firstfoe = foelist.front();
                clear_command();
                controller->turn(controller->facing(xdelta, ydelta));
                controller->stats->try_command(COMMAND_ATTACK, static_cast<Sint16>(30 + getRandomSint32(25)), 1, 1);
                myscreen->find_near_foe(controller);

                if (!controller->foe && firstfoe) {
                    controller->foe = firstfoe;
                    last_distance = controller->distance_to_ob(foe);
                }

                controller->init_fire();

                return true;
            } else {
                // Or foe has moved; we need a new one
                if (!commands.empty()) {
                    commands.front().commandcount = 0;
                }
            }
        } else {
            controller->find_path_to_foe();
        }

        // End of do_check
    }

    if (controller->path_to_foe.size() > 0) {
        controller->follow_path_to_foe();
        last_distance = static_cast<Uint32>(controller->distance_to_ob(foe));
    } else if (tempdistance < last_distance) {
        // Are we closer than we've ever been?
        // Ten set our checking distance...
        last_distance = tempdistance;

        // Cane we walk in a direct line to foe?
        if (!direct_walk()) {
            // If not, use right-hand walking
            right_walk();
        }
    } else {
        right_walk();
    }

    // Are we really really close? Stop searching, then :)
    if ((tempdistance < 30) && !commands.empty()) {
        commands.front().commandcount = 0;
    }

    return true;
}
