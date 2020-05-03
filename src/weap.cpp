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
// weap; A derived class of Walker
//

#include "weap.hpp"

#include "util.hpp"
#include "video_screen.hpp"

#include <sstream>

Weap::Weap(PixieData const &data)
    : Walker(data)
{
    // Don't normally bounce :)
    do_bounce = 0;
}

Weap::~Weap()
{
    // buffer: PORT: Can't call destructor w/o obj: Walker::~Walker();
}

Sint16 Weap::act()
{
    std::stringstream buf;

    // Make sure everyone we're pointing to is valid
    if (foe && foe->dead) {
        foe = nullptr;
    }

    if (leader && leader->dead) {
        leader = nullptr;
    }

    if (owner && owner->dead) {
        owner = nullptr;
    }

    if (!owner) {
        // To fix cases where our parent died!
        owner = this;
    }

    // Always start with no collision...
    collide_ob = nullptr;

    // Complete previous animations (liek firing)
    if (ani_type != ANI_WALK) {
        return animate();
    }

    // Log("weap %d is ani %d\n", family, ani_type);
    if (myscreen->level_data.mysmoother.query_genre_x_y(xpos, ypos) == TYPE_TREES) {
        if (lineofsight) {
            --lineofsight;
        }
    }

    switch (act_type) {
    case ACT_CONTROL:
        // We are the control character
        Log("Weapon is act_control?\n");

        return 1;
    case ACT_SIT:
        // For things like trees
        if ((family != FAMILY_TREE) && (family != FAMILY_BLOOD) && (family != FAMILY_DOOR)) {
            // Log("weapon sitting\n");
            myscreen->do_notify("Weapon sitting", this);
        }

        return 1;
    case ACT_GENERATE:
        // We are a generator
        Log("Weapon is act_generate?\n");
        // act_generate();

        break;
    case ACT_FIRE:
        // We are a weapon
        act_fire();

        return 1;
    case ACT_GUARD:
        Log("Weapon on guard mode?\n");
        act_guard();

        break;
    case ACT_DIE:
        this->dead = 1;

        return 1;
    case ACT_RANDOM:
        // We are randomly walking toward enemy
        buf << "Weapon " << family << " doing act random?";
        Log("Weapon doing act_random?\n");
        myscreen->do_notify(buf.str(), this);
        buf.clear();

        return 1;
    default:
        // Log("No act type set for weapon.\n");
        myscreen->do_notify("No act type set for weapon", this);

        return 0;
        // END SWITCH
    }

    return 0;
}

/*
 * death is called when an object dies (weapon destructed, etc.)
 * for special effects...
 */
