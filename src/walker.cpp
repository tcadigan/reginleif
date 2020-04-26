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
// walker.cpp

/*
 * ChangeLog:
 *     buffers: 07/32/02: *deleted some redundant headers
 */

#include "walker.hpp"

#include "glad.hpp"
#include "gloader.hpp"
#include "gparser.hpp"
#include "guy.hpp"
#include "map.hpp"
#include "pal32.hpp"
#include "pixie_data.hpp"
#include "screen.hpp"
#include "stats.hpp"
#include "util.hpp"
#include "view.hpp"
#include "weap.hpp"

#include <algorithm>
#include <list>
#include <sstream>
#include <vector>

#define ATTACK_LUNGE_SIZE 5
#define HIT_RECOIL_SIZE 3

#define MAP_WIDTH 400
// Should not really be duplicating this from screen.cpp
#define GRID_SIZE 16

#define MAKE_STATE(x, y) ((((y) / GRID_SIZE) * MAP_WIDTH) + ((x) / GRID_SIZE))
#define GET_STATE_X(state) (((state) % MAP_WIDTH) * GRID_SIZE)
#define GET_STATE_Y(state) (((state) / MAP_WIDTH) * GRID_SIZE)
#define ALIGN_TO_GRID(x) (((x) / GRID_SIZE) * GRID_SIZE)

/*
 * ****************************************************************************
 * WALKER -- Graphics routines
 *
 * WALKER is a PIXIEN with automatic frame changing when the direction it moves
 * is changed. This allows for the concept of "facings", though currently no
 * query-able variables allows for external functions to learn the facing.
 * ****************************************************************************
 */

bool debug_draw_paths = false;

Walker *path_walker = nullptr;

// From picker.cpp
extern Sint32 calculate_level(Uint32 temp_exp);
// From glad.cpp
Sint16 remaining_foes(ViewScreen *myscreen, Walker *myguy);

// From picker.cpp
extern Sint32 difficulty_level[DIFFICULTY_SETTINGS];
extern Sint32 current_difficulty;

// To avoid problems with limited precision
bool float_eq(float a, float b)
{
    return ((a == b) || (((a - 0.000001f) < b) && ((a + 0.000001f) > b)));
}

void draw_smallHealthBar(Walker *w, ViewScreen *view_buf)
{
    if (!cfg.is_on("effects", "mini_hp_bar")) {
        return;
    }

    if ((w->query_order() != ORDER_LIVING) && (w->query_order() != ORDER_GENERATOR)) {
        return;
    }

    Sint32 xscreen = static_cast<Sint32>((w->xpos - view_buf->topx) + view_buf->xloc);
    Sint32 yscreen = static_cast<Sint32>((w->ypos - view_buf->topy) + view_buf->yloc);

    Sint32 walkerstartx = xscreen;
    Sint32 walkerstarty = yscreen;
    Sint32 portstartx = view_buf->xloc;
    Sint32 portstarty = view_buf->yloc;
    Sint32 portendx = view_buf->endx;
    Sint32 portendy = view_buf->endy;

    SDL_Rect r = {
        static_cast<Sint16>(walkerstartx),
        static_cast<Sint16>((walkerstarty + w->sizey) + 1),
        static_cast<Uint16>(w->sizex),
        1
    };

    if ((r.x < portstartx) || (r.x > portendx) || (r.y < portstarty) || (r.y > portendy)) {
        return;
    }

    // Last hit's effect
    float last_points = w->last_hitpoints;
    float last_ratio = static_cast<float>(last_points) / w->stats->max_hitpoints;

    // Current HP
    float points = w->stats->hitpoints;
    float ratio = static_cast<float>(points) / w->stats->max_hitpoints;

    Uint8 whatcolor;

    if (float_eq(points, w->stats->max_hitpoints)) {
        whatcolor = MAX_HP_COLOR;
    } else if ((points * 3) < w->stats->max_hitpoints) {
        whatcolor = LOW_HP_COLOR;
    } else if (((points * 3) / 2) < w->stats->max_hitpoints) {
        whatcolor = MID_HP_COLOR;
    } else if (points < w->stats->max_hitpoints) {
        // HIGH_HP_COLOR;
        whatcolor = LIGHT_GREEN;
    } else {
        whatcolor = ORANGE_START;
    }

    if (ratio >= 0.0f) {
        if (ratio < 0.95f) {
            Uint16 max_w = r.w;

            if ((w->last_hitpoints > w->stats->hitpoints) && (last_ratio <= 1.0f)) {
                myscreen->draw_box(r.x, r.y, r.x + (r.w * last_ratio), r.y + r.h, 53, 1);
            }

            myscreen->draw_box(r.x, r.y, r.x + (r.w * ratio), r.y + r.h, whatcolor, 1);
            myscreen->draw_box(r.x - 1, r.y - 1, (r.x + max_w) + 1, (r.y + r.h) + 1, BLACK, 0);
        }
    }
}


Sint16 get_xp_from_attack(Walker *w, Walker *target, float damage)
{
    float x = w->stats->level - target->stats->level;

    // Whoo-ee! an interpolated (quintic) polynomial to fit ({0, 30}, {1, 25},
    // {2, 15}, {3, 10}, {4, 5}, {5, 2.5}, {6, 1.25}, {7, 0.5}, {8, 0.25},
    // {9, -10}) for 20 damage done. Being an odd order polynomial is important
    // so it can rise to infinity leftware and fall towards negative infinity
    // rightward. The factor was adjusted to make level ups happen at a good
    // rate.
    float poly = (((((-0.00246795 * pow(x, 5))
                     + (0.013243 * pow(x, 4)))
                    + (0.223208 * pow(x, 3)))
                   - (1.16091 * pow(x, 2)))
                  - (5.54277 * x))
        + 30.2923;

    float result = ((6.0f * damage) * poly) / 20.0f;

    if (result <= 0) {
        return 0;
    }

    return result;
}

Sint16 get_xp_from_kill(Walker *w, Walker *target)
{
    return get_xp_from_attack(w, target, 20);
}

Sint16 exp_from_action(ExpActionEnum action, Walker *w, Walker *target, Sint16 value)
{
    switch (action) {
    case EXP_ATTACK:
        // value == damage done

        return get_xp_from_attack(w, target, value);
    case EXP_KILL:

        return get_xp_from_kill(w, target);
    case EXP_HEAL:
        // value == number of hitpoints healed

        return (random(20 * value) / w->stats->level);
    case EXP_TURN_UNDEAD:
        // value == number of turned undead

        return (value * 3);
    case EXP_RAISE_SKELETON:
        // target == the new skeleton

        return 45;
    case EXP_RAISE_GHOST:
        // target == the new ghost

        return 60;
    case EXP_RESURRECT:
        // target = the revived guy or ghost (if it was an enemy)

        return 90;
    case EXP_RESURRECT_PENALTY:
        // target == the revived friend

        return ((target->stats->level * target->stats->level) * 100);
    case EXP_PROTECTION:
        // target == the friend receiving the protection

        return w->stats->level;
    case EXP_EAT_CORPSE:
        // target == the remains to be eaten

        return (target->stats->level * 5);
    }

    return 0;
}

float get_base_damage(Walker *w)
{
    float d = w->damage;
    float sqrtd = sqrtf(d);

    return ((d - (sqrtd / 2.0f)) + random(floor(sqrtd)));
}

float get_damage_reduction(Walker *w, float damage, Walker *target)
{
    if (damage <= 0) {
        return 0;
    }

    float result = target->stats->armor / 2.0f;

    if (result > (damage - 1)) {
        // Always do at least 1 damage
        return (damage - 1);
    }

    return result;
}

Walker::Walker(PixieData const &data)
    : PixieN(data)
{
    // Set our stats...
    stats = new Statistics(this);

    // We are facing DOWN
    curdir = FACE_DOWN;
    // We are trying to face DOWN
    enddir = FACE_DOWN;
    lastx = 0;
    lasty = 0;
    act_type = ACT_RANDOM;
    collide_ob = nullptr;
    cycle = 0;
    ani = nullptr;
    team_num = 0;
    // To show nothing's changed
    real_team_num = 255;
    ani_type = 0;
    busy = 0;
    foe = nullptr;
    leader = nullptr;
    owner = nullptr;
    myguy = nullptr;
    myself = this;
    // We're alive
    dead = 0;
    death_called = 0;
    bonus_rounds = 0;
    // The player's shifter/anternate is NOT pressed
    shifter_down = 0;
    // By default can't see treasures, etc. on radar
    view_all = 0;
    // No keys
    keys = 0;
    // No special action mode
    action = 0;
    // Don't ignore us! Collide with us...
    ignore = 0;
    // Just in case...
    current_weapon = FAMILY_KNIFE;
    default_weapon = current_weapon;
    // Default user status = no user
    user = -1;
    // Set our stats...
    set_frame(0);
    yo_delay = 0;
    flight_left = 0;
    invulnerable_left = 0;
    invisibility_left = 0;
    speed_bonus = 0;
    speed_bonus_left = 0;
    regen_delay = 0;
    charm_left = 0;
    outline = 0;
    drawcycle = 0;
    skip_exit = 0;
    // This to correct a problem with these not being allocated?
    ypos = -1;
    xpos = ypos;
    worldy = -1;
    worldx = worldy;
    // Default, used for fighers
    weapons_left = 1;
    myobmap = nullptr;

    if (myscreen != nullptr) {
        // Default obmap (spatial partitioning optimization?) changed when added to a list
        myobmap = myscreen->level_data.myobmap;
    }

    path_check_counter = (5 + rand()) % 10;
    hurt_flash = false;
    attack_lunge = 0.0f;
    hit_recoil = 0.0f;
    last_hitpoints = 0.0f;
}

bool Walker::reset(void)
{
    // Double comments were needed to make it work, maybe they are not needed???
    // // We are facing UP
    // curdir = 0;
    // // We are trying to face UP
    // enddir = 0;
    // lastx = 0;
    // lasty = 0;
    // act_type = ACT_RANDOM;
    // collide_ob = nullptr;
    // // cycle = 0;
    // ani = nullptr;
    // // team_num = 0;
    // // ani_type = 0;
    // // busy = 0;
    // foe = nullptr;
    // leader = nullptr;
    // owner = nullptr;
    // myguy = nullptr;
    // myself = this;
    // ani = null;
    // We're alive
    dead = 0;
    death_called = 0;
    // bonus_rounds = 0;
    // // The player's shifter/alternate is NOT pressed
    // shifter_down = 0;
    // // By default can't see treasures, etc. on radar
    // view_all = 0;
    // // No keys
    // keys = 0;
    // // No special action mode
    // action = 0;
    // // Don't ignore us! Collide with us...
    ignore = 0;
    // // Just in case...
    // current_weapon = FAMILY_KNIFE;
    // default_weapon = current_weapon;
    // // Default user stats = no user
    // user = -1;
    // // Set our stats...
    // set_frame(0);
    // yo_delay = 0;
    flight_left = 0;
    // invulnerable_left = 0;
    // invisibility_left = 0;
    // outline = 0;
    // drawcycle = 0;
    // skip_exit = 0;
    // // This to correct a problem with these not being allocated?
    // ypos = -1;
    // xpos = ypos;
    // // Default, used for fighters
    // weapons_left = 1;
    path_check_counter = (5 + rand()) % 10;
    regen_delay = 0;

    if (stats) {
        stats->bit_flags = 0;
    }

    hurt_flash = false;
    attack_lunge = 0.0f;
    hit_recoil = 0.0f;
    last_hitpoints = 0.0f;

    return true;
}

Walker::~Walker()
{
    // Debugging memory
    // Log("(Death) Removed ORDER %d FAMLIY %d, pos %dx%x\n", order, family, xpos, ypos);

    foe = nullptr;
    leader = nullptr;
    owner = nullptr;
    collide_ob = nullptr;
    dead = 1;

    if (myobmap != nullptr) {
        // Remove ourselves from obmap lists
        myobmap->remove(this);
    }

    delete stats;
    stats = nullptr;
    bmp = nullptr;

    if (myguy) {
        delete myguy;
    }

    myguy = nullptr;
    myself = nullptr;
}

bool Walker::move(Sint16 x, Sint16 y)
{
    return setxy(static_cast<Sint16>(xpos + x), static_cast<Sint16>(ypos + y));
}

void Walker::worldmove(float x, float y)
{
    return setworldxy(worldx + x, worldy + y);
}

Sint16 Walker::setxy(Sint16 x, Sint16 y)
{
    worldx = x;
    worldy = y;

    if (!ignore) {
        myobmap->move(this, x, y);
    } else {
        // Just remove us, in case :)
        myobmap->remove(this);
    }

    return Pixie::setxy(x, y);
}

void Walker::setworldxy(float x, float y)
{
    worldx = x;
    worldy = y;

    if (!ignore) {
        myobmap->move(this, x, y);
    } else {
        // Just remove us, in case :)
        myobmap->remove(this);
    }

    Pixie::setxy(x, y);
}

// WALK -- This function allows us to change facing when we walk. This includes
// an automatic frame change. It alos redraws the background at the coords it
// used to occupy. It calls the lower level function MOVE
bool Walker::walk()
{
    return walk(lastx, lasty);
}

Sint16 Walker::facing(Sint16 x, Sint16 y)
{
    Sint32 bigy = y * 100;
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
        } else if (slope > 414) {
            return FACE_DOWN_RIGHT;
        } else if (slope > -414) {
            return FACE_RIGHT;
        } else if (slope > -2414) {
            return FACE_UP_RIGHT;
        } else {
            return FACE_UP;
        }
    } else {
        if (slope > 2414) {
            return FACE_UP;
        } else if (slope > 414) {
            return FACE_UP_LEFT;
        } else if (slope > -414) {
            return FACE_LEFT;
        } else if (slope > -2414) {
            return FACE_DOWN_LEFT;
        } else {
            return FACE_DOWN;
        }
    }
}

Sint16 Walker::shove(Walker *target, Sint16 x, Sint16 y)
{
    // This code has been moved to Living, we should only shove livings
    if (x || y || target) {
        Log("Shoving a non-living. ORDER: %d FAMILY: %d\n", order, family);
    }

    return -1;
}

bool Walker::walkstep(float x, float y)
{
    Sint16 returnvalue;
    Sint16 ret1 = 0;
    Sint16 ret2 = 0;
    Sint16 oldcurdir = curdir;
    float step = stepsize;
    float halfstep;
    Sint32 i;
    // Walker *control1 = myscreen->viewob[0]->control;
    // Walker *control2;
    Sint16 mycycle;

    // Repeat last walk.
    lastx = x * stepsize;
    lasty = y * stepsize;

    if ((order == ORDER_LIVING) && (family == FAMILY_TOWER1)) {
        curdir = facing(x, y);
        enddir = curdir;
        lastx = x;
        lasty = y;

        return true;
    }

    returnvalue = walk(x * stepsize, y * stepsize);
    halfstep = 1;

    // Couldn't walk this direction...
    if (!returnvalue) {
        // Now try a baby step
        returnvalue = walk(x * halfstep, y * halfstep);

        // If we still fail
        if (!returnvalue) {
            // Means we are an NPC
            if (user == -1) {
                switch (facing(x, y)) {
                case FACE_UP:
                    // For cardinal directions, faile if we can't walk this direction
                    curdir = FACE_LEFT;
                    ret1 = walk(-step, 0);

                    break;
                case FACE_RIGHT:
                    curdir = FACE_UP;
                    ret1 = walk(0, -step);

                    break;
                case FACE_DOWN:
                    curdir = FACE_RIGHT;
                    ret1 = walk(step, 0);

                    break;
                case FACE_LEFT:
                    curdir = FACE_DOWN;
                    ret1 = walk(0, step);

                    break;
                    // return returnvalue
                case FACE_UP_RIGHT:
                    curdir = FACE_UP;
                    ret1 = walk(0, y * step);
                    curdir = FACE_RIGHT;
                    ret2 = walk(x * step, 0);

                    break;
                case FACE_DOWN_RIGHT:
                    curdir = FACE_DOWN;
                    ret1 = walk(0, y * step);
                    curdir = FACE_RIGHT;
                    ret2 = walk(x * step, 0);

                    break;
                case FACE_DOWN_LEFT:
                    curdir = FACE_DOWN;
                    ret1 = walk(0, y * step);
                    curdir = FACE_LEFT;
                    ret2 = walk(x * step, 0);

                    break;
                case FACE_UP_LEFT:
                    curdir = FACE_UP;
                    ret1 = walk(0, y * step);
                    curdir = FACE_LEFT;
                    ret2 = walk(x * step, 0);

                    break;
                default:
                    ret1 = 0;
                    ret2 = 0;

                    break;
                }

                // End of NPC switch
            } else {
                // We're a user
                // We can't move to where we want to. Can we slide against the wall?
                // Store our cycle
                mycycle = cycle;
                Sint16 myfacing = facing(x, y);
                bool gotup = false;
                bool gotover = false;
                Sint16 dx = 0;
                Sint16 dy = 0;

                switch (myfacing) {
                case FACE_UP:
                case FACE_RIGHT:
                case FACE_DOWN:
                case FACE_LEFT:
                    // For cardinal directions, fail if we can't walk this direction

                    break;
                case FACE_UP_RIGHT:
                    dx = 1;
                    dy = -1;

                    break;
                case FACE_DOWN_RIGHT:
                    dx = 1;
                    dy = 1;

                    break;
                case FACE_DOWN_LEFT:
                    dx = -1;
                    dy = 1;

                    break;
                case FACE_UP_LEFT:
                    dx = -1;
                    dy = -1;

                    break;
                default:
                    ret1 = 0;
                    ret2 = 0;

                    break;
                }

                if ((dx != 0) || (dy != 0)) {
                    for (i = 0; i < step; ++i) {
                        if (myscreen->query_passable(xpos, ypos + dy, this)) {
                            // Walk without turning...
                            worldmove(0, dy);
                            gotup = true;
                        }

                        if (myscreen->query_passable(xpos + dx, ypos, this)) {
                            worldmove(dx, 0);
                            gotover = true;
                        }

                        // Moved horizontally
                        if (!gotup && gotover) {
                            if (dx > 0) {
                                curdir = FACE_RIGHT;
                            } else {
                                curdir = FACE_LEFT;
                            }
                        } else if (gotup && !gotover) {
                            // Moved vertically
                            if (dy < 0) {
                                curdir = FACE_UP;
                            } else {
                                curdir = FACE_DOWN;
                            }
                        }

                        if (gotup || gotover) {
                            // We moved somewhere?
                            cycle = mycycle;
                            ++cycle;

                            if (ani[curdir][cycle] == -1) {
                                cycle = 0;
                            }

                            set_frame(ani[curdir][cycle]);

                            // End of cycled us a frame
                        }
                    }
                }
            }

            curdir = static_cast<Uint8>(oldcurdir);

            return (ret1 || ret2);
        }
    }

    return returnvalue;
}

