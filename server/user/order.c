/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
 * See GB_coypright.h for additional authors and details.
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
 * order.c -- Give orders to ship
 *
 * #ident  "@(#)order.c        1.16 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/order.c,v 1.8 2007/07/06 18:09:34 gbp Exp $
 */

#include <curses.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "orders.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

void order(int, int, int);
void mk_expl_aimed_at(int, int, shiptype *);
void DispOrdersHeader(int, int);
void DispOrders(int, int, shiptype *);
void route(int, int, int);
int get_order_type(char *);

void order(int playernum, int governor, int apcount)
{
    int shipno;
    int nextshipno;
    int hdrshown = 0;
    int ordertype = ORD_NULL;
    shiptype *ship;
    // char tmp[15];

    /* Display all ship orders */
    if (argn == 1) {
        /*
         * Modification to show current orders of ship if at the same scope
         * -mfw
         */
        if (Dir[playernum - 1][governor].level == LEVEL_SHIP) {
            /*
             * Changed mind for now, would have to account for giving order too
             *
             * sprintf(tmp, "#%d", Dir[playernum - 1][governor].shipno);
             * nextshipno = start_shiplist(playernum, governor, tmp);
             */
        } else {
            nextshipno = start_shiplist(playernum, governor, "");
        }

        shipno = do_shiplist(&ship, &nextshipno);

        while (shipno) {
            if (((ship->owner == playernum) && (authorized(governor, ship)))
                || races[playernum - 1]->God) {
                if (!hdrshown) {
                    DispOrdersHeader(playernum, governor);
                    hdrshown = 1;
                }

                DispOrders(playernum, governor, ship);
                free(ship);
            } else {
                free(ship);
            }

            shipno = do_shiplist(&ship, &nextshipno);
        }
    } else if (argn >= 2) {
        nextshipno = start_shiplist(playernum, governor, args[1]);

        if (argn > 2) {
            ordertype = get_order_type(args[2]);
        }

        shipno = do_shiplist(&ship, &nextshipno);

        while (shipno) {
            if (in_list(playernum, args[1], ship, &nextshipno)
                && (authorized(governor, ship) || races[playernum - 1]->God)) {
                if (!hdrshown) {
                    DispOrdersHeader(playernum, governor);
                    hdrshown = 1;
                }

                if (argn > 2) {
                    give_orders(playernum, governor, apcount, ship, ordertype);
                }

                DispOrders(playernum, governor, ship);
                free(ship);
            } else {
                free(ship);
            }

            shipno = do_shiplist(&ship, &nextshipno);
        }
    } else {
        notify(playernum, governor, "I don't understand what you mean.\n");
    }

    /* No ships found */
    if (!hdrshown) {
        notify(playernum, governor, "No applicable ships.\n");
    }
}

void do_inf_ord(int playernum, int governor, shiptype *ship)
{
    int maxx;
    int maxy;
    int x;
    int y;
    int i;
    int f;
    struct inf_setting *infs;
    planettype *planet;

    if (match(args[3], "usage") || match(args[3], "help")) {
        inf_usage(playernum, governor, "report");
        inf_usage(playernum, governor, "eff");
        inf_usage(playernum, governor, "fert");
        inf_usage(playernum, governor, "atmos");

        return;
    } else if (match(args[3], "report")) {
        if (argn < 4) {
            inf_usage(playernum, governor, "report");

            return;
        }

        if (match(args[4], "on")) {
            ship->inf.wants_reports = 1;
        } else {
            ship->inf.wants_reports = 0;
        }

        if (ship->inf.wants_reports) {
            sprintf(buf, "Reporting enabled\n");
        } else {
            sprintf(buf, "Reports disabled\n");
        }

        notify(playernum, governor, buf);

        return;
    } else if (match(args[3], "eff")) {
        if (argn < 4) {
            inf_usage(playernum, governor, "eff");

            return;
        }

        if (match(args[4], "limit")) {
            if (argn < 7) {
                inf_usage(playernum, governor, "eff");

                return;
            }

            if (!isdigit((unsigned char)args[5][0])) {
                inf_usage(playernum, governor, "eff");

                return;
            }

            /* Check bounds */
            sscanf(args[5], "%d,%d", &x, &y);
            getplanet(&planet, (int)ship->deststar, (int)ship->destpnum);
            maxx = planet->Maxx;
            maxy = planet->Maxy;
            free(planet);

            if ((x > maxx) || (y > maxy)) {
                sprintf(buf, "Invalid x,y for sector (%d,%d)\n", x, y);
                notify(playernum, governor, buf);

                return;
            }

            f = 0;

            for (i = 0; i < INF_MAX_TARGETS; ++i) {
                infs = &ship->inf.eff_targets[i];

                if ((infs->x == x) && (infs->y == y)) {
                    f = 1;
                    infs->max = atoi(args[6]);

                    if (infs->max < 0) {
                        infs->max = 0;
                    }

                    if (infs->max > INF_MAX) {
                        infs->max = INF_MAX;
                    }

                    return;
                }
            }

            sprintf(buf, "No such target [%s]\n", args[5]);
            notify(playernum, governor, buf);

            return;
        }

        if (match(args[4], "target")) {
            if (argn < 6) {
                inf_usage(playernum, governor, "eff");

                return;
            }

            if (!isdigit((unsigned char)args[5][0])) {
                inf_usage(playernum, governor, "eff");

                return;
            }

            sscanf(args[5], "%d,%d", &x, &y);
            getplanet(&planet, (int)ship->deststar, (int)ship->destpnum);
            maxx = planet->Maxx;
            maxy = planet->Maxy;
            free(planet);

            if ((x > maxx) || (y > maxy)) {
                sprintf(buf, "Invalid x,y for sector (%d,%d)\n", x, y);
                notify(playernum, governor, buf);

                return;
            }

            f = 0;

            for (i = 0; i < INF_MAX_TARGETS; ++i) {
                infs = &ship->inf.eff_targets[i];


                /*
                 * Check for dup
                 *
                 * if ((x == infs->x) && (y == infs->y)) {
                 *     sprintf(buf, "That targets already set\n");
                 *     notify(playernum, governor, buf);
                 *
                 *     return;
                 * }
                 */

                if (infs->x == 999) {
                    /* Need to get sectors and check range */
                    infs->x = x;
                    infs->y = y;

                    if (argn >= 7) {
                        infs->max = atoi(args[6]);

                        if (infs->max > INF_MAX) {
                            infs->max = INF_MAX;
                        }
                    } else {
                        infs->max = INF_MAX;
                    }

                    f = 1;

                    return;
                }
            }

            if (f) {
                sprintf(buf,
                        "Eff Target set to %d,%d and eff limit set to %d%%\n",
                        infs->x,
                        infs->y,
                        infs->max);
            } else {
                sprintf(buf,
                        "Max targets of %d already set\n",
                        INF_MAX_TARGETS);
            }
        } else {
            ship->inf.eff_setting = atoi(args[4]);
            sprintf(buf, "Eff setting: %d\n", ship->inf.eff_setting);
        }

        notify(playernum, governor, buf);

        return;
    } else if (match(args[3], "fert")) {
        if (match(args[4], "limit")) {
            if (argn < 7) {
                inf_usage(playernum, governor, "fert");

                return;
            }

            sscanf(args[5], "%d,%d", &x, &y);
            getplanet(&planet, (int)ship->deststar, (int)ship->destpnum);
            maxx = planet->Maxx;
            maxy = planet->Maxy;
            free(planet);

            if ((x > maxx) || (y > maxy)) {
                sprintf(buf, "Invalid x,y for sector (%d,%d)\n", x, y);
                notify(playernum, governor, buf);

                return;
            }

            f = 0;

            for (i = 0; i < INF_MAX_TARGETS; ++i) {
                infs = &ship->inf.eff_targets[i];

                if ((infs->x == x) && (infs->y == y)) {
                    f = 1;
                    infs->max = atoi(args[6]);

                    if (infs->max < 0) {
                        infs->max = 0;
                    }

                    if (infs->max > INF_MAX) {
                        infs->max = INF_MAX;
                    }

                    return;
                }
            }

            sprintf(buf, "No such target [%s]\n", args[5]);
            notify(playernum, governor, buf);

            return;
        }

        if (match(args[4], "target")) {
            if (argn < 6) {
                inf_usage(playernum, governor, "fert");

                return;
            }

            sscanf(args[5], "%d,%d", &x, &y);
            getplanet(&planet, (int)ship->deststar, (int)ship->destpnum);
            maxx = planet->Maxx;
            maxy = planet->Maxy;
            free(planet);

            if ((x > maxx) || (y > maxy)) {
                sprintf(buf, "Invalid x,y for sector (%d,%d)\n", x, y);
                notify(playernum, governor, buf);

                return;
            }

            f = 0;

            for (i = 0; i < INF_MAX_TARGETS; ++i) {
                infs = &ship->inf.fert_targets[i];

                /*
                 * Check for dup
                 *
                 * if ((x == infs->x) && (y == infs->y)) {
                 *     sprintf(buf, "That target already set\n");
                 *     notify(playernum, governor, buf);
                 *
                 *     return;
                 * }
                 */

                if (infs->x == 999) {
                    infs->x = x;
                    infs->y = y;

                    if (argn >= 7) {
                        infs->max = atoi(args[6]);

                        if (infs->max > INF_MAX) {
                            infs->max = INF_MAX;
                        }
                    } else {
                        infs->max = INF_MAX;
                    }

                    f = 1;

                    break;
                }
            }

            if (f) {
                sprintf(buf,
                        "Fert Target set to %d,%d and fert limit set to %d%%\n",
                        infs->x,
                        infs->y,
                        infs->max);
            } else {
                sprintf(buf,
                        "Max targets of %d already set\n",
                        INF_MAX_TARGETS);
            }
        } else {
            ship->inf.fert_setting = atoi(args[4]);
            sprintf(buf, "Fert setting: %d\n", ship->inf.fert_setting);
        }

        notify(playernum, governor, buf);

        return;
    } else if (match(args[3], "atmosphere")) {
        if (!Atmos(races[playernum - 1])) {
            sprintf(buf,
                    "You need [%d] tech to turn this on\n",
                    (int)TECH_ATMOS);

            notify(playernum, governor, buf);

            return;
        }

        if (argn < 4) {
            inf_usage(playernum, governor, "atmos");

            return;
        }

        if (!isdigit((unsigned char)args[4][0])) {
            sprintf(buf, "Invalid number: %s\n", args[4]);
            notify(playernum, governor, buf);

            return;
        }

        if (atoi(args[4]) > INF_MAX_ATMO_SETTING) {
            sprintf(buf,
                    "Setting to high, maximum is %d.\n",
                    INF_MAX_ATMO_SETTING);
        } else {
            ship->inf.atmos_setting = atoi(args[4]);
            sprintf(buf, "Atmosphere setting: %d\n", ship->inf.atmos_setting);
        }

        notify(playernum, governor, buf);

        return;
    }

    inf_usage(playernum, governor, "eff");
    inf_usage(playernum, governor, "fert");
    inf_usage(playernum, governor, "atmos");
}

