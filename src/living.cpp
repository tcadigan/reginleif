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
// Living; Derived class of Walker
//

#include "living.hpp"

#include "gparser.hpp"
#include "guy.hpp"
#include "screen.hpp"
#include "smooth.hpp"
#include "util.hpp"

#include <cstring>

// From picker
extern Sint32 difficulty_level[DIFFICULTY_SETTINGS];
extern Sint32 current_difficulty;

Living::Living(PixieData const &data)
    : Walker(data)
{
    current_special = 1;
    lifetime = 0;
}

Living::~Living()
{
}

Sint16 Living::act()
{
    // We get extra rounds to act this cycle
    if ((bonus_rounds > 0) && !dead) {
        --bonus_rounds;
        act();
    }

    if (dead) {
        return 0;
    }

    // make sure everyone we're pointing to is valid
    if (foe && (foe->dead || (random(foe->invisibility_left / 20) > 0))) {
        foe = nullptr;
    }

    if (is_friendly(foe)) {
        foe = nullptr;
    }

    if (leader && leader->dead) {
        leader = nullptr;
    }

    if (owner && owner->dead) {
        // owner = nullptr;
        // A living who had an owner who is now dead, dies as well
        dead = 1;
        death();

        return 0;
    }

    if (lifetime) {
        // Our owner gone?
        if (!owner || owner->dead) {
            dead = 1;
            death();

            return 0;
        }

        if (lifetime < 1) {
            --lifetime;
            dead = 1;

            return death();
        }

        --lifetime;

        // Do other things based on our type...
        switch (family) {
        case FAMILY_FIRE_ELEMENTAL:
            // We take a toll from our mage...
            if (stats->hitpoints < stats->max_hitpoints) {
                // We're hurt
                // Take a 'toll' of one health and 3 MP of mage, if there
                Sint32 temp = 0;

                if (owner->stats->hitpoints >= (owner->stats->max_hitpoints / 3)) {
                    temp = 1;
                    --owner->stats->hitpoints;
                }

                if (temp && (owner->stats->magicpoints >= 3)) {
                    temp += 1;
                    owner->stats->magicpoints -= 3;
                }

                // Had both MP and HP, so heal 1 unit
                if (temp == 2) {
                    ++stats->hitpoints;
                } else {
                    // Else go down one more unit of liftime
                    --lifetime;
                }
            } // End of hurt elemental

            // End of elemental drain
            break;
        default:

            break;
        } // End of special stuff for summon guys
    } // end of summoned monster stuff

    // Always start with no collison..
    collide_ob = nullptr;

    /*
     * if (ignore) {
     *     Log("Ignoring Living\n");
     *
     *     return;
     * }
     */

    // Regenerate magic
    if ((stats->magicpoints < stats->max_magicpoints) && !myscreen->enemy_freeze && !bonus_rounds) {
        stats->magicpoints += stats->magic_per_round;
        ++stats->current_magic_delay;

        if (stats->current_magic_delay >= stats->max_magic_delay) {
            ++stats->magicpoints;
            stats->current_magic_delay = 0;
        }

        if (stats->magicpoints > stats->max_magicpoints) {
            stats->magicpoints = stats->max_magicpoints;
        }
    } // End magic increment section

    // Regenerate hitpoints
    if (regen_delay > 0) {
        // Delay after being hit
        --regen_delay;
    } else {
        if ((stats->hitpoints < stats->max_hitpoints) && !myscreen->enemy_freeze && !bonus_rounds) {
            stats->hitpoints += stats->heal_per_round;
            ++stats->current_heal_delay;

            if (stats->current_heal_delay >= stats->max_heal_delay) {
                ++stats->hitpoints;
                stats->current_heal_delay = 0;
            }

            if (stats->hitpoints > stats->max_hitpoints) {
                stats->hitpoints = stats->max_hitpoints;
            }
        } // End hitpoint increment section
    }

    // Special-viewing
    if (view_all > 0) {
        --view_all;
    }

    // Invulnerability
    if (invulnerable_left > 0) {
        --invulnerable_left;
    }

    // Invisibility
    if (invisibility_left > 0) {
        --invisibility_left;
    } else {
        outline = 0;
    }

    // Flight
    if (flight_left > 0) {
        --flight_left;
    }

    if (!myscreen->query_grid_passable(xpos, ypos, this) && !flight_left) {
        ++flight_left;
        --stats->hitpoints;

        if (cfg.is_on("effects", "damage_numbers")) {
            damage_numbers.push_back(DamageNumber(xpos + (sizex / 2), ypos, 1, RED));
        }

        if (stats->hitpoints <= 0) {
            dead = 1;
            death();
        }
    }

    // Charmed-ness
    if (charm_left > 1) {
        --charm_left;
    } else {
        charm_left = 0;

        if (real_team_num != 255) {
            team_num = real_team_num;
            real_team_num = 255;
        }
    }

    if (stats->query_bit_flags(BIT_FORESTWALK)
        && ((myscreen->level_data.mysmoother.query_genre_x_y(xpos / GRID_SIZE, ypos / GRID_SIZE) == TYPE_TREES)
            || (myscreen->level_data.mysmoother.query_genre_x_y((xpos + sizex) / GRID_SIZE, ypos / GRID_SIZE) == TYPE_TREES)
            || (myscreen->level_data.mysmoother.query_genre_x_y((xpos + sizex) / GRID_SIZE, (ypos + sizey) / GRID_SIZE) == TYPE_TREES)
            || (myscreen->level_data.mysmoother.query_genre_x_y(xpos / GRID_SIZE, (ypos + sizey) / GRID_SIZE) == TYPE_TREES))) {
        // Charge us a point fo magic...
        if ((stats->magicpoints > 0.0f) && (stats->current_magic_delay == 0)) {
            --stats->magicpoints;
        }

        float temp;

        if (myguy) {
            temp = 4 - (myguy->dexterity / 10.0f);
        } else {
            temp = 4 - (stats->level / 2.0f);
        }

        if (temp < 0) {
            temp = 0;
        }

        stepsize -= temp;

        if (stepsize < 1) {
            stepsize = 1;
        }

        // End of forest walk check
    } else {
        stepsize = normal_stepsize;
    }

    // Speed bonus
    if (speed_bonus_left > 1) {
        --speed_bonus_left;
        stepsize += speed_bonus;
    }

    if (attack_lunge > 0.0f) {
        attack_lunge -= 0.4f;

        if (attack_lunge < 0.0f) {
            attack_lunge = 0.0f;
        }
    }

    if (hit_recoil > 0.0f) {
        hit_recoil -= 0.6f;

        if (hit_recoil < 0.0f) {
            hit_recoil = 0.0f;
        }
    }

    // Special things for various different living types
    switch (family) {
    case FAMILY_ARCHMAGE:
    {
        // Gets bonus viewing, at times
        Sint32 temp;

        if (stats->level >= 40) {
            temp = 1;
        } else {
            temp = 40 - stats->level;
        }

        // Then we get to see...
        if (!(drawcycle % temp)) {
            view_all += 1;
        }

        break;
    }
    default:

        break;
    } // End of special family auto-powers

    // Complete previous animations (like firing)
    if (ani_type != ANI_WALK) {
        return animate();
    }

    // Are we frozen?
    if (stats->frozen_delay) {
        --stats->frozen_delay;

        return 1;
    }

    if (busy > 0) {
        // This allows busy to be our FIRING delay
        --busy;
    }

    // Find new action

    // Turn if you want to (...turn, around the world...)
    if ((curdir != enddir) && (query_order() == ORDER_LIVING)) {
        return turn(enddir);
    }

    // Are we performing some action?
    if (stats->has_commands()) {
        Sint32 temp = stats->do_command();

        if (temp) {
            return 1;
        }
    }

    if (skip_exit > 0) {
        --skip_exit;
    }

    // Do we have a generic action-type set?
    if (action && (user == -1)) {
        Sint32 temp = do_action();

        if (temp) {
            return temp;
        }
    }

    switch (act_type) {
    case ACT_CONTROL:
        // We are the control character

        return 1;
    case ACT_GENERATE:
        Log("LIVING Generator?\n");

        break;
    case ACT_FIRE:
        Log("Living thinks it's a weapon (act_fire)\n");

        return 1;
    case ACT_GUARD:
        act_guard();

        break;
    case ACT_DIE:
        this->dead = 1;

        return 1;
    case ACT_RANDOM:
        // We are randomly walking toward enemy

        // 1 in 5 to do our special
        if (!random(5)) {
            // Should we do our special? Are we full of magic?
            if (stats->magicpoints >= stats->special_cost[1]) {
                current_special = static_cast<Uint8>(random((stats->level + 2) / 3) + 1);

                if ((current_special > 4) || myscreen->special_name[static_cast<Uint32>(family)][static_cast<Uint32>(current_special)] == "NONE") {
                    current_special = 1;
                }

                if (check_special()) {
                    return special();
                }
            } else {
                // Do random walking...
                act_random();

                return 1;
            }
        } else if (!random(5)) {
            // 1 in 5 to do act_random() function
            act_random();
        } else {
            if (!foe) {
                foe = myscreen->find_near_foe(this);
            }

            if (foe) { // && random(2))
                enddir = static_cast<Uint8>(enddir / 2 * 2);
                curdir = enddir;
                // stats->try_command(COMMAND_SEARCH, 40, 0, 0);
                stats->try_command(COMMAND_SEARCH, 300, 0, 0);
            } // else if (foe) {
            // stats->try_commnad(COMMAND_RIGHT_WALK, 40, 0, 0);
            // }
            else if (!random(2)) {
                foe = myscreen->find_far_foe(this);
            } else {
                stats->try_command(COMMAND_RANDOM_WALK, 20);
            }

            return 1;
        }

        // End RANDOM
        break;
    default:
        Log("No act type set.\n");

        return 0;
    } // End switch

    return 0;
}

