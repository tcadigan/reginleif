/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * fuel.c -- See estimations in fuel consumption and travel time.
 *
 * Programmed by varneyml@gm.dorm.clarkson.edu
 *
 * #ident  "@(#)fuel.c  1.9 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/fuel.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "fuel.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../server/doship.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/files_shl.h"
#include "../server/log.h"
#include "../server/max.h"
#include "../server/moveship.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/vars.h"

#include "fire.h"
#include "order.h"

/*
 * extern long next_segment_time;
 * extern long next_update_time;
 * extern long nsegments_done;
 * extern int update_time;
 */

static char plan_buf[1024];
static int number_segments;
static double x_0;
static double y_0;
static double x_1;
static double y_1;
static shiptype *tmpship;
static placetype tmpdest;

extern void proj_fuel(int, int, int);
extern void fuel_output(int, int, double, double, double, double, int);
extern int do_trip(double, double);

void proj_fuel(int playernum, int governor, int apcount)
{
    int shipno;
    int opt_settings;
    int current_settings;
    int current_segs;
    int computing = 1;
    int i;
    double fuel_usage;
    double level;
    double dist;
    double dmg;
    shiptype *ship;
    planettype *p;
    char buf[1024];
    double current_fuel = 0.0;
    double gravity_factor = 0.0;

    if ((argn < 2) || (argn > 3)) {
        notify(playernum,
               governor,
               "Invalid number of options.\n\"fuel #<shipnumber> [destination]\"...\n");

        return;
    }

    if (args[1][0] != '#') {
        notify(playernum,
               governor,
               "Invalid first operation.\n\"fuel #<shipnumber> [destination]\"...\n");

        return;
    }

    sscanf(args[1] + (args[1][0] == '#'), "%d", &shipno);

    if ((shipno > Numships()) || (shipno < 1)) {
        sprintf(buf, "rst: No such ship $%d\n", shipno);
        notify(playernum, governor, buf);

        return;
    }

    getship(&ship, shipno);

    if (ship->owner != playernum) {
        notify(playernum, governor, "You do not own this ship.\n");
        free(ship);

        return;
    }

    if (landed(ship) && (argn == 2)) {
        notify(playernum,
               governor,
               "You must specify a destination for landed or docked ships...\n");

        free(ship);

        return;
    }

    if (!ship->speed) {
        notify(playernum, governor, "That ship is not moving!\n");
        free(ship);

        return;
    }

    if (!ship->max_speed || (ship->type == OTYPE_FACTORY)) {
        notify(playernum,
               governor,
               "That ship does not have a speed rating...\n");

        free(ship);

        return;
    }

    if (landed(ship) && (ship->whatorbits == LEVEL_PLAN)) {
        getplanet(&p, (int)ship->storbits, (int)ship->pnumorbits);
        gravity_factor = gravity(p);

        sprintf(plan_buf,
                "/%s/%s",
                Stars[(int)ship->storbits]->name,
                Stars[(int)ship->storbits]->pnames[(int)ship->pnumorbits]);

        free(p);
    }

    if (argn == 2) {
        strcpy(args[2], prin_ship_dest(playernum, governor, ship));
    }

    tmpdest = Getplace(playernum, governor, args[2], 1);

    if (tmpdest.err) {
        notify(playernum, governor, "fuel: Bad scope.\n");
        free(ship);

        return;
    }

    if (tmpdest.level == LEVEL_SHIP) {
        getship(&tmpship, tmpdest.shipno);

        if (!followable(ship, tmpship)) {
            notify(playernum,
                   governor,
                   "The ship's destination is out of range.\n");

            free(tmpship);
            free(ship);

            return;
        }

        free(tmpship);
    }

    if ((tmpdest.level != LEVEL_UNIV)
        && (tmpdest.level != LEVEL_SHIP)
        && ((ship->storbits != tmpdest.snum) && tmpdest.level != LEVEL_STAR)
        && isclr(Stars[tmpdest.snum]->explored, ship->owner)) {
        notify(playernum,
               governor,
               "You haven't explored the destination system.\n");

        free(ship);

        return;
    }

    if (tmpdest.level == LEVEL_UNIV) {
        notify(playernum, governor, "Invalid ship destination.\n");
        free(ship);

        return;
    }

    y_1 = 0.0;
    x_1 = y_1;
    y_0 = x_1;
    x_0 = y_0;

    x_0 = ship->xpos;
    y_0 = ship->ypos;
    free(ship);

    if (tmpdest.level == LEVEL_UNIV) {
        notify(playernum,
               governor,
               "That ship currently has no destination orders...\n");

        return;
    }

    if (tmpdest.level == LEVEL_SHIP) {
        getship(&tmpship, tmpdest.shipno);

        if (tmpship->owner != playernum) {
            notify(playernum, governor, "Nice try.\n");

            return;
        }

        x_1 = tmpship->xpos;
        y_1 = tmpship->ypos;
        free(tmpship);
    } else if (tmpdest.level == LEVEL_PLAN) {
        getplanet(&p, (int)tmpdest.snum, (int)tmpdest.pnum);
        x_1 = p->xpos + Stars[tmpdest.snum]->xpos;
        y_1 = p->ypos + Stars[tmpdest.snum]->ypos;
        free(p);
    } else if (tmpdest.level == LEVEL_STAR) {
        x_1 = Stars[tmpdest.snum]->xpos;
        y_1 = Stars[tmpdest.snum]->ypos;
    } else {
        loginfo(ERRORLOG, WANTERRNO, "ERROR 99\n");
    }

    /* Compute the distance */
    dist = sqrt(Distsq(x_0, y_0, x_1, y_1));

    if (dist <= DIST_TO_LAND) {
        notify(playernum,
               governor,
               "That ship is within 10.0 unit of the destination.\n");

        return;
    }

    /* First get the results based on current fuel load. */
    getship(&tmpship, shipno);

    /*
     * Allocate ships[] if destination is a ship, or there are ships on
     * board. Bring in the other ships. Moveship() uses ships[].
     * Dave Anderson Mod
     */

    /* Watch this next block, it was commented out in VoS code -mfw */
    if ((tmpdest.level == LEVEL_SHIP) || tmpship->ships) {
        /* Bring in the other ships. Moveship() uses ships[]. */
        Num_ships = Numships();
        ships = (shiptype **)malloc((sizeof(shiptype *) * Num_ships) + 1);

        for (i = 1; i <= Num_ships; ++i) {
            getship(&ships[i], i);
        }
    }

    level = tmpship->fuel;
    current_settings = do_trip(tmpship->fuel, gravity_factor);
    current_segs = number_segments;

    if (current_settings) {
        current_fuel = level - tmpship->fuel;
    }

    /* 2nd loop to determine lowest fuel needed... */
    level = tmpship->max_fuel;
    fuel_usage = level;
    opt_settings = 0;
    free(tmpship);

    while (computing) {
        getship(&tmpship, shipno);
        computing = do_trip(level, gravity_factor);

        if (computing && (tmpship->fuel >= 0.001)) {
            fuel_usage = level;
            opt_settings = 1;
            level -= tmpship->fuel;
        } else if (computing) {
            computing = 0;
            fuel_usage = level;
        }

        free(tmpship);
    }

    getship(&tmpship, shipno);

    sprintf(buf,
            "\n----- ===== FUEL ESTIMATES ===== -----\n\nAt Current Fuel Cargo (%.2ff):\n",
            tmpship->fuel);

    domass(tmpship);
    notify(playernum, governor, buf);

    if (!current_settings) {
        strcpy(buf, "The ship will not be able to complete the trip.\n");
        notify(playernum, governor, buf);
    } else {
        fuel_output(playernum,
                    governor,
                    dist,
                    current_fuel,
                    gravity_factor,
                    tmpship->mass,
                    current_segs);
    }

    sprintf(buf, "At Optimum Fuel Level (%.2ff):\n", fuel_usage);
    notify(playernum, governor, buf);

    if (!opt_settings) {
        strcpy(buf, "The ship will not be able to complete the trip.\n");
        notify(playernum, governor, buf);
    } else {
        tmpship->fuel = fuel_usage;
        domass(tmpship);

        fuel_output(playernum,
                    governor,
                    dist,
                    fuel_usage,
                    gravity_factor,
                    tmpship->mass,
                    number_segments);
    }

    if (dist > SYSTEMSIZE) {
        dmg = 20 / (1 + (tmpship->tech / 50));

        sprintf(buf,
                "This ship has a %2.0f%% chance of receiving %2.0f%% damage\nper segment while in deep space.\n",
                dmg + tmpship->damage,
                dmg);

        notify(playernum, governor, buf);
    }

    /*
     * What this next block, it was comment out in VoS code -mfw
     *
     * Free any allocated ships.
     * Dave Anderson Mod
     *
     * Fixed tmpship to tmpdest...hope it helps (kse)
     */
    if ((tmpdest.level == LEVEL_SHIP) || tmpship->ships) {
        for (i = 1; i <= Num_ships; ++i) {
            free(ships[i]);
        }

        free(ships);
    }

    if (tmpship) {
        free(tmpship);
    }
}