void inf_usage(int playernum, int governor, char *which)
{
    /* Usage for report, eff, fert, and atmos */
    if (match(which, "report")) {
        sprintf(buf, "USAGE: order <ship> inf report on/off\n");
    } else if (match(which, "eff")) {
        sprintf(buf, "USAGE: order <ship> inf eff <setting>\nor order <ship> inf eff limit <x>,<y> <limit>\nor order <ship> inf eff target <x>,<y> [limit]\n");
    } else if (match(which, "fert")) {
        sprintf(buf, "USAGE: order <ship> inf fert <setting>\nor order <ship> inf fert limit <x>,<y> <limit>\nor order <ship> inf fert target <x>,<y> [limit]\n");
    } else if (match(which, "atmos")) {
        sprintf(buf, "USAGE: order <ship> inf atmos <setting>\n");
    }

    notify(playernum, governor, buf);
}

void give_orders(int playernum,
                 int governor,
                 int apcount,
                 shiptype *ship,
                 int ordertype)
{
    int i = 0;
    int j = 0;
    int max_crew = 0;
    int x;
    int y;
    placetype where;
    placetype pl;
    planettype *planet;
    shiptype *tmpship;

    if (!ship->active) {
        sprintf(buf,
                "%s is irradiated (%d); it cannot be given orders.\n",
                Ship(ship),
                ship->rad);

        notify(playernum, governor, buf);

        return;
    }

    if (ship->type == OTYPE_FACTORY) {
        max_crew = Shipdata[ship->type][ABIL_MAXCREW] - ship->troops;
    } else {
        max_crew = ship->max_crew - ship->popn;
    }

#ifdef USE_AMOEBA
    if ((ship->type != OTYPE_TRANSDEV)
        && (ship->type != OTYPE_AMOEBA)
        && !ship->popn
        && max_crew) {
        sprintf(buf, "%s has no crew and is not a robotic ship.\n", Ship(ship));
        notify(playernum, governor, buf);

        return;
    }

#else
    if ((ship->type != OTYPE_TRANSDEV) && !ship->popn && max_crew) {
        sprintf(buf, "%s has no crew and is not a robotic ship.\n", Ship(ship));
        notify(playernum, governor, buf);

        return;
    }
#endif

    switch (ordertype) {
    case ORD_INF:
        if (argn < 3) {
            sprintf(buf, "Not enough args\n");
            notify(playernum, governor, buf);

            return;
        }

        do_inf_ord(playernum, governor, ship);

        break;
    case ORD_REPORT:
        if (ship->wants_reports) {
            ship->wants_reports = 0;
        } else {
            ship->wants_reports = 1;
        }

        break;
    case ORD_CLOAK:
        if (Shipdata[ship->type][ABIL_CLOAK] && ship->cloak) {
            if (!Cloak(races[playernum - 1])) {
                sprintf(buf, "You need [%d] tech to cloak\n", (int)TECH_CLOAK);
                notify(playernum, governor, buf);

                break;
            }

            if (match(args[3], "on")) {
                if (!ship->cloaked) {
                    /* Use some fuel */
                    if (ship->fuel < (ship->size * 0.1)) {
                        sprintf(buf,
                                "Ship [%d] doesn't have enough fuel to cloak [%f]\n",
                                ship->number,
                                ship->size * 0.1);

                        notify(playernum, governor, buf);
                    }
                }

                ship->cloaking = 1;
            } else {
                ship->cloaking = 0;
            }

            if (ship->cloaking) {
                sprintf(buf, "Ship [%d] ordered to cloak.\n", ship->number);
            } else {
                sprintf(buf, "Ship [%d] ordered to decloak.\n", ship->number);
            }

            notify(playernum, governor, buf);
        } else {
            if (Shipdata[ship->type][ABIL_CLOAK] && s->cloak) {
                sprintf(buf, "Ship has no cloaking device\n");
            } else {
                sprintf(buf, "This ship cannot cloak\n");
            }

            notify(playernum, governor, buf);
        }

        break;
    case ORD_DEFE:
        if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
            && (ship->type != STYPE_MINEF)) {
            if (match(args[3], "off")) {
                ship->protect.planet = 0;
            } else {
                ship->protect.planet = 1;
            }
        } else {
            notify(playernum,
                   governor,
                   "That ship cannot be assigned those orders.\n");
        }

        break;
    case ORD_SCAT:
        if (ship->type != STYPE_MISSILE) {
            notify(playernum,
                   governor,
                   "Only missiles can be given this order.\n");
        } else {
            ship->special.impact.scatter = 1;
        }

        break;
    case ORD_IMPA:
        if (ship->type != STYPE_MISSILE) {
            notify(playernum,
                   governor,
                   "Only missiles can be designated for this.\n");
        } else {
            sscanf(args[3], "%d,%d", &x, &y);
            ship->special.impact.x = x;
            ship->special.impact.y = y;
            ship->special.impact.scatter = 0;
        }

        break;
    case ORD_JUMP:
        if (ship->docked) {
            notify(playernum,
                   governor,
                   "That ship is docked. Use 'launch' or 'undock' first.\n");
        } else {
            if (ship->hyper_drive.has) {
                if (match(args[3], "off")) {
                    ship->hyper_drive.on = 0;
                } else {
                    if ((ship->whatdest != LEVEL_STAR)
                        && (ship->whatdest != LEVEL_PLAN)) {
                        notify(playernum,
                               governor,
                               "Destination must be star or planet.\n");
                    } else {
                        if (isclr(Stars[ship->deststar]->explored, ship->owner)) {
                            notify(playernum,
                                   governor,
                                   "You have not explored that system, cannot jump.\n");
                        } else {
                            ship->hyper_drive.on = 1;
                            ship->navigate.on = 0;

                            if (ship->mounted) {
                                ship->hyper_drive.charge = 1;

                                /* Comment out to delay jump - mfw */
                                ship->hyper_drive.ready = 1;
                            }
                        }
                    }
                }
            } else {
                notify(playernum,
                       governor,
                       "This ship does not have hyper drive capability.\n");
            }
        }

        break;
#ifdef THRESHOLDING
    case ORD_THRE:
        /* CWL threshold loading */
        if (argn == 3) {
            /* Clear all thresholds */
            int i;

            for (i = 0 ; i <= TH_CRYSTALS; ++i) {
                ship->threshold[i] = 0;
            }

            notify(playernum, governor, "All thresholds cleared.\n");
        } else if ((argn == 4) || (argn == 5)) {
            /* Clear one threshold */
            unsigned int amount;
            char *c;

            if (argn == 5) {
                amount = (unsigned int)atoi(args[4]);
            } else {
                amount = 1000;
            }

            c = args[3];

            while (*c) {
                switch (*c) {
                case 'r':
                    ship->threshold[TH_RESOURCE] = MIN(ship->max_resource, amount);

                    break;
                case 'd':
                    ship->threshold[TH_DESTRUCT] = MIN(ship->max_destruct, amount);

                    break;
                case 'f':
                    ship->threshold[TH_FUEL] = MIN(ship->max_fuel, amount);

                    break;
                case 'x':
                    ship->threshold[TH_CRYSTALS] = MIN(127, amount);

                    break;
                default:
                    notify(playernum,
                           governor,
                           "Unknown commodity; use rdfx.\n");

                    break;
                }

                ++c;
            } /* End while */
        } else {
            notify(playernum, governor, "threshold <rdfx> [amount]\n");
        }

        break;
#endif
#ifdef AUTOSCRAP
    case ORD_AUTO:
        if (argn >= 4) {
            if (match(args[3], "on")) {
                ship->autoscrap = 1;
            } else {
                ship-autoscrap = 0;
            }
        } else {
            ship->autoscrap = !ship->autoscrap;
        }

        break;
#endif
    case ORD_PROT:
        if (argn > 3) {
            if (args[3][0] == '#') {
                sscanf(args[3] + 1, "%d", &j);
            } else {
                sscanf(args[3], "%d", &j);
            }
        } else {
            j = 0;
        }

        if (j == ship->number) {
            notify(playernum, governor, "You can't do that.\n");
        } else {
            if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
                && (ship->type != STYPE_MINEF)) {
                if (!j) {
                    ship->protect.on = 0;
                } else {
                    ship->protect.on = 1;
                    ship->protect.sip = j;
                }
            } else {
                notify(playernum, governor, "That ship cannot protect.\n");
            }
        }

        break;
    case ORD_NAVI:
        if (argn >= 5) {
            ship->navigate.on = 1;
            ship->navigate.bearing = atoi(args[3]);
            ship->navigate.turns = atoi(args[4]);
        } else {
            ship->navigate.on = 0;
        }

        if (ship->hyper_drive.on) {
            ship->hyper_drive.on = 0;
        }

        break;
    case ORD_SWIT:
        if (ship->type == OTYPE_FACTORY) {
            notify(playernum,
                   governor,
                   "Use \"on\" to bring factory online.\n");
        } else {
            if (Shipdata[ship->type][ABIL_HASSWITCH]) {
                if (ship->whatorbits == LEVEL_SHIP) {
                    notify(playernum,
                           governor,
                           "That ship is being transported.\n");
                } else {
                    ship->on = !ship->on;

                    if (ship->on) {
                        switch (ship->type) {
                        case STYPE_MINEF:
                            notify(playernum,
                                   governor,
                                   "Minefield armed and ready.\n");

                            break;
                        case OTYPE_TRANSDEV:
                            notify(playernum,
                                   governor,
                                   "Transporter ready to receive.\n");

                            break;
                        default:

                            break;
                        }
                    } else {
                        switch (ship->type) {
                        case STYPE_MINEF:
                            notify(playernum,
                                   governor,
                                   "Minefield disarmed.\n");

                            break;
                        case OTYPE_TRANSDEV:
                            notify(playernum,
                                   governor,
                                   "No longer receiving.\n");

                            break;
                        default:

                            break;
                        }
                    }
                }
            } else {
                notify(playernum,
                       governor,
                       "That ship does not have an on/off setting.\n");
            }
        }

        break;
    case ORD_DEST:
        if (ship->max_speed) {
            if (ship->docked) {
                notify(playernum,
                       governor,
                       "That ship is docked; use undock or launch first.\n");
            } else {
                where = Getplace(playernum, governor, args[3], 1);

                if (!where.err) {
                    if (where.level == LEVEL_SHIP) {
                        getship(&tmpship, where.shipno);

                        if (!followable(ship, tmpship)) {
                            notify(playernum,
                                   governor,
                                   "Warning: that ship is out of range.\n");

                            free(tmpship);

                            return;
                        }

                        free(tmpship);
                        ship->destshipno = where.shipno;
                        ship->whatdest = LEVEL_SHIP;
                    } else {
                        /* To foil cheaters */
                        if ((where.level!= LEVEL_UNIV)
                            && ((ship->storbits != where.snum)
                                && (where.level != LEVEL_STAR))
                            && isclr(Stars[where.snum]->explored, ship_owner)) {
                            notify(playernum,
                                   governor,
                                   "You haven't explored this system.\n");

                            return;
                        }

                        if (ship->hyper_drive.on
                            && isclr(Stars[where.snum]->explored, ship->owner)) {
                            notify(playernum,
                                   governor,
                                   "Unexplored system, deactivating hyperdrive.\n");

                            ship->hyper_drive.on = 0;
                        }

                        /* Stop pods from changing destination -mfw */
                        if (ship->type && ship->special.pod.navlock) {
                            notify(playernum,
                                   governor,
                                   "You cannot change the destination of a pod.\n");

                            return;
                        }

                        ship->whatdest = where.level;
                        ship->deststar = where.snum;
                        ship->destpnum = where.pnum;
                    }
                } else {
                    return;
                }
            }
        } else {
            notify(playernum, governor, "that ship cannot be launched.\n");
        }

        break;
    case ORD_EVAD:
        if (max_crew && Max_speed(ship)) {
            if (match(args[3], "on")) {
                ship->protect.evade = 1;
            } else if (match(args[3], "off")) {
                ship->protect.evade = 0;
            }
        }

        break;
    case ORD_BOMB:
        if (ship->type != OTYPE_OMCL) {
            if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
                && (ship->type != STYPE_MINEF)) {
                if (match(args[3], "off")) {
                    ship->bombard = 0;
                } else if (match(args[3], "on")) {
                    ship->bombard = 1;
                }
            } else {
                notify(playernum,
                       governor,
                       "This type of ship cannot be ordered to bombard.\n");
            }
        }

        break;
    case ORD_SMART:
        /* Smart gun orders from HAP -mfw */
        if (!strncmp(args[3], "strength", strlen(args[3]))) {
            int smart_str = atoi(args[4]);

            if (smart_str < 0) {
                notify(playernum,
                       governor,
                       "The strength must be greater than 0.\n");
            } else {
                ship->smart_strength = smart_str;
            }
        } else if (!strncmp(args[3], "list", strlen(args[3]))) {
            char cc;
            char *slp;
            int cp;
            int sp;
            int l4;
            int ip = 0;
            int isinthere;

            l4 = strlen(args[3]);

            for (cp = 0; cp < l4; ++cp) {
                cc = args[4][cp];

                for (sp = 0; sp < NUMSTYPES; ++sp) {
                    if (cc == Shipltrs[sp]) {
                        slp = ship->smart_list;
                        isinthere = 0;

                        while (!isinthere && *slp) {
                            if (cc == *slp) {
                                isinthere = 1;
                            } else {
                                isinthere = 0;
                            }

                            ++slp;
                        }

                        if (!isinthere) {
                            if (ip < SMART_LIST_SIZE) {
                                ship->smart_list[ip] = cc;
                                ++ip;
                            } else {
                                /* Break out simply */
                                cp = l4;
                            }
                        }

                        /* Break out simply */
                        sp = NUMSTYPES;
                    } /* If cc is a ship letter */
                } /* Looking through ship letters */
            } /* For cp is less then l4 */

            ship->smart_list[ip] = (char)0;
        } else if (!strncmp(args[3], "gun", strlen(args[3]))) {
            if (!strncmp(args[4], "primary", strlen(args[4]))) {
                ship->smart_gun = PRIMARY;
                ship->smart_strength = ship->primary;
                notify(playernum, governor, "Smart gun set to primary.\n");
            } else if (strncmp(args[4], "secondary", strlen(args[4]))) {
                ship->smart_gun = SECONDARY;
                ship->smart_strength = ship->secondary;
                notify(playernum, governor, "Smart gun set to secondary.\n");
            } else {
                ship->smart_gun = NONE;
                ship->smart_strength = 0;
                notify(playernum, governor, "smart gun turned off.\n");
            }
        } else {
            notify(playernum,
                   governor,
                   "Enter smart strength <number>, smart list <list> or smart gun <primary|secondary|off>.\n");
        }

        /* end ORD_SMART */
        break;
    case ORD_RETA:
        if ((ship->type != OTYPE_OMCL) && (ship->type != STYPE_MINEF)) {
            if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
                && (ship->type != STYPE_MINEF)) {
                if (match(args[3], "off")) {
                    ship->protect.self = 0;
                } else if (match(args[3], "on")) {
                    ship->protect.self = 1;
                }
            } else {
                notify(playernum,
                       governor,
                       "This type of ship cannot be set to retaliate.\n");
            }
        }

        break;
    case ORD_FOCU:
        if (ship->laser) {
            if (match(args[3], "on")) {
                ship->focus = 1;
            } else {
                ship->focus = 0;
            }
        } else {
            notify(playernum, governor, "no laser.\n");
        }

        break;
    case ORD_LASE:
        if (ship->laser) {
            if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
                && (ship->type != STYPE_MINEF)) {
                if (ship->mounted) {
                    if (match(args[3], "on")) {
                        /*
                         * Set laser strength to given value or max. safe value
                         */

                        if (argn > 4) {
                            ship->fire_laser = atoi(args[4]);
                        } else {
                            ship->fire_laser = (int)(((1.0 - (0.01 * ship->damage)) * ship->tech) / 4.0);
                        }
                    } else {
                        ship->fire_laser = 0;
                    }
                } else {
                    notify(playernum,
                           governor,
                           "You do not have a crystal mounted.\n");
                }
            } else {
                notify(playernum,
                       governor,
                       "This ship is not able to use lasers.\n");
            }
        } else {
            notify(playernum,
                   governor,
                   "This ship is not equipped with combat lasers.\n");
        }

        break;
    case ORD_MERC:
        if (match(args[3], "off")) {
            ship->merchant = 0;
        } else {
            j = atoi(args[3]);

            if ((j < 0) || (j > MAX_ROUTES)) {
                notify(playernum, governor, "Bad route number.\n");
            } else {
                ship->merchant = j;
            }
        }

        break;
    case ORD_SPEE:
        if (ship->max_speed) {
            j = atoi(args[3]);

            if (j < 0) {
                notify(playernum, governor, "Specify a positive speed.\n");
            } else {
                if (j > ship->max_speed)) {
                    j = ship->max_speed;
                }

                ship->speed = j;
            }
        } else {
            notify(playernum,
                   governor,
                   "This ship does not have a speed rating.\n");
        }

        break;
    case ORD_SALV:
        if ((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
            && (ship->type != STYPE_MINEF)) {
            j = atoi(args[3]);

            if (j < 0) {
                notify(playernum,
                       governor,
                       "Specify a positive number of guns.\n");
            } else {
                if ((ship->guns == PRIMARY) && (j > ship->primary)) {
                    j = ship->primary;
                } else if ((ship->guns == SECONDARY) && (j > ship->secondary)) {
                    j = ship->secondary;
                } else if (ship->guns == NONE) {
                    j = 0;
                }

                ship->retaliate = j;
            }
        } else {
            notify(playernum, governor, "This ship cannot be set a salvo.\n");
        }

        break;
    case ORD_PRIM:
        if (ship->primary) {
            if (argn < 4) {
                ship->guns = PRIMARY;

                if (ship->retaliate > ship->primary) {
                    ship->retaliate = ship->primary;
                }
            } else {
                j = atoi(args[3]);

                if (j < 0) {
                    notify(playernum,
                           governor,
                           "Specify a nonnegative number of guns.\n");
                } else {
                    if (j > ship->primary) {
                        j = ship->primary;
                    }

                    ship->retaliate = j;
                    ship->guns = PRIMARY;
                }
            }
        } else {
            notify(playernum,
                   governor,
                   "This ship does not have primary guns.\n");
        }

        break;
    case ORD_SECO:
        if (ship->secondary) {
            if (argn < 4) {
                ship->guns = SECONDARY;

                if (ship->retaliate > ship->secondary) {
                    ship->retaliate = ship->secondary;
                }
            } else {
                j = atoi(args[3]);

                if (j < 0) {
                    notify(playernum,
                           governor,
                           "Specify a nonnegative number of guns.\n");
                } else {
                    if (j > ship->secondary) {
                        j = ship->secondary;
                    }

                    ship->retaliate = j;
                    ship->guns = SECONDARY;
                }
            }
        } else {
            notify(playernum,
                   governor,
                   "This ship does not have secondary guns.\n");
        }

        break;
    case ORD_EXPL:
        switch (ship->type) {
        case STYPE_MINEF:
        case OTYPE_GR:
            ship->mode = 0;

            break;
        default:

            return;
        }

        break;
    case ORD_RADI:
        switch (ship->type) {
        case STYPE_MINEF:
        case OTYPE_GR:
            ship->mode = 1;

            break;
        default:

            return;
        }

        break;
    case ORD_MOVE:
        if ((ship->type == OTYPE_TERRA) || (ship->type == OTYPE_PLOW)) {
            i = 0;

            while (args[3][i]) {
                /* Make sure the list of moves is short enough */
                if (i == (SHIP_NAMESIZE - 1)) {
                    sprintf(buf,
                            "Warning: That is more than %d moves.\n",
                            SHIP_NAMESIZE - 1);

                    notify(playernum, governor, buf);

                    notify(playernum,
                           governor,
                           "these move orders have been truncated.\n");

                    args[3][i] = '\0';

                    break;
                }

                /* Make sure this move is OK. */
                if ((args[3][i] == 'c') || (args[3][i] == 's')) {
                    if ((i == 0) && (args[3][0] == 'c')) {
                        notify(playernum,
                               governor,
                               "Cycling move orders can not be empty!\n");

                        return;
                    }

                    if (args[3][i + 1]) {
                        sprintf(buf,
                                "Warning: '%c' should be the last character in the move order.\n",
                                args[3][i]);

                        notify(playernum, governor, buf);
                        notify(playernum,
                               governor,
                               "These move orders have been truncated.\n");

                        ++i;
                        args[3][i] = '\0';

                        break;
                    }
                } else if ((args[3][i] < '1') || ('9' < args[3][i])) {
                    sprintf(buf,
                            "'%c' is not a valid move direction.\n",
                            args[3][i]);

                    notify(playernum, governor, buf);

                    return;
                }

                ++i;
            }

            /* The move list might be empty... */
            if (i == 0) {
                strcpy(ship->class, "5");
            } else {
                strcpy(ship->class, args[3]);
            }

            /*
             * This is the index keeping track of which order in class is next
             */
            ship->special.terraform.index = 0;
        } else {
            notify(playernum,
                   governor,
                   "That ship is not a terraformer or space plow.\n");
        }

        break;
    case ORD_TRIG:
        if (ship->type == STYPE_MINEF) {
            if (atoi(args[3]) < 0) {
                ship->special.trigger.radius = 0;
            } else {
                ship->special.trigger.radius = atoi(args[3]);
            }
        } else {
            notify(playernum,
                   governor,
                   "This ship cannot be assigned a trigger radius.\n");
        }

        break;
    case ORD_DISP:
        if (ship->type == STYPE_MINEF) {
            if (ship->special.trigger.disperse) {
                ship->special.trigger.disperse = FALSE;
            } else if (!ship->special.trigger.disperse) {
                ship->special.trigger.disperse = TRUE;
            }
        }

        break;
    case ORD_TRAN:
        if (ship->type == OTYPE_TRANSDEV) {
            ship->special.transport.target == atoi(args[3]);

            if (ship->special.transport.target == ship->number) {
                notify(playernum,
                       governor,
                       "A transporter cannot transport to itself.");

                ship->special.transport.target = 0;
            } else {
                sprintf(buf,
                        "Target ship is %d.\n",
                        ship->special.transport.target);

                notify(playernum, governor, buf);
            }
        } else {
            notify(playernum, governor, "This ship is not a transporter.\n");
        }

        break;
    case ORD_AIM:
        /*
         * Any ship that flies can aim no. Time Brown.
         * There was no point, nothing was coded for it, changed back -mfw
         */
        if (((ship->type >= STYPE_MIRROR) && (ship->type <= OTYPE_TRACT))
            || (ship->type == OTYPE_OMCL)) {
            if ((ship->type == OTYPE_GTELE)
                || (ship->type == OTYPE_TRACT)
                || (ship->fuel >= FUEL_MANEUVER)) {
                if ((ship->type == STYPE_MIRROR) && ship->docked) {
                    notify(playernum,
                           governor,
                           "docked; use undock or launch first.\n");
                } else {
                    pl = Getplace(playernum, governor, args[3], 1);

                    if (pl.err) {
                        notify(playernum, governor, "Error in destination.\n");
                    } else {
                        if ((get_num_updates() < CombatUpdate)
                            && (ship->type == STYPE_MIRROR)
                            && (pl.level == LEVEL_SHIP)) {
                            sprintf(buf,
                                    "You may not aim this type of ship at another ship until after the COMBAT UPDATE [%d]\n",
                                    CombatUpdate);

                            notify(playernum, governor, buf);
                        } else {
                            ship->special.aimed_at.level = pl.level;
                            ship->special.aimed_at.pnum = pl.pnum;
                            ship->special.aimed_at.snum = pl.snum;
                            ship->special.aimed_at.shipno = pl.shipno;

                            if ((ship->type != OTYPE_TRACT)
                                && (ship->type != OTYPE_GTELE)) {
                                use_fuel(ship, FUEL_MANEUVER);
                            }

                            if ((ship->type == OTYPE_GTELE)
                                || (ship->type == OTYPE_STELE)) {
                                mk_expl_aimed_at(playernum, governor, ship);
                            }

                            sprintf(buf,
                                    "Aimed at %s\n",
                                    prin_aimed_at(playernum, governor, ship));

                            notify(playernum, governor, buf);
                        } /* If mirror */
                    }
                }
            } else {
                sprintf(buf,
                        "Not enough maneuvering fuel (%.2f).\n",
                        FUEL_MANEUVER);

                notify(playernum, governor, buf);

                return;
            }
        } else {
            notify(playernum, governor, "You can't aim that kind of ship.\n");
        }

        break;
    case ORD_INTE:
        if (ship->type == STYPE_MIRROR) {
            ship->special.aimed_at.intensity = MAX(0, MIN(100, atoi(args[3])));
        }

        break;
    case ORD_ON:
        if (!Shipdata[ship->type][ABIL_HASSWITCH]) {
            notify(playernum,
                   governor,
                   "this ship does not have an on/off setting.\n");
        } else {
            if (ship->damage && (ship->type != OTYPE_FACTOR)) {
                notify(playernum,
                       governor,
                       "Damaged ships cannot be activated.\n");

                return;
            }

            if (ship->on) {
                notify(playernum,
                       governor,
                       "This ship is already activated.\n");

                return;
            }

            if (ship->type == OTYPE_FACTORY) {
                unsigned int oncost;

                /*
                 * HUTm (kse) If ship type to build is not specified factory
                 * should not be turned on
                 */
                if (!ship->build_type || (ship->build_type == OTYPE_FACTORY)) {
                    notify(playernum, governor, "No ship type specified.\n");

                    return;
                } else {
                    if (ship->whatorbits == LEVEL_SHIP) {
                        shiptype *s2;
                        int hangerneeded;

                        getship(&s2, (int)ship->destshipno);

                        if (s2->type == STYPE_HABITAT) {
                            oncost = HAB_FACT_ON_COST * ship->build_cost;

                            if (s2->resource < oncost) {
                                sprintf(buf,
                                        "You don't have %d resource on Habitat #%d to activate this factory.\n",
                                        oncost,
                                        ship->destshipno);

                                notify(playernum, governor, buf);
                                free(s2);

                                return;
                            }

                            hangerneeded = (1 + (int)(HAB_FACT_SIZE * (double)ship_size(ship))) - ((s2->max_hanger - s2->hanger) + ship_size);

                            if (hangerneeded > 0) {
                                sprintf(buf,
                                        "Not enough hanger space free on Habitat #%d. Need %d more.\n",
                                        ship->destshipno,
                                        hangerneeded);

                                notify(playernum, governor, buf);
                                free(s2);

                                return;
                            }

                            s2->resource -= oncost;
                            s2->hanger -= (unsigned short)ship->size;
                            ship->size = 1 + (int)(HAB_FACT_SIZE * (double)ship->size(ship));
                            s2->hanger += (unsigned short)ship->size;
                            putship(s2);
                            free(s2);
                        } else {
                            notify(playernum,
                                   governor,
                                   "The factory is currently being transported.\n");

                            free(s2);

                            return;
                        }
                    } else if (!landed(ship)) {
                        notify(playernum,
                               governor,
                               "You cannot activate the factory here.\n");

                        return;
                    } else {
                        getplanet(&planet,
                                  (int)ship->deststar,
                                  (int)ship->destpnum);

                        oncost = 2 * ship->build_cost;

                        if (planet->inf[playernum - 1].resource < oncost) {
                            sprintf(buf,
                                    "You don't have %d resources on the planet to activate this factory.\n",
                                    oncost);

                            notify(playernum, governor, buf);
                            free(planet);

                            return;
                        }

                        if (!ship->build_type) {
                            sprintf(buf,
                                    "Ship has not bee told what to build yet.\n");

                            notify(playernum, governor, buf);
                            free(planet);

                            return;
                        } else {
                            planet->info[playernum - 1].resource -= oncost;
                            putplanet(planet,
                                      (int)ship->deststar,
                                      (int)ship->destpnum);

                            free(planet);
                        }
                    }

                    sprintf(buf,
                            "Factory activated at a cost of %d resources.\n",
                            oncost);

                    notify(playernum, governor, buf);
                }
            }

            ship->on = 1;
        }

        break;
    case ORD_OFF:
#ifdef USE_VN
        if ((ship->type == OTYPE_FACTORY) && ship->on) {
            notify(playernum,
                   governor,
                   "You can't deactivate a factory once it's online. Consider using 'scrap'.\n");
        }
        else if ((ship->type == OTYPE_VN) && ship->on) {
            notify(playernum,
                   governor,
                   "VNs may not be switched off once activated.\n");
        } else {
            ship->on = 0;
        }

#else
        if ((ship->type == OTYPE_FACTORY) && ship->on) {
            notify(playernum,
                   governor,
                   "You can't deactivate a factory once it's online. Consider using 'scrap'.\n");
        } else {
            ship->on = 0;
        }

#endif

        break;
    case ORD_LIMI:
        if (argn >= 4) {
            ship->limit = MAX(0, MIN(100, atoi(args[3])));
        } else {
            ship->limit = 100;
        }

        break;
    case ORD_STOC:
        if (argn >= 4) {
            if (match(args[3], "on")) {
                ship->use_stock = 1;
            } else {
                ship->use_stock = 0;
            }
        } else {
            if (ship->use_stock) {
                ship->use_stock = 0;
            } else {
                ship->use_stock = 1;
            }
        }

        break;
    case ORD_HOP:
        if (argn >= 4) {
            if (match(args[3], "on")) {
                ship->hop = 1;
            } else {
                ship->hop = 0;
            }
        } else {
            if (ship->hop) {
                ship->hop = 0;
            } else {
                ship->hop = 1;
            }
        }

        break;
    case ORD_FLEET:
        /* Handle fleet orders -mfw */
        if (argn >= 4) {
            char cf;
            int fl;

            if (args[3][0] == '%') {
                cf = args[3][1];
            } else {
                cf = args[3][0];
            }

            fl = fctofi(cf);

            if ((fl > 0)
                && (races[playernum - 1]->fleet[fl].admiral != governor)
                && (races[playernum - 1]->fleet[fl].flagship != 0)) {
                notify(playernum,
                       governor,
                       "You're not admiral of the destination fleet.\n");

                break;
            }

            if ((fl < 1) || (fl > MAXFLEETS)) {
                /* arg 0, remove from fleet */
                remove_sh_fleet(playernum, governor, ship);
            } else {
                if (ship->fleetmember) {
                    /* It's already in a fleet, remove it first */
                    remove_sh_fleet(playernum, governor, ship);
                }

                /* Add ship to fleet */
                insert_sh_fleet(playernum, governor, ship, fl);
            }
        } else {
            /* No arguments, remove it. */
            remove_sh_fleet(playernum, governor, ship);
        }

        break;
    }

    ship->notified = 0;
    putship(ship);
}