Sint16 Living::shove(Walker *target, Sint16 x, Sint16 y)
{
    // Return 0; // Debug memory

    // We are alive and allied
    if (target && !target->dead && (query_order() == ORDER_LIVING) && is_friendly(target)) {
        // Make sure WE don't get shoved
        if (random(3) && (target->query_act_type() != ACT_CONTROL)) {
            // We have to prevent a build-up of shoves which is
            // caused by a blocked target. We do so for now by clearing
            // all commands
            target->stats->clear_command();

            if (target->query_family() == FAMILY_CLERIC) {
                // Healing
                target->current_special = 1;
                target->special();
            }

            target->stats->set_command(COMMAND_WALK, 4, x, y);

            return 1;
        }
    }

    return 0;
}

bool Living::walk(float x, float y)
{
    Sint16 dir;
    // Sint16 newdir;
    // Sint16 newcurdir;
    // Sint16 distance; // Distance between current and desired facings

    // Repeat last walk.
    // lastx = x;
    // lasty = y;

    dir = facing(x, y);

    // If continue decision
    if (curdir == dir) {
        // Check if off map
        if (((x + xpos) < 0)
            || ((x + xpos) >= (myscreen->level_data.grid.w * GRID_SIZE))
            || ((y + ypos) < 0)
            || ((y + ypos) >= (myscreen->level_data.grid.h * GRID_SIZE))) {
            return 0;
        }

        // Here we check if the move is valid
        // Normally we would check if the object at this grid point is
        // passable (I cheated for now)
        // FIXME: These additional check are a hack for the corner clipping
        //        bug (you could get into trees, etc.)
        if (myscreen->query_passable(xpos + x, ypos + y, this)
            && myscreen->query_passable(xpos + ceilf(x), ypos + ceilf(y), this)
            && myscreen->query_passable(xpos + floorf(x), ypos + floorf(y), this)) {
            // Control object does complete redraw anyway
            worldmove(x, y);
            ++cycle;

            // if (!ani || ((curdir * cycle) > sizeof(ani))) {
            //     Log("WALKER::WALK: Bad ani!\n");
            // }

            if (ani[curdir][cycle] == -1) {
                cycle = 0;
            }

            set_frame(ani[curdir][cycle]);

            return 1;
        } else {
            // Invalid move?
            if (collide_ob && !collide_ob->dead) {
                if ((collide_ob->query_order() == ORDER_LIVING) && is_friendly(collide_ob)) {
                    shove(collide_ob, x, y);
                }
            } // End hit some object

            // Animate regardless...
            if (stats->query_bit_flags(BIT_ANIMATE)) {
                ++cycle;

                if (ani[curdir][cycle] == -1) {
                    cycle = 0;
                }

                set_frame(ani[curdir][cycle]);
            }

            return 0;
        }
    } else {
        // Just changing direction
        enddir = static_cast<Uint8>(dir);

        // Technically, control gets an EXTRA call to TURN because first we call
        // WALK, then ACT, wheras other walkers call ACT. This whould cause
        // controll to turn TWICE on the first call to walk, which is bad.
        // So we stop that behavior here.
        if ((this->query_act_type() != ACT_CONTROL) || stats->has_commands()) {
            turn(enddir);
        }
    }

    return 1;
}

