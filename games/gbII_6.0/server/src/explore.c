/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
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
 * The GNU General Public License is contains in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * explore.c -- Display systems/worlds explored (this command is written by Dan
 *              Corrin, dan@geomech.engrg.uwo.ca)
 *
 * Thu Apr 11 02:19:44 MDT 1991
 *   Make mods to this prior, but forget the header. :) Evan Koffler
 *
 * $Header: /var/cvs/gbp/GB+/user/explore.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "explore.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "client.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

#include "csp_explore.h"
#include "tech.h"

extern char Psymbol[];
extern char *Planet_types[];

extern void colonies_at_star(int, int, racetype *, int, int);
extern void star_location(int, int, int);
extern void tech_report_star(int, int, startype *, int, int *, double *, double *);

void colonies_at_star(int playernum,
                      int governor,
                      racetype *race,
                      int star,
                      int mode)
{
    int i;
    int j;
    planettype *pl;
    racetype *r;

    getstar(&Stars[star], star);

    if (!isset(Stars[star]->explored, playernum)) {
        return;
    }

    r = races[playernum - 1];

    for (i = 0; i < Stars[star]->numplanets; ++i) {
        getplanet(&pl, star, i);

        if (pl->info[playernum - 1].explored
            && pl->info[playernum - 1].numsectsowned
            && (!governor
                || (Stars[star]->governor[playernum - 1] == governor)
                || (r->governor[governor].rank == GENERAL))) {
            switch (mode) {
            case -1: /* Combined report */
            case 0: /* Colonies */
                sprintf(buf,
                        " %c %4.4s/%-4.4s%c%1d%3d%5d%7ld%3d%6d%5d%6d %3d/%-3d%3.0f/%-3d%3d/%-3d",
                        Psymbol[pl->type],
                        Stars[star]->name,
                        Stars[star]->pnames[i],
                        pl->info[playernum - 1].autorep ? '*' : ' ',
                        Stars[star]->governor[playernum - 1],
                        pl->info[playernum - 1].numsectsowned,
                        pl->info[playernum - 1].tech_invest,
                        pl->info[playernum - 1].popn,
                        pl->info[playernum - 1].crystals,
                        pl->info[playernum - 1].resource,
                        pl->info[playernum - 1].destruct,
                        pl->info[playernum - 1].fuel,
                        pl->info[playernum - 1].tax,
                        pl->info[playernum - 1].newtax,
                        compatibility(pl, race),
                        pl->conditions[TOXIC],
                        pl->info[playernum - 1].comread,
                        pl->info[playernum - 1].mob_set);

                notify(playernum, governor, buf);

                for (j = 1; j <= Num_races; ++j) {
                    if ((j != playernum)
                        && (pl->info[j - 1].numsectsowned > 0)) {
                        sprintf(buf, " %d", j);
                        notify(playernum, governor, buf);
                    }
                }

                notify(playernum, governor, buf);

                if (mode == 0) {
                    break;
                }

                /* Fall through if mode == -1 */
            case 1: /* Production */
                sprintf(buf,
                        " %c %4.4s/%-4.4s%c%3d%8.4f%8ld%3d%6d%5d%6d %6ld  %3d%8.2f\n",
                        Psymbol[pl->type],
                        Stars[star]->name,
                        Stars[star]->pnames[i],
                        pl->info[playernum - 1].autorep ? '*' : ' ',
                        Stars[star]->governor[playernum - 1],
                        pl->info[playernum - 1].prod_tech,
                        pl->total_resources,
                        pl->info[playernum - 1].prod_crystals,
                        pl->info[playernum - 1].prod_res,
                        pl->info[playernum - 1].prod_dest,
                        pl->info[playernum - 1].prod_fuel,
                        pl->info[playernum - 1].prod_money,
                        pl->info[playernum - 1].tox_thresh,
                        pl->info[playernum - 1].est_production);

                notify(playernum, governor, buf);

                break;
            default:

                break;
            }
        }

        free(pl);
    }
}