char const *prin_aimed_at(int playernum, int governor, shiptype *ship)
{
    placetype targ;

    targ.level = ship->special.aimed_at.level;
    targ.snum = ship->special.aimed_at.snum;
    targ.pnum = ship->special.aimed_at.pnum;
    targ.shipno = ship->special.aimed_at.shipno;

    return Dispplace(playernum, governor, &targ);
}

char const *prin_ship_dest(int playernum, int governor, shiptype *ship)
{
    placetype dest;

    dest.level = ship->whatdest;
    dest.snum = ship->deststar;
    dest.pnum = ship->destpnum;
    dest.shipno = ship->destshipno;

    return Dispplace(playernum, governor, &dest);
}

/* Mark wherever the ship is aimed at, as explored by the owning player. */
void mk_expl_aimed_at(int playernum, int governor, shiptype *s)
{
    double dist;
    startype *str;
    planettype *p;
    double xf;
    double yf;

    str = Stars[s->special.aimed_at.snum];
    xf = s->xpos;
    yf = s->ypos;

    switch (s->special.aimed_at.level) {
    case LEVEL_UNIV:
        sprintf(buf, "There is nothing out here to aim at.");
        notify(playernum, governor, buf);

        break;
    case LEVEL_STAR:
        sprintf(buf, "Star %s ", prin_aimed_at(playernum, governor, s));
        notify(playernum, governor, buf);
        dist = sqrt(Distsq(xf, yf, str->xpos, str->ypos));

        if (dist <= tele_range((int)s->type, s->tech)) {
            getstar(&str, (int)s->special.aimed_at.snum);
            setbit(str->explored, playernum);
            putstar(str, (int)s->special.aimed_at.snum);
            sprintf(buf, "Surveyed, distance %g.\n", dist);
            notify(playernum, governor, buf);
            free((char *)str);
        } else {
            sprintf(buf
                    "Too far to see (%g, max %g).\n",
                    dist,
                    tele_range((int)s->type, s->tech));

            notify(playernum, governor, buf);
        }

        break;
    case LEVEL_PLAN:
        sprintf(buf, "Planet %s ", prin_aimed_at(playernum, governor, s));
        notify(playernum, governor, buf);

        getplanet(&p,
                  (int)s->special.aimed_at.snum,
                  (int)s->special.aimed_at.pnum);

        dist = sqrt(Distsq(xf, yf, str->xpos + p->xpos, str->ypos + p->ypos));

        if (dist <= tele_range((int)s->type, s->tech)) {
            setbit(str->explored, playernum);
            p->info[playernum - 1].explored = 1;

            putplanet(p,
                      (int)s->special.aimed_at.snum,
                      (int)s->special.aimed_at.pnum);

            sprintf(buf, "Surveyed, distance %g.\n", dist);
            notify(playernum, governor, buf);
        } else {
            sprintf(buf,
                    "Too far to see (%g, max %g).\n",
                    dist,
                    tele_range((int)s->type, s->tech));

            notify(playernum, governor, buf);
        }

        free(p);

        break;
    case LEVEL_SHIP:
        sprintf(buf, "You can't see anything of use there.\n");
        notify(playernum, governor, buf);

        break;
    }
}