Sint16 Living::collide(Walker *ob)
{
    collide_ob = ob;

    // return 1; // Debug

    if (ob && walkerIsAutoAttackable(ob) && (is_friendly(ob) == 0) && !ob->dead && !dead) {
        init_fire();
    }

    return 1;
}

Walker *Living::do_summon(Uint8 whatfamily, Uint16 lifetime)
{
    Walker *newob;

    newob = myscreen->level_data.add_ob(ORDER_LIVING, whatfamily);
    newob->owner = this;
    newob->lifetime = lifetime;
    newob->transform_to(ORDER_LIVING, whatfamily);
    // Log("\n\nSummoned %d, life %d\n", whatfamily, lifetime);

    return newob;
}

// Returns true or false on whether it's good to do the special or not...
Sint16 Living::check_special()
{
    Uint32 distance;
    Uint32 myrange;
    Sint16 howmany;

    // On or off, randomly...
    shifter_down = random(2);

    // Make sure we have enough...
    if (stats->magicpoints < stats->special_cost[static_cast<Sint32>(current_special)]) {
        // Make us do default...
        current_special = 1;
    }

    switch (family) {
    case FAMILY_SOLDIER:
        // Check for foe in range x
        // Already have a foe...
        if (foe) {
            distance = static_cast<Uint32>(distance_to_ob(foe));
            // static_cast<Sint32>(deltax * deltax) + static_cast<Sint32>(deltay * deltay);

            // About 3 squares max, 1 square min
            if ((distance < 75) && (distance > 20)) {
                return 1;
            }

            return 0;
        } else {
            // Get a new foe...
            foe = myscreen->find_near_foe(this);

            if (!foe) {
                return 0;
            }

            distance = static_cast<Uint32>(distance_to_ob(foe)); // (deltax * deltax) + static_cast<Sint32>(deltay * deltay);

            // About 3 squares max, 1 min
            if ((distance < 75) && (distance > 20)) {
                return 1;
            }

            return 0;
        }

        break; // End of fighter case
    case FAMILY_FIRE_ELEMENTAL:
    case FAMILY_ARCHER:
    case FAMILY_GHOST:
    case FAMILY_ORC:
        // Check for for in range x
        // Already have a foe...
        if (foe) {
            distance = static_cast<Uint32>(distance_to_ob(foe));
            // static_cast<Sint32>(deltax * deltax) + static_cast<Sint32>(deltay * deltay);

            // About 6 squares
            if (distance < 130) {
                return 1;
            }

            return 0;
        } else {
            // Get a new foe
            foe = myscreen->find_near_foe(this);

            if (!foe) {
                return 0;
            }

            distance = static_cast<Uint32>(distance_to_ob(foe));
            // static_cast<Sint32>(deltax * deltax) + static_cast<Sint32>(deltay * deltay);

            // About 6 squares
            if (distance < 130) {
                return 1;
            }

            return 0;
        }

        break; // End of fighter case
    case FAMILY_THIEF:
        // Drop bomb
        if (current_special == 1) {
            // Already have a foe...
            if (foe) {
                distance = static_cast<Uint32>(distance_to_ob(foe));
                // (deltax * deltax) + static_cast<Sint32>(deltay * deltay);

                // About 6 squares max, 2 min
                if ((distance < 130) && (distance > 35)) {
                    return 0;
                }
            } else {
                // Get a new foe...
                myscreen->find_foes_in_range(myscreen->level_data.oblist, 110, &howmany, this);

                if (howmany < 3) {
                    return 0;
                }

                return 1;
            }

            break; // End of thief case
            // End of bomb
        } else if (current_special == 3) {
            // Taunt
            if (!shifter_down) {
                myrange = 80 + (4 * stats->level);
            } else {
                // Charm
                myrange = 16 + (4 * stats->level);
            }

            myscreen->find_foes_in_range(myscreen->level_data.oblist, myrange, &howmany, this);

            if (howmany < 1) {
                return 0;
            } else {
                return 1;
            }
        } else {
            // Default is go for it
            return 1;
        }

        break;
    case FAMILY_MAGE:
        // TP if away from guys...
        howmany = 0;
        myscreen->find_foes_in_range(myscreen->level_data.oblist, 110, &howmany, this);

        // Away from anybody...
        if (howmany < 1) {
            return 1;
        }

        // Too many enemies!
        if (howmany > 3) {
            return 1;
        }

        return 0;

        break; // End of fighter case
    case FAMILY_SLIME:
        if (myscreen->level_data.numobs < MAXOBS) {
            return 1;
        } else {
            return 0;
        }

        break;
    case FAMILY_CLERIC:
        // Any friends?
        // Healing
        if (current_special == 1) {
            myscreen->find_friends_in_range(myscreen->level_data.oblist, 60, &howmany, this);

            // Other than ourselves?
            if (howmany > 1) {
                // We're HEALING
                shifter_down = 1;

                return 1;
            } else if (stats->magicpoints >= (stats->max_magicpoints / 2)) {
                // Do mace...
                shifter_down = 1;

                return 1;
            } else {
                return 0;
            }

            // End of healing/mace
        } else {
            return 1;
        }

        break;
    case FAMILY_SKELETON:
        // Tunnel if no foes near...
        howmany = 0;
        myscreen->find_foes_in_range(myscreen->level_data.oblist, 5 * GRID_SIZE, &howmany, this);

        // Away from anybody...
        if (howmany < 1) {
            // So tunnel
            return 1;
        }

        return 0;

        break; // End of skeleton case
    default:

        return 1;
    }

    return 1; // Unreachable?
}

