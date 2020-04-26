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
// treasure; a derived class of walker
//

#include "treasure.hpp"

#include "gloader.hpp"
#include "guy.hpp"
#include "input.hpp"
#include "picker.hpp"
#include "pixie_data.hpp"
#include "screen.hpp"
#include "util.hpp"

#include <algorithm>
#include <sstream>

Treasure::Treasure(PixieData const &data)
    : Walker(data)
{
    ignore = 0;
    dead = 0;
}

Treasure::~Treasure()
{
    // buffers: PORT: Cannot call destructor w/o obj: walker::~walker();
}

Sint16 Treasure::act()
{
    // Abort all later code for now...
    return 1;
}

Sint16 Treasure::eat_me(Walker *eater)
{
    bool guys_here;
    std::stringstream buf;
    std::string message;
    Sint32 distance;
    Walker *target;
    Walker *flash;
    std::string exitname;
    Sint32 leftside;
    Sint32 rightside;

    switch (family) {
    case FAMILY_DRUMSTICK:
        if (eater->stats->hitpoints >= eater->stats->max_hitpoints) {
            return 1;
        } else {
            Sint16 amount = (10 * stats->level) + getRandomSint32(10 * stats->level);

            eater->stats->hitpoints = std::min(eater->stats->hitpoints + amount,
                                               eater->stats->max_hitpoints);

            do_heal_effects(nullptr, eater, amount);
            dead = 1;

            if (on_screen()) {
                myscreen->soundp->play_sound(SOUND_EAT);
            }

            return 1;
        }

        break;
    case FAMILY_GOLD_BAR:
        if ((eater->team_num == 0) || eater->myguy) {
            myscreen->save_data.m_score[eater->team_num] += (200 * stats->level);
            dead = 1;

            if (on_screen()) {
                myscreen->soundp->play_sound(SOUND_MONEY);
            }
        }

        return 1;
    case FAMILY_SILVER_BAR:
        if ((eater->team_num == 0) || eater->myguy) {
            myscreen->save_data.m_score[eater->team_num] += (50 * stats->level);
            dead = 1;

            if (on_screen()) {
                myscreen->soundp->play_sound(SOUND_MONEY);
            }
        }

        return 1;
    case FAMILY_FLIGHT_POTION:
        if (!eater->stats->query_bit_flags(BIT_FLYING)) {
            eater->flight_left += (150 * stats->level);

            if (eater->user != -1) {
                buf << "Potion of Flight(" << stats->level << ")!";
                message = buf.str();
                buf.clear();
                myscreen->do_notify(message, eater);
            }

            dead = 1;
        }

        return 1;
    case FAMILY_MAGIC_POTION:
        eater->stats->magicpoints = std::min(eater->stats->magicpoints + (50 * stats->level),
                                             eater->stats->magicpoints);
        dead = 1;

        if (eater->user != -1) {
            buf << "Potion of Mana(" << stats->level << ")!";
            message = buf.str();
            buf.clear();
            myscreen->do_notify(message, eater);
        }

        return 1;
    case FAMILY_INVULNERABLE_POTION:
        if (!eater->stats->query_bit_flags(BIT_INVINCIBLE)) {
            eater->invulnerable_left += (150 * stats->level);
            dead = 1;

            if (eater->user != -1) {
                buf << "Potion of Invulnerability(" << stats->level << ")!";
                message = buf.str();
                buf.clear();
                myscreen->do_notify(message, eater);
            }
        }

        return 1;
    case FAMILY_INVIS_POTION:
        eater->invisibility_left += (150 * stats->level);

        if (eater->user != -1) {
            buf << "Potion of Invisibility(" << stats->level << ")!";
            message = buf.str();
            buf.clear();
            myscreen->do_notify(message, eater);
        }

        dead = -1;

        return 1;
    case FAMILY_SPEED_POTION:
        eater->speed_bonus_left += (50 * stats->level);
        eater->speed_bonus = stats->level;

        if (eater->user != -1) {
            buf << "Potion of Speed(" << stats->level << ")!";
            message = buf.str();
            buf.clear();
            myscreen->do_notify(message, eater);
        }

        dead = 1;

        return 1;
    case FAMILY_EXIT:
        // Go to another level, possibly
        if (eater->in_act) {
            return 1;
        }

        if ((eater->query_act_type() != ACT_CONTROL) || (eater->skip_exit > 1)) {
            return 1;
        }

        eater->skip_exit = 10;

        // See if there are any enemies left...
        if (myscreen->level_done == 0) {
            guys_here = true;
        } else {
            guys_here = false;
        }

        // Get the name of our exit...
        buf << "scen" << stats->level;
        message = buf.str();
        buf.clear();

        exitname = myscreen->get_scen_title(message, myscreen);

        // buffers: PORT: Using strcmp instead of stricmp
        if (exitname != "none") {
            buf << "Level " << stats->level;
            exitname = buf.str();
            buf.clear();
        }

        leftside = 160 - ((exitname.length() + 18) * 3);
        rightside = 160 + ((exitname.length() + 18) * 3);

        /*
         * First check to see if we're withdrawing into somewhere we've been,
         * in which case we abort this level, and set our current level to
         * that pointed to by the exit...
         */
        if (myscreen->save_data.is_level_completed(stats->level)
            && !myscreen->save_data.is_level_completed(myscreen->save_data.scen_num)
            && guys_here) {
            // Okay to leave
            leftside -= 12;
            rightside += 12;

            buf << "Withdraw to " << exitname << "?";
            bool result = yes_or_no_prompt("Exit Field", buf.str(), false);
            buf.clear();

            // Redraw screen...
            myscreen->redrawme = 1;

            // Accepted level change
            if (result) {
                clear_keyboard();
                // Delete all of our current information and abort...
                for (auto const &w : myscreen->level_data.oblist) {
                    if (w && (w->query_order() == ORDER_LIVING)) {
                        w->dead = 1;
                        myscreen->level_data.myobmap->remove(w);
                    }
                }

                /*
                 * Now reload the autosave to revert our changes during battle
                 * (don't use SaveData::update_guys())
                 */
                myscreen->save_data.load("save0");

                // Go to the exit's level
                myscreen->save_data.scen_num = stats->level;
                myscreen->end = 1;

                /*
                 * Autosave because we escaped to a new level.
                 * Save with the new current level
                 */
                myscreen->save_data.save("save0");

                // Retreat
                return myscreen->endgame(1, stats->level);

                // End of accepted withdraw to new level...
            }

            clear_keyboard();
            // End of checking for withdrawal to completed level
        }

        // buffers: Also, allow exit if scenario_type == can exit
        // Nobody evil left, so okay to exit level...
        if (!guys_here || (myscreen->level_data.type == SCEN_TYPE_CAN_EXIT)) {
            buf << "Exit to " << exitname << "?";
            bool result = yes_or_no_prompt("Exit Field", buf.str(), false);
            buf.clear();

            // Redraw screen...
            myscreen->redrawme = 1;

            // Accepted level change
            if (result) {
                clear_keyboard();

                return myscreen->endgame(0, stats->level);
            }

            clear_keyboard();

            return 1;
        }

        return 1;
    case FAMILY_TELEPORTER:
        if (eater->skip_exit > 1) {
            return 1;
        }

        // How away?
        distance = distance_to_ob_center(eater);

        if (distance > 21) {
            return 1;
        }

        if ((distance < 4) && eater->skip_exit) {
            // ++eater->skip_exit;
            eater->skip_exit = 8;

            return 1;
        }

        // If we're close enough, teleport...
        eater->skip_exit += 20;

        if (!leader) {
            target = find_teleport_target();
        } else {
            target = leader;
        }

        if (!target) {
            return 1;
        }

        leader = target;
        eater->center_on(target);

        if (!myscreen->query_passable(eater->xpos, eater->ypos, eater)) {
            eater->center_on(this);

            return 1;
        }

        // No do special effects
        flash = myscreen->level_data.add_ob(ORDER_FX, FAMILY_FLASH);
        flash->ani_type = ANI_EXPAND_8;
        flash->center_on(this);

        return 1;
    case FAMILY_LIFE_GEM:
        // Get back som of lost man's xp...
        // Only our team can get these
        if (eater->team_num != team_num) {
            return 1;
        }

        myscreen->save_data.m_score[eater->team_num] += stats->hitpoints;
        flash = myscreen->level_data.add_ob(ORDER_FX, FAMILY_FLASH);
        flash->ani_type = ANI_EXPAND_8;
        flash->center_on(this);
        dead = 1;
        death();

        return 1;
    case FAMILY_KEY:
        // Get the key to this door...
        // Just got it?
        if (!(eater->keys & static_cast<Sint32>(pow(static_cast<double>(2), stats->level)))) {
            // i.e. 2, 4, 8, 16...
            eater->keys |= static_cast<Sint32>(pow(static_cast<double>(2), stats->level));

            if (eater->myguy) {
                buf << eater->myguy->name << " picks up key " << stats->level;
            } else {
                buf << eater->stats->name << " picks up key " << stats->level;
            }

            message = buf.str();
            buf.clear();

            // Only show players picking up keys
            if (eater->team_num == 0) {
                myscreen->do_notify(message, eater);

                if (eater->on_screen()) {
                    myscreen->soundp->play_sound(SOUND_MONEY);
                }
            }
        }

        return 1;
    default:

        return 1;
        // End of treasure check
    }
}

