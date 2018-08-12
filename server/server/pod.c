/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_Copyright.h for additional authors and details.
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
 * Pod.c -- Functions dealing with the meso/pod relation aspects of the game
 *
 * #ident  "%W% %G% %Q%"
 *
 * $Header: /var/cvs/gbp/GB+/server.pod.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)      $RCSfile: pod.c,v $ $Revision: 1.3 $";
 */
#include "pod.h"

#include <math.h>
#include <string.h>

#include "buffers.h"
#include "doturn.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

void do_pod(shiptype *);
int infect_planet(int, int, int, int);
void do_meta_infect(int, planettype *);

void do_pod(shiptype *ship)
{
    int starsys;
    int planetno;

    memset((char *)buf, 0, sizeof(buf));

    if (ship->popn != 0) {
        if (ship->whatorbits == LEVEL_STAR) {
            if (ship->special.pod.temperature >= POD_THRESHOLD) {
                starsys = ship->storbits;
                planetno = int_rand(0, (int)Stars[starsys]->numplanets - 1);
                sprintf(telegram_buf,
                        "%s has warmed and exploded at %\n",
                        Ship(ship),
                        prin_ship_orbits(ship));

                /* Seed passes through wormhole */
                if (Stars[starsys]->wh_has_wormhole
                    && (planetno == (Stars[starsys]->numplanets - 1))) {
                    planets[starsys][planetno]->info[ship->owner - 1].explored = 1;
                    starsys = Stars[starsys]->wh_dest_starnum;
                    planetno = int_rand(0, (int)Stars[starsys]->numplanets - 2);
                    sprintf(buf,
                            "                          The spores traversed a wormhole to %s.\n",
                            Stars[starsys]->name);

                    strcat(telegram_buf, buf);
                }

                if (infect_planet((int)ship->type, (int)ship->owner, starsys, planetno)) {
                    sprintf(buf,
                            "                          Meta-colony established on %s.",
                            Stars[starsys]->pnames[planetno]);
                } else {
                    sprintf(buf, "                          No spores have survived.");
                }

                strcat(telegram_buf, buf);
                push_telegram((int)ship->owner,
                              (int)ship->governor,
                              telegram_buf);

                kill_ship((int)ship->owner, ship);
            } else {
                /* pod.temp >= POD_THRESHOLD */
                ship->special.pod.temperature += round_rand((double)Stars[ship->storbits]->temperature / (double)segments);
            }
        } else if (ship->whatorbits == LEVEL_PLAN) {
            if (ship->special.pod.decay >= POD_DECAY) {
                starsys = ship->storbits;
                planetno = ship->pnumorbits;
                sprintf(telegram_buf,
                        "%s has decayed at %s\n",
                        Ship(ship),
                        prin_ship_orbits(ship));

                /* Seed passes through wormhole */
                if (Stars[starsys]->wh_has_wormhole
                    && (planetno == (Stars[starsys]->numplanets - 1))) {
                    starsys = Stars[starsys]->wh_dest_starnum;
                    planetno = int_rand(0, (int)Stars[starsys]->numplanets - 2);
                    sprintf(buf,
                            "                          The spores traversed a wormhole to %s.\n",
                            Stars[starsys]->name);

                    strcat(telegram_buf, buf);
                }

                if (infect_planet((int)ship->type, (int)ship->owner, starsys, planetno)) {
                    sprintf(buf,
                            "                          Meta-colony established on %s.",
                            Stars[starsys]->pnames[planetno]);
                } else {
                    sprintf(buf, "                          No spores have survived.");
                }

                strcat(telegram_buf, buf);
                push_telegram((int)ship->owner,
                              (int)ship->governor,
                              telegram_buf);

                kill_ship((int)ship->owner, ship);
            } else {
                /* decay > POD_DECAY */
                ship->special.pod.decay += round_rand(1.0 / (double) segments);
            }
        }
    } else {
        /* If no population on board, don't infect! --jpd-- */
        /* Don't decay if military on board. */
        if (!ship->troops) {
            sprintf(telegram_buf,
                    "%s has no population and has decayed at %s\n",
                    Ship(ship),
                    prin_ship_orbits(ship));

            push_telegram((int)ship->owner, (int)ship->governor, telegram_buf);
            kill_ship((int)ship->owner, ship);
        }
    } /* If ship->popn != 0 --jpd -- */
}

