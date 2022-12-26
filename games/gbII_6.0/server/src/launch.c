/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version
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
 * launch.c -- Launch or undock a ship (also undock)
 *
 * #ident  "@(#)launch.c       1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/launch.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "launch.h"

#include <stdlib.h>

#include "buffers.h"
#include "doship.h"
#include "files_shl.h"
#include "GB_server.h"
#include "lists.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ranks.h"
#include "ships.h"
#include "shlmisc.h"
#include "vars.h"

#include "fire.h"
#include "land.h"
#include "load.h"

/*
 * In launch, when ship is launched it is checked whether launched ship was
 * dhuttle. If so, a little more information is provided. Kharush.
 */
void launch(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int sh2;
    shiptype *s;
    shiptype *s2;
    planettype *p;
    int shipno;
    int i;
    int nextshipno;
    int roll = 0;
    int max_resource = 0;
    double fuel;
    char *ship_to_launch;
    char tmp[10];

    /*
     * Added LEVEL_SHIP check below, if player is at ship scope they should just
     * be able to type launch to launch the ship at their scope. -mfw
     */
    if (argn < 2) {
        if (Dir[playernum - 1][governor].level != LEVEL_SHIP) {
            notify(playernum, governor, "Launch what?\n");

            return;
        } else {
            sprintf(tmp, "#%d", Dir[playernum - 1][governor].shipno);
            ship_to_launch = tmp;
        }
    } else {
        ship_to_launch = args[1];
    }

    nextshipno = start_shiplist(playernum, governor, ship_to_launch);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {

        if (in_list(playernum, ship_to_launch, s, &nextshipno)) {
            if (!s->max_speed && landed(s)) {
                sprintf(buf, "That ship is not designed to be launched.\n");
                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!s->docked && (s->whatorbits != LEVEL_SHIP)) {
                sprintf(buf, "%s is not landed or docked.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!landed(s)) {
                apcount = 0;
            }

            if (s->type == OTYPE_FACTORY) {
                max_resource = Shipdata[s->type][ABIL_CARGO];
            } else {
                max_resource = s->max_resource;
            }

            if (landed(s) && (s->resource > max_resource)) {
                sprintf(buf, "%s is too overloaded to launch.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (s->whatorbits == LEVEL_SHIP) {
                /* Factories cannot be launched once turns on. Maarten */
                if ((s->type == OTYPE_FACTORY) && s->on) {
                    notify(playernum,
                           governor,
                           "Factories cannot be launched once turned on.\n");

                    notify(playernum, governor, "Consider using 'scrap'.\n");
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                sh2 = s->destshipno;
                getship(&s2, sh2);

                if (landed(s2)) {
                    remove_sh_ship(s, s2);
                    getplanet(&p, (int)s2->storbits, (int)s2->pnumorbits);
                    insert_sh_plan(p, s);
                    putplanet(p, (int)s2->storbits, (int)s2->pnumorbits);
                    s->storbits = s2->storbits;
                    s->pnumorbits = s2->pnumorbits;
                    s->deststar = s2->deststar;
                    s->xpos = s2->xpos;
                    s->ypos = s2->ypos;
                    s->land_x = s2->land_x;
                    s->land_y = s2->land_y;
                    s->docked = 1;
                    s->whatdest = LEVEL_PLAN;

                    if ((s2->mass - s->mass) < s2->base_mass) {
                        s2->mass = s2->base_mass;
                    } else {
                        s2->mass = s2->mass - s->mass;
                    }

                    if ((unsigned short)s->size > s2->hanger) {
                        s2->hanger = 0;
                    } else {
                        s2->hanger = s2->hanger - (unsigned short)s->size;
                    }

                    sprintf(buf,
                            "Landed on %s/%s.\n",
                            Stars[s->storbits]->name,
                            Stars[s->storbits]->pnames[s->pnumorbits]);

                    notify(playernum, governor, buf);
                    free(p);
                    putship(s);
                    putship(s2);
                } else if (s2->whatorbits == LEVEL_PLAN) {
                    remove_sh_ship(s, s2);

                    /*
                     * // Got rid of Dhuttle, silly... -mfw
                     * if (s->type == STYPE_DHUTTLE) {
                     *     sprintf(buf,
                     *             "%s launched from %s.\nCreepy will return!\n",
                     *             Ship(s),
                     *             Ship(s2));
                     *
                     * } else {
                     *     sprintf(buf,
                     *             "%s launched from %s.\n",
                     *             Ship(s),
                     *             Ship(s2));
                     * }
                     */

                    sprintf(buf, "%s launched from %s.\n", Ship(s), Ship(s2));
                    notify(playernum, governor, buf);
                    s->xpos = s2->xpos;
                    s->ypos = s2->ypos;
                    s->docked = 0;
                    s->whatdest = LEVEL_UNIV;

                    if ((s2->mass - s->mass) < s2->base_mass) {
                        s2->mass = s2->base_mass;
                    } else {
                        s2->mass = s2->mass - s->mass;
                    }

                    if ((unsigned short)s->size > s2->hanger) {
                        s2->hanger = 0;
                    } else {
                        s2->hanger = s2->hanger - (unsigned short)s->size;
                    }

                    getplanet(&p, (int)s2->storbits, (int)s2->pnumorbits);
                    insert_sh_plan(p, s);
                    s->storbits = s2->storbits;
                    s->pnumorbits = s2->pnumorbits;
                    putplanet(p, (int)s2->storbits, (int)s2->pnumorbits);

                    sprintf(buf,
                            "Orbiting %s/%s.\n",
                            Stars[s->storbits]->name,
                            Stars[s->storbits]->pnames[s->pnumorbits]);

                    notify(playernum, governor, buf);
                    free(p);
                    putship(s);
                    putship(s2);
                } else if (s2->whatorbits == LEVEL_STAR) {
                    remove_sh_ship(s, s2);

                    /*
                     * // Got rid of Dhuttle, silly... -mfw
                     * if (s->type == STYPE_DHUTTLE) {
                     *     sprintf(buf,
                     *             "%s launched from %s.\nCreepy will return!\n",
                     *             Ship(s),
                     *             ship(s2));
                     * } else {
                     *     sprintf(buf,
                     *             "%s launched from %s.\n",
                     *             Ship(s),
                     *             Ship(s2));
                     * }
                     */

                    sprintf(buf, "%s launched form %s.\n", Ship(s), Ship(s2));

                    notify(playernum, governor, buf);
                    s->xpos = s2->xpos;
                    s->ypos = s2->ypos;
                    s->docked = 0;
                    s->whatdest = LEVEL_UNIV;

                    if ((s2->mass - s->mass) < s2->base_mass) {
                        s2->mass = s2->base_mass;
                    } else {
                        s2->mass = s2->mass - s->mass;
                    }

                    if ((unsigned short)s->size > s2->hanger) {
                        s2->hanger = 0;
                    } else {
                        s2->hanger = s2->hanger - (unsigned short)s->size;
                    }

                    getstar(&Stars[s2->storbits], (int)s2->storbits);
                    insert_sh_star(Stars[s2->storbits], s);
                    s->storbits = s2->storbits;
                    putstar(Stars[s2->storbits], (int)s2->storbits);
                    sprintf(buf, "Orbiting %s.\n", Stars[s->storbits]->name);
                    notify(playernum, governor, buf);
                    putship(s);
                    putship(s2);
                } else if (s2->whatorbits == LEVEL_UNIV) {
                    remove_sh_ship(s, s2);

                    /*
                     * // Got rid of Dhuttle, silly... -mfw
                     * if (s->type == STYPE_DHUTTLE) {
                     *     sprintf(buf,
                     *             "%s launched from %s.\nCreepy will return!\n",
                     *             Ship(s),
                     *             Ship(s2));
                     * } else {
                     *     sprintf(buf,
                     *             "%s launched form %s.\n",
                     *             Ship(s),
                     *             ship(s2));
                     * }
                     */

                    sprintf(buf, "%s launched from %s.\n", Ship(s), Ship(s2));

                    notify(playernum, governor, buf);
                    s->xpos = s2->xpos;
                    s->ypos = s2->ypos;
                    s->docked = 0;
                    s->whatdest = LEVEL_UNIV;

                    if ((s2->mass - s->mass) < s2->base_mass) {
                        s2->mass = s2->base_mass;
                    } else {
                        s2->mass = s2->mass - s->mass;
                    }

                    if ((unsigned short)s->size > s2->hanger) {
                        s2->hanger = 0;
                    } else {
                        s2->hanger = s2->hanger - (unsigned short)s->size;
                    }

                    getsdata(&Sdata);
                    insert_sh_univ(&Sdata, s);
                    notify(playernum, governor, "Universe level.\n");
                    putsdata(&Sdata);
                    putship(s);
                    putship(s2);
                } else {
                    notify(playernum,
                           governor,
                           "You can't launch that ship.\n");

                    free(s2);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                free(s2);
                free(s);
            } else if (s->whatdest == LEVEL_SHIP) {
                sh2 = s->destshipno;
                getship(&s2, sh2);

                if (s2->whatorbits == LEVEL_UNIV) {
                    if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    } else {
                        deductAPs(playernum, governor, apcount, 0, 1);
                    }
                } else {
                    if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    } else {
                        deductAPs(playernum, governor, apcount, (int)s->storbits, 0);
                    }
                }

                s->docked = 0;
                s->whatdest = LEVEL_UNIV;
                s->destshipno = 0;
                s2->docked = 0;
                s2->whatdest = LEVEL_UNIV;
                s2->destshipno = 0;
                sprintf(buf, "%s undocked from %s.\n", Ship(s), Ship(s2));
                notify(playernum, governor, buf);
                putship(s);
                putship(s2);
                free(s);
                free(s2);
            } else {
                if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                    free(s);

                    return;
                } else {
                    deductAPs(playernum, governor, apcount, (int)s->storbits, 0);
                }

                if (s->damage) {
                    sprintf(buf,
                            "Attempting to launch ship with %d%% damage...\n",
                            (int)s->damage);

                    notify(playernum, governor, buf);
                }

                /* Adjust x,y position to absolute coordinates */
                getplanet(&p, (int)s->storbits, (int)s->pnumorbits);

                sprintf(buf,
                        "Planet /%s/%s has gravity field of %.2f\n",
                        Stars[s->storbits]->name,
                        Stars[s->storbits]->pnames[s->pnumorbits],
                        gravity(p));

                notify(playernum, governor, buf);

                s->xpos = Stars[s->storbits]->xpos + p->xpos + (double)int_rand((int)(-DIST_TO_LAND / 4), (int)(DIST_TO_LAND / 4));

                s->ypos = Stars[s->storbits]->ypos + p->ypos + (double)int_rand((int)(-DIST_TO_LAND / 4), (int)(DIST_TO_LAND / 4));

                /* Subtract fuel from ship */
                fuel = gravity(p) * s->mass * LAUNCH_GRAV_MASS_FACTOR;

                if (s->fuel < fuel) {
                    sprintf(buf,
                            "%s does not have enough fuel! (%.1f)\n",
                            Ship(s),
                            fuel);

                    notify(playernum, governor, buf);
                    free(p);
                    free(s);

                    return;
                }

                /* Check to see if the ship crashes from damage */
                if (crash(s, fuel)) {
                    /* Damaged ships stand chance of exploding on launch */
                    if (roll) {
                        sprintf(buf, "You rolled a %d!\n", roll);
                        notify(playernum, governor, buf);
                    }

                    sprintf(buf,
                            "%s explodes upon launch!\nPollutants are released into the atmosphere.\n",
                            Ship(s));

                    notify(playernum, governor, buf);

                    p->conditions[TOXIC] += (s->fuel / 10);

                    if (p->conditions[TOXIC] > 100) {
                        p->conditions[TOXIC] = 100;
                    }

                    /* Send message to anyone that's there */
                    sprintf(buf,
                            "%s observed exploding upon launch from /%s/%s.\n",
                            Ship(s),
                            Stars[s->storbits]->name,
                            Stars[s->storbits]->pnames[s->pnumorbits]);

                    for (i = 1; i <= Num_races; ++i) {
                        if (p->info[i - 1].numsectsowned && (i != playernum)) {
                            notify(i,
                                   (int)Stars[s->storbits]->governor[i - 1],
                                   buf);
                        }
                    }

                    remove_sh_plan(s);
                    kill_ship((int)s->owner, s);
                    putship(s);
                    free(s);

                    putplanet(p, (int)s->storbits, (int)s->pnumorbits);
                    free(p);

                    return;
                }

                use_fuel(s, fuel);
                s->docked = 0;
                /* No destination */
                s->whatdest = LEVEL_UNIV;

                switch (s->type) {
                case OTYPE_CANIST:
                case OTYPE_GREEN:
                    s->special.timer.count = 0;

                    break;
                default:

                    break;
                }

                s->notified = 0;
                putship(s);

                if (!p->explored) {
                    /*
                     * Not yet explored by owner; space exploration causes the
                     * player to see a whole map
                     */
                    p->explored = 1;
                    putplanet(p, (int)s->storbits, (int)s->pnumorbits);
                }

                /*
                 * // Got rid of Dhuttle, silly... -mfw
                 * if (s->type == STYPE_DHUTTLE) {
                 *     sprintf(buf,
                 *             "%s observed launching from planet /%s/%s.\nCreepy will return!\n",
                 *             Ship(s),
                 *             Stars[s->storbits]->name,
                 *             Stars[s->storbits]->pnames[s->numorbits]);
                 * } else {
                 *     sprintf(buf,
                 *             "%s observed launching from planet /%s/%s.\n",
                 *             Ship(s),
                 *             Stars[s->storbits]->name,
                 *             Stars[s->storbits]->pnames[s->pnumorbits]);
                 * }
                 */

                sprintf(buf,
                        "%s observed launching from planet /%s/%s.\n",
                        Ship(s),
                        Stars[s->storbits]->name,
                        Stars[s->storbits]->pnames[s->pnumorbits]);

                for (i = 1; i <= Num_races; ++i) {
                    if (p->info[i - 1].numsectsowned && (i != playernum)) {
                        notify(i,
                               (int)Stars[s->storbits]->governor[i - 1],
                               buf);
                    }
                }

                free(p);
                sprintf(buf, "%s launched from planet,", Ship(s));
                notify(playernum, governor, buf);

                /*
                 * // Got rid of Dhuttle, silly... -mfw
                 * if (s->type == STYPE_DHUTTLE) {
                 *     sprintf(buf,
                 *             " using %.1f fuel.\nCreepy will return!\n",
                 *             fuel);
                 * } else {
                 *     sprintf(buf, " using %.1f fuel.\n", fuel);
                 * }
                 */

                sprintf(buf, " using %.1f fuel.\n", fuel);
                notify(playernum, governor, buf);

                switch (s->type) {
                case OTYPE_CANIST:
                    notify(playernum,
                           governor,
                           "A cloud of dust envelopes your planet.\n");

                    break;
                case OTYPE_GREEN:
                    notify(playernum,
                           governor,
                           "Green house gases surround the planet.\n");

                    break;
                default:

                    break;
                }

                free(s);
            }
        } else {
            free(s);
        }

        shipno = do_shiplist(&s, &nextshipno);
    }
}