bool Walker::walk(float x, float y)
{
    Sint16 dir;

    dir = facing(x, y);

    if ((order == ORDER_LIVING) && (family == FAMILY_TOWER1)) {
        curdir = dir;

        return true;
    }

    if (!x && !y) {
        // Log("Walker %d:%d walking 0,0\n", order, family);
        // This happens sometimes, and shouldn't, but it is non-fatal
        return true;
    }

    // If continue direction
    if (curdir == dir) {
        // Check if off map
        if (((x + xpos) < 0)
            || ((x + xpos) >= myscreen->level_data.grid.w * GRID_SIZE)
            || ((y + ypos) < 0)
            || ((y + ypos) >= myscreen->level_data.grid.h * GRID_SIZE)) {
            return false;
        }

        // Here we check if the move is valid
        if (myscreen->query_passable(xpos + x, ypos + y, this)) {
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

            return true;
        } else {
            // Invalid move?
            // We're not alive
            // Animate regardless...
            if (stats->query_bit_flags(BIT_ANIMATE)) {
                ++cycle;

                if (ani[curdir][cycle] == -1) {
                    cycle = 0;
                }

                set_frame(ani[curdir][cycle]);
            }

            return false;
        }
    } else {
        // Changed direction
        curdir = static_cast<Uint8>(dir);
        cycle = 0;
        set_frame(ani[curdir][cycle]);
        worldmove(0, 0);
    }

    return true;
}

float Walker::get_current_angle()
{
    switch (curdir) {
    case FACE_UP:

        return -M_PI_2;
    case FACE_UP_RIGHT:

        return -M_PI_4;
    case FACE_RIGHT:

        return 0.0f;
    case FACE_DOWN_RIGHT:

        return M_PI_4;
    case FACE_DOWN:

        return M_PI_2;
    case FACE_DOWN_LEFT:

        return (3 * M_PI_4);
    case FACE_LEFT:

        return M_PI;
    case FACE_UP_LEFT:

        return (5 * M_PI_4);
    default:

        return 0.0f;
    }
}

bool Walker::turn(Sint16 targetdir)
{
    Sint16 distance;

    // We use a clock-orderd
    // of directions to numbers) to a clock-orderd
    // mapping of directions so that we can calculate what
    // our next facing should be based on our current one

    // Find how we have to turn
    distance = static_cast<Sint16>(curdir - targetdir);

    // Figure out if we should turn clockwise or counterclockwise
    if (((distance >= -4) && (distance < 0)) || (distance >= 4)) {
        curdir = static_cast<Uint8>((curdir + 1) % 8);
    } else {
        curdir = static_cast<Uint8>((curdir + 7) % 8);
    }

    // Now we set our lastx and lasty (facing) variables correctly
    if ((order != ORDER_LIVING) || (family != FAMILY_TOWER1)) {
        switch (curdir) {
        case FACE_UP:
            lastx = 0;
            lasty = -stepsize;

            break;
        case FACE_UP_RIGHT:
            lastx = stepsize;
            lasty = -stepsize;

            break;
        case FACE_RIGHT:
            lastx = stepsize;
            lasty = 0;

            break;
        case FACE_DOWN_RIGHT:
            lastx = stepsize;
            lasty = stepsize;

            break;
        case FACE_DOWN:
            lastx = 0;
            lasty = stepsize;

            break;
        case FACE_DOWN_LEFT:
            lastx = -stepsize;
            lasty = stepsize;

            break;
        case FACE_LEFT:
            lastx = -stepsize;
            lasty = 0;

            break;
        case FACE_UP_LEFT:
            lastx = -stepsize;
            lasty = -stepsize;

            break;
        default:
            lastx = 0;
            lasty = -stepsize;
        }
    }

    cycle = 0;
    set_frame(ani[curdir][cycle]);
    worldmove(0, 0);

    return true;
}

// This is the function you actually call when you want something to fire.
// It initializes the animation if animation is valid and checks to see if
// the object is too busy.
bool Walker::init_fire()
{
    return init_fire(lastx, lasty);
}

bool Walker::init_fire(Sint16 xdir, Sint16 ydir)
{
    // Turn if we want to fire another direction
    // If a non-player fires in a set direction, turn!
    if (facing(xdir, ydir) != curdir) {
        enddir = static_cast<Uint8>(facing(xdir, ydir));
    }

    if ((curdir != enddir) && (query_order() == ORDER_LIVING)) {
        // if (family == FAMILY_TOWER1) {
        //     enddir = curdir;
        // }

        if (query_act_type() == ACT_CONTROL) {
            return false;
        } else {
            return turn(enddir);
        }
    }

    if (busy > 0) {
        // Too busy
        return false;
    }

    // This pauses a few rounds
    busy += fire_frequency;

    // if ((ani_type == ANI_WALK) && (query_order() == ORDER_LIVING))
    // This should allow generators to animate
    if (ani_type == ANI_WALK) {
        ani_type = ANI_ATTACK;
        cycle = 0;
        animate();

        return true;
    } else {
        if (fire()) {
            return true;
        } else {
            return false;
        }
    }
}

Walker *Walker::fire()
{
    Walker *weapon = nullptr;
    Sint16 waver;
    // Sint16 xp;
    // Sint16 yp;

    // Do we have enough spellpoints for our weapon
    if (stats->magicpoints < stats->weapon_cost) {
        return nullptr;
    }

    weapon = create_weapon();

    if (!weapon) {
        return nullptr;
    }

    stats->magicpoints -= stats->weapon_cost;

    // Determine how much the thrown weapon can "waver"
    // Absolute amount...
    waver = static_cast<Sint8>(weapon->stepsize / 2);
    waver = static_cast<Sint8>(random(waver + 1) - (waver / 2));

    switch (facing(lastx, lasty)) {
    case FACE_RIGHT:
        weapon->setxy((xpos + sizex) + 1, ypos + ((sizey - weapon->sizey) / 2));
        weapon->lastx = weapon->stepsize;
        weapon->lasty = waver;

        break;
    case FACE_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, ypos + ((sizey - weapon->sizey) / 2));
        weapon->lastx = -weapon->stepsize;
        weapon->lasty = waver;

        break;
    case FACE_DOWN:
        weapon->setxy(xpos + ((sizex - weapon->size) / 2), (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize;
        weapon->lastx = waver;

        break;
    case FACE_UP:
        weapon->setxy(xpos + ((sizex - weapon->sizex) / 2), (ypos - weapon->sizey) - 1);
        weapon->lasty = -weapon->stepsize;
        weapon->lastx = waver;

        break;
    case FACE_UP_RIGHT:
        weapon->setxy((xpos + sizex) + 1, (ypos - weapon->sizey) - 1);
        weapon->lastx = weapon->stepsize + waver;
        weapon->lasty = -weapon->stepsize + waver;

        break;
    case FACE_UP_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, (ypos - weapon->sizey) - 1);
        weapon->lastx = -weapon->stepsize - waver;
        weapon->lasty = -weapon->stepsize + waver;

        break;
    case FACE_DOWN_RIGHT:
        weapon->setxy((xpos + sizex) + 1, (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize + waver;
        weapon->lastx = weapon->stepsize - waver;

        break;
    case FACE_DOWN_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize + waver;
        weapon->lastx = -weapon->stepsize + waver;

        break;
    }

    weapon->set_frame(frame);

    // Make sure our current direction is wrong so first walk will just
    // be draw (grumble curse)
    weapon->curdir = static_cast<Uint8>((frame + 1) % 2);

    // xp = weapon->xpos;
    // yp = weapon->ypos;

    // Actual combat
    if (!myscreen->query_passable(weapon->xpos, weapon->ypos, weapon)) {
        // *** Melee combat ***
        if (weapon->collide_ob && !weapon->collide_ob->dead) {
            if (attack(weapon->collide_ob) && on_screen()) {
                myscreen->soundp->play_sound(SOUND_CLANG);

                if (cfg.is_on("effects", "attack_lunge")) {
                    if (query_order() == ORDER_LIVING) {
                        attack_lunge = 1.0f;
                        attack_lunge_angle = get_current_angle();
                    }
                }
            }

            if (myguy) {
                // Record that we fired/attacked
                ++myguy->total_shots;
                ++myguy->scen_shots;
            }
        }

        weapon->dead = 1;

        return nullptr;
    } else if (stats->query_bit_flags(BIT_NO_RANGED)) {
        weapon->dead = 1;

        return nullptr;
    } else {
        if ((order == ORDER_LIVING) && (family == FAMILY_SOLDIER)) {
            if (weapons_left <= 0) {
                // Give back the magic it cost, since we didn't throw it
                stats->magicpoints += stats->weapon_cost;
                weapon->dead = 1;

                return nullptr;
            } else {
                --weapons_left;
            }
        }

        // Record our shot...
        if (myguy) {
            ++myguy->total_shots;
            ++myguy->scen_shots;
        }

        // *** Ranged combat ***
        if (on_screen()) {
            if (weapon->query_family() == FAMILY_FIREBALL) {
                myscreen->soundp->play_sound(SOUND_BLAST);
            } else if (weapon->query_family() == FAMILY_METEOR) {
                // play_sound(SOUND_FIREBALL)
                myscreen->soundp->play_sound(SOUND_BLAST);
            } else if (weapon->query_family() == FAMILY_SPRINKLE) {
                // play_sound(SOUND_SPARKLE);
                myscreen->soundp->play_sound(SOUND_SPARKLE);
            } else if (weapon->query_family() == FAMILY_ARROW) {
                // play_sound(SOUND_FIRE);
                myscreen->soundp->play_sound(SOUND_BOW);
            } else if (weapon->query_family() == FAMILY_FIRE_ARROW) {
                // play_sound(SOUND_FIRE));
                myscreen->soundp->play_sound(SOUND_BOW);
            } else if (weapon->query_family() == FAMILY_LIGHTNING) {
                myscreen->soundp->play_sound(SOUND_BOLT);
            } else {
                // play_sound(SOUND_FWIP);
                myscreen->soundp->play_sound(SOUND_FWIP);
            }
        }

        if (order == ORDER_GENERATOR) {
            switch (family) {
            case FAMILY_TOWER: // mages, no lifetime
            case FAMILY_TREEHOUSE: // Elves also have no lifetime
                if (order == FAMILY_TOWER) {
                    // Mages teleport
                    weapon->ani_type = ANI_TELE_IN;
                }

                weapon->stats->level = random(stats->level) + 1;
                weapon->set_difficulty(static_cast<Uint32>(weapon->stats->level));
                weapon->owner = nullptr;

                break;
            default: // Tents, bones, etc.
                weapon->lifetime = 800 + (stats->level * 11);
                weapon->stats->level = random(stats->level) + 1;
                weapon->set_difficulty(static_cast<Uint32>(weapon->stats->level));

                break;
            }
        } else if (order == ORDER_LIVING) {
            // Archmage gets 1/20th of "extra" magic for more damage...
            if (family == FAMILY_ARCHMAGE) {
                float extra = stats->magicpoints / 20;
                stats->magicpoints -= extra;
                // Get this in damage
                weapon->damage += extra;
            }
        }

        return weapon;
    }
}

void Walker::set_weapon_heading(Walker *weapon)
{
    Sint8 waver;

    // Determine how much the thrown weapon can "waver"
    // Absolute amount...
    waver = static_cast<Sint8>((weapon->stepsize) / 2);
    waver = static_cast<Sint8>(random(waver + 1) - (waver / 2));

    // These are from the "owner"
    switch (facing(lastx, lasty)) {
    case FACE_RIGHT:
        weapon->setxy((xpos + sizex) + 1, ypos + ((sizey - weapon->sizey) / 2));
        weapon->lastx = weapon->stepsize;
        weapon->lasty = waver;

        break;
    case FACE_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, ypos + ((sizey - weapon->sizey) / 2));
        weapon->lastx = -weapon->stepsize;
        weapon->lasty = waver;

        break;
    case FACE_DOWN:
        weapon->setxy(xpos +((sizex - weapon->sizex) / 2), (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize;
        weapon->lastx = waver;

        break;
    case FACE_UP:
        weapon->setxy(xpos + ((sizex - weapon->sizex) / 2), (ypos - weapon->sizey) - 1);
        weapon->lasty = -weapon->stepsize;
        weapon->lastx = waver;

        break;
    case FACE_UP_RIGHT:
        weapon->setxy((xpos + sizex) + 1, (ypos - weapon->sizey) - 1);
        weapon->lastx = weapon->stepsize + waver;
        weapon->lasty = -weapon->stepsize + waver;

        break;
    case FACE_UP_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, (ypos - weapon->sizey) - 1);
        weapon->lastx = -weapon->stepsize - waver;
        weapon->lasty = -weapon->stepsize + waver;

        break;
    case FACE_DOWN_RIGHT:
        weapon->setxy((xpos + sizex) + 1, (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize + waver;
        weapon->lastx = weapon->stepsize - waver;

        break;
    case FACE_DOWN_LEFT:
        weapon->setxy((xpos - weapon->sizex) - 1, (ypos + sizey) + 1);
        weapon->lasty = weapon->stepsize + waver;
        weapon->lastx = -weapon->stepsize + waver;

        break;
    }
}

Sint16 Walker::draw(ViewScreen *view_buf)
{
    // Update the drawing coords from the real position
    xpos = worldx;
    ypos = worldy;

    Sint32 xscreen;
    Sint32 yscreen;

    // No need for on screen check, it will be checked at the draw level and the
    // draw level code is cleaner anyway
    // if (!this) {
    //     return 0;
    // }

    if (dead) {
        Log("Drawing a dead guy!\n");

        return 0;
    }

    // if (!bmp) {
    //     Log("No bitmap!\n");
    //     return 0;
    // }

    ++drawcycle;

    xscreen = static_cast<Sint32>((xpos - view_buf->topx) + view_buf->xloc);
    yscreen = static_cast<Sint32>((ypos - view_buf->topy) + view_buf->yloc);

    if (attack_lunge > 0.0f) {
        xscreen += ((attack_lunge * ATTACK_LUNGE_SIZE) * cos(attack_lunge_angle));
        yscreen += ((attack_lunge * ATTACK_LUNGE_SIZE) * sin(attack_lunge_angle));
    }

    if (hit_recoil > 0.0f) {
        xscreen += ((hit_recoil * HIT_RECOIL_SIZE) * cos(hit_recoil_angle));
        yscreen += ((hit_recoil * HIT_RECOIL_SIZE) * sin(hit_recoil_angle));
    }

    if (stats->query_bit_flags(BIT_NAMED) || invisibility_left || flight_left || invulnerable_left) {
        if (outline == OUTLINE_INVULNERABLE) {
            if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }

            if (outline != OUTLINE_NAMED) {
                if (invisibility_left) {
                    outline = OUTLINE_INVISIBLE;
                }
            }
        } else if (outline == OUTLINE_FLYING) {
            // if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
            //     outline = OUTLINE_NAMED;
            // } else if (invisibility_left) {
            //     outline = OUTLINE_INVISIBLE;
            // } else if (invulnerable_left) {
            //     outline = OUTLINE_INVULNERABLE;
            // }

            if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }

            if (outline != OUTLINE_NAMED) {
                if (invisibility_left) {
                    outline = OUTLINE_INVISIBLE;
                } else if (invulnerable_left) {
                    outline = OUTLINE_INVULNERABLE;
                }
            }
        } else if (outline == OUTLINE_NAMED) {
            if (invisibility_left) {
                outline = OUTLINE_INVISIBLE;
            } else if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            }
        } else if (outline == OUTLINE_INVISIBLE) {
            if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }
        } else {
            if (invisibility_left) {
                outline = OUTLINE_INVISIBLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }
        }
    } else {
        outline = 0;
    }

    if (view_buf->control != nullptr) {
        if ((outline == 0)
            && (user != -1)
            && (this != view_buf->control)
            && (this->team_num == view_buf->control->team_num)) {
            outline = OUTLINE_INVISIBLE;
        }
    }

    bool should_draw_hp = true;
    Sint32 fill_mode = 0;
    Sint32 outline_style = 0;
    Sint32 invisibility_amount = 0;
    Sint32 phantom_mode = 0;

    // WE ARE A PHANTOM
    if (stats->query_bit_flags(BIT_PHANTOM)) {
        fill_mode = PHANTOM_MODE;
        phantom_mode = SHIFT_RANDOM;
        should_draw_hp = false;
    } else if (invisibility_left && (view_buf->control != nullptr)) {
        // WE ARE INVISIBLE
        if (this->team_num == view_buf->control->team_num) {
            fill_mode = INVISIBLE_MODE;
            invisibility_amount = invisibility_left + 10;
            outline_style = outline;
            should_draw_hp = false;
        }
    } else if (stats->query_bit_flags(BIT_FORESTWALK)
               && (myscreen->level_data.mysmoother.query_genre_x_y(xpos / GRID_SIZE, ypos / GRID_SIZE) == TYPE_TREES)
               && !stats->query_bit_flags(BIT_FLYING)
               && (flight_left < 1)) {
        fill_mode = INVISIBLE_MODE;
        invisibility_amount = 1000;
        outline_style = 1;
        should_draw_hp = false;
    } else if (outline) {
        // WE HAVE SOME OUTLINE
        fill_mode = OUTLINE_MODE;
        outline_style = outline;
    }

    // Draw me
    if (hurt_flash) {
        hurt_flash = false;
        myscreen->walkputbuffer_flash(xscreen, yscreen, sizex, sizey,
                                      view_buf->xloc, view_buf->yloc,
                                      view_buf->endx, view_buf->endy,
                                      bmp, query_team_color());
    } else {
        if ((fill_mode == 0) && (outline_style == 0)) {
            myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey,
                                    view_buf->xloc, view_buf->yloc,
                                    view_buf->endx, view_buf->endy,
                                    bmp, query_team_color());
        } else {
            myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey,
                                    view_buf->xloc, view_buf->yloc,
                                    view_buf->endx, view_buf->endy,
                                    bmp, query_team_color(),
                                    /* mode */ fill_mode,
                                    /* invsibility */ invisibility_amount,
                                    /* outline */ outline_style,
                                    /* type of phantom */ phantom_mode);
        }
    }

    if (should_draw_hp) {
        draw_smallHealthBar(this, view_buf);
    }

    damage_numbers.erase(std::remove_if(damage_numbers.begin(),
                                        damage_numbers.end(),
                                        [](auto x) { return x.t < 0; }),
                         damage_numbers.end());

    for (auto & e : damage_numbers) {
        e.t -= 0.05f;
        e.y -= 1.5f;

        if (view_buf->control == this) {
            e.draw(view_buf);
        }
    }

    if (debug_draw_paths) {
        draw_path(view_buf);
    }

    return 1;
}