void DispOrdersHeader(int playernum, int governor)
{
    notify(playernum,
           governor,
           "    #       name   gov sp orbits     destin     options\n");
}

void DispOrders(int playernum, int governor, shiptype *ship)
{
#ifdef THRESHOLDING
    int i;
#endif

    struct inf_setting *infs;
    char mtemp[2048];
    char itemp[256];
    planettype *p;

    if ((!races[playernum - 1]->God && (ship->owner != playernum))
        || (!races[playernum - 1]->God && !authorized(governor, ship))
        || !ship->alive) {
        return;
    }

    if (ship->docked) {
        if (ship->whatdest == LEVEL_SHIP) {
            sprintf(temp, "D#%d", ship->destshipno);
        } else {
            sprintf(temp, "L%2d,%-2d", ship->land_x, ship->land_y);
        }
    } else {
        strcpy(temp, prin_ship_dest_brief(playernum, governor, ship));
    }

    sprintf(buf,
            "%5d %c %11.11s %d %c %1u %-10s %-10.10s ",
            ship->number;
            Shipltrs[ship->type],
            ship->name,
            ship->governor,
            ship->hyper_drive.has ?
            (ship->mount ?
             (ship->mounted ? '+' : '-')
             : '*')
            : ' ',
            ship->speed,
            Dispshiploc_brief(ship),
            temp);

    notify(playernum, governor, buf);
    memset(buf, 0, sizeof(buf));
    buf[0] = '\0';

    if (ship->fleetmember) {
        sprintf(temp, "/fleet %c", fitofc(ship->fleetmember));
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->hyper_drive.on) {
        sprintf(temp,
                "/jump %s %d",
                (ship->hyper_drive.ready ? "ready" : "charging"),
                ship->hyper_drive.charge);

        AddOrderToString(playernum, governor, temp);
    }

    if (ship->cloaked) {
        if (!ship->cloaking) {
            sprintf(temp, "/decloaking");
            AddOrderToString(playernum, governor, temp);
        } else {
            sprintf(temp, "/cloaked");
            AddOrderToString(playernum, governor, temp);
        }
    } else if (ship->cloaking) {
        sprintf(temp, "/cloaking");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->wants_reports) {
        sprintf(temp, "/report");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->protect.self) {
        sprintf(temp, "/retal");
        AddOrderToString(playernum, governor, temp);
    }

    /* Smart gun from HAP -mfw */
    if (ship->smart_gun && ship->smart_list[0]) {
        sprintf(temp,
                "/smartgun %s %d %s",
                ship->smart_list,
                ship->smart_strength,
                ship->smart_gun == PRIMARY ? "primary" : "secondary");

        AddOrderToString(playernum, governor, temp);
    }

    if (ship->guns == PRIMARY) {
        switch (ship->primtype) {
        case LIGHT:
            sprintf(temp, "/lgt primary");

            break;
        case MEDIUM:
            sprintf(temp, "/med primary");

            break;
        case HEAVY:
            sprintf(temp, "/hvy primary");

            break;
        default:
            sprintf(temp, "/none");
        }

        AddOrderToString(playernum, governor, temp);
    } else if (ship->guns == SECONDARY) {
        switch (ship->sectype) {
        case LIGHT:
            sprintf(temp, "/lgt secondary");

            break;
        case MEDIUM:
            sprintf(temp, "/med secondary");

            break;
        case HEAVY:
            sprintf(temp, "/hvy secondary");

            break;
        default:
            sprintf(temp, "/none");
        }

        AddOrderToString(playernum, governor, temp);
    }

    if (ship->fire_laser) {
        sprintf(temp, "/laser %d", ship->fire_laser);
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->focus) {
        sprintf(temp, "/focus");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->retaliate) {
        sprintf(temp, "/salvo %d", ship->retaliate);
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->protect.planet) {
        sprintf(temp, "/defense");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->protect.on) {
        sprintf(temp, "/prot %d", ship->protect.ship);
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->navigate.on) {
        sprintf(temp,
                "/nav %d (%d)",
                ship->navigate.bearing,
                ship->navigate.turns);

        AddOrderToString(playernum, governor, temp);
    }

    if (ship->merchant) {
        sprintf(temp, "/merchant %d", ship->merchant);
        AddOrderToString(playernum, governor, temp);
    }

    if (Shipdata[ship->type][ABIL_HASSWITCH]) {
        if (ship->on) {
            sprintf(temp, "/on");
        } else {
            sprintf(temp, "/off");
        }

        AddOrderToString(playernum, governor, temp);
    }

    if (ship->protect.evade) {
        sprintf(temp, "/evade");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->bombard) {
        sprintf(temp, "/bomb");
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->limit != 100) {
        sprintf(temp, "/limit %d", ship->limit);
        AddOrderToString(playernum, governor, temp);
    }

    if (!ship->use_stock) {
        sprintf(temp, "/limit %d", ship->limit);
        AddOrderToString(playernum, governor, temp);
    }

    if (!ship->use_stock) {
        sprintf(temp, "/no stockpile");
        AddOrderToString(playernum, governor, temp);
    }

    if (!ship->hop) {
        sprintf(temp, "/no hop");
        AddOrderToString(playernum, governor, temp);
    }

    if ((ship->type == STYPE_MINEF) || (ship->type == OTYPE_GR)) {
        if (ship->mode) {
            sprintf(temp, "/radiate");
        } else {
            sprintf(temp, "/explode");
        }

        AddOrderToString(playernum, governor, temp);
    }

    if ((ship->type == OTYPE_TERRA) || (ship->type == OTYPE_PLOW)) {
        int i;

        sprintf(temp, "/move %s", &ship->class[ship->special.terraform.index]);
        i = strlen(temp) - 1;

        if (temp[i] == 'c') {
            char c = ship->class[ship->special.terraform.index];
            ship->class[ship->special.terraform.index] = '\0';
            sprintf(temp + i, "%sc", ship->class);
            ship->class[ship->special.terraform.index] = c;
        }

        temp[27] = '+';
        temp[28] = '\0';
        AddOrderToString(playernum, governor, temp);
    }

    if ((ship->type == STYPE_MISSILE) && (ship->whatdest == LEVEL_PLAN)) {
        if (ship->special.impact.scatter) {
            sprintf(temp, "/scatter");
            AddOrderToString(playernum, governor, temp);
        } else {
            sprintf(temp,
                    "/impact %d,%d",
                    ship->special.impact.x,
                    ship->special.impact.y);

            AddOrderToString(playernum, governor, temp);
        }
    }

    if (ship->type == STYPE_MINEF) {
        sprintf(temp, "/trigger %d", ship->special.trigger.radius);
        AddOrderToString(playernum, governor, temp);

        if (ship->special.trigger.disperse) {
            sprintf(temp, "/disperse");
            AddOrderToString(playernum, governor, temp);
        }
    }

    if (ship->type == OTYPE_TRANSDEV) {
        sprintf(temp, "/target %d", ship->special.transport.target);
        AddOrderToString(playernum, governor, temp);
    }

    if (ship->type == STYPE_MIRROR) {
        sprintf(temp,
                "/aim %s/int %d",
                prin_aimed_at(playernum, governor, ship),
                ship->special.aimed_at.intensity);

        AddOrderToString(playernum, governor, temp);
    }

#ifdef AUTOSCRAP
    if (ship->autoscrap) {
        sprintf(temp, "%s", "/autoscrap");
        AddOrderToString(playernum, governor, temp);
    }
#endif

#ifdef THRESHOLDING
    for (i = 0; i <= TH_CRYSTALS; ++i) {
        if (ship->threshold[i]) {
            char c;

            if (i == TH_RESOURCE) {
                c = 'r';
            } else if (i == TH_DESTRUCT) {
                c = 'd';
            } else if (i == TH_FUEL) {
                c = 'f';
            } else {
                c = 'x';
            }

            sprintf(temp, "/%c%d", c, ship->threshold[i]);
            AddOrderToString(playernum, governor, temp);
        }
    }
#endif

    strcat(buf, "/\n");
    notify(playernum, governor, buf);

    if (ship->type == OTYPE_INF) {
        getplanet(&p, (int)ship->storbits, (int)ship->pnumorbits);

        sprintf(mtemp, "\nInfrastructure Settings:\n");
        sprintf(mtemp,
                "Eff Setting: %d, Fert Setting: %d, Atmos Setting: %d\n",
                ship->inf.eff_setting,
                ship->inf.fert_setting,
                ship->inf.atmos_setting);

        sprintf(mtemp,
                "Reports: %c, Switch: %s\n",
                ship->inf.wants_reports ? 'Y' : 'N',
                ship->on ? "ON" : "OFF");

        sprintf(mtemp,
                "Planet resource reserve: %d\n",
                p->info[ship->owner - 1].res_reserve);

        sprintf(mtemp,
                "Planet fuel reserve:     %d\n",
                p->info[ship->owner - 1].fuel_reserve);

        for (i = 0; i < INF_MAX_TARGETS; ++i) {
            infs = &ship->inf.eff_targets[i];

            if (infs->x != 999) {
                sprintf(itemp,
                        "Eff target: %d,%d and limit: %d%%\n",
                        infs->x,
                        infs->y,
                        infs->max);

                strcat(mtemp, itemp);
            }
        }

        for (i = 0; i < INF_MAX_TARGETS; ++i) {
            infs = &ship->inf.fert_targets[i];


            if (infs->x != 999) {
                sprintf(itemp,
                        "Fert Target: %d,%d and limit: %d%%\n",
                        infs->x,
                        infs->y,
                        infs->max);

                strcat(mtemp, itemp);
            }
        }

        free(p);
        notify(playernum, governor, mtemp);
    }

    /*
     * If hyperspace is on estimate how much fuel it will cost to get to the
     * destination
     */
    if (ship->hyper_drive.on) {
        double dist;
        double fuse;

        dist = sqrt(Distsq(ship->xpos, ship->ypos, Stars[ship->deststar]->xpos, Stars[ship->deststar]->ypos));

        fuse = calc_fuse(ship, dist);

        sprintf(buf,
                "  *** distance %.0f - jump will cost at least %.1ff ***\n",
                dist,
                fuse);

        notify(playernum, governor, buf);

        if (ship->max_fuel < fuse) {
            notify(playernum,
                   governor,
                   "Your ship cannot carry enough fuel to do this jump.\n");
        }
    }
}