void Living::set_difficulty(Uint32 whatlevel)
{
    // Apparently not used anymore
    // Sint32 calcdelay;
    // Sint32 calcrate;

    Uint32 dif1 = difficulty_level[current_difficulty];
    Uint32 levmult = static_cast<Uint32>(whatlevel) * static_cast<Uint32>(whatlevel);

    switch (family) {
    case FAMILY_ARCHER:
        stats->max_hitpoints += (11 * levmult);
        stats->max_magicpoints += (12 * levmult);
        damage += (4 * whatlevel);
        stats->armor += levmult;

        break;
    case FAMILY_MAGE:
        stats->max_hitpoints += (7 * levmult);
        stats->max_magicpoints += (14 * levmult);
        damage += (3 * whatlevel);
        stats->armor += (levmult / 2.0f);

        break;
    case FAMILY_CLERIC:
    case FAMILY_DRUID:
        stats->max_hitpoints += (9 * levmult);
        stats->max_magicpoints += (12 * levmult);
        damage += (4* whatlevel);
        stats->armor += (levmult / 2.0f);

        break;
    case FAMILY_SOLDIER:
        // Default as soldier
        stats->max_hitpoints += (13 * levmult);
        stats->max_magicpoints += (8 * levmult);
        weapons_left = static_cast<Sint16>((whatlevel + 1) / 2);
        damage += (5 * whatlevel);
        stats->armor += (2 * levmult);

        break;
    case FAMILY_ORC:
        stats->max_hitpoints += (14 * levmult);
        stats->max_magicpoints += (7 * levmult);
        damage += (6 * whatlevel);
        stats->armor += (3 * levmult);

        break;
    case FAMILY_GOLEM:
        stats->max_hitpoints += (18 * levmult);
        stats->max_magicpoints += (5 * levmult);
        damage += (7 * whatlevel);
        stats->armor += (4 * levmult);

        break;
    default:
        stats->max_hitpoints += (11 * levmult);
        stats->max_magicpoints += (11 * levmult);
        damage += (static_cast<Sint16>(4 * whatlevel));
        stats->armor += (2 * levmult);

        break;
    }

    // Adjust for difficulty settings now...
    // Do all EXCEPT player characters
    if (team_num != 0) {
        stats->max_hitpoints = (stats->max_hitpoints * dif1) / 100.0f;
        stats->max_magicpoints = (stats->max_magicpoints * dif1) / 100.0f;
        damage = (damage * dif1) / 100.0f;
    }

    stats->hitpoints = stats->max_hitpoints;
    stats->magicpoints = stats->max_magicpoints;

    stats->max_heal_delay = REGEN; // Defined in graph.h
    stats->current_heal_delay = levmult * 4; // For purposes of calculation only

    // This takes care of the integer part
    while (stats->current_heal_delay > REGEN) {
        stats->current_heal_delay -= REGEN;
        ++stats->heal_per_round;
    }

    // Now calculate the fraction
    if (stats->current_heal_delay > 1) {
        stats->max_heal_delay /= static_cast<Sint32>(stats->current_heal_delay + 1);
    }

    // Start off without healing
    stats->current_heal_delay = 0;

    // Make sure we have at least a 2 wait, otherwise we should have calculated
    // our heal_per_round as one higher, and the math must have bee screwed up
    // some how
    stats->max_heal_delay = std::max(stats->max_heal_delay, 2);

    // Set the magic delay...
    stats->max_magic_delay = REGEN;
    stats->current_magic_delay = static_cast<Sint32>(levmult * 30); // For calculation only

    // This takes care of the integer part
    while (stats->current_magic_delay > REGEN) {
        stats->current_magic_delay -= REGEN;
        ++stats->magic_per_round;
    }

    // Now calculate the fraction
    if (stats->current_magic_delay > 1) {
        stats->max_magic_delay /= static_cast<Sint32>(stats->current_magic_delay + 1);
    }

    // Start off without magic regen
    stats->current_magic_delay = 0;

    // Make sure we have at least a 2 wait, otherwise we should have calculated
    // our magic_per_round as one higher, and the math must have been screwed
    // up some how
    stats->max_magic_delay = std::max(stats->max_magic_delay, 2);
}