Sint16 Walker::draw_tile(ViewScreen *view_buf)
{
    Sint32 xscreen;
    Sint32 yscreen;

    // No need for an onscreen check, it will be cheked at the draw level and
    // the draw level code is cleaner anyway

    // if (!this) {
    //     return 0;
    // }

    if (dead) {
        Log("drawing a dead guy!\n");

        return 0;
    }

    // if (!bmp) {
    //     Log("No bitmap!\n");
    //
    //     return 0;
    // }

    ++drawcycle;

    xscreen = static_cast<Sint32>((xpos - view_buf->topx) + view_buf->xloc);
    yscreen = static_cast<Sint32>((ypos - view_buf->topy) + view_buf->yloc);

    if (stats->query_bit_flags(BIT_NAMED)
        || invisibility_left
        || flight_left
        || invulnerable_left) {
        if (outline == OUTLINE_INVULNERABLE) {
            if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }

            if (outline != OUTLINE_NAMED) {
                if (invisibility_left) {
                    outline = OUTLINE_INVISIBLE;
                }
            }
        } else if (outline == OUTLINE_FLYING) {
            // if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
            //     outline = OUTLINE_NAMED;
            // } else if (invisibility_left) {
            //     outline = OUTLINE_INVISIBLE;
            // } else if (invulnerable_left) {
            //     outline = OUTLINE_INVULNERABLE;
            // }

            if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }

            if (outline != OUTLINE_NAMED) {
                if (invisibility_left) {
                    outline = OUTLINE_INVISIBLE;
                } else if (invulnerable_left) {
                    outline = OUTLINE_INVULNERABLE;
                }
            }
        } else if (outline == OUTLINE_NAMED) {
            if (invisibility_left) {
                outline = OUTLINE_INVISIBLE;
            } else if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            }
        } else if (outline == OUTLINE_INVISIBLE) {
            if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }
        } else {
            if (invisibility_left) {
                outline = OUTLINE_INVISIBLE;
            } else if (flight_left) {
                outline = OUTLINE_FLYING;
            } else if (invulnerable_left) {
                outline = OUTLINE_INVULNERABLE;
            } else if (view_buf->control) {
                if (stats->query_bit_flags(BIT_NAMED) && (team_num != view_buf->control->team_num)) {
                    outline = OUTLINE_NAMED;
                }
            }
        }
    } else {
        outline = 0;
    }

    if ((outline == 0)
        && (user != -1)
        && (this != view_buf->control)
        && (this->team_num == view_buf->control->team_num)) {
        outline = OUTLINE_INVISIBLE;
    }

    // WE ARE A PHANTOM
    if (stats->query_bit_flags(BIT_PHANTOM)) {
        myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey,
                                view_buf->xloc, view_buf->yloc,
                                xscreen + GRID_SIZE, yscreen + GRID_SIZE,
                                bmp, query_team_color(),
                                /* mode */ PHANTOM_MODE,
                                /* invisibilty */ 0,
                                /* outline */ 0,
                                /* type of phantom */ SHIFT_RANDOM);
    } else if (invisibility_left) {
        // WE HAVE SOME OUTLINE
        myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey,
                                view_buf->xloc, view_buf->yloc,
                                xscreen + GRID_SIZE, yscreen + GRID_SIZE,
                                bmp, query_team_color(),
                                /* mode */ OUTLINE_MODE,
                                /* invisibilty */ 0,
                                /* outline */ outline,
                                /* type of phantom */ 0);

        draw_smallHealthBar(this, view_buf);
    } else {
        myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey,
                                view_buf->xloc, view_buf->yloc,
                                xscreen + GRID_SIZE, yscreen + GRID_SIZE,
                                bmp, query_team_color());

        draw_smallHealthBar(this, view_buf);
    }

    return 1;
}

void Walker::find_path_to_foe()
{
    path_to_foe.clear();
    Map map(this);

    Sint32 startState = MAKE_STATE(xpos, ypos);
    Sint32 endState = MAKE_STATE(foe->xpos, foe->ypos);

    path_to_foe = map.solve(startState, endState);
}

void Walker::follow_path_to_foe()
{
    while (path_to_foe.size() > 0) {
        std::vector<Sint32>::iterator node = path_to_foe.begin();
        Sint32 state = *node;
        Sint32 dx = GET_STATE_X(state) - ALIGN_TO_GRID(xpos);
        Sint32 dy = GET_STATE_Y(state) - ALIGN_TO_GRID(ypos);

        if ((dx != 0) || (dy != 0)) {
            // Normalize the deltas so walkstep can use them as stepsize factors.
            if (dx != 0) {
                dx /= abs(dx);
            }

            if (dy != 0) {
                dy /= abs(dy);
            }

            // Walk toward there and we're done.
            walkstep(dx, dy);

            break;
        }

        // We already made it to this node, so remove it
        path_to_foe.erase(node);
    }
}

void Walker::draw_path(ViewScreen *view_buf)
{
    if (path_to_foe.size() == 0) {
        return;
    }

    // Our pointer is a unique integer...but probably aligned to 4 bytes
    Uint8 mycolor = query_team_color() + (reinterpret_cast<intptr_t>(this) % 5);

    Sint16 offsetx = (view_buf->topx - view_buf->xloc) - 8;
    Sint16 offsety = (view_buf->topy - view_buf->yloc) - 8;

    std::vector<Sint32>::iterator e = path_to_foe.begin();
    Sint32 px = GET_STATE_X(*e) - offsetx;
    Sint32 py = GET_STATE_Y(*e) - offsety;

    while (e != path_to_foe.end()) {
        Sint32 x1 = GET_STATE_X(*e) - offsetx;
        Sint32 y1 = GET_STATE_Y(*e) - offsety;

        myscreen->draw_line(px, py, x1, y1, mycolor);
        myscreen->fastbox_outline(x1 - 1, y1 - 1, 2, 2, mycolor);
        ++e;
        px = x1;
        py = y1;
    }
}

Sint16 Walker::act()
{
    Sint16 temp;

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

    // Always start with no collison...
    collide_ob = nullptr;

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
        // This allows bust to be our FIRING delay.
        --busy;
    }

    // Find new action

    // Turn if you want to
    // if ((curdir != enddir) && (query_order() == ORDER_LIVING)) {
    //     return turn(enddir);
    // }

    // No action for us if we are ACT_CONTROL!
    // if (act_type == ACT_CONTROL) {
    //     stats->clear_command();
    // }

    // Are we performing some action?
    if (stats->has_commands()) {
        temp = stats->do_command();

        if (temp) {
            return 1;
        }
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

    switch (act_type) {
    case ACT_CONTROL:
        // We are the control character

        return 1;
    case ACT_GENERATE:
        // We are a generator
        act_generate();

        break;
    case ACT_FIRE:
        // We are a weapon
        act_fire();

        return 1;
    case ACT_GUARD:
        act_guard();

        break;
    case ACT_DIE:
        this->dead = 1;

        return 1;
    case ACT_RANDOM:
        // We are randomly walking toward enemy
        if (!random(4)) {
            // A 1 in 4 then 1 in 20 chance of random walk
            if (!random(20)) {
                if (!special()) {
                    stats->try_command(COMMAND_WALK, random(30), random(3) - 1, random(3) - 1);
                }

                return 1;
            }

            // 1 in 4 followed by 19 in 20 of doing this
            act_random();
        } else {
            // 3 of 4 times
            if (!foe) {
                foe = myscreen->find_far_foe(this);
            }

            if (foe) {
                // stats->try_command(COMMAND_SEARCH, 60, 0, 0);
                stats->try_command(COMMAND_SEARCH, 500, 0, 0);
            }

            return 1;
        }

        // END RANDOM
        break;
    default:
        Log("No act type set.\n");

        return 0;
    }

    // END_SWITCH
    return 0;
}

Sint16 Walker::set_act_type(Sint16 num)
{
    old_act_type = act_type;
    act_type = static_cast<Uint8>(num);

    return num;
}

Sint16 Walker::restore_act_type()
{
    act_type = old_act_type;

    return old_act_type;
}

Sint16 Walker::query_act_type()
{
    return act_type;
}

Sint16 Walker::set_old_act_type(Sint16 num)
{
    old_act_type = static_cast<Uint8>(num);

    return num;
}

Sint16 Walker::query_old_act_type()
{
    return old_act_type;
}

Sint16 Walker::collide(Walker *ob)
{
    collide_ob = ob;

    return 1;
}

void Walker::do_heal_effects(Walker *healer, Walker *target, Sint16 amount)
{
    if (!cfg.is_on("effects", "heal_numbers")) {
        return;
    }

    if (healer) {
        healer->damage_numbers.push_back(DamageNumber(target->xpos + (target->sizex / 2),
                                                      target->ypos, amount, 56));
    }

    target->damage_numbers.push_back(DamageNumber(target->xpos + (target->sizex / 2),
                                                  target->ypos, amount, 56));
}

void Walker::do_hit_effects(Walker *attacker, Walker *target, Sint16 tempdamage)
{
    if (cfg.is_on("effects", "damage numbers")) {
        // Orange numbers for the atta attacker to see
        if (attacker) {
            attacker->damage_numbers.push_back(DamageNumber(target->xpos + (target->sizex / 2),
                                                            target->ypos, tempdamage, 235));
        }

        // RED numbers for the target to see
        target->damage_numbers.push_back(DamageNumber(target->xpos + (target->sizex / 2),
                                                      target->ypos, tempdamage, RED));
    }

    if (target->stats->hitpoints < 0) {
        tempdamage += target->stats->hitpoints;
    }

    if (cfg.is_on("effects", "hit_anim")) {
        // Create hit effect
        if ((query_order() != ORDER_FX) || (query_family() == FAMILY_KNIFE_BACK)) {
            Walker *newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_HIT);

            if (newob) {
                newob->owner = target;
                newob->team_num = team_num;
                newob->stats->level = 1;
                newob->damage = 0;
                newob->ani_type = (1 + rand()) % 3;

                if (attacker == this) {
                    newob->center_on(target);
                } else {
                    // A projectile
                    // Make the hit effect start at the projectile position
                    // Then move it a little close to its target (average)
                    newob->setworldxy(((target->worldx + (target->sizex / 2)) + newob->worldx) / 2, ((target->worldy + (target->sizey / 1)) + newob->worldy) / 2);
                }
            }
        }
    }

    if (tempdamage > 0) {
        if (cfg.is_on("effects", "hit_flash")) {
            target->hurt_flash = true;
        }

        if (cfg.is_on("effects", "hit_recoil")) {
            if (target->query_order() == ORDER_LIVING) {
                target->hit_recoil = 1.0f;
                target->hit_recoil_angle = atan2(((target->ypos + (target->sizey / 2)) - ypos) - (sizey / 2), ((target->xpos + (target->sizex / 2)) - xpos) - (sizex / 2));
            }
        }
    }
}

void Walker::do_combat_damage(Walker *attacker, Walker *target, Sint16 tempdamage)
{
    // Record damage done for records...
    // Hit a living
    if (attacker && attacker->myguy && (target->query_order() == ORDER_LIVING)) {
        attacker->myguy->total_damage += tempdamage;
        attacker->myguy->scen_damage += tempdamage;
    }

    // Deal the damage
    target->last_hitpoints = target->stats->hitpoints;
    target->stats->hitpoints -= tempdamage;

    do_hit_effects(attacker, target, tempdamage);

    if (target->stats->hitpoints < 0) {
        tempdamage += target->stats->hitpoints;
    }

    // Delay HP regeneration
    if (tempdamage > 0) {
        target->regen_delay = 50;
    }

    if (target->myguy != nullptr) {
        target->myguy->scen_damage_taken += tempdamage;

        if (target->myguy->scen_min_hp > target->stats->hitpoints) {
            target->myguy->scen_min_hp = target->stats->hitpoints;
        }
    }
}

