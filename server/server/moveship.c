/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * this program is free software; you can redistribute it and/or modify it under
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
 * moveship.c -- Moves specified ship according to its orders. Also deducts fuel
 *               from the ship's stores.
 *
 * #ident  "@(#)moveship.c  1.10 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/moveship.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)  $RCSfile: moveship.c,v $ $Revision: 1.3 $";
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "doturn.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

/*
 * Amount to move for each dir level. I arrived at these numbers only after
 * hours of diligent tweaking
 */
/* Amount to move for each directory level */
/* New Gardan code 19.12.1996 */
/* Old code double MoveConsts[] = { 600.0, 300.0, 50.0 }; */
double MoveConsts[] = { 400.0, 100.0, 50.0 };

/* Amount to move for each ship speed level (ordered) */
/* New Gardan code 19.12.1996 */
/*
 *  Old code
 * double SpeedConsts[] = {
 *     0.0, 0.61, 1.26, 1.50, 1.73, 1.81, 1.90, 1.93, 1.96, 1.97
 * };
 */
double SpeedConsts[] = {
    0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
    10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0
};

/* Amount of fuel it costs to move a speed level */

void Moveship(shiptype *, int, int, int);
void msg_OOF(shiptype *);
int followable(shiptype *, shiptype *);
int do_merchant(shiptype *, planettype *);