void colonies(int playernum, int governor, int apcount, int mode, orbitinfo *unused5)
{
    int i;
    int star;
    racetype *race;
    placetype where;

    switch (mode) {
    case -1:
        notify(playernum,
               governor,
               "          ======== Colonies Prod Report ========\n");

        notify(playernum,
               governor,
               "  Planet     gov sec tech    popn  x   res  des  fuel  tax  cmpt/tox mod   Aliens\n");

        notify(playernum,
               governor,
               "               tox  deposit\n");

        break;
    case 0:
        notify(playernum,
               governor,
               "          ========= Colonization Report =========\n");

        notify(playernum,
               governor,
               "  Planet   gov sec tech  popn  x   res  des  fuel   tax cmpt/tox  mob Aliens\n");

        break;
    case 1:
        notify(playernum,
               governor,
               "          =========== Production Report ===========\n");

        notify(playernum,
               governor,
               "  Planet     gov    tech deposit  x   res  des  fuel    tax    tox  est prod\n");

        break;
    default:
        notify(playernum,
               governor,
               "          ============== Unknown Report ==============\n");

        return;
    }

    race = races[playernum - 1];
    getsdata(&Sdata);

    if (argn < 2) {
        for (star = 0; star < Sdata.numstars;++star) {
            colonies_at_star(playernum, governor, race, star, mode);
        }
    } else {
        for (i = 1; i < argn; ++i) {
            where = Getplace(playernum, governor, args[i], 0);

            if (where.err
                || (where.level == LEVEL_UNIV)
                || (where.level == LEVEL_SHIP)) {
                sprintf(buf, "Bad location '%s'.\n", args[i]);
                notify(playernum, governor, buf);

                continue;
            } else {
                /* OK, a proper location */
                colonies_at_star(playernum, governor, race, where.snum, mode);
            }
        }
    }

    notify(playernum, governor, "\n");
}

void distance(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    placetype from;
    placetype to;
    planettype *p;
    double x0;
    double y0;
    double x1;
    double y1;
    double dist;
    shiptype *ship;

    if (argn < 3) {
        notify(playernum, governor, "Syntax: 'distance <from> <to>'.\n");

        return;
    }

    from = Getplace(playernum, governor, args[1], 1);

    if (from.err) {
        if (isset(Stars[from.snum]->explored, playernum)
            || races[playernum - 1]->God) {
            sprintf(buf, "Bad scope '%s'.\n", args[1]);
        } else {
            sprintf(buf, "Nice try.\n");
        }

        notify(playernum, governor, buf);

        return;
    }

    to = Getplace(playernum, governor, args[2], 1);

    if (to.err) {
        if (isset(Stars[to.snum]->explored, playernum)
            || races[playernum - 1]->God) {
            sprintf(buf, "Bad scope '%s'.\n", args[2]);
        } else {
            sprintf(buf, "Nice try.\n");
        }

        notify(playernum, governor, buf);

        return;
    }

    x0 = 0.0;
    y0 = 0.0;
    x1 = 0.0;
    y1 = 0.0;

    /* Get position in absolute units */
    if (from.level == LEVEL_SHIP) {
        getship(&ship, from.shipno);

        if ((ship->owner != playernum) && !races[playernum - 1]->God) {
            notify(playernum, governor, "Nice try.\n");
            free(ship);

            return;
        }

        x0 = ship->xpos;
        y0 = ship->ypos;
        free(ship);
    } else if (from.level == LEVEL_PLAN) {
        getplanet(&p, (int)from.snum, (int)from.pnum);

        if (!isset(Stars[from.snum]->explored, playernum)
            && !races[playernum - 1]->God) {
            notify(playernum, governor, "Nice try.\n");
            free(p);

            return;
        }

        x0 = p->xpos + Stars[from.snum]->xpos;
        y0 = p->ypos + Stars[from.snum]->ypos;
        free(p);
    } else if (from.level == LEVEL_STAR) {
        x0 = Stars[from.snum]->xpos;
        y0 = Stars[from.snum]->ypos;
    }

    if (to.level == LEVEL_SHIP) {
        getship(&ship, to.shipno);

        if ((ship->owner != playernum) && !races[playernum - 1]->God) {
            notify(playernum, governor, "Nice try dude.\n");
            free(ship);

            return;
        }

        x1 = ship->xpos;
        y1 = ship->ypos;
        free(ship);
    } else if (to.level == LEVEL_PLAN) {
        getplanet(&p, (int)to.snum, (int)to.pnum);

        if (!isset(Stars[to.snum]->explored, playernum)
            && !races[playernum - 1]->God) {
            notify(playernum, governor, "Nice try.\n");
            free(p);

            return;
        }

        x1 = p->xpos + Stars[to.snum]->xpos;
        y1 = p->ypos + Stars[to.snum]->ypos;
        free(p);
    } else if (to.level == LEVEL_STAR) {
        x1 = Stars[to.snum]->xpos;
        y1 = Stars[to.snum]->ypos;
    }

    /* Compute the distance */
    dist = sqrt(Distsq(x0, y0, x1, y1));
    sprintf(buf, "Distance = %f\n", dist);
    notify(playernum, governor, buf);
}