Sint16 Walker::attack(Walker *target)
{
    // Temporary stain
    Walker *blood;
    // Guy at top of chain...
    Walker *headguy;
    Sint16 playerteam = -1;
    std::stringstream buf;
    std::string message;
    float tempdamage = get_base_damage(this);
    Sint16 getscore = 0;
    Uint8 targetorder = target->query_order();
    Uint8 targetfamily = target->query_family();
    // Us or our owner...
    Walker *attacker;
    Sint16 tom = 0;

    if ((myguy != nullptr) || (team_num == 0)) {
        getscore = 1;
    }

    if (target && target->dead) {
        return 0;
    }

    // if (((targetorder == ORDER_LIVING) || is_friendly(target)) ||
    if (is_friendly(target) || (targetorder == ORDER_TREASURE)) {
        return 0;
    }

    if (target->stats->query_bit_flags(BIT_INVINCIBLE) || (target->invulnerable_left != 0)) {
        return 0;
    }

    if ((order != ORDER_LIVING) && owner) {
        attacker = owner;
    } else {
        attacker = this;
    }

    // Who's the top of our chain (i.e., weapon->summoned->mage)
    headguy = this;

    while (headguy->owner && (headguy->owner != headguy)) {
        headguy = headguy->owner;
    }

    if (headguy->myguy && (headguy->user == 0) && (order == ORDER_WEAPON)) {
        ++tom;
    }

    // Modify attack value based on things like magical attacks, etc.
    // Generally going to be be livings...
    switch (targetorder) {
    case ORDER_LIVING:
        // Hit a living target, so we get credit for a hit
        if (attacker->myguy) {
            ++attacker->myguy->total_hits;
            ++attacker->myguy->scen_hits;
        }

        switch (targetfamily) {
            // Slimes are hurt MORE by magical or fire weapons
        case FAMILY_SLIME:
        case FAMILY_SMALL_SLIME:
        case FAMILY_MEDIUM_SLIME:
            if (stats->query_bit_flags(BIT_MAGICAL)) {
                // Twice as susceptible to magic.
                tempdamage *= 2;
            }

            // End of slimes
            break;
        case FAMILY_BARBARIAN:
            // Barbarians get LESS damaged by magical attacks
            if (stats->query_bit_flags(BIT_MAGICAL)) {
                tempdamage /= 2;
            }

            break;
        default:
            // Do nothing in default living case

            break;
        }

        // End of living
        break;
    default:
        // We hit something, but it wasn't living, so don't count
        // as a shot, OR as a hit...
        if (attacker->myguy) {
            // Since we already counted it
            --attacker->myguy->total_shots;
            --attacker->myguy->scen_shots;
        }

        break;
    }

    tempdamage -= get_damage_reduction(attacker, tempdamage, target);

    if (tempdamage < 0) {
        tempdamage = 0;
    }

    do_combat_damage(attacker, target, damage);

    // Base exp from attack damage
    Sint16 newexp = exp_from_action(EXP_ATTACK, this, target, tempdamage);

    // Set our target to fighting our owner in the case of our weapon
    // hit something
    if ((order != ORDER_LIVING) && owner) {
        owner->foe = target;
        target->stats->hit_response(owner);

        if (headguy->myguy) {
            headguy->myguy->exp += newexp;
        }
    } else {
        // Melee combat, set target to hit_response to us
        target->stats->hit_response(this);

        if (myguy) {
            myguy->exp += newexp;

            if (getscore) {
                myscreen->save_data.m_score[team_num] += (tempdamage + target->stats->level);
            }
        }
    }

    if (order == ORDER_WEAPON) {
        stats->hitpoints -= tempdamage;
        --damage;

        if (stats->hitpoints <= 0) {
            if (!stats->query_bit_flags(BIT_IMMORTAL)) {
                dead = 1;
            }

            death();
        }

        // Special effects
        switch (query_family()) {
        case FAMILY_SPRINKLE:
            // Faerie's fire freezes foes :)
            if (targetorder == ORDER_LIVING) {
                if (target->myguy) {
                    target->stats->frozen_delay = random((FAERIE_FREEZE_TIME + (owner->stats->level * 2)) - (target->myguy->constitution / 21));
                } else {
                    target->stats->frozen_delay = random(FAERIE_FREEZE_TIME + (owner->stats->level * 2));
                }

                if (target->stats->frozen_delay < 0) {
                    target->stats->frozen_delay = 0;
                }
            }

            break;
        default:

            break;
        }
    }

    playerteam = 0;

    // Positive score for hurting enemies, negative for us
    // Are we still alive?
    if (owner && (targetorder != ORDER_WEAPON)) {
        if (playerteam != target->team_num) {
            if (getscore) {
                myscreen->save_data.m_score[team_num] += (tempdamage + target->stats->level); // / 2;
            }

            if (headguy->myguy) {
                headguy->myguy->exp += newexp;
            }
        }
    }

    if (target->stats->hitpoints <= 0) {
        if (targetorder == ORDER_LIVING) {
            if (playerteam > -1) {
                if (playerteam != target->team_num) {
                    // headguy can == this
                    if (headguy->myguy) {
                        headguy->myguy->exp += (newexp + exp_from_action(EXP_KILL, this, target, 0));
                        ++headguy->myguy->kills;
                        ++headguy->myguy->scen_kills;
                        headguy->myguy->level_kills += target->stats->level;
                    } // else if (myguy) {
                    // myguy->exp += (newexp + (8 * target->stats->level));
                    // ++myguy->kills;
                    // myguy->level_kills += target->stats->level;
                    // }

                    if (getscore) {
                        myscreen->save_data.m_score[team_num] += (tempdamage + (10 * target->stats->level));
                    }

                    // In named, alert us of the enemy's death
                    // Do we have an NPC name?
                    if (!target->stats->name.empty() && !target->lifetime && !target->owner) {
                        buf << "ENEMY DEATH: " << target->stats->name << " DIED!";
                        message = buf.str();
                        buf.clear();
                        message.resize(80);
                        myscreen->viewob[0]->set_display_text(message, STANDARD_TEXT_TIME);
                    }

                    // This is the last foe
                    if (remaining_foes(myscreen, this) == 1) {
                        buf << "All foes defeated!";
                        message = buf.str();
                        buf.clear();
                        myscreen->viewob[0]->set_display_text(message, STANDARD_TEXT_TIME);
                    }
                } else {
                    // Alert us of the death
                    // Summoned? And have name
                    if ((target->owner || target->lifetime) && !target->stats->name.empty()) {
                        buf << target->stats->name << " Dispelled!";
                    } else if (!target->stats->name.empty()) {
                        // Do we have an NPC name?
                        buf << target->stats->name << " DIED!";
                    } else if (target->myguy && !target->myguy->name.empty()) {
                        buf << target->myguy->name;
                    } else {
                        switch (target->query_family()) {
                        case FAMILY_SOLDIER:
                            buf << "SOLDIER SLAIN";

                            break;
                        case FAMILY_ARCHER:
                            buf << "ARCHER DIED";

                            break;
                        case FAMILY_THIEF:
                            buf << "THIEF KILLED";

                            break;
                        case FAMILY_ELF:
                            buf << "ELF KILLED";

                            break;
                        case FAMILY_MAGE:
                            buf << "MAGE_DIED";

                            break;
                        case FAMILY_SKELETON:
                            buf << "SKELETON CRUMBLED";

                            break;
                        case FAMILY_CLERIC:
                            buf << "CLERIC DIED";

                            break;
                        case FAMILY_FIRE_ELEMENTAL:
                            buf << "FIRE ELEMENTAL EXTINGUISHED";

                            break;
                        case FAMILY_FAERIE:
                            buf << "FAERIE POPPED";

                            break;
                        case FAMILY_SMALL_SLIME:
                        case FAMILY_MEDIUM_SLIME:
                        case FAMILY_SLIME:
                            buf << "SLIME DESTROYED";

                            break;
                        case FAMILY_GHOST:
                            buf << "GHOST VANISHED";

                            break;
                        case FAMILY_DRUID:
                            buf << "DRUID VANQUISHED";

                            break;
                        case FAMILY_ORC:
                            buf << "ORC DIED";

                            break;
                        default:
                            buf << "SOMEONE DIED";

                            break;
                        }
                    }

                    message = buf.str();
                    buf.clear();
                    message.resize(80);
                    myscreen->viewob[0]->set_display_text(message, STANDARD_TEXT_TIME);
                }
            }

            /* blood splats at death */
            // Make temporary stain
            blood = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_BLOOD);
            blood->team_num = target->team_num;
            blood->ani_type = ANI_GROW;
            // So that we can be walked over...?
            blood->ignore = 1;
            blood->setxy(target->xpos, target->ypos);
        }
        if (on_screen() && (targetorder == ORDER_LIVING)) {
            if (random(2)) {
                myscreen->soundp->play_sound(SOUND_DIE1);
            } else {
                myscreen->soundp->play_sound(SOUND_DIE2);
            }
        }

        target->dead = 1;
        // Any special effect upon death...
        target->death();
    }

    collide_ob = nullptr;

    return 1;
}

Sint16 Walker::animate()
{
    Walker *newob;

    set_frame(ani[curdir + (ani_type * NUM_FACINGS)][cycle]);
    ++cycle;

    if (ani[curdir + (ani_type * NUM_FACINGS)][cycle] == -1) {
        // if ((ani_type == ANI_ATTACK) && (query_order() == ORDER_LIVING)) {
        if (ani_type == ANI_ATTACK) {
            fire();
            ani_type = ANI_WALK;
            cycle = 0;

            return 1;
        }

        // Finished teleport out sequence
        // if ((ani_type == ANI_SKEL_GROW) && (family == FAMILY_SKELETON)) {
        if ((ani_type == ANI_SKEL_GROW) && query_type(ORDER_LIVING, FAMILY_SKELETON)) {
            ani_type = ANI_WALK;
            cycle = 0;

            return 1;
        }

        if ((ani_type == ANI_TELE_OUT) && (order == ORDER_LIVING)) {
            if ((family == FAMILY_MAGE) || (family == FAMILY_ARCHMAGE)) {
                ani_type = ANI_TELE_IN;
                cycle = 0;
                teleport();

                return 1;
            } else if (family == FAMILY_SKELETON) {
                ani_type = ANI_TELE_IN;
                cycle = 0;
                teleport_ranged(stats->level * 18);

                return 1;
            } else {
                ani_type = ANI_WALK;
                cycle = 0;

                return 0;
            }
        }

        // Were we a slime who just split?
        if ((ani_type == ANI_SLIME_SPLIT) && (order == ORDER_LIVING)) {
            ani_type = ANI_WALK;
            cycle = 0;

            // First, shrink (and move) normal guy...
            transform_to(ORDER_LIVING, FAMILY_SMALL_SLIME);
            // Diagonal "down left" of normal
            setxy(xpos - 10, ypos + 10);

            // Create a new small slime...
            newob = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_SMALL_SLIME);
            newob->setxy(xpos + 12, ypos - 12);

            // Transfer stats/etc. across to new guy...
            // stats->magicpoints -= stats->special_cost[0];
            transfer_stats(newob);

            if (newob->myguy && (newob->myguy->exp < (1000 * stats->level))) {
                // Can't be "sustained" if too low
                delete newob->myguy;
                newob->myguy = nullptr;
                // Generic name
                newob->stats->name = "SLIME";
                newob->stats->level = calculate_level(myguy->exp / 2);
            } else if (newob->myguy) {
                // Split our experience
                Uint32 exp = myguy->exp / 2;
                Sint16 newlevel = calculate_level(exp);

                // Downgrade us and the copy
                myguy->upgrade_to_level(newlevel);
                myguy->update_derived_stats(this);
                myguy->exp = exp;

                newob->myguy->upgrade_to_level(newlevel);
                newob->myguy->update_derived_stats(newob);
                newob->myguy->exp = exp;
            }

            newob->team_num = team_num;
            newob->foe = foe;
            newob->leader = leader;

            return 1;
        }

        ani_type = ANI_WALK;
        cycle = 0;
    }

    return 1;
}

Sint16 Walker::set_order_family(Uint8 neworder, Uint8 newfamily)
{
    order = neworder;
    family = newfamily;

    return 1;
}

Walker *Walker::create_weapon()
{
    Walker *weapon;
    Sint16 weapon_type;

    // Special case for generators
    if (query_order() == ORDER_GENERATOR) {
        weapon = myscreen->level_data.add_ob(ORDER_LIVING, static_cast<Uint8>(default_weapon));
        weapon->team_num = team_num;
        weapon->owner = this;
        weapon->set_difficulty(stats->level);

        return weapon;
    }

    // Normally, only livings fire
    weapon_type = current_weapon;

    weapon = myscreen->level_data.add_ob(ORDER_WEAPON, static_cast<Uint8>(weapon_type));
    weapon->team_num = team_num;
    weapon->owner = this;
    weapon->set_difficulty(stats->level);
    weapon->damage = (weapon->damage * (stats->level + 3))/ 4;

    if (myguy) {
        weapon->lineofsight += ((myguy->strength / 23) + (myguy->dexterity / 31));
        weapon->damage += (myguy->strength / 7.0f);
    } else {
        weapon->damage *= stats->level;
    }

    weapon->lineofsight += (stats->level / 3);

    // Make "circular" ranges
    switch(facing(lastx, lasty)) {
    case FACE_UP:
    case FACE_RIGHT:
    case FACE_DOWN:
    case FACE_LEFT:
        // This will multiple by 1.207...
        weapon->lineofsight *= 309;
        // = 1.207 for circular range
        weapon->lineofsight /= 256;
        // This will multiply by 1.414
        weapon->stepsize *= 362;
        weapon->stepsize /= 256;

        break;
    default:

        break;
    }

    if (query_family() == FAMILY_CLERIC) {
        weapon->ani_type = ANI_GLOWGROW;
        weapon->lifetime += (stats->level * 110);
    }

    // if (query_family() == FAMILY_DRUID) {
    //     weapon->ani_type = ANI_GROW;
    // }

    // duhhhh he's not using this as his normal weapon
    return weapon;
}

bool Walker::query_next_to()
{
    Sint16 newx;
    Sint16 newy;

    newx = xpos;
    newy = ypos;

    if (lastx > 0) {
        newx += sizex;
    } else if (lastx < 0) {
        newx += -sizex;
    }

    if (lasty > 0) {
        newy += sizey;
    } else /* if (lasty < 0) */ {
        newy += -sizey;
    }

    return !myscreen->query_object_passable(newx, newy, this);
}