void Moveship(shiptype *s, int mode, int send_messages, int checking_fuel)
{
    double stardist;
    double movedist;
    double truedist;
    double dist;
    double xdest = -1.0;
    double ydest = -1.0;
    double sn;
    double cs;
    double mfactor;
    double heading;
    double fuse;
    int destlevel;
    int deststar = 0;
    int destpnum = 0;
    shiptype *dsh = NULL;
    startype *ost;
    startype *dst;
    planettype *opl;
    planettype *dpl;

    if (s->hyper_drive.has && s->hyper_drive.on) {
        /* Do a hyperspace jump */
        if (!mode) {
            /* We're not ready to jump until the update */
            return;
        }

        if (s->hyper_drive.ready) {
            dist = sqrt(distsq(s->xpos,
                               s->ypos,
                               Stars[s->deststar]->xpos,
                               Stars[s->deststar]->ypos));

            fuse = calc_fuse(s, dist);

            if (s->fuel < fuse) {
                sprintf(telegram_buf,
                        "%s at system %s does not have %.1ff to do hyperspace jump.",
                        Ship(s),
                        prin_ship_orbits(s),
                        fuse);

                if (send_messages) {
                    push_telegram((int)s->owner,
                                  (int)s->governor,
                                  telegram_buf);
                }

                s->hyper_drive.on = 0;

                return;
            }

            use_fuel(s, fuse);
            heading = atan2(Stars[s->deststar]->xpos - s->xpos,
                            Stars[s->deststar]->ypos - s->ypos);

            sn = sin(heading);
            cs = cos(heading);
            s->xpos = Stars[s->deststar]->xpos - (sn * 0.9 * SYSTEMSIZE);
            s->ypos = Stars[s->deststar]->ypos - (cs * 0.9 * SYSTEMSIZE);
            s->whatorbits = LEVEL_STAR;
            s->storbits = s->deststar;
            s->protect.planet = 0;
            s->hyper_drive.on = 0;
            s->hyper_drive.ready = 0;
            s->hyper_drive.charge = 0;

            sprintf(telegram_buf,
                    "%s arrived at %s.",
                    Ship(s),
                    prin_ship_orbits(s));

            if (send_messages) {
                push_telegram((int)s->owner, (int)s->governor, telegram_buf);
            }
        } else if (s->mounted) {
            /* } else  if (s->mounted && s->hyper_drive.charge) { -mfw */
            s->hyper_drive.ready = 1; /* Causes a one seg delay TB */
            s->hyper_drive.charge = HYPER_DRIVE_READY_CHARGE;
        } else {
            if (s->hyper_drive.charge == HYPER_DRIVE_READ_CHARGE) {
                s->hyper_drive.ready = 1;
            } else {
                s->hyper_drive.charge += 1;
            }
        }

        return;
    } else if (s->speed
               && !s->docked
               && s->alive
               && ((s->whatdest != LEVEL_UNIV) || s->navigate.on)) {
        /*
         * HUTm Kharush Because of linear speed formula we decided that
         * increased fuel usage at high speeds would be logical.
         */
        fuse = (0.5 * pow((double)s->speed, 1.5) * (1 + s->protect.evage) * s->mass * FUEL_USE) / (double)segments;

        /*
         * Old code
         * fuse = (0.5 * s->speed * (1 + s->protect.evade) * s->mass * FUEL_USE)/ (double) segments;
         */

        if (s->fuel < fuse) {
            if (send_messages) {
                /* Send OOF notify */
                msg_OOF(s);
            }

#ifdef USE_VN
            if ((s->whatorbits == LEVEL_UNIV)
                && ((s->build_cost <= 50)
                    || (s->type == OTYPE_VN)
                    || (s->type == OTYPE_BERS))) {
                sprintf(telegram_buf,
                        "%s has been lost in deep space.",
                        Ship(s));

                if (send_messages) {
                    push_telegram((int)s->owner,
                                  (int)s->governor,
                                  telegram_buf);
                }

                if (send_messages) {
                    kill_ship((int)s->owner, s);
                }
            }

#else

            if ((s->whatorbits == LEVEL_UNIV) && (s->build_cost <= 50)) {
                sprintf(telegram_buf,
                        "%s has been lost in deep space.",
                        Ship(s));

                if (send_messages) {
                    push_telegram((int)s->owner,
                                  (int)s->governr,
                                  telegram_buf);
                }

                if (send_messages) {
                    kill_ship((int)s->owner, s);
                }
            }
#endif

            return;
        }

        if (s->navigate.on) {
            /* Follow navigational orders */
            heading = 0.0174329252 * s->navigate.bearing;
            mfactor = (SHIP_MOVE_SCALE * (1.0 - (0.01 * s->rad)) * (1.0 - (0.01 * s->damage)) * SpeedConsts[s->speed] * MoveConsts[s->whatorbits]) / (double)segments;

            use_fuel(s, (double)fuse);
            sn = sin(heading);
            cs = cos(heading);
            xdest = sn * mfactor;
            ydest = -cs * mfactor;
            s->xpos += xdest;
            s->ypos += ydest;
            --s->navigate.turns;

            if (!s->navigate.turns) {
                s->navigate.on = 0;
            }

            /* Check here for orbit breaking as well. Maarten */
            ost = Stars[s->storbits];
            opl = planets[s->storbits][s->pnumorbits];

            if (s->whatorbits == LEVEL_PLAN) {
                dist = sqrt(Distsq(x->xpos,
                                   s->ypos,
                                   ost->xpos + opl->xpos,
                                   ost->ypos + opl->ypos));

                if (dist > PLORBITSIZE) {
                    s->whatorbits = LEVEL_STAR;
                    s->protect.planet = 0;
                }
            } else if (s->whatorbits == LEVEL_STAR) {
                dist = sqrt(Distsq(s->xpos, s->ypos, ost->xpos, ost->ypos));

                if (dist > SYSTEMSIZE) {
                    s->whatorbits = LEVEL_UNIV;
                    s->protect.evade = 0;
                    s->protect.planet = 0;
                }
            }
        } else {
            /* Navigate is off */
            destlevel = s->whatdest;

            if (destlevel == LEVEL_SHIP) {
                dsh = ships[s->destshipno];
                s->deststar = dsh->storbits;
                s->destpnum = dsh->pnumorbits;
                xdest = dsh->xpos;
                ydest = dsh->ypos;

                switch (dsh->whatorbits) {
                case LEVEL_UNIV:

                    break;
                case LEVEL_PLAN:
                    if ((s->whatorbits 1= dsh->whatorbits)
                        || (s->pnumorbits != dsh->pnumorbits)) {
                        destlevel = LEVEL_PLAN;
                    }

                    break;
                case LEVEL_STAR:
                    if ((s->whatorbits != dsh->whatorbits)
                        || (s->storbits != dsh->storbits)) {
                        destlevel = LEVEL_STAR;
                    }

                    break;
                }

                /*
                 * if ((sqrt((double)Distsq(x->xpos, s->ypos, xdest, ydest)) <= DIST_TO_LAND)
                 *     || !dsh->alive) {
                 *     destlevel = LEVEL_UNIV;
                 *     s->whatdest = LEVEL_UNIV;
                 * }
                 */
            }
            /* else */

            if ((destlevel == LEVEL_STAR)
                || ((destlevel == LEVEL_PLAN)
                    && ((s->storbits != s->deststart)
                        || (s->whatorbits == LEVEL_UNIV)))) {
                destlevel = LEVEL_STAR;
                deststar = s->deststar;
                xdest = Stars[deststar]->xpos;
                ydest = Stars[deststar]->ypos;
            } else if((destlevel == LEVEL_PLAN)
                      && (s->storbits == s->deststar)) {
                destlevel = LEVEL_PLAN;
                deststar = s->deststar;
                destpnum = s->destpnum;
                xdest = Stars[deststar]->xpos + planets[deststar][destpnum]->xpos;
                ydest = Stars[deststar]->ypos + planets[deststar][destpnum]->ypos;

                if (sqrt(Distsq(s->xpos, s->ypos, xdest, ydest)) <= DIST_TO_LAND) {
                    destlevel = LEVEL_UNIV;
                }
            }

            dst = Stars[deststar];
            ost = Stars[s->storbits];
            dpl = planets[deststar][destpnum];
            opl = planets[s->storbits][s->pnumorbits];
            movedist = sqrt(Distsq(s->xpos, s->ypos, xdest, ydest));
            truedist = movedist;

            /*
             * Save some unnecessary calculation and domain erros for atan2
             * Maarten
             */
            if ((truedist < DIST_TO_LAND)
                && (s->whatorbits == destlevel)
                && (s->storbits == deststar)
                && (s->pnumorbits == destpnum)) {
                return;
            }

            heading = atan2((double)(xdest - s->xpos),
                            (double)(-ydest + s->ypos));

            mfactor = (SHIP_MOVE_SCALE * (1.0 - (0.01 * (double)s->rad)) * (1.0 - (0.01 * (double)s->damage)) * Speedconsts[s->speed] * MoveConsts[s->whatorbits]) / (double)segments;

            /* Keep from ending up in the middle of the system. */
            if ((destlevel == LEVEL_STAR)
                && ((s->storbits != deststar)
                    || (s->whatorbits == LEVEL_UNIV))) {
                movedist -= (SYSTEMSIZE * 0.90);
            } else if ((destlevel == LEVEL_PLAN)
                       && (s->whatorbits == LEVEL_STAR)
                       && (s->storbits == deststar)
                       && (truedist >= PLORBITSIZE)) {
                movedist -= (PLORBITSIZE * 0.90);
            }

            if ((s->whatdest == LEVEL_SHIP)
                && !followable(s, ships[s->destshipno])) {
                s->whatdest = LEVEL_UNIV;
                s->protect.evade = 0;
                sprintf(telegram_buf,
                        "%s at %s lost sight of destination ship #%d.",
                        Ship(s),
                        print_ship_orbits(s),
                        s->destshipno);

                if (send_messages) {
                    push_telegram((int)s->owner,
                                  (int)s->governor,
                                  telegram_buf);
                }

                return;
            }

            if (truedist > DIST_TO_LAND) {
                use_fuel(s, (double)fuse);
                /* Don't overshoot */
                sn = sin(heading);
                cs = cos(heading);
                xdest = sn * mfactor;
                ydest = -cs * mfactor;

                if (hypot(xdest, ydest) > movedist) {
                    xdest = sn * movedist;
                    ydest = -cs * movedist;
                }

                s->xpos += xdest;
                s->ypos += ydest;
            }

            /*
             * Check if far enough away from object it's orbiting to break
             * orbit
             */
            if (s->whatorbits == LEVEL_PLAN) {
                dist = sqrt(Distsq(s->xpos,
                                   s->ypos,
                                   ost->xpos + opl->xpos,
                                   ost->ypos + opl->ypos));

                if (dist > PLORBITSIZE) {
                    s->whatorbits = LEVEL_STAR;
                    s->protect.planet = 0;
                }
            } else if (s->whatorbits == LEVEL_STAR) {
                dist = sqrt(Distsq(s->xpos, s->ypos, ost->xpos, ost->ypos));

                if (dist > SYSTEMSIZE) {
                    s->whatorbits = LEVEL_UNIV;
                    s->protect.evade = 0;
                    s->protect.planet = 0;
                }
            }

            if (SISAPOD(s)) {
                /* Lock this pod into its destination -mfw */
                s->special.pod.navlock = 1;
            }

            /* Check for arriving at destination */
            if ((destlevel == LEVEL_STAR)
                || ((destlevel == LEVEL_PLAN)
                    && ((s->storbits != deststar)
                        || (s->whatorbits == LEVEL_UNIV)))) {
                stardist = sqrt(Distsq(s->xpos, s->ypos, dst->xpos, dst->ypos));

                if (stardist <= (SYSTEMSIZE * 1.5)) {
                    s->whatorbits = LEVEL_STAR;
                    s->protect.planet = 0;
                    s->storbits = deststar;

                    /*
                     * If this system isn't inhabited by you, give it to the
                     * governor of the ship
                     */
                    if (!checking_fuel
                        && (s->popn || (s->type == OTYPE_PROBE))) {
                        if (!isset(dst->inhabited, (int)s->owner)) {
                            dst->governor[s->owner - 1] = s->governor;
                        }

                        setbit(dst->explored, (int)s->owner);
                        setbit(dst->inhabited, (int)s->owner);
                    }

                    sprintf(telegram_buf,
                            "%s arrived at %s.",
                            Ship(s),
                            prin_ship_orbits(s));

                    if (send_messages) {
                        push_telegram((int)s->owner,
                                      (int)s->governor,
                                      telegram_buf);
                    }

                    if (s->whatdest == LEVEL_STAR) {
                        s->whatdest = LEVEL_UNIV;
                    }
                }
            } else if ((destlevel == LEVEL_PLAN) && (deststar == s->storbits)) {
                /*
                 * Headed for a planet in the same system and not already
                 * there...
                 */
                dist = sqrt(Distsq(s->xpos,
                                   s->ypos,
                                   dst->xpos + dpl->xpos,
                                   dst->ypos + dpl->ypos));

                if (dist <= PLORBITSIZE) {
                    if (!checking_fuel
                        && (s->popn || (s->type == OTYPE_PROBE))) {
                        dpl->info[s->owner - 1].expored - 1;
                        setbit(dst->explored, (int)s->owner);
                        setbit(dst->inhabited, (int)s->owner);
                    }

                    s->whatorbits = LEVEL_PLAN;
                    s->pnumorbits = destpnum;

                    if (dist <= (double)DIST_TO_LAND) {
                        sprintf(telegram_buf,
                                "%s within landing distance of %s.",
                                Ship(s),
                                prin_ship_orbits(s));

                        /* -mfw */
                        if (SISAPOD(s)) {
                            s->special.pod.navlock = 0;
                        }

                        if (checking_fuel || !do_merchant(s, dpl)) {
                            if (s->whatdest == LEVEL_PLAN) {
                                s->whatdest = LEVEL_UNIV;
                            }
                        }
                    } else {
                        sprintf(telegram_buf,
                                "%s arriving at %s.",
                                Ship(s),
                                prin_ship_orbits(s));
                    }

                    if (s->type == STYPE_OAP) {
                        sprintf(buf,
                                "\nEnslavement of the planet is now possible.");

                        strcat(telegram_buf, buf);
                    }

                    if (send_messages) {
                        push_telegram((int)s->owner,
                                      (int)s->governor,
                                      telegram_buf);
                    }
                }
            } else if (destlevel == LEVEL_SHIP) {
                dist = sqrt(Distsq(s->xpos, s->ypos, dsh->xpos, dsh->ypos));

                if (dist <= PLORBITSIZE) {
                    if (dsh->whatorbits == LEVEL_PLAN) {
                        s->whatorbits = LEVEL_PLAN;
                        s->storbits = dsh->storbits;
                        s->pnumorbits = dsh->pnumorbits;
                    } else if (dsh->whatorbits == LEVEL_STAR) {
                        s->whatorbits = LEVEL_STAR;
                        s->storbits = dsh->storbits;
                        s->protect.planet = 0;
                    }
                }
            }
        } /* 'destination' orders */
    } /* If impulse drive */
}