int AddOrderToString(int playernum, int governor, char *str2add)
{
    char buf9[80];

    sprintf(buf9, "%s%s", buf, str2add);

    if (strlen(buf9) > 30) {
        strcat(buf, "/\n                                                ");
        notify(playernum, governor, buf);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", str2add);
    } else {
        strcat(buf, str2add);
    }

    return 1;
}

void route(int playernum, int governor, int apcount)
{
    int i;
    int x;
    int y;
    unsigned char star;
    unsigned char planet;
    unsigned char load;
    unsigned char unload;
    char *c;
    planettype *p;
    placetype where;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum,
               governor,
               "You have to 'cs' to a planet to examine routes.\n");

        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if (argn == 1) {
        /* Display all shipping routes that are active */
        for (i = 1; i < MAX_ROUTES; ++i) {
            if (p->info[playernum - 1].route[i - 1].set) {
                star = p->info[playernum - 1].route[i - 1].dest_star;
                planet = p->info[playernum - 1].route[i - 1].dest_planet;
                load = p->info[playernum - 1].route[i - 1].load;
                unload = p->info[playernum - 1].route[i - 1].unload;

                sprintf(buf,
                        "%2d  land %2d,%2d   ",
                        i,
                        p->info[playernum - 1].route[i - 1].x,
                        p->info[playernum - 1].route[i - 1].y);

                strcat(buf, "load: ");

                if (Fuel(load)) {
                    strcat(buf, "f");
                } else {
                    strcat(buf, " ");
                }

                if (Destruct(load)) {
                    strcat(buf, "d");
                } else {
                    strcat(buf, " ");
                }

                if (Resources(load)) {
                    strcat(buf, "r");
                } else {
                    strcat(buf, " ");
                }

                if (Crystals(load)) {
                    strcat(buf, "x");
                } else {
                    strcat(buf, " ");
                }

                if (Military(load)) {
                    strcat(buf, "m");
                } else {
                    strcat(buf, " ");
                }

                strcat(buf, "  unload: ");

                if (Fuel(unload)) {
                    strcat(buf, "f");
                } else {
                    strcat(buf, " ");
                }

                if (Destruct(unload)) {
                    strcat(buf, "d");
                } else {
                    strcat(buf, " ");
                }

                if (Resources(unload)) {
                    strcat(buf, "r");
                } else {
                    strcat(buf, " ");
                }

                if (Crystals(unload)) {
                    strcat(buf, "x");
                } else {
                    strcat(buf, " ");
                }

                if (Military(unload)) {
                    strcat(buf, "m");
                } else {
                    strcat(buf, " ");
                }

                sprintf(temp,
                        "  -> %s/%s\n",
                        Stars[star]->name,
                        Stars[star]->pnames[planet]);

                strcat(buf, temp);
                notify(playernum, governor, buf);
            }
        }

        notify(playernum, governor, "Done.\n");
        free(p);

        return;
    } else if (argn == 2) {
        sscanf(args[1], "%d", &i);

        if ((i > MAX_ROUTES) || (i < 1)) {
            notify(playernum, governor, "Bad route number.\n");
            free(p);

            return;
        }

        if (p->info[playernum - 1].route[i - 1].set) {
            star = p->info[playernum - 1].route[i - 1].dest_star;
            planet = p->info[playernum - 1].route[i - 1].dest_planet;
            load = p->info[playernum - 1].route[i - 1].load;
            unload = p->info[playernum - 1].route[i - 1].unload;

            sprintf(buf,
                    "%2d  land %2d,%2d   ",
                    i,
                    p->info[playernum - 1].route[i - 1].x,
                    p->info[playernum - 1].route[i - 1].y);

            if (load) {
                sprintf(temp, "load: ");
                strcat(buf, temp);

                if (Fuel(load)) {
                    strcat(buf, "f");
                }

                if (Destruct(load)) {
                    strcat(buf, "d");
                }

                if (Resources(load)) {
                    strcat(buf, "r");
                }

                if (Crystals(load)) {
                    strcat(buf, "x");
                }

                if (Military(load)) {
                    strcat(buf, "m");
                }
            }

            if (unload) {
                sprintf(temp, "  unload: ");
                strcat(buf, temp);

                if (Fuel(unload)) {
                    strcat(buf, "f");
                }

                if (Destruct(unload)) {
                    strcat(buf, "d");
                }

                if (Resources(unload)) {
                    strcat(buf, "r");
                }

                if (Crystals(unload)) {
                    strcat(buf, "x");
                }

                if (Military(unload)) {
                    strcat(buf, "m");
                }
            }

            sprintf(temp,
                    "  -> %s/%s\n",
                    Stars[star]->name,
                    Stars[star]->pnames[planet]);

            strcat(buf, temp);
            notify(playernum, governor, buf);
        }

        notify(playernum, governor, "Done.\n");
        free(p);

        return;
    } else if (argn == 3) {
        sscanf(args[1], "%d", &i);

        if ((i > MAX_ROUTES) || (i < 1)) {
            notify(playernum, governor, "Bad route number.\n");
            free(p);

            return;
        }

        if (match(args[2], "activate")) {
            p->info[playernum - 1].route[i - 1].set = 1;
        } else if (match(args[2], "deactivate")) {
            p->info[playernum - 1].route[i - 1].set = 0;
        } else {
            where = Getplace(playernum, governor, args[2], 1);

            if (!where.err) {
                if (where.level != LEVEL_PLAN) {
                    notify(playernum,
                           governor,
                           "You have to designate a planet.\n");

                    free(p);

                    return;
                }

                p->info[playernum - 1].route[i - 1].dest_star = where.snum;
                p->info[playernum - 1].route[i - 1].dest_planet = where.pnum;
                notify(playernum, governor, "Set.\n");
            } else {
                notify(playernum, governor, "Illegal destination.\n");
                free(p);

                return;
            }
        }
    } else {
        sscanf(args[1], "%d", &i);

        if ((i > MAX_ROUTES) || (i < 1)) {
            notify(playernum, governor, "Bad route number.\n");
            free(p);

            return;
        }

        if (match(args[2], "land")) {
            sscanf(args[3], "%d,%d", &x, &y);

            if ((x < 0)
                || (x > (p->Maxx - 1))
                || (y < 0)
                || (y > (p->Maxy - 1))) {
                notify(playernum, governor, "Bad sector coordinates.\n");
                free(p);

                return;
            }

            p->info[playernum - 1].route[i - 1].x = x;
            p->info[playernum - 1].route[i - 1].y = y;
        } else if (match(args[2], "load")) {
            p->info[playernum - 1].route[i - 1].load = 0;
            c = args[3];

            while (*c) {
                if (*c == 'f') {
                    p->info[playernum - 1].route[i - 1].load |= M_FUEL;
                }

                if (*c == 'd') {
                    p->info[playernum - 1].route[i - 1].load |= M_DESTRUCT;
                }

                if (*c == 'r') {
                    p->info[playernum - 1].route[i - 1].load |= M_RESOURCES;
                }

                if (*c == 'x') {
                    p->info[playernum - 1].route[i - 1].load |= M_CRYSTALS;
                }

                if (*c == 'm') {
                    p->info[playernum - 1].route[i - 1].load |= M_MILITARY;
                }

                ++c;
            }
        } else if (match(args[2], "unload")) {
            p->info[playernum - 1].route[i - 1].unload = 0;
            c = args[3];

            while (*c) {
                if (*c == 'f') {
                    p->info[playernum - 1].route[i - 1].unload |= M_FUEL;
                }

                if (*c == 'd') {
                    p->info[playernum - 1].route[i - 1].unload |= M_DESTRUCT;
                }

                if (*c == 'r') {
                    p->info[playernum - 1].route[i - 1].unload |= M_RESOURCES;
                }

                if (*c == 'x') {
                    p->info[playernum - 1].route[i - 1].unload |= M_CRYSTALS;
                }

                if (*c == 'm') {
                    p->info[playernum - 1].route[i - 1].unload |= M_MILITARY;
                }

                ++c;
            }
        } else {
            notify(playernum, governor, "What are you trying to do?\n");
            free(p);

            return;
        }

        notify(playernum, governor, "Set.\n");
    }

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    free(p);
}

