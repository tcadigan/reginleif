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

// OBMAP -- An object to handle locations of pixies on a hash table.

#include "obmap.hpp"

#include "base.hpp"
#include "text.hpp"
#include "util.hpp"
#include "video_screen.hpp"
#include "view.hpp"
#include "walker.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

bool debug_draw_obmap = false;

Sint16 ob_pass_check(Sint16 x, Sint16 y, Walker *ob, std::list<Walker *> const &pile);
Sint16 collide(Sint16 x, Sint16 y, Sint16 xsize, Sint16 ysize,
               Sint16 x2, Sint16 y2, Sint16 xsize2, Sint16 ysize2);

#define OBRES 32 // GRID_SIZE

// These are passed in as PIXEL coordinates now...
ObjectMap::ObjectMap()
{
    obmapres = OBRES;
    pos_to_walker.clear();
    walker_to_pos.clear();
}

ObjectMap::~ObjectMap()
{
}

void ObjectMap::draw()
{
    Text &t = myscreen->text_normal;
    Sint16 offsetx = myscreen->viewob[0]->topx;
    Sint16 offsety = myscreen->viewob[0]->topy;
    std::stringstream buf;

    // Draw the number of objects in each pile
    for (auto const &e : pos_to_walker) {
        Sint16 cx = (unhash(e.first.first) - offsetx) + (OBRES / 2);
        Sint16 cy = (unhash(e.first.second) - offsety) + (OBRES / 2);
        myscreen->draw_box(cx - (OBRES / 2), cy - (OBRES / 2), cx + (OBRES / 2), cy + (OBRES / 2), YELLOW, false);
        buf << e.second.size();
        t.write_xy(cx, cy, YELLOW, buf);
        buf.clear();
    }

    // Draw a box for each walker
    for (auto const &e : walker_to_pos) {
        // Get bounds
        bool unset = true;
        SDL_Rect r = { 0, 0, 1, 1 };

        for (auto const &f : e.second) {
            if (unset) {
                r.x = f.first;
                r.y = f.second;
                unset = false;

                continue;
            }

            if (f.first < r.x) {
                r.w += (r.x - f.first);
                r.x = f.first;
            }

            if (f.second < r.y) {
                r.h += (r.y - f.second);
                r.y = f.second;
            }

            if (f.first > (r.x + r.w)) {
                r.w += (f.first - (r.x + r.w));
            }

            if (f.second > (r.y + r.h)) {
                r.h += (f.second - (r.y + r.h));
            }
        }

        if (!unset) {
            // Draw the rect
            Sint16 x = unhash(r.x) - offsetx;
            Sint16 y = unhash(r.y) - offsety;
            Sint16 w = unhash(r.w);
            Sint16 h = unhash(r.h);
            myscreen->draw_box(x, y, x + w, y + h, e.first->query_team_color(), false);
        }
    }
}

size_t ObjectMap::size() const
{
    return walker_to_pos.size();
}

Sint16 ObjectMap::query_list(Walker *ob, Sint16 x, Sint16 y)
{
    Sint16 numx;
    Sint16 startnumx;
    Sint16 endnumx;
    Sint16 numy;
    Sint16 startnumy;
    Sint16 endnumy;

    if (!ob || ob->dead) {
        Log("Bad object to query_list.\n");

        return 1;
    }

    startnumx = hash(x);
    endnumx = hash(x + ob->sizex);
    startnumy = hash(y);
    endnumy = hash(y + ob->sizey);

    // For each y grid row we are in...
    for (numx = startnumx; numx <= endnumx; ++numx) {
        for (numy = startnumy; numy < endnumy; ++numy) {
            // We should be finding the same item over and over
            if (!ob_pass_check(x, y, ob, pos_to_walker[std::make_pair(numx, numy)])) {
                return 0;
            }
        }
    }

    return 1;
}

// This goes in walker's destructor
Sint16 ObjectMap::remove(Walker *ob)
{
    // Find all the instances of the object and remove them from the map
    // Get the list of position that the walker occupies
    auto e = walker_to_pos.find(ob);

    if (e != walker_to_pos.end()) {
        // For each position...
        for (auto const &f : e->second) {
            // Get the pile
            auto g = pos_to_walker.find(f);

            // Find out guy in this pile and remove him
            auto h = std::find(g->second.begin(), g->second.end(), ob);

            if (h != g->second.end()) {
                g->second.erase(h);
            }
        }

        // Erase the walker from the walker map too
        walker_to_pos.erase(e);

        return true;
    }

    return false;
}

// This goes in the walker's setxy
Sint16 ObjectMap::move(Walker *ob, Sint16 x, Sint16 y)
{
    // Do we really need to move?
    if ((x == ob->xpos) && (y == ob->ypos)) {
        return 1;
    }

    remove(ob);
    add(ob, x, y);

    return 1;
}

std::list<Walker *> &ObjectMap::obmap_get_list(Sint16 x, Sint16 y)
{
    return pos_to_walker[std::make_pair(hash(x), hash(y))];
}