Sint16 Walker::special()
{
    Walker *newob;
    Weap *fireob;
    Walker *alive;
    Walker *tempwalk;
    Sint16 tempx;
    Sint16 tempy;
    Sint16 i;
    Sint16 j;
    Sint16 targetx;
    Sint16 targety;
    Uint32 distance;
    Sint16 howmany;
    Sint16 didheal;
    Sint16 generic;
    Sint16 generic2 = 0;
    std::stringstream buf;
    std::string message;
    Sint16 person;

    // Are we somehow dead already?
    if (dead) {
        Log("Dead guy doing special!\n");

        return 0;
    }

    // Do we have a stats object? If not, freak out and exit :)
    if (!stats) {
        Log("Special with no stats\n");

        return 0;
    }

    // Do we have enough for our special ability?
    if (stats->magicpoints < stats->special_cost[static_cast<Sint32>(current_special)]) {
        return 0;
    }

    if (query_order() != ORDER_LIVING) {
        return 0;
    }

    switch (query_family()) {
    case FAMILY_ARCHER:
        switch (current_special) {
        case 1:
            // Fire arrows
            tempx = lastx;
            tempy = lasty;
            curdir = -1;
            lastx = 0;
            lasty = 0;
            stats->magicpoints += (8 * stats->weapon_cost);
            stats->add_command(COMMAND_SET_WEAPON, 1, FAMILY_FIRE_ARROW, 0);
            stats->add_command(COMMAND_QUICK_FIRE, 1, 0, -1);
            stats->add_command(COMMAND_QUICK_FIRE, 1, 1, -1);
            stats->add_command(COMMAND_QUICK_FIRE, 1, 1, 0);
            stats->add_command(COMMAND_QUICK_FIRE, 1, 1, 1);
            stats->add_command(COMMAND_QUICK_FIRE, 1, 0, 1);
            stats->add_command(COMMAND_QUICK_FIRE, 1, -1, 1);
            stats->add_command(COMMAND_QUICK_FIRE, 1, -1, 0);
            stats->add_command(COMMAND_QUICK_FIRE, 1, -1, -1);
            // stats->add_command(COMMAND_WALK, 1, tempx / stepsize, tempy / stepsize);
            stats->add_command(COMMAND_RESET_WEAPON, 1, 0, 0);

            break;
        case 2:
            // Flurry of arrows
            if (busy) {
                return 0;
            }

            stats->magicpoints += (3 * stats->weapon_cost);
            fire();
            fire();
            fire();
            busy += (fire_frequency * 2);

            break;
        case 3: // Exploding arrows
        case 4:
        default:
            if (busy) {
                return 0;
            }

            generic = current_weapon;
            current_weapon = FAMILY_FIRE_ARROW;
            newob = fire();
            current_weapon = generic;

            if (!newob) {
                // Failsafe
                return 0;
            }

            // Used as a dummy variable to signify exploding... :(
            newob->skip_exit = 5000;

            // Buffed arrows
            newob->stats->hitpoints = 500;
            newob->damage *= 2;

            break;
        }

        // End of archer
        break;
    case FAMILY_SOLDIER:
        switch (current_special) {
        case 1:
            // Charge enemy
            if (!stats->forward_blocked()) {
                stats->add_command(COMMAND_RUSH, 3, lastx / stepsize, lasty / stepsize);

                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_CHARGE);
                }
            } else {
                return 0;
            }

            break;
        case 2:
            // Boomerang
            newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_BOOMERANG);
            newob->owner = this;
            newob->team_num = team_num;
            // Dummy, non-zero value
            newob->ani_type = 1;
            newob->lifetime = 30 + (stats->level * 12);
            newob->stats->hitpoints += (stats->level * 12);
            newob->stats->max_hitpoints = newob->stats->hitpoints;
            newob->damage += (stats->level * 4);

            break;
        case 3:
            // Whirlwind attack
            if (busy) {
                // Can't do while attacking, etc.
                return 0;
            }

            busy += 8;
            tempx = lastx;
            tempy = lasty;
            curdir = -1;
            lastx = 0;
            lasty = 0;
            // stats->magicpoints += (8 * stats->weapon_cost);
            stats->add_command(COMMAND_WALK, 1, 0, -1);
            stats->add_command(COMMAND_WALK, 1, 1, -1);
            stats->add_command(COMMAND_WALK, 1, 1, 0);
            stats->add_command(COMMAND_WALK, 1, 1, 1);
            stats->add_command(COMMAND_WALK, 1, 0, 1);
            stats->add_command(COMMAND_WALK, 1, -1, 1);
            stats->add_command(COMMAND_WALK, 1, -1, 0);
            stats->add_command(COMMAND_WALK, 1, -1, -1);

            {
                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 32 + (stats->level * 2), &howmany, this);

                for (auto & w : newlist) {
                    if (w) {
                        tempx = w->xpos - xpos;

                        if (tempx) {
                            tempx = tempx / abs(tempx);
                        }

                        tempy = w->ypos - ypos;

                        if (tempy) {
                            tempy = tempy / abs(tempy);
                        }

                        attack(w);
                        w->stats->force_command(COMMAND_WALK, 8, tempx, tempy);
                    }
                }
            }

            // End of whirlwind attack
            break;
        case 4:
            // Disarm opponent
            if (busy) {
                return 0;
            }

            if (!stats->forward_blocked()) {
                // Can't do this if no frontal enemy
                return 0;
            }

            {
                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 28, &howmany, this);
                generic = 0;

                for (auto & w : newlist) {
                    if (w) {
                        if (random(stats->level) >= random(w->stats->level)) {
                            w->busy += (6 * ((stats->level - w->stats->level) + 1));
                        }

                        // Disarmed at least one guy
                        generic = 1;
                    }
                }

                if (generic) {
                    if (on_screen()) {
                        myscreen->soundp->play_sound(SOUND_CHARGE);
                    }

                    // Player's team
                    if ((team_num == 0) || myguy) {
                        myscreen->do_notify("Fighter Disarmed Enemy!", this);
                    }

                    busy += 5;
                } else {
                    return 0;
                }
            }

            break;
        default:

            break;
        }

        // End of fighter
        break;
    case FAMILY_CLERIC:
        switch (current_special) {
        case 1:
            // Heal/Mystic mace
            // Then do normal heal
            if (!shifter_down) {
                std::list<Walker *> newlist = myscreen->find_friends_in_range(myscreen->level_data.oblist, 60, &howmany, this);

                didheal = 0;

                // Some friends here...
                if (howmany > 1) {
                    for (auto & newob : newlist) {
                        if ((newob->stats->hitpoints < newob->stats->max_hitpoints) && (newob != this)) {
                            // Get the cost first
                            generic = (stats->magicpoints / 4) + random(stats->magicpoints / 4);
                            Sint32 cost = generic / 2;
                            // Add bonus healing
                            generic += (stats->level * 5);

                            if (stats->magicpoints < cost) {
                                generic -= stats->magicpoints;
                                cost -= stats->magicpoints;
                            }

                            // Didn't heal any for this guy
                            if ((generic <= 0) || (cost <= 0)) {
                                break;
                            }

                            // Do the heal
                            newob->stats->hitpoints += generic;
                            stats->magicpoints -= cost;

                            if (myguy) {
                                myguy->exp += exp_from_action(EXP_HEAL, this, newob, generic);
                            }

                            ++didheal;
                            do_heal_effects(this, newob, generic);
                        }
                    }

                    if (!didheal) {
                        // Everyon was healthy, don't charge us
                        return 0;
                    } else {
                        if (!cfg.is_on("effects", "heal_numbers")) {
                            // Inform screen/view to print a message...
                            if (didheal == 1) {
                                buf << "Cleric healed 1 man!";
                            } else {
                                buf << "Cleric healed " << didheal << " men!";
                            }

                            if ((team_num == 0) || myguy) {
                                // Home team
                                message = buf.str();
                                buf.clear();
                                message.resize(80);
                                myscreen->do_notify(message, this);
                            }
                        }
                        // Play sound...
                        if (on_screen()) {
                            myscreen->soundp->play_sound(SOUND_HEAL);
                        }

                        // End of did heal guys case
                    }
                } else {
                    // No friends, so don't charge us
                    return 0;
                }
                // End of normal heal
            } else {
                // Else do mystic mace
                // Can't do more than 1/5 rounds
                if (busy > 0) {
                    return 0;
                }

                // Do we have the int?
                // Need 50+
                if (myguy && (myguy->intelligence < 50)) {
                    // Only players get this
                    if (user != -1) {
                        myscreen->do_notify("50 Int required for Mystic Mace!", this);
                    }

                    return 0;
                }

                if (myguy) {
                    // Record that we fired/attacked
                    ++myguy->total_shots;
                    ++myguy->scen_shots;
                }

                // All okay, let's summon!
                newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_MAGIC_SHIELD);

                // Safety check
                if (!newob) {
                    return 0;
                }

                newob->owner = this;
                newob->team_num = team_num;
                // Dummy, non-zero value
                newob->ani_type = 1;
                // Specify settings based on our mana...
                generic = stats->magicpoints - stats->special_cost[static_cast<Uint32>(current_special)];
                // Get half our excess magic
                generic /= 2;
                newob->lifetime = 100 + generic;
                newob->stats->hitpoints += (generic / 2);
                newob->damage += (generic / 4.0f);

                // Remove those excess magic points :>
                stats->magicpoints -= generic;
                busy += 5;
            }

            break;
        case 2:
            // Raise skeletons
            // Turn undead, low level
            if (shifter_down) {
                if (busy > 0) {
                    return 0;
                }

                // Check for minimum req.
                if (myguy && (myguy->intelligence < 60)) {
                    if (((team_num == 0) || myguy) && on_screen()) {
                        myscreen->do_notify("You need 60 Ine to Turn Undead", this);
                    }

                    busy += 5;

                    return 0;
                }

                generic = turn_undead(4 * stats->level, stats->level);

                if (generic == -1) {
                    // Failed to turn undead
                    return 0;
                }

                if (myguy && generic) {
                    myguy->exp += exp_from_action(EXP_TURN_UNDEAD, this, nullptr, generic); // stats->level / 2);

                    if ((team_num == 0) || myguy) {
                        buf << myguy->name << " turned " << generic << " undead.";
                        message = buf.str();
                        buf.clear();
                        message.resize(80);
                        myscreen->do_notify(message, this);

                        // End of notify visually
                    }
                }

                // Play sound...
                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_HEAL);
                }

                // End of turn undead, low level
            } else {
                newob = myscreen->find_nearest_blood(this);

                if (newob) {
                    targetx = newob->xpos;
                    targety = newob->ypos;
                    distance = static_cast<Uint32>(distance_to_ob(newob)); // ((targetx - xpos) * (targetx - xpos)) + ((targety - ypos) * (targety - ypos));

                    if (myscreen->query_passable(targetx, targety, newob) && (distance < 60)) {
                        alive = do_summon(FAMILY_SKELETON, 125 + (stats->level * 40));

                        if (!alive) {
                            return 0;
                        }

                        alive->team_num = team_num;
                        alive->stats->level = random(stats->level) + 1;
                        alive->set_difficulty(static_cast<Uint32>(alive->stats->level));
                        alive->setxy(newob->xpos, newob->ypos);
                        alive->owner = this;
                        // myscreen->remove_fx_ob(newob);
                        // myscreen->remove_ob(newob, 0);
                        newob->dead = 1;

                        if (myguy) {
                            myguy->exp += exp_from_action(EXP_RAISE_SKELETON, this, alive, 0);
                        }

                        // End passable check
                    } else {
                        return 0;
                    }

                    // End if-newob check
                } else {
                    // End of raise skeletons
                    return 0;
                }
            }

            // End of else-check
            break;
        case 3:
            // Raise ghosts...
            // Turn undead, high level
            if (shifter_down) {
                if (busy > 0) {
                    return 0;
                }

                // check for minimum req.
                if (myguy && (myguy->intelligence < 60)) {
                    if (((team_num == 0) || myguy) && on_screen()) {
                        myscreen->do_notify("You need 60 Int to Turn Undead", this);
                    }

                    busy += 5;

                    return 0;
                }

                generic = turn_undead(4 * stats->level, stats->level);

                if (generic == -1) {
                    // Failed to turn undead
                    return 0;
                }

                if (myguy && generic) {
                    myguy->exp += exp_from_action(EXP_TURN_UNDEAD, this, nullptr, generic); // stats->level / 2);

                    if ((team_num == 0) || myguy) {
                        buf << myguy->name << " turned " << generic << " undead.";
                        message = buf.str();
                        buf.clear();
                        message.resize(80);
                        myscreen->do_notify(message, this);

                        // End of notify visually
                    }
                }

                // Play sound...
                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_HEAL);
                }

                // End of run undead, high level
            } else {
                newob = myscreen->find_nearest_blood(this);

                if (newob) {
                    targetx = newob->xpos;
                    targety = newob->ypos;
                    distance = static_cast<Uint32>(distance_to_ob(newob)); // ((targetx - xpos) * (targetx - xpos) + ((targety - ypos) * (targety - ypos));

                    if (myscreen->query_passable(targetx, targety, newob) && (distance < 50)) {
                        // alive = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_SKELETON);
                        alive = do_summon(FAMILY_GHOST, 150 + (stats->level * 40));

                        if (!alive) {
                            return 0;
                        }

                        alive->stats->level = random(stats->level) + 1;
                        alive->set_difficulty(static_cast<Uint32>(alive->stats->level));
                        alive->team_num = team_num;
                        alive->setxy(newob->xpos, newob->ypos);
                        alive->owner = this;
                        // myscreen->remove_fx(newob);
                        // myscreen->remove_ob(newob, 0);
                        newob->dead = 1;

                        if (myguy) {
                            myguy->exp += exp_from_action(EXP_RAISE_GHOST, this, alive, 0);
                        }

                        // End of passable check
                    } else {
                        return 0;
                    }

                    // Enf of if-newob check
                } else {
                    // End of raise ghosts
                    return 0;
                }

                // End of else check
            }

            break;
        case 4: // Resurrect our guys...
        default:
            newob = myscreen->find_nearest_blood(this);

            if (newob) {
                targetx = newob->xpos;
                targety = newob->ypos;
                distance = distance_to_ob(newob); // ((targetx - xpos) * (targetx - xpos)) + ((targety - ypos) * (targety - ypos));

                if (myscreen->query_passable(targetx, targety, newob) && (distance < 30)) {
                    // Normal resurrection
                    if (is_friendly(newob)) {
                        alive = myscreen->level_data.add_ob(ORDER_LIVING, newob->stats->old_family);

                        if (!alive) {
                            // failsafe
                            return 0;
                        }

                        // Restore our old values...
                        newob->transfer_stats(alive);

                        alive->stats->hitpoints = alive->stats->max_hitpoints / 2;
                        do_heal_effects(this, alive, alive->stats->max_hitpoints / 2);
                        alive->team_num = newob->team_num;

                        // Take some EXP away as penalty if we're a player
                        if (myguy) {
                            Uint16 exp_loss = exp_from_action(EXP_RESURRECT_PENALTY, this, newob, 0);

                            if (myguy->exp >= exp_loss) {
                                myguy->exp -= exp_loss;
                            } else {
                                myguy->exp = 0;
                            }
                        }
                    } else {
                        // Raise an opponent as undead
                        alive = do_summon(FAMILY_GHOST, 200);

                        if (!alive) {
                            return 0;
                        }

                        alive->team_num = team_num;
                        alive->stats->level = random(stats->level) + 1;
                        alive->set_difficulty(static_cast<Uint32>(alive->stats->level));
                        alive->owner = this;
                    }

                    alive->setxy(newob->xpos, newob->ypos);
                    // myscreen->remove_fx_ob(newob);
                    // myscreen->remove_ob(newob, 0);
                    newob->dead = 1;

                    if (myguy) {
                        myguy->exp += exp_from_action(EXP_RESURRECT, this, alive, 0);
                    }

                    // End of passable
                } else {
                    return 0;
                }

                // End of if newob
            } else {
                // End of resurrection
                return 0;
            }

            break;
        }

        // End of cleric
        break;
    case FAMILY_MAGE:
        switch (current_special) {
        case 1:
            // Teleport
            if ((ani_type == ANI_TELE_OUT) || (ani_type == ANI_TELE_IN)) {
                return 0;
            }

            // Leave/Remove a marker
            if (shifter_down) {
                if (busy > 0) {
                    return 0;
                }

                if (myguy && (myguy->intelligence < 75)) {
                    // We're a real player...
                    if (user != -1) {
                        myscreen->do_notify("Need 75 Int for Marker!", this);
                    }

                    // So as not to charge player
                    return 0;
                }

                // Remove a marker, if present
                // Used to check progress
                generic = 0;

                for (auto & ob : myscreen->level_data.oblist) {
                    if (ob
                        && (ob->query_order() == ORDER_FX)
                        && (ob->query_family() == FAMILY_MARKER)
                        && (ob->owner == this)
                        && !ob->dead) {
                        ob->dead = 1;
                        ob->death();

                        if (((team_num == 0) || myguy) && (user != -1)) {
                            myscreen->do_notify("(Old Marker Removed)", this);
                        }

                        busy += 8;

                        break;
                    }
                }

                // Force new placement, for now
                generic = 0;

                // Didn't remove a marker so place one
                if (!generic) {
                    newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_MARKER);

                    if (!newob) {
                        // Failsafe
                        return 0;
                    }

                    newob->owner = this;
                    newob->center_on(this);

                    if (myguy) {
                        newob->lifetime = myguy->intelligence / 33;
                    } else {
                        newob->lifetime = (stats->level / 4) + 1;
                    }

                    // Non-walking
                    newob->ani_type = ANI_SPIN;

                    if (((team_num == 0) || myguy) && (user != 1)) {
                        myscreen->do_notify("Teleport Marker Placed", this);
                        buf << "(" << newob->lifetime << " Uses)";
                        message = buf.str();
                        buf.clear();
                        message.resize(80);
                        myscreen->do_notify(message, this);
                    }

                    busy += 8;

                    // Take an extra cost for placing a marker
                    generic = stats->magicpoints - stats->special_cost[static_cast<Sint32>(current_special)];
                    // Reduce our "extra" by half
                    generic /= 2;
                    stats->magicpoints -= generic;
                }

                // End of put marker
            } else {
                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_TELEPORT);
                }

                ani_type = ANI_TELE_OUT;
                cycle = 0;
            }

            break;
        case 2:
            // Store our facing
            tempx = lastx;
            tempy = lasty;
            // Do we have extra magic points to spend?
            generic = stats->magicpoints - stats->special_cost[static_cast<Sint32>(current_special)];

            if (generic > 0) {
                // Take 7% of remaining magic...
                generic = generic / 15;
                // An subtract this cost...
                stats->magicpoints -= generic;
            } else {
                generic = 0;
            }

            // now face each directon and fire...
            stats->magicpoints += (8 * stats->weapon_cost);

            for (i = -1; i < 2; ++i) {
                for (j = -1; j < 2; ++j) {
                    if (i || j) {
                        lastx = i;
                        lasty = j;
                        newob = fire();

                        if (newob) {
                            // Bonus for extra mp
                            newob->damage += generic;
                            newob->lineofsight+= (generic / 3);

                            if (newob->lastx != 0.0f) {
                                newob->lastx /= fabs(newob->lastx);
                            }

                            if (newob->lasty != 0.0f) {
                                newob->lasty /= fabs(newob->lasty);
                            }

                            // End of got a valie weapon
                        }

                        // End checked for not center
                    }

                    // End did all 8 directions
                }
            }

            // Restore old facing
            lastx = tempx;
            lasty = tempy;

            break;
        case 3:
            // Freeze time
            // The player's team
            if ((team_num == 0) || myguy) {
                myscreen->enemy_freeze += (20 + (11 * stats->level));
                set_palette(myscreen->bluepalette);
            } else {
                generic = 5 + (2 * stats->level);

                if (generic > 50) {
                    generic = 50;
                }

                buf << "TIME IS FROZEN! (" << generic << " rounds)";
                message = buf.str();
                buf.clear();
                message.resize(80);
                myscreen->viewob[0]->set_display_text(message, 2);
                myscreen->viewob[0]->redraw();
                myscreen->viewob[0]->refresh();
                // myscreen->buffer_to_screen(0, 0, 320, 200);
                std::list<Walker *> newlist = myscreen->find_friends_in_range(myscreen->level_data.oblist, 30000, &howmany, this);

                for (auto & w : newlist) {
                    if (w) {
                        w->bonus_rounds += generic;
                    }
                }
            }

            break;
        case 4:
            // Energy wave
            newob = fire();

            if (!newob) {
                // Failed somehow? ?!?
                return 0;
            }

            alive = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_WAVE);
            alive->center_on(newob);
            alive->owner = this;
            alive->stats->level = stats->level;
            alive->lastx = newob->lastx;
            alive->lasty = newob->lasty;
            newob->dead = 1;

            break;
        case 5:
        default:
        {
            // Burst enemies into flame...
            std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 80 + (2 * stats->level), &howmany, this);

            if (!howmany) {
                // Didn't find any enemies...
                return 0;
            }

            generic = stats->magicpoints - stats->special_cost[5];
            generic /= 2;
            // So do half magic, div enemies
            generic /= howmany;

            if (myguy) {
                myguy->total_shots += howmany;
                myguy->scen_shots += howmany;
            }

            busy += 5;

            // Create explosions on top of the target objects
            for (auto & ob : newlist) {
                newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_EXPLOSION);

                if (!newob) {
                    // Failsafe
                    return 0;
                }

                newob->owner = this;
                newob->team_num = team_num;
                newob->stats->level = stats->level;
                newob->damage = generic;
                newob->center_on(ob);

                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_EXPLODE);
                }

                newob->ani_type = ANI_EXPLODE;
                newob->stats->set_bit_flags(BIT_MAGICAL, 1);
                // Don't hurt caster
                newob->skip_exit = 100;
                stats->magicpoints -= generic;
            }

            // End of burst enemies
            break;
        }
        }

        break;
    case FAMILY_ARCHMAGE:
        switch (current_special) {
        case 1:
            // Teleport
            if ((ani_type == ANI_TELE_OUT) || (ani_type == ANI_TELE_IN)) {
                return 0;
            }

            // Leave/Remove a marker
            if (shifter_down) {
                if (busy > 0) {
                    return 0;
                }

                if (myguy && (myguy->intelligence < 75)) {
                    myscreen->do_notify("Need 75 Int for Marker!", this);

                    // So as not to charge player
                    return 9;
                }

                // Remove a marker, if present
                // Used to check progress
                generic = 0;

                for (auto & ob : myscreen->level_data.oblist) {
                    if (ob
                        && (ob->query_order() == ORDER_FX)
                        && (ob->query_family() == FAMILY_MARKER)
                        && (ob->owner == this)
                        && !ob->dead) {
                        ob->dead = 1;
                        ob->death();

                        if ((team_num == 0) || myguy) {
                            myscreen->do_notify("(Old Marker Removed)", this);
                        }
                        
                        busy += 8;
                        generic = 1;

                        break;
                    }

                    // End of cycle through object list
                }

                // Now place a marker...
                newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_MARKER);

                if (!newob) {
                    // Failsafe
                    return 0;
                }

                newob->owner = this;
                newob->center_on(this);

                if (myguy) {
                    newob->lifetime = myguy->intelligence / 33;
                } else {
                    newob->lifetime = (stats->level / 4) + 1;
                }

                // Non-walking
                newob->ani_type = 2;

                if ((team_num == 0) || myguy) {
                    myscreen->do_notify("Teleport Marker Placed", this);
                    buf << "(" << newob->lifetime << " Uses)";
                    message = buf.str();
                    buf.clear();
                    message.resize(80);
                    myscreen->do_notify(message, this);
                }

                busy += 8;

                // Take an extra cost for placing a marker
                generic = stats->magicpoints - stats->special_cost[static_cast<Sint32>(current_special)];
                // Reduce our "extra" by half
                generic /= 2;

                stats->magicpoints -= generic;

                // End of put a marker (shifter_down)
            } else {
                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_TELEPORT);
                }

                ani_type = ANI_TELE_OUT;
                cycle = 0;
            }

            // End of ArchMage's teleport
            break;
        case 2:
            // Burst enemies into flame, or chain lightning...
            if (busy > 0) {
                return 0;
            }

            if (shifter_down) {
                if (myguy) {
                    // Range to scan for enemies
                    generic = 200 + (myguy->intelligence / 2);
                } else {
                    generic = 200 + (stats->level * 5);
                }
            } else {
                generic = 80;
            }

            {
                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, generic + (2 * stats->level), &howmany, this);

                if (!howmany) {
                    // Didn't find any enemies...
                    return 0;
                }

                // Normal usage
                if (!shifter_down) {
                    generic = stats->magicpoints - stats->special_cost[2];
                    generic /= 2;
                    // So do half magic, div enemies
                    generic /= howmany;

                    if (myguy) {
                        myguy->total_shots += howmany;
                        myguy->scen_shots += howmany;
                    }

                    busy += 5;

                    // Create explosions on the target objects
                    for (auto & ob : newlist) {
                        newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_EXPLOSION);

                        if (!newob) {
                            // Failsafe
                            return 0;
                        }

                        newob->owner = this;
                        newob->team_num = team_num;
                        newob->stats->level = stats->level;
                        newob->stats->set_bit_flags(BIT_MAGICAL, 1);
                        newob->damage = generic;
                        newob->center_on(ob);

                        if (on_screen()) {
                            myscreen->soundp->play_sound(SOUND_EXPLODE);
                        }

                        newob->ani_type = ANI_EXPLODE;
                        newob->stats->set_bit_flags(BIT_MAGICAL, 1);
                        // Don't hurt casters
                        newob->skip_exit = 100;
                        stats->magicpoints -= generic;
                    }

                    // End of heartburst, standard case
                } else {
                    // Do chain-lightning
                    busy += 5;

                    if (myguy) {
                        // So can get > 100% :)
                        ++myguy->total_shots;
                        ++myguy->scen_shots;
                    }

                    newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_CHAIN);
                    newob->center_on(this);
                    newob->owner = this;
                    newob->stats->level = stats->level;
                    newob->team_num = team_num;
                    // Use half our remaining magic...
                    generic = stats->magicpoints - stats->special_cost[2];
                    generic /= 2;
                    stats->magicpoints -= generic;
                    newob->damage = generic;

                    // Find closest of our foes in range
                    generic = 30000;

                    for (auto const & w : newlist) {
                        Sint16 dist = distance_to_ob_center(w);

                        if (generic > dist) {
                            generic = dist;
                            newob->leader = w;
                        }
                    }

                    // newob->ani_type = ANI_ATTACK;

                    // End of chain-lightning
                }
            }

            // End of burst enemies, chain lightning
            break;
        case 3:
            // Summoning...real or illusion
            if (busy > 0) {
                return 0;
            }

            // Then we do true summoning...
            if (shifter_down) {
                // Do we have the in?
                // Need 150+
                if (myguy && (myguy->intelligence < 150)) {
                    // Only players get this
                    if (user != -1) {
                        myscreen->do_notify("150 Int required to Summon!", this);
                    }

                    return 0;
                }

                // Take an extra 50% mana-cost
                generic = stats->magicpoints - stats->special_cost[3];
                generic /= 2;
                stats->magicpoints -= generic;
                // First make the guy we'd summon, at least physically
                newob = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_FIRE_ELEMENTAL);

                if (!newob) {
                    // Failsafe
                    return 0;
                }

                // We need to check for space around the ArchMage...
                // This means we have or haven't found room
                generic = 0;

                for (i = -1; i <= 1; ++i) {
                    for (j = -1; j <= 1; ++j) {
                        if (((i == 0) && (j == 0)) || generic) {
                            continue;
                        }

                        if (myscreen->query_passable(xpos + ((newob->sizex + 1) * i), ypos + ((newob->sizey + 1) * j), newob)) {
                            // We've found a legal spot...
                            generic = 1;
                            newob->setxy(xpos + ((newob->sizex + 1) * i),
                                         ypos + ((newob->sizey + 1) * j));

                            newob->stats->level = (stats->level + 1) / 2;
                            newob->set_difficulty(newob->stats->level);
                            // Set to our team
                            newob->team_num = team_num;
                            // We're owned!
                            newob->owner = this;
                            newob->lifetime = 200 + (60 * stats->level);

                            // End of successfully put summoned created
                        }
                    }

                    // End of I and J loops
                }

                // We never found a legal spot
                if (!generic) {
                    newob->dead = 1;

                    return 0;
                }

                // Takes lots of time :)
                busy += 15;

                // End of shifter down true summoning
            } else {
                // Standard, illusion-only
                // Determine what type of thing to summon image of
                generic = stats->magicpoints - stats->special_cost[3];

                // Lowest type
                if (generic < 100) {
                    person = FAMILY_ELF;
                } else if (generic < 250) {
                    switch (random(3)) {
                    case 0:
                        person = FAMILY_ELF;

                        break;
                    case 1:
                        person = FAMILY_SOLDIER;

                        break;
                    case 2:
                        person = FAMILY_ARCHER;

                        break;
                    default:
                        person = FAMILY_SOLDIER;

                        break;
                    }
                } else if (generic < 500) {
                    switch (random(5)) {
                    case 0:
                        person = FAMILY_ELF;

                        break;
                    case 1:
                        person = FAMILY_SOLDIER;

                        break;
                    case 2:
                        person = FAMILY_ARCHER;

                        break;
                    case 3:
                        person = FAMILY_ORC;

                        break;
                    case 4:
                        person = FAMILY_SKELETON;

                        break;
                    default:
                        person = FAMILY_ARCHER;

                        break;
                    }
                } else if (generic < 1000) {
                    switch (random(7)) {
                    case 0:
                        person = FAMILY_ELF;

                        break;
                    case 1:
                        person = FAMILY_SOLDIER;

                        break;
                    case 2:
                        person = FAMILY_ARCHER;

                        break;
                    case 3:
                        person = FAMILY_ORC;

                        break;
                    case 4:
                        person = FAMILY_SKELETON;

                        break;
                    case 5:
                        person = FAMILY_DRUID;

                        break;
                    case 6:
                        person = FAMILY_CLERIC;

                        break;
                    default:
                        person = FAMILY_ARCHER;

                        break;
                    }
                } else {
                    // Our maximum possible, insert before if needed
                    switch (random(9)) {
                    case 0:
                        person = FAMILY_ELF;

                        break;
                    case 1:
                        person = FAMILY_SOLDIER;

                        break;
                    case 2:
                        person = FAMILY_ARCHER;

                        break;
                    case 3:
                        person = FAMILY_ORC;

                        break;
                    case 4:
                        person = FAMILY_SKELETON;

                        break;
                    case 5:
                        person = FAMILY_DRUID;

                        break;
                    case 6:
                        person = FAMILY_CLERIC;

                        break;
                    case 7:
                        person = FAMILY_FIRE_ELEMENTAL;

                        break;
                    case 8:
                        person = FAMILY_BIG_ORC;

                        break;
                    default:
                        person = FAMILY_ARCHER;
                    }
                }

                // Now make the guy we'd summon, at least physically
                newob = myscreen->level_data.add_ob(ORDER_LIVING, person);

                if (!newob) {
                    // Failsafe
                    return 0;
                }

                // We need to check for a space around the ArchMage
                // This means we have or haven't found room
                generic = 0;

                for (i = -1; i <= 1; ++i) {
                    for (j = -1; j <= 1; ++j) {
                        if (((i == 0) && (j == 0)) || generic) {
                            continue;
                        }

                        if (myscreen->query_passable(xpos + ((newob->sizex + 1) * i), ypos + ((newob->sizey + 1) * j), newob)) {
                            // We've found a legal spot...
                            generic = 1;
                            newob->setxy(xpos + ((newob->sizex + 1) * i),
                                         ypos + ((newob->sizey + 1) * j));

                            newob->stats->level = (stats->level + 2) / 3;
                            newob->set_difficulty(newob->stats->level);
                            // Set to our team
                            newob->team_num = team_num;
                            // We're owned!
                            newob->owner = this;
                            newob->lifetime = 100 + (20 * stats->level);
                            // newob->stats->armor = -(newob->stats->max_hitpoints * 10);
                            newob->stats->max_hitpoints = 1;
                            newob->stats->hitpoints = 0;
                            newob->stats->armor = 0;
                            // Just to help out...
                            newob->foe = foe;
                            // We're magical
                            newob->stats->set_bit_flags(BIT_MAGICAL, 1);
                            newob->stats->name = "Phantom";

                            // End of successfully put summoned creature-image
                        }
                    }

                    // End of I and J loops
                }

                // We never found a legal spot
                if (!generic) {
                    newob->dead = 1;

                    return 0;
                }

                // Takes lots of time :)
                busy += 15;

                // End of summon illusion
            }

            // End of summoning/illusion cases
            break;
        case 4:
            // Mind-control enemies
            if (busy > 0) {
                return 0;
            }

            {
                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 80 + (4 * stats->level), &howmany, this);

                if (howmany < 1) {
                    // No one to influence
                    return 0;
                }

                // howmany actually done yet?
                didheal = 0;
                generic2 = (stats->magicpoints - stats->special_cost[static_cast<Sint32>(current_special)]) + 10;

                for (auto & ob : newlist) {
                    if ((ob->real_team_num == 200) // Never been charmed
                        && (ob->query_order() == ORDER_LIVING) // Alive
                        && /* Not too charmed */ (ob->charm_left <= 10)) {
                        // Count cost for additional guy
                        generic2 -= 10;
                        generic = stats->level - ob->stats->level;

                        // Trying to control a higher-level
                        if ((generic < 0) || !random(20)) {
                            ob->real_team_num = ob->team_num;
                            ob->team_num = random(8);
                            ob->charm_left = 25 + random(generic * 20);
                        } else {
                            ob->real_team_num = ob->team_num;
                            ob->team_num = team_num;
                            // Allow choice of new foe
                            ob->foe = nullptr;
                            ob->charm_left = 25 + random(generic * 20);
                        }

                        ++didheal;
                    }
                }
            }

            // Didn't actually get anyone?
            if (!didheal) {
                return 0;
            }

            // Notify screen of our action
            // Do we have an NPC name?
            if (!stats->name.empty()) {
                buf << stats->name;
            } else  if (myguy && !myguy->name.empty()) {
                buf << myguy->name;
            } else {
                buf << "ArchMage";
            }

            buf << " has controlled " << didheal << " men";
            message = buf.str();
            buf.clear();
            message.resize(80);
            myscreen->do_notify(message, this);
            generic2 = stats->magicpoints - stats->special_cost[static_cast<Sint32>(current_special)];

            // Sap our extra based on how many guys
            if (generic2 > 0) {
                while ((didheal > 0) && (generic2 >= 10)) {
                    // 10 is cose of each additional guy
                    if (generic2 > 10) {
                        generic2 -= 10;
                    }

                    --didheal;
                }

                // End of extra-cost sapping
            }

            // Takes a while
            busy += 10;

            // End of Mind control
            break;
        default:

            break;
        }

        // End of ArchMage
        break;
    case FAMILY_FIRE_ELEMENTAL:
        switch (current_special) {
        case 1: // Lots o' fireballs
        case 2:
        case 3:
        case 4:
        default:
            // Store our facing
            tempx = lastx;
            tempy = lasty;

            // Now face each directio nand fire...
            stats->magicpoints += (8 * stats->weapon_cost);

            for (i = -1; i < 2; ++i) {
                for (j = -1; j < 2; ++j) {
                    if (i || j) {
                        lastx = i;
                        lasty = j;
                        fire();
                    }
                }
            }

            // Restore old facing
            lastx = tempx;
            lasty = tempy;

            break;
        }

        // End of fire elemental
        break;
    case FAMILY_SMALL_SLIME: // Grow...
    case FAMILY_MEDIUM_SLIME:
        // Room to grow?
        if (spaces_clear() > 7) {
            if (query_family() == FAMILY_SMALL_SLIME) {
                transform_to(ORDER_LIVING, FAMILY_MEDIUM_SLIME);
            } else {
                transform_to(ORDER_LIVING, FAMILY_SLIME);
            }
        } else {
            stats->set_command(COMMAND_WALK, 10, random(3) - 1, random(3) - 1);

            return 0;
        }

        break;
    case FAMILY_SLIME:
        // Big slime splits to two small slimes
        ani_type = ANI_SLIME_SPLIT;
        cycle = 0;

        break;
    case FAMILY_GHOST:
        // Do nifty scare thing
        newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_GHOST_SCARE); // ,1 == underneath
        newob->ani_type = ANI_SCARE;
        newob->setxy((xpos + (sizex / 2)) - (newob->sizex / 2),
                     (ypos + (sizey / 2)) - (newob->sizey / 2));

        newob->owner = this;
        newob->stats->level = stats->level;
        // So we scare OTHER teams
        newob->team_num = team_num;
        // Actual scare effect done in scare's "death" in effect

        break;
    case FAMILY_THIEF:
        switch (current_special) {
        case 1:
            // Drop a bomb, unregistered
            newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_BOMB, 1); // 1 == underneath
            newob->ani_type = ANI_BOMB;

            if (myguy) {
                ++myguy->total_shots;
                ++myguy->scen_shots;
            }

            newob->damage = (stats->level + 1) * 15;
            newob->setxy((xpos + (sizex / 2)) - (newob->sizex / 2),
                         (ypos + (sizey / 2)) - (newob->sizey / 2));

            newob->owner = this;
            // Run away if we're AI
            person = 0;

            for (i = 0; i < myscreen->numviews; ++i) {
                if (myscreen->viewob[i]->control == this) {
                    person = 1;
                }
            }

            if (!person) {
                tempx = random(3) - 1;
                tempy = random(3) - 1;

                if ((tempx == 0) && (tempy == 0)) {
                    tempx = 1;
                }

                stats->force_command(COMMAND_WALK, 20, tempx, tempy);
            }

            break;
        case 2:
            // Thief cloaking ability, Registered
            invisibility_left += (20 + (random(20) * stats->level));

            break;
        case 3:
            // Thief taunt (draw enemies), Registered
            // Normal taunt
            if (!shifter_down) {
                if (busy > 0) {
                    return 0;
                }

                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 80 + (4 * stats->level), &howmany, this);

                for (auto & ob : newlist) {
                    if (ob && (random(stats->level) >= random(ob->stats->level))) {
                        // Set our enemy's foe to us...
                        ob->foe = this;
                        // A hack, yeah
                        ob->leader = this;

                        if (ob->query_act_type() != ACT_CONTROL) {
                            ob->stats->force_command(COMMAND_FOLLOW, 10 + random(stats->level), 0, 0);
                        }
                    }
                }

                if (myguy) {
                    buf << myguy->name;
                } else if (!stats->name.empty()) {
                    buf << stats->name;
                } else {
                    buf << "THIEF";
                }

                buf << ": 'Nyah Nyah!'";
                message = buf.str();
                buf.clear();
                message.resize(80);
                myscreen->do_notify(message, this);
                busy += 2;

                // End of taunt
            } else {
                // Charm opponent
                if (busy > 0) {
                    return 0;
                }

                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 16 + (4 * stats->level), &howmany, this);

                if (howmany < 1) {
                    // no one to influence
                    return 0;
                }

                // Howmany actually done yet?
                didheal = 0;

                for (auto & ob : newlist) {
                    if ((ob->real_team_num == 255) // Never been charmed
                        && (ob->query_order() == ORDER_LIVING) // Alive
                        && /* Not too charmed */ 1 /* (ob->charm_left <= 10) */) {
                        generic = stats->level - ob->stats->level;

                        // Trying to control a higher-level
                        if ((generic < 0) || !random(20)) {
                            // Enemy gets free attack...
                            ob->foe = this;
                            ob->attack(this);
                            generic2 = 1;
                        } else {
                            ob->real_team_num = ob->team_num;
                            ob->team_num = team_num;

                            if (foe == ob) {
                                ob->foe = nullptr;
                            } else {
                                ob->foe = foe;
                            }

                            ob->charm_left = 75 + (generic * 25);
                            generic2 = 0;
                        }

                        ++didheal;

                        // End of if-valid-target
                    }

                    // End of until-got-target loop
                }

                if (!didheal) {
                    return 0;
                }

                // Notify screen of our action
                // Do we have an NPC name?
                if (!stats->name.empty()) {
                    buf << stats->name;
                } else if (myguy && !myguy->name.empty()) {
                    buf << myguy->name;
                } else {
                    buf << "Thief";
                }

                // Then we actually failed to charm
                if (generic2) {
                    buf << " failed to charm!";
                } else {
                    buf << " charmed an opponent!";
                }

                message = buf.str();
                buf.clear();
                message.resize(80);
                myscreen->do_notify(message, this);
                // Takes a while
                busy += 10;

                // End of Charm Opponent
            }

            break;
        case 4: // Throw poison cloud
        default:
            if (busy > 0) {
                return 0;
            }

            newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_CLOUD);

            if (!newob) {
                // Failsafe
                return 0;
            }

            busy += 5;
            newob->ignore = 1;
            newob->lifetime = 40 + (3 * stats->level);
            newob->center_on(this);
            newob->invisibility_left = 10;
            // Non-walking
            newob->ani_type = ANI_SPIN;
            newob->team_num = team_num;
            newob->stats->level = stats->level;
            newob->damage = stats->level;
            newob->owner = this;

            break;
        }

        break;
    case FAMILY_ELF:
        switch (current_special) {
        case 1:
            // Some rocks (norma)
            stats->magicpoints += (2 * stats->weapon_cost);
            fireob = static_cast<Weap *>(fire());

            if (!fireob) {
                // Failsafe
                return 0;
            }

            fireob->lastx *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            fireob->lasty *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            fireob= static_cast<Weap *>(fire());

            if (!fireob) {
                // Failsafe
                return 0;
            }

            fireob->lastx *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            fireob->lasty *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));

            break;
        case 2:
            // More rocks, and bouncing
            stats->magicpoints += (3 * stats->weapon_cost);

            for (i = 0; i < 2; ++i) {
                fireob = static_cast<Weap *>(fire());

                if (!fireob) {
                    // Failsafe
                    return 0;
                }

                // We get 50% longer, too!
                fireob->lineofsight *= 3;
                fireob->lineofsight /= 2;
                fireob->do_bounce = 1;
                fireob->lastx *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
                fireob->lasty *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            }

            break;
        case 3:
            stats->magicpoints += (4 * stats->weapon_cost);

            for (i = 0; i < 3; ++i) {
                fireob = static_cast<Weap *>(fire());

                if (!fireob) {
                    // Failsafe
                    return 0;
                }

                // Get double distance
                fireob->lineofsight *= 2;
                fireob->do_bounce = 1;
                fireob->lastx *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
                fireob->lasty *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            }

            break;
        case 4:
        default:
            stats->magicpoints += (5 * stats->weapon_cost);

            for (i = 0; i < 4; ++i) {
                fireob = static_cast<Weap *>(fire());

                if (!fireob) {
                    // Failsafe
                    return 0;
                }

                // We get 150% longer, too!
                fireob->lineofsight *= 5;
                fireob->lineofsight /= 2;
                fireob->do_bounce = 1;
                fireob->lastx *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
                fireob->lasty *= (0.8f + ((0.4f * (rand() % 101)) / 100.0f));
            }

            break;
        }

        break;
    case FAMILY_DRUID:
        switch (current_special) {
        case 1:
            // Plant tree
            if (busy > 0) {
                return 0;
            }

            stats->magicpoints += stats->weapon_cost;
            newob = fire();

            if (!newob) {
                return 0;
            }

            busy += (fire_frequency * 2);
            alive = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_TREE);
            alive->setxy(newob->xpos, newob->ypos);
            alive->team_num = team_num;
            alive->ani_type = ANI_GROW;
            alive->owner = this;
            newob->dead = 1;

            break;
        case 2:
            // Summon faerie
            if (busy > 0) {
                return 0;
            }

            stats->magicpoints += stats->weapon_cost;
            newob = fire();

            if (!newob) {
                return 0;
            }

            alive = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_FAERIE);
            alive->setxy(newob->xpos, newob->ypos);
            alive->team_num = team_num;
            alive->owner = this;
            alive->lifetime = 50 + (stats->level * 40);
            newob->dead = 1;

            if (!myscreen->query_passable(alive->xpos, alive->ypos, alive)) {
                alive->dead = 1;

                return 0;
            }

            busy += (fire_frequency * 3);

            break;
        case 3:
            // Reveal items
            if (busy > 0) {
                return 0;
            }

            view_all += (stats->level * 10);
            busy += (fire_frequency * 4);

            break;
        case 4: // Circle of protection
        default:
            if (busy > 0) {
                return 0;
            }

            {
                std::list<Walker *> newlist = myscreen->find_friends_in_range(myscreen->level_data.oblist, 60, &howmany, this);

                didheal = 0;

                // Some friends here...
                if (howmany > 1) {
                    // Log(Found %d friends\n", howmany - 1);
                    for (auto const & newob : newlist) {
                        // Not four ourselves
                        if (newob != this) {
                            // First see if this person already has protection (slow)
                            tempwalk = nullptr;

                            for (auto & ob : myscreen->level_data.oblist) {
                                // Found a circle already on newob...
                                if (ob
                                    && (ob->owner == newob)
                                    && (ob->query_order() == ORDER_WEAPON)
                                    && (ob->query_family() == FAMILY_CIRCLE_PROTECTION)) {
                                    tempwalk = ob;

                                    break;
                                }
                            }

                            // Target wasn't protected yet
                            if (!tempwalk) {
                                alive = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION);

                                // Failed somehow
                                if (!alive) {
                                    return 0;
                                }

                                alive->owner = newob;
                                alive->center_on(newob);
                                alive->team_num = newob->team_num;
                                alive->stats->level = newob->stats->level;
                                ++didheal;

                                // End of target wasn't protected
                            } else {
                                alive = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION);

                                // Failed somehow
                                if (!alive) {
                                    return 0;
                                }

                                tempwalk->stats->hitpoints += alive->stats->hitpoints;
                                alive->dead = 1;
                                ++didheal;

                                // TODO: Should we show healing numbers here?

                                // End of target WAS protected
                            }

                            // Get experience either way
                            if (myguy) {
                                myguy->exp += exp_from_action(EXP_PROTECTION, this, newob, 0);
                            }

                            // End of did one guy
                        }

                        // End of cycling through guys
                    }

                    if (!didheal) {
                        // Everyone was okay; don't charge us...
                        return 0;
                    } else {
                        // Inform screen/view to print a message...
                        if (didheal == 1) {
                            buf << "Druid protected 1 man!";
                        } else {
                            buf << "Druid protected " << didheal << " men!";
                        }

                        message = buf.str();
                        buf.clear();
                        message.resize(80);

                        // Home team
                        if ((team_num == 0) || myguy) {
                            myscreen->do_notify(message, this);
                        }

                        // Play sound...
                        if (on_screen()) {
                            myscreen->soundp->play_sound(SOUND_HEAL);
                        }

                        // End of did protect guys case
                    }

                    // End of checking for friends
                } else {
                    // No friends, so don't charge us
                    return 0;
                }
            }

            break;
            // End of druid's specials...
        }

        // End of switch on druid case
        break;
    case FAMILY_ORC:
        // Registered monster
        switch (current_special) {
        case 1:
            // Yell and "freeze" foes
            if (busy > 0) {
                return 0;
            }

            busy += 2;

            {
                std::list<Walker *> newlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, 160 + (20 * stats->level), &howmany, this);

                for (auto & ob : newlist) {
                    if (ob) {
                        if (ob->myguy) {
                            tempx = ob->myguy->constitution;
                        } else {
                            tempx = ob->stats->hitpoints / 30;
                        }

                        tempy = (10 + random(stats->level * 10)) - random(tempx * 10);

                        if (tempy < 0) {
                            tempy = 0;
                        }

                        ob->stats->frozen_delay += tempy;
                    }
                }

                if (on_screen()) {
                    myscreen->soundp->play_sound(SOUND_ROAR);
                }
            }

            break;
        case 2: // Eat corpse for health
        case 3:
        case 4:
        default:
            if (stats->hitpoints >= stats->max_hitpoints) {
                // Can't eat if we're "full"
                return 0;
            }

            newob = myscreen->find_nearest_blood(this);

            if (!newob) {
                // No blood, so do nothing
                return 0;
            }

            distance = static_cast<Uint32>(distance_to_ob_center(newob));

            // Must be close enough
            if (distance > 24) {
                return 0;
            }

            stats->hitpoints += (newob->stats->level * 5);
            do_heal_effects(nullptr, this, newob->stats->level * 5);

            // Print the eating notice
            if (myguy) {
                myguy->exp += exp_from_action(EXP_EAT_CORPSE, this, newob, 0);
                buf << myguy->name;
            } else if (!stats->name.empty()) {
                buf << stats->name;
            } else {
                buf << "Orc";
            }

            buf << " ate a corpse";

            message = buf.str();
            buf.clear();
            message.resize(80);

            if (!cfg.is_on("effects", "heal_numbers")) {
                myscreen->do_notify(message, this);
            }

            if (stats->hitpoints > stats->max_hitpoints) {
                stats->hitpoints = stats->max_hitpoints;
            }

            newob->dead = 1;
            newob->death();

            // End of eat corpse
            break;
        }

        // End of orc case
        break;
    case FAMILY_SKELETON:
        switch (current_special) {
        case 1: // Tunnel
        case 2:
        case 3:
        case 4:
        default:
            if ((ani_type == ANI_TELE_OUT) || (ani_type == ANI_TELE_IN)) {
                return 0;
            }

            ani_type = ANI_TELE_OUT;
            cycle = 0;

            // End of tunnel case
            break;
        }

        // End of skeleton case
        // End of Skeleton
        break;
    case FAMILY_BARBARIAN:
        switch (current_special) {
        case 1: // Hurl Boulder
        case 2: // Exploding Boulder
        case 3:
        case 4:
            if (busy > 0) {
                return 0;
            }

            newob = fire();

            if (!newob) {
                // Failed somehow? !?!
                return 0;
            }

            alive = myscreen->level_data.add_ob(ORDER_WEAPON, FAMILY_BOULDER);
            alive->center_on(newob);
            alive->owner = this;
            alive->stats->level = stats->level;
            alive->lastx = newob->lastx;
            alive->lasty = newob->lasty;

            // Set our boulder's speed and extra damage...
            if (myguy) {
                alive->stepsize = 1.0f + (myguy->strength / 7);
                alive->damage += (myguy->strength / 5.0f);
            } else {
                alive->stepsize = stats->level * 2;
                alive->damage += stats->level;
            }

            alive->stepsize = std::max(alive->stepsize, 1.0f);
            alive->stepsize = std::min(alive->stepsize, 15.0f);

            if (alive->lasty > 0) {
                alive->lasty = alive->stepsize;
            } else if (alive->lasty < 0) {
                alive->lasty = -alive->stepsize;
            }

            if (alive->lastx > 0) {
                alive->lastx = alive->stepsize;
            } else if (alive->lastx < 0) {
                alive->lastx = -alive->stepsize;
            }

            // If we're on "exploding boulder", then make it explode on impact.
            if (current_special == 2) {
                // Signify exploding
                alive->skip_exit = 5000;
            } else {
                alive->skip_exit = 0;
            }

            newob->dead = 1;
            busy += (1 + (current_special * 5));

            // End of hurl boulder
            break;
        }

        // End of Barbarian
        break;

        // End of family switch
    }

    stats->magicpoints -= stats->special_cost[static_cast<Sint32>(current_special)];

    return 0;
}