Sint16 Living::facing(Sint16 x, Sint16 y)
{
    Sint32 bigy = static_cast<Sint32>(y * 1000);
    Sint32 slope;

    if (!x) {
        if (y > 0) {
            return FACE_DOWN;
        } else {
            return FACE_UP;
        }
    }

    slope = bigy / x;

    if (x > 0) {
        if (slope > 2414) {
            return FACE_DOWN;
        }

        if (slope > 414) {
            return FACE_DOWN_RIGHT;
        }

        if (slope > -414) {
            return FACE_RIGHT;
        }

        if (slope > -2414) {
            return FACE_UP_RIGHT;
        }

        return FACE_UP;
    } else {
        if (slope > 2414) {
            return FACE_UP;
        }

        if (slope > 414) {
            return FACE_UP_LEFT;
        }

        if (slope > -414) {
            return FACE_LEFT;
        }

        if (slope > -2414) {
            return FACE_DOWN_LEFT;
        }

        return FACE_DOWN;
    }
}

Sint16 Living::act_random()
{
    // Apparently not used anymore
    // Sint16 newx;
    // Sint16 newy;

    Sint16 xdist;
    Sint16 ydist;

    // Find our foe
    if (!random(80) || !foe) {
        foe = myscreen->find_near_foe(this);
    }

    if (!foe) {
        return stats->try_command(COMMAND_RANDOM_WALK, 40);
    }

    xdist = static_cast<Sint16>(foe->xpos - xpos);
    ydist = static_cast<Sint16>(foe->ypos - ypos);

    // If foe is in firing range, turn and fire
    if ((abs(xdist) < (lineofsight & GRID_SIZE)) && (abs(ydist) < (lineofsight * GRID_SIZE))) {
        if (fire_check(xdist, ydist)) {
            init_fire(xdist, ydist);
            stats->set_command(COMMAND_FIRE, static_cast<Sint16>(random(24)), xdist, ydist);

            return 1;
        } else {
            // Nearest foe is blocked
            turn(facing(xdist, ydist));
        }
    }

    stats->try_command(COMMAND_SEARCH, 200, 0, 0);
    // stats->try_command(COMMAND_RIGHT_WALK, 50, 0, 0);

    return 1;
}

Sint16 Living::do_action()
{
    if (!action) {
        return 0;
    }

    switch (action) {
    case ACTION_FOLLOW:
        // Follow our leader, attack his targets...
        if (foe) {
            // Continue as normal
            return 0;
        }

        leader = myscreen->find_nearest_player(this);

        if (!leader) {
            // Continue as normal...Shouldn't happen
            return 0;
        }

        if (leader->foe) {
            foe = leader->foe;

            // Continue from this point...
            return 0;
        }

        // Else follow our leader
        stats->force_command(COMMAND_FOLLOW, 5, 0, 0);

        return 1;


        break;
    default:

        return 0;
    }
}

bool Living::walkerIsAutoAttackable(Walker *ob)
{
    return ((ob->query_order() == ORDER_LIVING)
        || (ob->query_family() == FAMILY_TENT)
        || (ob->query_family() == FAMILY_TOWER)
        || (ob->query_family() == FAMILY_TOWER1)
        || (ob->query_family() == FAMILY_TREEHOUSE)
        || (ob->query_family() == FAMILY_BONES)
        || (ob->query_family() == FAMILY_GLOW)
        || (ob->query_family() == FAMILY_TREE)
        || (ob->query_family() == FAMILY_DOOR));
}