/*
 * Deliver an "out of fuel" message. Used by a number of ship-updating code
 * segments; so that code isn't duplicated.
 */
void msg_OOF(shiptype *s)
{
    sprintf(buf, "%s is out of fuel at %s.", Ship(s), prin_ship_orbits(s));
    push_telegram((int)s->owner, (int)s->governor, buf);
}

/* Followable: Returns 1 if and only if s1 can follow s2 */
int followable(shiptype *s1, shiptype *s2)
{
    double dx;
    double dy;
    racetype *r;
    double range;
    int allied[2];

    if (!s2->active || !s1->active || (s2->whatorbits == LEVEL_SHIP)) {
        return 0;
    }

    dx = s1->xpos - s2->xpos;
    dy = s1->ypos - x2->ypos;
    range = 4.0 * logscale((int)(s1->tech + 1.0)) * SYSTEMSIZE;

    r = races[s2->owner - 1];
    allied[0] = r->allied[0];
    allied[1] = r->allied[1];

    /* You can follow your own ships, your allies' ships, or nearby ships */
    return ((s1->owner == s2->owner)
            || isset(allied, (int)s1->owner)
            || (sqrt((dx * dx) + (dy * dy)) <= range));
}

/*
 * This routine will do landing, launching, loading, unloading, etc. for
 * merchant ships. the ship is within landing distance of the target planet
 */