bool Walker::teleport()
{
    Sint16 newx;
    Sint16 newy;
    Sint32 distance;

    // First check to see if we have a marker to go to
    // NOTE: It must be a bit away from us...
    for (auto & ob : myscreen->level_data.oblist) {
        if (ob
            && (ob->query_order() == ORDER_FX)
            && (ob->query_family() == FAMILY_MARKER)
            && (ob->owner == this)
            && !ob->dead) {
            // Found our marker!
            distance = distance_to_ob(ob);
            if (myscreen->query_passable(ob->xpos, ob->ypos, this) && (distance > 64)) {
                center_on(ob);
                --ob->lifetime;

                if (ob->lifetime < 1) {
                    ob->dead = 1;
                    ob->death();
                }

                return 1;

                // End of successful transport
            } else {
                // Blocked somehow?
                // Only tell players
                if ((user != -1) && (distance > 64)) {
                    myscreen->do_notify("Marker is Blocked!", this);
                }
            }
        }

        // End of checking for marker (we failed)
    }

    newx = random(myscreen->level_data.grid.w) * GRID_SIZE;
    newy = random(myscreen->level_data.grid.h) * GRID_SIZE;

    while (!myscreen->query_passable(newx, newy, this)) {
        newx = random(myscreen->level_data.grid.w) * GRID_SIZE;
        newy = random(myscreen->level_data.grid.h) * GRID_SIZE;
    }

    setxy(newx, newy);

    return true;
}