int infect_planet(int type, int who, int star, int p)
{
#ifdef DEBUG
    char godbuf[200];
#endif

    int i;
    int x = int_rand(1, 100);
    planettype *pp;

    pp = planets[star][p];

    if (get_num_updates() < CombatUpdate) {
        /* If before combat, check to see if planet occupied. */
        for (i = 1; i <= Num_races; ++i) {
            if ((i != who)
                && pp->info[i - 1].popn
                && !isset(races[who - 1]->allied, i)
                && !isset(races[i - 1]->allied, who)) {
                sprintf(buf,
                        "Can only land on allies planets before Combat enabled.\nCombat enabled at update [%d]\n",
                        CombatUpdate);

                notify(who, 0, buf);

                return 0;
            }
        }
    }

    if (((x < SPORE_SUCCESS_RATE) && (type == STYPE_SUPERPOD))
        || ((x < NORMAL_SUCCESS_RATE) && (type == STYPE_POD))) {
#ifdef DEBUG
        if (type == STYPE_SUPERPOD) {
            sprintf(godbuf,
                    "SUPERPOD SUCCESS (%d%%) at /%s/%s by %s\n",
                    x,
                    Stars[star]->name,
                    Stars[star]->pnames[p],
                    races[who - 1]->name);
        } else {
            sprintf(godbuf,
                    "POD SUCCESS (%d%%) at /%s/%s by %s\n",
                    x,
                    Stars[star]->name,
                    Stars[star]->pnames[p],
                    races[who - 1]->name);
        }

        push_telegram(1, 0, godbuf);
#endif

        do_meta_infect(who, planets[star][p]);

        return 1;
    } else {
#ifdef DEBUG
        if (type == STYPE_SUPERPOD) {
            sprintf(godbuf,
                    "SUPERPOD FAILURE (%d%%) at /%s/%s by %s\n",
                    x,
                    Stars[star]->name,
                    Stars[star]->pnames[p],
                    races[who - 1]->name);
        } else {
            sprintf(godbuf,
                    "POD FAILURE (%d%%) at /%s/%s by %s\n",
                    x,
                    Stars[star]->name,
                    Stars[star]->pnames[p],
                    races[who - 1]->name);
        }

        push_telegram(1, 0, godbuf);
#endif

        return 0;
    }
}

void do_meta_infect(int who, planettype *p)
{
    int owner;
    int x;
    int y;
    double military;
    int converted_civilians;
    int unconverted_civilians;

    getsmap(Smap, p);
    PermuteSects(p);
    memset((char *)Sectinfo, 0, sizeof(Sectinfo));
    x = int_rand(0, p->Maxx - 1);
    y = int_rand(0, p->Maxy - 1);
    owner = Sector(*p, x, y).owner;

    /*
     * HUTm Kharush converting existing civilians are back! Constants are in
     * hdrs/tweakables.h
     */
    if (!owner) {
        p->info[who - 1].explored = 1;
        p->info[who - 1].numsectsowned += 1;
        Sector(*p, x, y).popn = races[who - 1]->number_sexes;
        Sector(*p, x, y).owner = who;
        Sector(*p, x, y).condition = sector(*p, x, y).type;

#ifdef POD_TERRAFORM
        Sector(*p, x, y).condition = races[who - 1]->likesbest;
#endif

        putsmap(Smap, p);
    } else if (who != owner) {
        military = (double)Sector(*p, x, y).troops;

        converted_civilians = races[who - 1]->number_sexes + ((int)((double)Sector(*p, x, y).popn) * ABSORB_RATE * pow((MILITARY_PROPORTION * military) / ((MILITARY_PROPORTION * military) + (double)Sector(*p, x, y).popn), MILITARY_WEIGHT));

        unconverted_civilians = Sector(*p, x, y).popn - converted_civilians + races[who - 1]->number_sexes;

        if ((military * (double)races[owner - 1]->fighters * 10.0) >= ((double)converted_civilians * (double)races[who - 1]->fighters)) {
            /* Military wins */
            military -= (((double)converted_civilians * (double)races[who - 1]->fighters) / ((double)races[owner - 1]->fighters * 10.0));

            if (military < 1.0) {
                military = 0.0;
            }

            Sector(*p, x, y).troops = (int)military;
            Sector(*p, x, y).popn = unconverted_civilians;

            /* No survivors */
            if (!military && !unconverted_civilians) {
                Sector(*p, x, y).owner = 0;

                if (p->info[owner - 1].numsectsowned) {
                    p->info[owner - 1].numsectsowned -= 1;
                }
            }

            putsmap(Smap, p);
        } else {
            /* Podder wins */
            converted_civilians -= (int)((military * (double)races[owner - 1]->fighters * 10.0) / (double)races[who - 1]->fighters);

            if (converted_civilians < 1) {
                converted_civilians = 0;
            }

            Sector(*p, x, y).troops = 0;
            Sector(*p, x, y).popn = converted_civilians;

            if (p->info[owner - 1].numsectsowned) {
                p->info[owner - 1].numsectsowned -= 1;
            }

            /* No survivors */
            if (!converted_civilians) {
                Sector(*p, x, y).owner = 0;
            } else {
                p->info[who - 1].explored = 1;
                p->info[who - 1].numsectsowned += 1;
                Sector(*p, x, y).owner = who;
                Sector(*p, x, y).condition = Sector(*p, x, y).type;

#ifdef POD_TERRAFORM
                Sector(*p, x, y).condition = races[who - 1]->likesbest;

#endif
            }

            putsmap(Smap, p);
        }
    }

    /*
     * Old code starts
     *
     *     if (!owner
     *         || ((who != owner)
     *             && ((double)int_rand(1, 100) > (100.0 * (1.0 - exp(-(double)((Sector(*p, x, y).troops * races[owner - 1]->fighters) / 50.0))))))) {
     *         p->info[who - 1].explored = 1;
     *         p->info[who - 1].numsectsowned += 1;
     *         Sector(*p, x, y).troops = 0;
     *         Sector(*p, x, y).popn = races[who - 1]->number_sexes;
     *         Sector(*p, x, y).owner = who;
     *         Sector(*p, x, y).condition = Sector(*p, x, y).type;

     * #ifdef POD_TERRAFORM
     *         Sector(*p, x, y).condition = races[who - 1]->likesbest;
     * #endif

     *         putsmap(Smap, p);
     *     }
     *
     * Old code ends
     */
}