void fuel_output(int playernum,
                 int governor,
                 double dist,
                 double fuel,
                 double grav,
                 double mass,
                 int segs)
{
    time_t effective_time;
    char buf[1024];
    char grav_buf[1024];

    if (grav > 0.0) {
        sprintf(grav_buf,
                " (%.2f used to launch from %s)\n",
                (double)(grav * mass * (double)LAUNCH_GRAV_MASS_FACTOR),
                plan_buf);
    } else {
        strcpy(grav_buf, " ");
    }

    sprintf(buf,
            "Total Distance = %.2f   Number of Segments = %d\nFuel = %.2f%s  ",
            dist,
            segs,
            fuel,
            grav_buf);

    notify(playernum, governor, buf);

    if (nsegments_done > segments) {
        notify(playernum,
               governor,
               "Estimated arrival time not available due to segment # discrepancy.\n");
    } else {
        effective_time = next_segment_time + (time_t)((segs - 1) * (update_time / segments) * 60);

        if (segments == 1) {
            effective_time = next_update_time + (time_t)((segs - 1) * (update_time * 60));
        }

        sprintf(buf, "ESTIMATED Arrival Time: %s\n", ctime(&effective_time));
        notify(playernum, governor, buf);
    }
}

int do_trip(double fuel, double gravity_factor)
{
    int effective_segment_number;
    int trip_resolved;
    double gravity_fuel;
    double tmpdist;
    double fuel_level1;

    /* Load up the pseudo-ship */
    tmpship->fuel = fuel;
    effective_segment_number = nsegments_done;

    /* Set our temporary destination... */
    tmpship->destshipno = (unsigned short)tmpdest.shipno;
    tmpship->whatdest = tmpdest.level;
    tmpship->deststar = tmpdest.snum;
    tmpship->destpnum = tmpdest.pnum;

    trip_resolved = 0;

    /* Reset counter. */
    number_segments = 0;

    /* Launch the ship if it's on a planet. */
    gravity_fuel = gravity_factor * tmpship->mass * LAUNCH_GRAV_MASS_FACTOR;
    tmpship->fuel -= gravity_fuel;
    tmpship->docked = 0;

    while (trip_resolved == 0) {
        domass(tmpship);
        fuel_level1 = tmpship->fuel;

        if (effective_segment_number == segments) {
            Moveship(tmpship, 1, 0, 1);
        } else {
            Moveship(tmpship, 0, 0, 1);
        }

        ++number_segments;
        ++effective_segment_number;

        if (effective_segment_number == (segments + 1)) {
            effective_segment_number = 1;
        }

        x_0 = (double)tmpship->xpos;
        y_0 = (double)tmpship->ypos;
        tmpdist = sqrt(Distsq(x_0, y_0, x_1, y_1));

        switch ((int)tmpship->whatdest) {
        case LEVEL_STAR:
            if (tmpdist <= (double)SYSTEMSIZE) {
                trip_resolved = 1;
            }

            break;
        case LEVEL_PLAN:
            if (tmpdist <= (double)PLORBITSIZE) {
                trip_resolved = 1;
            }

            break;
        case LEVEL_SHIP:
            if (tmpdist <= (double)DIST_TO_LAND) {
                trip_resolved = 1;
            }

            break;
        default:
            trip_resolved = 1;
        }

        if (((tmpship->fuel == fuel_level1) && !tmpship->hyper_drive.on)
            && (trip_resolved == 0)) {
            return 0;
        }
    }

    return 1;
}