char const *prin_ship_dest_brief(int playernum, int governor, shiptype *ship)
{
    placetype dest;

    dest.level = ship->whatdest;
    dest.snum = ship->deststar;
    dest.pnum = ship->destpnum;
    dest.shipno = ship->destshipno;

    return Dispplace_brief(playernum, governor, &dest);
}

int get_order_type(char *ord)
{
    it (match(ord, "inf")) {
        return ORD_INF;
    }

    if (match(ord, "report")) {
        return ORD_REPORT;
    }

    if (match(ord, "cloak")) {
        return ORD_CLOAK;
    }

    if (match(ord, "defense")) {
        return ORD_DEFE;
    }

    if (match(ord, "scatter")) {
        return ORD_SCAT;
    }

    if (match(ord, "impact")) {
        return ORD_IMPA;
    }

    if (match(ord, "jump")) {
        return ORD_JUMP;
    }

    if (match(ord, "THRESHOLDING")) {
        return ORD_THRE;
    }

    if (match(ord, "autoscrap")) {
        return ORD_AUTO;
    }

    if (match(ord, "protect")) {
        return ORD_PROT;
    }

    if (match(ord, "navigate")) {
        return ORD_NAVI;
    }

    if (match(ord, "switch")) {
        return ORD_SWIT;
    }

    if (match(ord, "destination")) {
        return ORD_DEST;
    }

    if (match(ord, "evade")) {
        return ORD_EVAD;
    }

    if (match(ord, "bombard")) {
        return ORD_BOMB;
    }

    if (match(ord, "retaliate")) {
        return ORD_RETA;
    }

    if (match(ord, "focus")) {
        return ORD_FOCU;
    }

    if (match(ord, "laser")) {
        return ORD_LASE;
    }

    if (match(ord, "merchant")) {
        return ORD_MER;
    }

    if (match(ord, "speed")) {
        return ORD_SPEE;
    }

    if (match(ord, "salvo")) {
        return ORD_SALV;
    }

    if (match(ord, "primary")) {
        return ORD_PRIM;
    }

    if (match(ord, "secondary")) {
        return ORD_SECO;
    }

    if (match(ord, "explosive")) {
        return ORD_EXPL;
    }

    if (match(ord, "radiative")) {
        return ORD_RADI;
    }

    if (match(ord, "move")) {
        return ORD_MOVE;
    }

    if (match(ord, "trigger")) {
        return ORD_TRIG;
    }

    if (match(ord, "disperse")) {
        return ORD_DISP;
    }

    if (match(ord, "transport")) {
        return ORD_TRAN;
    }

    if (match(ord, "aim")) {
        return ORD_AIM;
    }

    if (match(ord, "intensity")) {
        return ORD_INTE;
    }

    if (match(ord, "on")) {
        return ORD_ON;
    }

    if (match(ord, "off")) {
        return ORD_OFF;
    }

    if (match(ord, "stockpile")) {
        return ORD_STOC;
    }

    if (match(ord, "limit")) {
        return ORD_LIMI;
    }

    if (match(ord, "hop")) {
        return ORD_HOP;
    }

    if (match(ord, "fleet")) {
        return ORD_FLEET;
    }

    if (match(ord, "smart")) {
        return ORD_SMART;
    }

    return ORD_NULL;
}