Sint16 ObjectMap::hash(Sint16 y)
{
    // For now, this assumes no one is smaller than size 8. Also note that
    // the hash table never loops.

    Sint16 num = y / OBRES;

    if (num > 198) {
        num = 199;
    }

    if (num < 0) {
        num = 199;
    }

    return num;
}

Sint16 ObjectMap::unhash(Sint16 y)
{
    return (y * OBRES);
}

// This goes in walker's constructor
Sint16 ObjectMap::add(Walker *ob, Sint16 x, Sint16 y)
{
    Sint16 numx;
    Sint16 startnumx;
    Sint16 endnumx;
    Sint16 numy;
    Sint16 startnumy;
    Sint16 endnumy;

    if ((x < 0) || (y < 0)) {
        return 0;
    }

    startnumx = hash(x);
    endnumx = hash(x + ob->sizex);
    startnumy = hash(y);
    endnumy = hash(y + ob->sizey);

    // Figure out all of the positions that are occupied
    std::list<std::pair<Sint16, Sint16>> pos;

    for (numx = startnumx; numx <= endnumx; ++numx) {
        for (numy = startnumy; numy <= endnumy; ++numy) {
            // Store this position
            pos.push_back(std::make_pair(numx, numy));

            // Put the walker here too
            pos_to_walker[std::make_pair(numx, numy)].push_back(ob);
        }
    }

    // Now record where he is
    walker_to_pos.insert(std::make_pair(ob, pos));

    return 1;
}


// All pass checking from here down.

Sint16 ob_pass_check(Sint16 x, Sint16 y, Walker *ob, std::list<Walker *> const &pile)
{
    Sint16 oxsize;
    Sint16 oysize;
    Sint16 x2;
    Sint16 y2;
    Sint16 xsize2;
    Sint16 ysize2;
    Sint16 targetorder;
    Sint16 myorder;

    oxsize = ob->sizex;
    oysize = ob->sizey;

    myorder = ob->query_order();

    if (!ob) {
        return 1;
    }

    // Check each object to see if sizes collide.
    for (auto const &w : pile) {
        if ((w != ob) && !w->dead) {
            targetorder = w->query_order();

            // Let our own team's weapons pass over us
            if (((targetorder == ORDER_WEAPON) || (myorder == ORDER_WEAPON)) && ob->is_friendly(w)) {
                continue;
            } else if ((targetorder == ORDER_WEAPON) && (myorder == ORDER_WEAPON) && (getRandomSint32(10) > 3)) {
                // Allow weapons to sometimes "miss" opposing team's weapons
                continue;
            } else if ((targetorder == ORDER_TREASURE) && (myorder == ORDER_WEAPON)) {
                // Weapons never hit treasure
                continue;
            } else {
                x2 = w->xpos;
                y2 = w->ypos;
                xsize2 = w->sizex;
                ysize2 = w->sizey;

                if (collide(x, y, oxsize, oysize, x2, y2, xsize2, ysize2)) {
                    if (targetorder == ORDER_TREASURE) {
                        w->eat_me(ob);
                    } else if ((targetorder == ORDER_WEAPON) && (w->query_family() == FAMILY_DOOR)) {
                        // Can we unlock this door?
                        if (ob->keys & (2 << w->stats.level)) {
                            // Open the door...
                            w->dead = 1;
                            w->death();
                            ob->collide(w);

                            if (ob->stats.query_bit_flags(BIT_NO_COLLIDE)) {
                                    return 1;
                            }

                            // Block for this round
                            return 0;

                            // End of unlocked door
                        } else {
                            // Do we notify?
                            if (!ob->skip_exit && (ob->user != -1)) {
                                std::stringstream buf("Key ");
                                buf << w->stats.level << " needed!";
                                std::string temp(buf.str());
                                buf.clear();
                                temp.resize(80);

                                myscreen->do_notify(temp, ob);
                                ob->skip_exit = 10;
                            } // End of failed open door notification

                            ob->collide(w);

                            // Failed to open door
                            return 0;
                        }

                        // End of door case
                    } else {
                        ob->collide(w);

                        if (ob->stats.query_bit_flags(BIT_NO_COLLIDE)) {
                            return 1;
                        }

                        return 0;
                    }
                } else {
                    // Lets just assume it's safe
                    ob->collide_ob = nullptr;
                }
            }
        }
    }

    return 1;
}

Sint16 collide(Sint16 x, Sint16 y, Sint16 xsize, Sint16 ysize,
               Sint16 x2, Sint16 y2, Sint16 xsize2, Sint16 ysize2)
{
    Sint16 xright;
    Sint16 x2right;
    Sint16 ydown;
    Sint16 y2down;

    // Shrink values:
    x += 1;
    x2 += 1;
    y += 1;
    y2 += 1;
    xsize -= 2;
    xsize2 -= 2;
    ysize -= 2;
    ysize2 -= 2;

    x2right = x2 + xsize2;

    if (x > x2right) {
        return 0;
    }

    xright = x + xsize;

    if (xright < 2) {
        return 0;
    }

    y2down = y2 + ysize2;

    if (y > y2down) {
        return 0;
    }

    ydown = y + ysize;

    if (ydown < y2) {
        return 0;
    }

    return 1;
}