void Treasure::set_direct_frame(Sint16 whatframe)
{
    PixieData data;
    frame = whatframe;

    data = myscreen->level_data.myloader->graphics[PIX(order, family)];
    bmp = data.data + (frame * size);
}

// Finds the next connected teleporter in the fxlist for you to warp to.
Walker * Treasure::find_teleport_target()
{
    auto const &ls = myscreen->level_data.fxlist;
    // Log(Teleporting from #%d...", number);

    // First find where we are in the list...
    auto mine = std::find(ls.begin(), ls.end(), this);
    if (mine == ls.end()) {
        return nullptr;
    }

    // Now search the rest of the list...
    auto e = mine;
    ++e;

    while (e != ls.end()) {
        Walker *w = *e;

        if (w && !w->dead) {
            if ((w->query_order() == ORDER_TREASURE)
                && (w->query_family() == FAMILY_TELEPORTER)
                && (w->stats->level == stats->level)) {
                // Log(" to target %d\n", number);
                return w;
            }
        }

        ++e;
    }

    // Hit the end of the list, look from top down now...
    e = ls.begin();

    while (e != mine) {
        Walker *w = *e;

        if (w && !w->dead) {
            if ((w->query_order() == ORDER_TREASURE)
                && (w->query_family() == FAMILY_TELEPORTER)
                && (w->stats->level == stats->level)) {
                // Log(" to looped target %d\n", number);
                return w;
            }
        }

        ++e;
    }

    return nullptr;
}