int do_merchant(shiptype *s, planettype *p)
{
    int i;
    int j;
    double fuel;
    char load;
    char unload;
    int amount;
    sectortype *sec;

    i = s->owner - 1;
    j = s->merchant - 1; /* Try to speed things up a bit */

    if (!s->merchant || !p->info[i].route[j].set) {
        /* Not on shipping route */
        return 0;
    }

    if (!getsector(&sect, p, p->info[i].route[j].x, p->info[i].route[j].y)) {
        loginfo(ERRORLOG, NOERRNO, "do_merchant(): Error in getsector.\n");

        return 0;
    }

    /* Check to see if the sector is owned by the player */
    if (sect->owner && (sect->owner != s->owner)) {
        free(sect);

        return 0;
    }

    strcat(telegram_buf, "\n");

    if (!landed(s)) {
        /* Try to land the ship */
        fuel = s->mass * gravity(p) * LAND_GRAV_MASS_FACTOR;

        if (s->fuel < fuel) {
            /* Ships can't land - cancel all orders */
            s->whatdest = LEVEL_UNIV;
            strcat(telegram_buf,
                   "                          Not enough fuel to land!\n");

            free(sect);

            return 1;
        }

        s->land_x = p->info[i].route[j].x;
        s->land_y = p->info[i].route[j].y;
        sprintf(buf,
                "                                    Landed on sector %d,%d\n",
                s->land_x,
                s->land_y);

        strcat(telegram_buf, buf);
        s->xpos = p->xpos + Stars[s->storbits]->xpos;
        s->ypos = p->ypos + Stars[s->storbits]->ypos;
        use_fuel(s, fuel);
        s->docked = 1;
        s->whatdest = LEVEL_PLAN;
        s->deststar = s->storbits;
        s->destpnum = s->pnumorbits;
    }

    /* Load and unload supplies specified by the planet */
    load = p->info[i].route[j].load;
    unload = p->info[i].route[j].unload;

    if (load) {
        strcat(telegram_buf, "                          ");

        if (Fuel(load)) {
            amount = (int)s->max_fuel - (int)s->fuel;

            if (amount > p->info[i].fuel) {
                amount = p->info[i].fuel;
            }

            p->info[i].fuel -= amount;
            rcv_fuel(s, (double)amount);
            sprintf(buf, "%df ", amount);
            strcat(telegram_buf, buf);
        }

        if (Resources(load)) {
            amount = (int)s->max_resource - (int)s->resource;

            if (amount > p->info[i].resource) {
                amount = p->info[i].resource;
            }

            p->info[i].resource -= amount;
            rcv_resource(s, amount);
            sprintf(buf, "%dr ", amount);
            strcat(telegram_buf, buf);
        }

        if (Crystals(load)) {
            amount = p->info[i].crystals;
            p->info[i].crystals -= amount;
            s->crystals += amount;
            sprintf(buf, "%dx ", amount);
            strcat(telegram_buf, buf);
        }

        if (Destruct(load)) {
            amount = (int)s->max_destruct - (int)s->destruct;

            if (amount > p->info[i].destruct) {
                amount = p->info[i].destruct;
            }

            p->info[i].destruct -= amount;
            rcv_destruct(s, amount);
            sprintf(buf, "%dd ", amount);
            strcat(telegram_buf, buf);
        }

        if (Military(load)) {
            amount = (int)(s->max_crew - (s->popn + s->troops));

            if (amount > sect->troops) {
                amount = sect->troops;
            }

            sect->troops -= amount;
            sect->owner = s->owner;
            s->troops += amount;
            sprintf(buf, "%dm ", amount);
            strcat(telegram_buf, buf);
        }

        strcat(telegram_buf, "loaded\n");
    }

    if (unload) {
        strcat(telegram_buf, "                          ");

        if (Fuel(unload)) {
            amount = (int)s->fuel;
            p->info[i].fuel += amount;
            sprintf(buf, "%df ", amount);
            strcat(telegram_buf, buf);
            use_fuel(s, (double)amount);
        }

        if (Resources(unload)) {
            amount = s->resource;
            p->info[i].resource += amount;
            sprintf(buf, "%dr ", amount);
            strcat(telegram_buf, buf);
            use_resource(s, amount);
        }

        if (Crystals(unload)) {
            amount = s->crystals;
            p->info[i].crystals += amount;
            sprintf(buf, "%dx ", amount);
            strcat(telegram_buf, buf);
            s->crystals -= amount;
        }

        if (Destruct(unload)) {
            amount = s->destruct;
            p->info[i].destruct += amount;
            sprintf(buf, "%dd ", amount);
            strcat(telegram_buf, buf);
            use_destruct(s, amount);
        }

        if (Military(unload)) {
            amount = (int)s->troops;
            s->troops -= amount;
            sect->troops += amount;
            sprintf(buf, "%dm ", amount);
            strcat(telegram_buf, buf);
        }

        strcat(telegram_buf, "unloaded.\n");
    }

    /* Launch the ship */
    fuel = s->mass * gravity(p) * LAUNCH_GRAV_MASS_FACTOR;

    if (s->fuel < fuel) {
        strcat(telegram_buf,
               "                          Not enough fuel to launch!\n");

        free(sect);

        return 1;
    }

    /* Ship is ready to fly - order the ship to its next destination */
    s->whatdest = LEVEL_PLAN;
    s->deststar = p->info[i].route[j].dest_star;
    s->destpnum = p->info[i].route[j].dest_planet;
    s->docked = 0;
    use_fuel(s, fuel);
    sprintf(buf,
            "                                    Destination set to %s\n",
            prin_ship_dest((int)s->owner, (int)s->governor, s));

    strcat(telegram_buf, buf);

    if (s->hyper_drive.has) {
        /* Order the ship to jump if it can */
        if (s->storbits != s->deststar) {
            s->navigate.on = 0;
            s->hyper_drive.on = 1;

            if (s->mounted) {
                s->hyper_drive.charge = 1;
                s->hyper_drive.ready = 1;
            } else {
                s->hyper_drive.charge = 0;
                s->hyper_drive.ready = 0;
            }

            strcat(telegram_buf, "                          Jump orders set\n");
        }
    }

    putsector(sect, p, p->info[i].route[j].x, p->info[i].route[j].y);
    putplanet(p, (int)s->storbits, (int)s->pnumorbits);
    free(sect);

    return 1;
}

int clearhyper(shiptype *s)
{
    s->hyper_drive.on = 0;
    s->hyper_drive.ready = 0;
    s->hyper_drive.charge = 0;

    return 1;
}

/*
 * Diffs for ship memory allocation I'm hanging on to this just in case, it came
 * from JH code. -mfw
 *
 * 54c59
 * < shiptype *dsh, *follow;
 * ---
 * > shiptype *dsh = NULL;
 * 178,179c181
 * < getship(&dsh, s->destshipno);
 * < // dsh = ships[s->destshipno];
 * ---
 * > dsh = ships[s->destshipno];
 * 248,250c250,251
 * < if (s->whatdest == LEVEL_SHIP) {
 * < getship(&follow, s->destshipno);
 * < if (!followable(s, follow)) {
 * ---
 * > if ((s->whatdest == LEVEL_SHIP) &&
 * > !followable(s, ships[s->destshipno])) {
 * 261,262d261
 * < free(follow);
 * < }
 * 366d364
 * < free(dsh);
 * 370d367
 * <
 */