void star_locations(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int i;
    double dist;
    double x;
    double y;
    int max;

    x = Dir[playernum - 1][governor].lastx[1];
    y = Dir[playernum - 1][governor].lasty[1];

    if (argn > 1) {
        max = atoi(args[1]);
    } else {
        max = 999999;
    }

    for (i = 0; i < Sdata.numstars; ++i) {
        dist = sqrt(Distsq(Stars[i]->xpos, Stars[i]->ypos, x, y));

        if ((int)dist <= max) {
            sprintf(buf,
                    "(%2d) %20.20s (%8.0f,%8.0f) %7.0f\n",
                    i + 1,
                    Stars[i]->name,
                    Stars[i]->xpos,
                    Stars[i]->ypos,
                    dist);

            notify(playernum, governor, buf);
        }
    }
}

void exploration(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int star;
    int starq;
    int i;
    int j;
    planettype *pl;
    placetype where;
    racetype *race;

    starq = -1;

    if (client_can_understand(playernum, governor, CSP_EXPLORE_INTRO)) {
        CSP_exploration(playernum, governor);
    } else {
        if (argn == 2) {
            where = Getplace(playernum, governor, args[1], 0);

            if (where.err) {
                sprintf(buf, "explore: bad scope.\n");
                notify(playernum, governor, buf);

                return;
            } else {
                if ((where.level == LEVEL_SHIP)
                    || (where.level == LEVEL_UNIV)) {
                    sprintf(buf, "Bad scope '%s'.\n", args[1]);
                    notify(playernum, governor, buf);

                    return;
                }

                starq = where.snum;
            }

            race = races[playernum - 1];
            getsdata(&Sdata);
            sprintf(buf, "         ========== Exploration Report ==========\n");
            notify(playernum, governor, buf);
            sprintf(buf,
                    " Global action points: [%2d]\n",
                    Sdata.AP[playernum - 1]);

            notify(playernum, governor, buf);

            sprintf(buf,
                    " Star  (stability)[AP]   #  Planet [Attributes] Type (compatibility)\n");

            notify(playernum, governor, buf);

            for (star = 0; star < Sdata.numstars; ++star) {
                if ((starq == -1) || (starq == star)) {
                    getstar(&Stars[star], star);

                    if (isset(Stars[star]->explored, playernum)) {
                        for (i = 0; i < Stars[star]->numplanets; ++i) {
                            getplanet(&pl, star, i);

                            if (i == 0) {
                                if (race->tech >= TECH_SEE_STABILITY) {
                                    sprintf(buf,
                                            "\n%13s (%2d)[%2d]\n",
                                            Stars[star]->name,
                                            Stars[star]->stability,
                                            Stars[star]->AP[playernum - 1]);

                                    notify(playernum, governor, buf);
                                } else {
                                    sprintf(buf,
                                            "\n%13s (/?/?)[%2d]\n",
                                            Stars[star]->name,
                                            Stars[star]->AP[playernum - 1]);

                                    notify(playernum, governor, buf);
                                }
                            }

                            sprintf(buf,
                                    "  #%d. %-15s [",
                                    i + 1,
                                    Stars[star]->pnames[i]);

                            notify(playernum, governor, buf);

                            if (pl->info[playernum - 1].explored) {
                                sprintf(buf, "Ex ");
                                notify(playernum, governor, buf);

                                if (pl->info[playernum - 1].autorep) {
                                    sprintf(buf, "Rep ");
                                    notify(playernum, governor, buf);
                                }

                                if (pl->info[playernum - 1].numsectsowned) {
                                    sprintf(buf, "Inhab ");
                                    notify(playernum, governor, buf);
                                }

                                if (pl->slaved_to) {
                                    sprintf(buf, "SLAVED ");
                                    notify(playernum, governor, buf);
                                }

                                for (j = 1; j <= Num_races; ++j) {
                                    if ((j != playernum) && pl->info[j - 1].numsectsowned) {
                                        sprintf(buf, "%d ", j);
                                        notify(playernum, governor, buf);
                                    }
                                }

                                if (pl->conditions[TOXIC] > 70) {
                                    sprintf(buf, "TOXIC ");
                                    notify(playernum, governor, buf);
                                }

                                sprintf(buf,
                                        "] %s %2.0f%%\n",
                                        Planet_types[pl->type],
                                        compatibility(pl, race));

                                notify(playernum, governor, buf);
                            } else {
                                sprintf(buf, "No Data ]\n");
                                notify(playernum, governor, buf);
                            }

                            free(pl);
                        }
                    }
                }
            }
        } else {
            /* if (argn != 2) -mfw */
            sprintf(buf, "Explore where...what star?\n");
            notify(playernum, governor, buf);
        }
    }
}