Sint16 Weap::death()
{
    /*
     * Note that the 'dead' variable should ALREADY be set by the
     * time this function is called, so that we can easily reverse
     * the decision :)
     */

    Walker *newob = nullptr;

    // Make sure we don't get multiple deaths
    if (death_called) {
        return 0;
    }

    death_called = 1;

    switch (family) {
    case FAMILY_KNIFE:
        // For return knife
        if (owner && (owner->query_family() != FAMILY_SOLDIER)) {
            // Only soldiers get returning knives
            break;
        }

        newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_KNIFE_BACK);
        newob->owner = owner;
        newob->center_on(this);
        newob->lastx = lastx;
        newob->lasty = lasty;
        newob->stepsize = stepsize;
        newob->ani_type = ANI_ATTACK;
        newob->damage = damage;

        break;
        // End of soldier returning knife
    case FAMILY_ROCK:
        // Used for the elf's bouncing rock, etc.
        // Died of natural causes
        if (!do_bounce || !lineofsight || collide_ob) {
            break;
        }

        // First, undead us so we can collide...
        dead = 0;

        // Did we hit a barrier?
        if (myscreen->query_grid_passable(xpos + lastx, ypos + lasty, this)) {
            dead = 1;

            // If not, die like normal
            break;
        }

        if (myscreen->query_grid_passable(xpos - lastx, ypos + lasty, this)) {
            // Bounce "down-left"
            setxy(xpos - lastx, ypos + lasty);
            lastx = -lastx;
            death_called = 0;

            break;
        }

        if (myscreen->query_grid_passable(xpos + lastx, ypos - lasty, this)) {
            // Bounce "up-right"
            setxy(xpos + lastx, ypos - lasty);
            lasty = -lasty;
            death_called = 0;

            break;
        }

        if (myscreen->query_grid_passable(xpos - lastx, ypos - lasty, this)) {
            setxy(xpos - lastx, ypos - lasty);
            lastx = -lastx;
            lasty = -lasty;
            death_called = 0;

            break;
        }

        // Else we're really stuck, so die :)
        dead = 1;

        break;
    case FAMILY_FIRE_ARROW:
        // Only for exploding, really
    case FAMILY_BOULDER:
        if (!skip_exit) {
            // skip_exit means we're supposed to export :)
            break;
        }

        if (!owner || owner->dead) {
            owner = this;
        }

        newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_EXPLOSION, 1);

        if (!newob) {
            // Failsafe
            break;
        }

        if (on_screen()) {
            myscreen->soundp->play_sound(SOUND_EXPLODE);
        }

        newob->owner = owner;
        newob->stats.hitpoints = 0;
        newob->stats.level = owner->stats.level;
        newob->ani_type = ANI_EXPLODE;
        newob->center_on(this);
        newob->damage = damage * 2;

        break;
        // End fire (exploding) arrows
    case FAMILY_WAVE:
        // Grow to wave2
        dead = 0;
        transform_to(ORDER_WEAPON, FAMILY_WAVE2);
        stats.hitpoints = stats.max_hitpoints;

        break;
        // End wave -> wave2
    case FAMILY_WAVE2:
        // Grow to wave3
        dead = 0;
        transform_to(ORDER_WEAPON, FAMILY_WAVE3);
        stats.hitpoints = stats.max_hitpoints;

        break;
        // End wave2 -> wave3
    case FAMILY_DOOR:
        // Display open picture
        newob = myscreen->level_data.add_weap_ob(ORDER_FX, FAMILY_DOOR_OPEN);

        if (!newob) {
            break;
        }

        newob->ani_type = ANI_DOOR_OPEN;
        newob->setxy(xpos, ypos);
        newob->stats.level = stats.level;
        newob->team_num = team_num;
        // newob->ignore = 1;

        // What way are we "facing"?
        // A wall above us?
        if (myscreen->level_data.mysmoother.query_genre_x_y(xpos / GRID_SIZE, (ypos / GRID_SIZE) - 1) == TYPE_WALL) {
            newob->curdir= FACE_RIGHT;
            // And move us "up"
            // newob->setxy(xpos, ypos - 12);
        } else {
            curdir = FACE_UP;
        }

        break;
        // End open the door...
    default:

        break;
    }

    return 1;
}

Sint16 Weap::animate()
{
    // Walker *newob

    // We never use ani_type as far as I can tell; always use 0
    /*
     * if (ani_type) {
     *     Log("weap ani_type = %d\n", ani_type);
     *     ani_type = 0;
     * }
     */

    switch (family) {
    case FAMILY_TREE:
    case FAMILY_BLOOD:
        if (ani_type > 1) {
            ani_type = 0;
        }

        set_frame(ani[curdir + (ani_type * NUM_FACINGS)][cycle]);
        ++cycle;

        if (ani[curdir + (ani_type * NUM_FACINGS)][cycle] == -1) {
            // ANI_WALK;
            ani_type = 0;
            cycle = 0;
        }

        break;
    case FAMILY_CIRCLE_PROTECTION:
        if (!owner || owner->dead || (stats.hitpoints <= 0)) {
            dead = 1;

            return death();
        }

        center_on(owner);

        break;
    case FAMILY_GLOW:
        // Illegal case
        if (ani_type > 2) {
            ani_type = 2;
        }

        set_frame(ani[curdir + (ani_type * NUM_FACINGS)][cycle]);
        ++cycle;

        if (ani[curdir + (ani_type * NUM_FACINGS)][cycle] == -1) {
            // Pulse
            ani_type = 2;
            cycle = 0;
        }

        if (lifetime < 1) {
            --lifetime;
            dead = 1;
            death();
        } else {
            --lifetime;
        }

        break;
    default:
        ani_type = 0;
        set_frame(ani[curdir][cycle]);
        ++cycle;

        if (ani[curdir][cycle] == -1) {
            cycle = 0;
        }

        break;

        // End of family switch
    }

    return 1;
}

Sint16 Weap::setxy(Sint16 x, Sint16 y)
{
    return Walker::setxy(x, y);
}