bool Walker::teleport_ranged(Sint32 range)
{
    Sint16 newx;
    Sint16 newy;
    // Maxtries
    Sint16 keep_going = 200;

    newx = (random(2 * range) - range) + xpos;
    newy = (random(2 * range) - range) + ypos;

    while (!myscreen->query_passable(newx, newy, this) && keep_going) {
        newx = (random(2 * range) - range) + xpos;
        newy = (random(2 * range) - range) + ypos;
        --keep_going;
    }

    if (keep_going) {
        setxy(newx, newy);

        return true;
    }

    // Failed to find a safe spot
    return false;
}

// Turns undead; i.e., skeleton or ghost, within range
// Returns the number of dead destroyed
Sint32 Walker::turn_undead(Sint32 range, Sint32 power)
{
    Sint32 killed = 0;
    Sint16 targets;

    std::list<Walker *> deadlist = myscreen->find_foes_in_range(myscreen->level_data.oblist, range, &targets, this);

    if (!targets) {
        return -1;
    }

    for (auto & w : deadlist) {
        if (w && ((w->query_family() == FAMILY_SKELETON) || (w->query_family() == FAMILY_GHOST))) {
            if (random(range * 40) > random(w->stats->level * 10)) {
                w->dead = 1;
                w->stats->hitpoints = 0;
                // w->death();
                // To generate bloodspot, etc.
                attack(w);
                ++killed;
            }
        }
    }

    return killed;
}

// ****************************
// MONSTER INTELLIGENC ROUTINES
// ****************************

// Basically, we check a direction for foes. If we find one, we init_fire.
// If not, we do nothing. init_fire will take care of turning us if we
// need it.
Sint16 Walker::fire_check(Sint16 xdelta, Sint16 ydelta)
{
    Walker *weapon = nullptr;
    // Sint16 newx = 0;
    // Sint16 newy = 0;
    Sint16 i;
    Sint16 loops;
    Sint16 xdir = 0;
    Sint16 ydir = 0;
    Sint32 distance;
    Sint16 targetdir;

    // Allow generators to "always" succeed
    if (order == ORDER_GENERATOR) {
        return 1;
    }

    weapon = create_weapon();

    if (!weapon) {
        return 0;
    }

    // Set lastx, lasty based on our facing...
    set_weapon_heading(weapon);
    weapon->collide_ob = nullptr;

    // Based on facing, we alter the weapon's proposed size so the collison
    // check is fooled into checking a Sint32 strip equal to the lineofsight
    // times the size of a weapon.
    // Nobody to fire at?
    if (!foe) {
        // Log("fire check, no foe.\n");
        // This does happen! But it appears harmless

        return 0;
    }

    if (stats->query_bit_flags(BIT_NO_RANGED)) {
        weapon->dead = 1;

        return 0;
    }

    if (stats->weapon_cost > stats->magicpoints) {
        weapon->dead = 1;

        return 0;
    }

    distance = distance_to_ob(foe);
    if (distance > static_cast<Sint32>(static_cast<Sint32>(weapon->stepsize) * static_cast<Sint32>(weapon->lineofsight))) {
        weapon->dead = 1;

        return 0;
    }

    targetdir = facing(xdelta, ydelta);
    if (targetdir != curdir) {
        // turn(targetdir);
        weapon->dead = 1;

        return 0;
    }

    if (xdelta != 0) {
        xdir = xdelta / abs(xdelta);
    }

    if (ydelta != 0) {
        ydir = ydelta / abs(ydelta);
    }

    /*
     * // Why are we assuming walls don't matter in these two cases?
     * // Aligned on major axis
     * if (!xdelta || !ydelta) {
     *     weapon->dead = 1;
     *
     *     return 1;
     * }
     *
     * if (abs(abs(xdelta) - abs(ydelta)) < 3) {
     *     weapon->dead = 1;
     *
     *     return 1;
     * } else {
     *     weapon->dead = 1;
     *
     *     return 0;
     * }
     */

    // Run weapon through where it would go it all went well...
    for (i = 0; i < weapon->lineofsight; ++i) {
        weapon->setxy(weapon->xpos + (i * weapon->lastx),
                      weapon->ypos + (i * weapon->lasty));

        if (!myscreen->query_grid_passable(weapon->xpos, weapon->ypos, weapon)) {
            // We hit a wall, so fail
            weapon->dead = 1;

            return 0;
        }

        if (!myscreen->query_object_passable(weapon->xpos, weapon->ypos, weapon)) {
            // We hit an enemy, so good!
            weapon->dead = 1;

            return 1;
        }
    }

    // By this point, we should have won or lost...fail if we went our range
    // and didn't hit anyone...
    weapon->dead = 1;

    return 0;

    // Detemine number of loops to look for guys
    if (abs(xdelta) > abs(ydelta)) {
        loops = abs(xdelta);
    } else {
        loops = abs(ydelta);
    }

    // * 16 is to match with grid coords
    // Half a grid square
    for (i = 0; i <= loops; i += 8) {
        if (!myscreen->query_grid_passable(xpos + (i * xdir), ypos + (i * ydir), weapon)) {
            weapon->dead = 1;
            // Can't hit this guy
            // foe = nullptr;
            // stats->try_command(COMMAND_RANDOM_WALK, random(8));

            return 0;
        }
    }

    weapon->dead = 1;

    // We have a good chance of hitting, so...
    return 1;
}

/*
 * *********************
 * Act routines (static)
 * *********************
 */

bool Walker::act_generate()
{
    if ((myscreen->level_data.numobs < MAXOBS)
        && (random(stats->level * 3) > random(300 + (myscreen->level_data.numobs * 8)))) {
        lastx = 1 - random(3);
        lasty = 1 - random(3);

        if (!lastx && !lasty) {
            lastx = 1;
        }

        init_fire(lastx, lasty);
        // lastx = 0;
        // lasty = 0;
        ++stats->hitpoints;

        if (stats->hitpoints > stats->max_hitpoints) {
            --stats->hitpoints;
        }
    }

    return true;
}

bool Walker::act_fire()
{
    --lineofsight;

    // This is the range of the weapon
    if (!(lineofsight + 1)) {
        dead = 1;
        death();
    } else if (!walk() || stats->query_bit_flags(BIT_NO_COLLIDE)) {

        // Hit the collide_ob
        if (collide_ob && !collide_ob->dead) {
            attack(collide_ob);
        }

        if (!stats->query_bit_flags(BIT_IMMORTAL)) {
            dead = 1;
            death();
        }
    }

    return true;
}

bool Walker::act_guard()
{
    // Check all directions for foes
    // If we find one, fire
    // if (fire_check(lastx, lasty)
    //     || fire_check(lasty, lastx)
    //     || fire_check(-lasty, -lastx)
    //     || fire_check(-lastx, -lasty))
    foe = myscreen->find_near_foe(this);

    if (foe) {
        curdir = static_cast<Uint8>(facing(foe->xpos - xpos, foe->ypos - ypos));
        stats->try_command(COMMAND_FIRE, random(30));

        return true;
    }

    return false;
}