void tech_status(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int star;
    int k;
    placetype where;
    double total_gain = 0.0;
    double total_max_gain = 0.0;
    int total_invest = 0;

    getsdata(&Sdata);

    sprintf(buf, "             ========== Technology Report ==========\n\n");
    notify(playernum, governor, buf);

    if (argn == 1 ) {
        for (star = 0; star < Sdata.numstars; ++star) {
            getstar(&Stars[star], star);

            tech_report_star(playernum,
                             governor,
                             Stars[star],
                             star,
                             &total_invest,
                             &total_gain,
                             &total_max_gain);
        }
    } else {
        /* Several arguments */
        for (k = 1; k < argn; ++k) {
            where = Getplace(playernum, governor, args[k], 0);

            if (where.err
                || (where.level == LEVEL_UNIV)
                || (where.level == LEVEL_SHIP)) {
                sprintf(buf, "Bad location '%s'.\n", args[k]);
                notify(playernum, governor, buf);

                continue;
            } else {
                /* Ok, a proper location */
                star = where.snum;
                getstar(&Stars[star], star);

                tech_report_star(playernum,
                                 governor,
                                 Stars[star],
                                 star,
                                 &total_invest,
                                 &total_gain,
                                 &total_max_gain);
            }
        }
    }

    sprintf(buf, "       Total Popn: %7ld\n", Power[playernum - 1].popn);
    notify(playernum, governor, buf);

    sprintf(buf,
            "Tech: %31d%8.3f%8.3f\n",
            total_invest,
            total_gain,
            total_max_gain);

    notify(playernum, governor, buf);
}

void tech_report_star(int playernum,
                      int governor,
                      startype *star,
                      int snum,
                      int *t_invest,
                      double *t_gain,
                      double *t_max_gain)
{
    int i;
    planettype *pl;
    char str[200];
    double gain;
    double max_gain;

    if (isset(star->explored, playernum)
        && (!governor || (star->governor[playernum - 1] == governor))) {
        for (i = 0; i < star->numplanets; ++i) {
            getplanet(&pl, snum, i);

            if (pl->info[playernum - 1].explored
                && pl->info[playernum - 1].numsectsowned) {
                sprintf(str, "%s/%s", star->name, star->pnames[i]);

                gain = tech_prod((int)pl->info[playernum - 1].tech_invest,
                                 (int)pl->info[playernum - 1].popn);

                max_gain = tech_prod((int)pl->info[playernum - 1].prod_res,
                                     (int)pl->info[playernum - 1].popn);

                sprintf(buf,
                        "%16.16s %10ld%10d%8.3f%8.3f %s\n",
                        str,
                        pl->info[playernum - 1].popn,
                        pl->info[playernum - 1].tech_invest,
                        gain,
                        max_gain,
                        pl->info[playernum - 1].autorep ? " True" : " False");

                notify(playernum, governor, buf);
                *t_invest += pl->info[playernum - 1].tech_invest;
                *t_gain += gain;
                *t_max_gain += max_gain;
            }

            free(pl);
        }
    }
}