Sint16 Walker::act_random()
{
    Sint16 newx;
    Sint16 newy;
    Sint16 xdist;
    Sint16 ydist;

    // Specifically put in to attempt to make enemy harder
    // if (random(sizex / GRID_SIZE)) {
    //     return 0;
    // }

    // Find our foe
    if (!random(70) || !foe) {
        foe = myscreen->find_far_foe(this);
    }

    if (!foe) {
        return stats->try_command(COMMAND_RANDOM_WALK, 20);
    }

    xdist = foe->xpos - xpos;
    ydist = foe->ypos - ypos;

    // If foe is in firing range, turn and fire
    if ((abs(xdist) < (lineofsight * GRID_SIZE)) && (abs(ydist) < (lineofsight * GRID_SIZE))) {
        if (fire_check(xdist, ydist)) {
            init_fire(xdist, ydist);
            stats->set_command(COMMAND_FIRE, random(24), xdist, ydist);

            return 1;
        } else {
            // Nearest foe is blocked
            // foe = nullptr;
            turn(facing(xdist, ydist));
        }
    }

    // Otherwise, try to walk toward foe
    newx = 0;
    newy = 0;

    if (foe) {
        // total horizontal distance...
        newx = xdist;

        // If it's not 0, then get the normal of it...
        if (newx) {
            newx = newx / abs(newx);
        }

        newy = ydist;

        if (newy) {
            newy = newy / abs(newy);
        }

        // End of if we had a foe...
    } else {
        while (!newx && !newy) {
            // Walk in some random direction other than 0,0 :)
            newx = 1 - random(3);
            newy = 1 - random(3);
        }
    }

    // If blocked
    collide_ob = nullptr;

    // We can slide now, so always just walkstep, NOT using stepsize...
    return walkstep(newx, newy);

    // return 1;
}

// Returns the spaces "clear" around us, out of a maximum of eight...
Sint16 Walker::spaces_clear()
{
    Sint16 count = 0;
    Sint16 i;
    Sint16 j;

    for (i = -1; i < 2; ++i) {
        for (j = -1; j < 2; ++j) {
            // Don't check our own location
            if (i || j) {
                if (myscreen->query_passable(xpos + (i * sizex), ypos + (j * sizey), this)) {
                    ++count;
                }
            }
        }
    }

    return count;
}

void Walker::transfer_stats(Walker *newob)
{
    Guy *newguy;
    Sint16 i;

    // First do the "stats" stuff...
    newob->stats->hitpoints = stats->hitpoints;
    newob->stats->max_hitpoints = stats->max_hitpoints;
    newob->stats->heal_per_round = stats->heal_per_round;
    newob->stats->max_heal_delay = stats->max_heal_delay;

    // Magic...
    newob->stats->magicpoints = stats->magicpoints;
    newob->stats->max_magicpoints = stats->max_magicpoints;
    newob->stats->magic_per_round = stats->magic_per_round / 2;
    newob->stats->max_magic_delay = stats->max_magic_delay;

    newob->stats->level = stats->level;
    newob->stats->frozen_delay = stats->frozen_delay;

    for (i = 0; i < 5; ++i) {
        newob->stats->special_cost[i] = stats->special_cost[i];
    }

    newob->stats->weapon_cost = stats->weapon_cost;

    newob->stats->bit_flags = stats->bit_flags;
    newob->stats->delete_me = stats->delete_me;

    // Do we have a "guy"?
    if (myguy) {
        newguy = new Guy();
        newguy->name = myguy->name;
        newguy->strength = myguy->strength;
        newguy->constitution = myguy->constitution;
        newguy->dexterity = myguy->dexterity;
        newguy->intelligence = myguy->intelligence;
        newguy->set_level_number(myguy->get_level());
        newguy->armor = myguy->armor;
        newguy->exp = myguy->exp;

        // "Kill-stats"
        newguy->kills = myguy->kills;
        newguy->level_kills = myguy->level_kills;
        newguy->total_damage = myguy->total_damage;
        newguy->total_hits = myguy->total_hits;
        newguy->total_shots = myguy->total_shots;

        newguy->scen_damage = myguy->scen_damage;
        newguy->scen_kills = myguy->scen_kills;
        newguy->scen_damage_taken = myguy->scen_damage_taken;
        newguy->scen_min_hp = myguy->scen_min_hp;
        newguy->scen_shots = myguy->scen_shots;
        newguy->scen_hits = myguy->scen_hits;

        newob->myguy = newguy;
    }
}

// Change picture, etc. but NOT stats (use transfer_stats for that)
void Walker::transform_to(Uint8 whatorder, Uint8 whatfamily)
{
    PixieData data;
    Sint16 xcenter;
    Sint16 ycenter;
    Sint16 tempxpos;
    Sint16 tempypos;
    Sint16 reset = 0;
    Sint16 tempact = query_act_type();

    // First remove us from the collison table...
    myobmap->remove(this);

    // Same object type
    if (order == whatorder) {
        reset = 1;
        tempact = query_act_type();
    }

    // Reset bit flags
    stats->clear_bit_flags();

    // Do this before restting graphic so illegal family values don't try to
    // set graphics. order and family are only set if legal
    myscreen->set_walker(this, whatorder, whatfamily);

    // Reset the graphics
    data = myscreen->level_data.myloader->graphics[PIX(order, family)];
    facings = data.data;
    bmp = data.data;
    frames = data.frames;
    frame = 0;
    cycle = 0;

    // Deal with resisng and centering...
    xcenter = xpos + (sizex / 2);
    ycenter = ypos + (sizey / 2);

    sizex = data.w;
    sizey = data.h;
    size = sizex * sizey;

    tempxpos = xcenter - (sizex / 2);
    tempypos = ycenter - (sizey / 2);

    if (reset) {
        set_act_type(tempact);
    }

    // Automatically re-adds us to the list...
    setxy(tempxpos, tempypos);
    // set_frame(ani[curdir + (ani_type * NUM_FACINGS)][cycle]);

    // Don't manually set the frame here -- it can break circles of protection,
    // etc. which are special cases...instead:
    set_frame(0);

    animate();
}

// Death is called when an object dies (or weapon destructed, etc.) for
// special effects...
Sint16 Walker::death()
{
    // Note that the "dead" variable shoud ALREADY be set by the time this
    // function is called, so that we can easily reverse the decision :)
    Walker *newob = nullptr;
    Sint32 i;

    if (death_called) {
        return 0;
    }

    death_called = 1;

    // Were we a real character? Then make a heart...
    if (myguy) {
        newob = myscreen->level_data.add_ob(ORDER_TREASURE, FAMILY_LIFE_GEM, 1);
        newob->stats->hitpoints = myguy->query_heart_value();
        // 75%, divided by 2, since socre is doubled at end of level
        newob->stats->hitpoints *= (0.75 / 2);
        newob->team_num = team_num;
        newob->center_on(this);
    }

    switch (order) {
    case ORDER_LIVING:
        if (((team_num == 0) || myguy) // ourteam
            && (myscreen->level_data.type & SCEN_TYPE_SAVE_ALL)
            && /* We were named */ !stats->name.empty()) {

            // Failed
            return myscreen->endgame(SCEN_TYPE_SAVE_ALL);
        }

        switch (family) {
        case FAMILY_FIRE_ELEMENTAL:
            // Make us explode
            dead = 0;
            stats->magicpoints += stats->special_cost[1];
            special();
            dead = 1;

            break;
        case FAMILY_SLIME:
            // Shrink to medium
            dead = 1;
            // transform_to(ORDER_LIVING, FAMILY_MEDIUM_SLIME);
            newob = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_MEDIUM_SLIME);
            newob->team_num = team_num;
            newob->stats->level = stats->level;
            newob->set_difficulty(stats->level);
            newob->foe = foe;
            newob->leader = leader;

            if (!stats->name.empty()) {
                stats->name = newob->stats->name;
            }

            if (myguy) {
                newob->myguy = myguy;
                myguy = nullptr;
            }

            newob->center_on(this);
            stats->hitpoints = stats->max_hitpoints;

            break;
        case FAMILY_MEDIUM_SLIME:
            // Shrink to small...
            dead = 1;
            // transform_to(ORDER_LIVING, FAMILY_SMALL_SLIME);
            newob = myscreen->level_data.add_ob(ORDER_LIVING, FAMILY_SMALL_SLIME);
            newob->team_num = team_num;
            newob->stats->level = stats->level;
            newob->set_difficulty(stats->level);
            newob->foe = foe;
            newob->leader = leader;

            if (!stats->name.empty()) {
                stats->name = newob->stats->name;
            }

            if (myguy) {
                newob->myguy = myguy;
                myguy = nullptr;
            }

            newob->center_on(this);
            stats->hitpoints = stats->max_hitpoints;

            break;
        case FAMILY_GHOST: // Undead don't leave bloodspots...
        case FAMILY_SKELETON:
        case FAMILY_TOWER1: // Neither do towers

            break;
        default:
            generate_bloodspot();

            // End of family switch
            break;
        }

        // End of order livings case
        break;
    case ORDER_GENERATOR:
        // Go up in flames :>
        for (i = 0; i < 4; ++i) {
            newob = myscreen->level_data.add_ob(ORDER_FX, FAMILY_EXPLOSION, 1);

            if (!newob) {
                // Failsafe
                break;
            }

            newob->team_num = team_num;
            newob->stats->level = stats->level;
            newob->ani_type = ANI_EXPLODE;
            newob->setxy((xpos + random(sizex - 8)) + 4, (ypos + 4) + random(sizey - 8));
            newob->damage = stats->level * 2;
            newob->set_frame(random(3));

            if (on_screen()) {
                myscreen->soundp->play_sound(SOUND_EXPLODE);
            }
        }

        break;
    case ORDER_FX:
        // case ORDER_TREASURE:
        Log("Effect dying in walker?\n");

        // End of effect object case
        break;
    default:

        // End of switch orders
        break;
    }

    return 1;
}

// Generates bloodspot for desired walker...
void Walker::generate_bloodspot()
{
    Walker *bloodstain;
    // Uint8 *data;

    // Make permanent stain:

    // Just in case...
    dead = 1;

    bloodstain = myscreen->level_data.add_fx_ob(ORDER_TREASURE, FAMILY_STAIN);
    bloodstain->ignore = 1;
    transfer_stats(bloodstain);

    bloodstain->order = ORDER_TREASURE;
    bloodstain->family = FAMILY_STAIN;
    bloodstain->stats->old_order = order;
    bloodstain->stats->old_family = family;

    bloodstain->team_num = team_num;
    bloodstain->dead = 0;
    bloodstain->setxy(xpos, ypos);
    // data = myscreen->myloader->graphics[PIX(ORDER_TREASURE, FAMILY_STAIN)];
    // We can't select other "bloodspot" frames, because set_frame appears to
    // check the order and family and reset our picture to a living guy...
    // we need to find a way around this...
    // Has no effect yet...
    bloodstain->set_frame(random(4));
    bloodstain->ani_type = ANI_WALK;
    // Our image
    // bloodstain->bmp = static_cast<Uint8 *>(data + 3);
}

Sint16 Walker::eat_me(Walker *eater)
{
    if (eater) {
        Log("EATING A NON_TREASURE!\n");
    }

    return 0;
}

void Walker::set_direct_frame(Sint16 whichframe)
{
    PixieData data;
    frame = whichframe;

    data = myscreen->level_data.myloader->graphics[PIX(order, family)];
    bmp = data.data + (frame * size);
}

Walker *Walker::do_summon(Uint8 whatfamily, Uint16 lifetime)
{
    if (whatfamily || lifetime) {
        Log("Shoudl not be hitting Walker::do_summon!\n");
    }

    return nullptr;
}

Sint16 Walker::check_special()
{
    Log("Should not be hitting Walker::check_special\n");

    return 0;
}

// Center us on target walker
void Walker::center_on(Walker *target)
{
    Sint16 newx;
    Sint16 newy;

    // First get the center of our target...
    newx = target->xpos + (target->sizex / 2);
    newy = target->ypos + (target->sizey / 2);

    // Now adjust for our position...
    newx -= (sizex / 2);
    newy -= (sizey / 2);

    // Now set our position...
    setxy(newx, newy);
}

void Walker::set_difficulty(Uint32 whatlevel)
{
    Uint32 temp;
    Uint32 dif1;

    dif1 = difficulty_level[current_difficulty];

    switch (order) {
    case ORDER_GENERATOR:
        temp = 100 * whatlevel;
        temp = (temp * dif1) / 100;
        stats->hitpoints = temp;

        break;
    default:
        // Adjust standard settings for the rest...
        // Do all EXCEPT player characters
        if (team_num != 0) {
            stats->max_hitpoints = (stats->max_hitpoints * dif1) / 100.0f;
            stats->max_magicpoints = (stats->max_magicpoints * dif1) / 100.0f;
            damage = (damage * dif1) / 100.0f;
        }

        break;
    }
}

Sint32 Walker::distance_to_ob(Walker *target)
{
    // Sint32 xdelta;
    // Sint32 ydelta;

    // xdelta = static_cast<Sint32>(target->xpos - xpos) + static_cast<Sint32>((target->sizex - sizex) / 2);
    // ydelta = static_cast<Sint32>(target->ypos - ypos) + static_cast<Sint32>((target->sizey - sizey) / 2);
    // return static_cast<Sint32>((xdelta * xdelta) + (ydelta * ydelta));

    return (abs(target->xpos - xpos) + abs(target->ypos - ypos));
}

Sint32 Walker::distance_to_ob_center(Walker *target)
{
    Sint32 xdelta;
    Sint32 ydelta;

    xdelta = static_cast<Sint32>(target->xpos - xpos) + static_cast<Sint32>((target->sizex - sizex) / 2);
    ydelta = static_cast<Sint32>(target->ypos - ypos) + static_cast<Sint32>((target->sizey - sizey) / 2);

    return static_cast<Sint32>((xdelta * xdelta) + (ydelta * ydelta));
}

Uint8 Walker::query_team_color()
{
    // Debugging...
    // if (foe && !foe->dead) {
    return static_cast<Uint8>((team_num * 16) + 40);
    // } else {
    //     return static_cast<Uint8>((7 * 16) + 40);
    // }
}

bool Walker::is_friendly(Walker *target)
{
    // is_friendly determines if _target_ is "friendly" towards this walker.
    // Sint16 allied_mode;
    Sint16 has_myguy;
    Walker *headguy;
    Walker *headus;
    Walker *headtarget;

    // In case we're passed a null pointer somehow, we're always unfriendly :)
    if (target == nullptr) {
        return false;
    }

    // If either of us is dead, we're also unfriendly :)
    if (dead || target->dead) {
        return false;
    }

    // Who's the top on our chains (i.e., weapon->summoned->mage)
    // First us...
    headguy = this;

    while (headguy->owner && (headguy->owner->dead == 0) && (headguy->owner != headguy)) {
        headguy = headguy->owner;
    }

    headus = headguy;

    // Now our target...
    headguy = target;

    while (headguy->owner && (headguy->owner->dead == 0) && (headguy->owner != headguy)) {
        headguy = headguy->owner;
    }

    headtarget = headguy;

    // First, get our allied setting from screen...
    // 0 is "enemy", and non-zero is "friendly"
    // allied_mode = myscreen->allied_mode;

    // Now if we or the target don't contain a "myguy" pointer, then we don't
    // care about allied_mode, and we'll treat our state as always in "enemy"
    // mode
    if ((headtarget->myguy == nullptr) && (headus->myguy == nullptr)) {
        has_myguy = 0;
    } else if ((headtarget->myguy == nullptr) || (headus->myguy == nullptr)) {
        has_myguy = 2;
    } else {
        has_myguy = 1;
    }

    // Is allied mode set to zero (enemy)?
    // If so, then if our team numbers don't match, we are not friendly
    if ((myscreen->save_data.allied_mode == 0) || (has_myguy == 0)) {
        return (headus->team_num == headtarget->team_num);
    }

    // Allied
    if (has_myguy == 2) {
        // One person is missing a myguy points. The one with a myguy pointer
        // is owned by a player. If the other person belongs to team 0 (red),
        // then they are friendly.
        return (((headtarget->myguy == nullptr) && (headtarget->team_num == 0))
            || ((headus->myguy == nullptr) && (headus->team_num == 0)));
    }

    // If we're in "friendly" mode, then everyone with a "myguy" pointer
    // (a real, saved, character) is friendly to each other...By now we know
    // that both us and the target have myguy's, so we're friendly
    return true;
}

bool Walker::is_friendly_to_team(Uint8 team)
{
    // is_friendly_to_team determines if _team_ is "friendly"
    // towards this walker.
    // Sint16 allied_mode;
    bool has_myguy;
    Walker *headguy;
    Walker *headus;

    // If dead, we're also unfriendly :)
    if (dead) {
        return false;
    }

    // Who's the top on our chains (i.e., weapon->summoned->mage)
    // First us...
    headguy = this;

    while ((headguy->owner) && (headguy->owner->dead == 0) && (headguy->owner != headguy)) {
        headguy = headguy->owner;
    }

    headus = headguy;

    // First, get our allied setting from screen...
    // 0 is "enemy", and non-zero is "friendly"
    // allied_mode = myscreen->allied_mode;

    // Now, if we or the target don't contain a "myguy" points, then we don't
    // care aobut allied_mode, and we'll treat our state as always in "enemy"
    // mode
    if (headus->myguy == nullptr) {
        has_myguy = false;
    } else {
        has_myguy = true;
    }

    // Is allied mode set to zero (enemy) or were we not hired (!myguy)?
    // If so, then our team number must match.
    if ((myscreen->save_data.allied_mode == 0) || !has_myguy) {
        return (headus->team_num == team);
    }

    // If we're a hired guy in allied mode, then we're friendly with
    // team 0 (red)
    return (has_myguy && (team == 0));
}
