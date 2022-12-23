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
 * load.c -- Load/unload stuff
 *
 * #ident  "#(@)load.c 1.13 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/load.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "load.h"

#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/files_shl.h"
#include "../server/getplace.h"
#include "../server/GB_server.h"
#include "../server/misc.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "fire.h"
#include "land.h"
#include "load.h"
#include "move.h"

extern char Dessymbols[];
extern char *Desnames[];
extern int Defensedata[];

static char buff[128];
static char bufr[128];
static char bufd[128];
static char bufc[128];
static char bufx[128];
static char bufm[128];

extern void load(int, int, int, int);
extern void jettison(int, int, int);
extern int jettison_check(int, int, int, int);
extern void dump(int, int, int);
extern void transfer(int, int, int);
extern void mount(int, int, int);
extern void dismount(int, int, int);
extern void _mount(int, int, int, int);
extern void use_destruct(shiptype *, int);
extern void use_resource(shiptype *, int);
extern void use_popn(shiptype *, int, double);
extern void rcv_fuel(shiptype *, double);
extern void rcv_resource(shiptype *, int);
extern void rcv_destruct(shiptype *, int);
extern void rcv_popn(shiptype *, int, double);
extern void rcv_troops(shiptype *, int, double);
extern void do_transporter(racetype *, int, shiptype *);
extern int landed_on(shiptype *, int);
extern void unload_onto_alien_sector(int,
                                     int,
                                     planettype *,
                                     shiptype *,
                                     sectortype *,
                                     int,
                                     int);

int inship(shiptype *);

void load(int playernum, int governor, int apcount, int mode)
{
    char commod;
    unsigned char sh = 0;
    unsigned char diff = 0;
    int lolim;
    int uplim;
    int amt;
    int transfercrew;
    int crew;
    shiptype *s;
    shiptype *s2;
    planettype *p;
    sectortype *sect;
    racetype *race;
    int shipno;
    int nextshipno;

    if (argn < 2) {
        notify(playernum, governor, "Load what?\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
            if ((s->owner != playernum) || !s->alive) {
                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!s->active) {
                sprintf(buf, "%s is irradiated and inactive.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            }

            if (s->whatorbits == LEVEL_UNIV) {
                if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
                    free(s);
                    do_shiplist(&s, &nextshipno);

                    continue;
                }
            } else if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!s->docked) {
                sprintf(buf, "%s is not landed or docked.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            } else {
                /* Ship has a recipient */
                if (s->whatdest == LEVEL_PLAN) {
                    sprintf(buf,
                            "%s ad %d,%d\n",
                            Ship(s),
                            s->land_x,
                            s->land_y);

                    notify(playernum, governor, buf);

                    if ((s->storbits != Dir[playernum -1][governor].snum)
                        || (s->pnumorbits != Dir[playernum - 1][governor].pnum)) {
                        notify(playernum,
                               governor,
                               "Change scope to the planet this ship is landed on.\n");
                        free(s);
                        do_shiplist(&s, &nextshipno);

                        continue;
                    }
                } else {
                    /* Ship is docked */
                    if (!s->destshipno) {
                        sprintf(buf, "%s is not docked.\n", Ship(s));
                        free(s);
                        do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (!getship(&s2, (int)s->destshipno)) {
                        notify(playernum,
                               governor,
                               "Destination ship is bogus.\n");

                        free(s);
                        do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (!s2->alive
                        || ((s->whatorbits != LEVEL_SHIP)
                            && (s2->destshipno != shipno))) {
                        /*
                         * The ship it was docked with died or undocked with it
                         * or something
                         */
                        s->docked = 0;
                        s->whatdest = LEVEL_UNIV;
                        putship(s);
                        sprintf(buf, "%s is not docked.\n", Ship(s2));
                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (overloaded(s2) && (s2->whatorbits == LEVEL_SHIP)) {
                        sprintf(buf, "%s is overloaded!\n", Ship(s2)) ;
                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    sprintf(buf, "%s docked with %s\n", Ship(s), Ship(s2));
                    notify(playernum, governor, buf);
                    sh = 1;

                    if (s2->owner != playernum) {
                        sprintf(buf, "Player %d owns that ship.\n", s2->owner);
                        notify(playernum, governor, buf);
                        diff = 1;
                    }
                }
            }

            commod = args[2][0];

            if (argn > 3) {
                amt = atoi(args[3]);
            } else {
                amt = 0;
            }

            if (mode) {
                /* Unload */
                amt = -amt;
            }

            if (!sh) {
                getplanet(&p,
                          Dir[playernum - 1][governor].snum,
                          Dir[playernum - 1][governor].pnum);
            }

            if (!sh && ((commod == 'c') || (commod == 'm'))) {
                if (!getsector(&sect, p, (int)s->land_x, (int)s->land_y)) {
                    notify(playernum,
                           governor,
                           "Error in sector database, notify deity.\n");

                    return;
                }
            }

            switch (commod) {
            case 'x':
            case '&':
                if (sh) {
                    if (diff) {
                        uplim = diff;
                        lolim = 0;
                    } else {
                        uplim = MIN(s2->crystals, 127 - s->crystals);
                        lolim = -MIN(s->crystals, 127 - s2->crystals);
                    }
                } else {
                    uplim = MIN(p->info[playernum - 1].crystals,
                                127 - s->crystals);

                    /* lolim = -s->crystals; -mfw */
                    lolim = -MIN(s->crystals,
                                 USHRT_MAX - p->info[playernum - 1].crystals);
                }

                break;
            case 'c':
                if (sh) {
                    if (diff) {
                        uplim = 0;
                        lolim = 0;
                    } else {
                        if (s->type == OTYPE_FACTORY) {
                            uplim = MIN(s2->popn,
                                        Shipdata[s->type][ABIL_MAXCREW]
                                        - s->troops
                                        - s->popn);
                        } else {
                            uplim = MIN(s2->popn,
                                        s->max_crew
                                        - s->troops
                                        - s->popn);
                        }

                        if (s2->type == OTYPE_FACTORY) {
                            lolim = MIN(s->popn,
                                        Shipdata[s2->type][ABIL_MAXCREW]
                                        - s2->troops
                                        - s2->popn);
                        } else {
                            lolim = MIN(s->popn,
                                        s2->max_crew
                                        - s2->troops
                                        - s2->popn);
                        }
                    }
                } else {
                    if (s->type == OTYPE_FACTORY) {
                        uplim = MIN(sect->popn,
                                    Shipdata[s->type][ABIL_MAXCREW]
                                    - s->troops
                                    - s->popn);
                    } else {
                        uplim = MIN(sect->popn,
                                    s->max_crew
                                    - s->troops
                                    - s->popn);
                    }

                    /* lolim = -s->popn; -mfw */
                    lolim = -MIN(s->popn,
                                 USHRT_MAX - p->info[playernum - 1].popn);
                }

                break;
            case 'm':
                if (sh) {
                    if (diff) {
                        uplim = 0;
                        lolim = 0;
                    } else {
                        if (s->type == OTYPE_FACTORY) {
                            uplim = MIN(s2->troops,
                                        Shipdata[s->type][ABIL_MAXCREW]
                                        - s->popn
                                        - s->troops);
                        } else {
                            uplim = MIN(s2->troops,
                                        s->max_crew - s->popn - s->troops);
                        }

                        if (s2->type == OTYPE_FACTORY) {
                            lolim = -MIN(s->troops,
                                         Shipdata[s2->type][ABIL_MAXCREW]
                                         - s2->popn
                                         - s2->troops);
                        } else {
                            lolim = -MIN(s->troops,
                                         s2->max_crew - s2->popn - s2->troops);
                        }
                    }
                } else {
                    if (s->type == OTYPE_FACTORY) {
                        uplim = MIN(sect->troops,
                                    Shipdata[s->type][ABIL_MAXCREW]
                                    - s->popn
                                    - s->troops);
                    } else {
                        uplim = MIN(sect->troops,
                                    s->max_crew - s->popn - s->troops);
                    }

                    /* lolim = -s->troops; -mfw */
                    lolim = -MIN(s->troops,
                                 USHRT_MAX - p->info[playernum - 1].troops);
                }

                break;
            case 'd':
                if (sh) {
                    if (diff) {
                        uplim = 0;
                    } else if (s->type == OTYPE_FACTORY) {
                        uplim = MIN(s2->destruct,
                                    Shipdata[s->type][ABIL_DESTCAP] - s->destruct);
                    } else {
                        uplim = MIN(s2->destruct,
                                    s->max_destruct - s->destruct);
                    }

                    if (s2->type == OTYPE_FACTORY) {
                        lolim = -MIN(s->destruct,
                                     Shipdata[s->type][ABIL_DESTCAP] - s2->destruct);
                    } else {
                        lolim = -MIN(s->destruct,
                                     s2->max_destruct - s2->destruct);
                    }
                } else {
                    if (s->type == OTYPE_FACTORY) {
                        uplim = MIN(p->info[playernum - 1].destruct,
                                    Shipdata[s->type][ABIL_DESTCAP] - s->destruct);
                    } else {
                        uplim = MIN(p->info[playernum - 1].destruct,
                                    s->max_destruct - s->destruct);
                    }

                    /* New code by Kharush. */
                    lolim = -MIN(s->destruct,
                                 USHRT_MAX - p->info[playernum - 1].destruct);
                }

                break;
            case 'f':
                if (sh) {
                    if (diff) {
                        uplim = 0;
                    } else if (s->type == OTYPE_FACTORY) {
                        uplim = MIN((int)s2->fuel,
                                    (int)Shipdata[s->type][ABIL_FUELCAP]
                                    - (int)s->fuel);
                    } else {
                        uplim = MIN((int)s2->fuel,
                                    (int)s->max_fuel - (int)s->fuel);
                    }

                    if (s2->type == OTYPE_FACTORY) {
                        lolim = -MIN((int)s->fuel,
                                     (int)Shipdata[s2->type][ABIL_FUELCAP]
                                     - (int)s2->fuel);
                    } else {
                        lolim = -MIN((int)s->fuel,
                                     (int)s2->max_fuel - (int)s2->fuel);
                    }
                } else {
                    if (s->type == OTYPE_FACTORY) {
                        uplim = MIN((int)p->info[playernum - 1].fuel,
                                    (int)Shipdata[s->type][ABIL_FUELCAP]
                                    - (int)s->fuel);
                    } else {
                        uplim = MIN((int)p->info[playernum - 1].fuel,
                                    (int)s->max_fuel - (int)s->fuel);
                    }

                    /*
                     * New code by Kharush. Should not need that +1, but to be
                     * extra sure.
                     */

                    /*
                     * if ((USHRT_MAX - ((int)p->info[playernum - 1].fuel + 1)) <= 0) {
                     *     lolim = -MIN((int)s->fuel, 0);
                     * } else {
                     *     lolim = -MIN((int)s->fuel,
                     *                  USHRT_MAX - ((int)p->info[playernum - 1].fuel + 1));
                     */

                    /* Old code: lolim = -(int)s->fuel; */
                    lolim = -MIN(s->fuel,
                                 USHRT_MAX - p->info[playernum - 1].fuel);
                }

                break;
            case 'r':
                if (sh) {
                    /*
                     * New code by Kharush. check STYPE_DHUTTLE added to two ifs
                     * below
                     */
                    /* I've removed them, Dhuttle was silly -mfw */
                    if ((s->type == STYPE_SHUTTLE)
                        && (s->whatorbits != LEVEL_SHIP)) {
                        if (diff) {
                            uplim = 0;
                        } else {
                            uplim = s2->resource;
                        }
                    } else {
                        if (diff) {
                            uplim = 0;
                        } else {
                            if (s->type == OTYPE_FACTORY) {
                                uplim = MIN(s2->resource,
                                            Shipdata[s->type][ABIL_CARGO]
                                            - s->resource);
                            } else {
                                uplim = MIN(s2->resource,
                                            s->max_resource - s->resource);
                            }
                        }
                    }

                    if ((s2->type == STYPE_SHUTTLE)
                        && (s->whatorbits != LEVEL_SHIP)) {
                        lolim = -s->resource;
                    } else {
                        if (s2->type == OTYPE_FACTORY) {
                            lolim = -MIN(s->resource,
                                         Shipdata[s2->type][ABIL_CARGO]
                                         - s2->resource);
                        } else {
                            lolim = -MIN(s->resource,
                                         s2->max_resource - s2->resource);
                        }
                    }
                } else {
                    if (s->type == OTYPE_FACTORY) {
                        uplim = MIN(p->info[playernum - 1].resource,
                                    Shipdata[s->type][ABIL_CARGO]
                                    - s->resource);
                    } else {
                        uplim = MIN(p->info[playernum - 1].resource,
                                    s->max_resource - s->resource);
                    }

                    /* New code by Kharush. */
                    lolim = -MIN(s->resource,
                                 USHRT_MAX - p->info[playernum - 1].resource);
                }

                break;
            default:
                notify(playernum, governor, "No such commodity valid.\n");

                if (sh) {
                    free(s2);
                } else {
                    free(p);
                }

                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            }

            if ((amt < lolim) || (amt > uplim)) {
                sprintf(buf,
                        "You can only transfer between %d and %d.\n",
                        lolim,
                        uplim);

                notify(playernum, governor, buf);

                if (sh) {
                    /* Core dump here, was line 293 */
                    free(s2);
                } else {
                    free(p);
                }

                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            }

            race = races[playernum - 1];

            if (amt == 0) {
                if (mode) {
                    amt = lolim;
                } else {
                    amt = uplim;
                }
            }

            switch (commod) {
            case 'c':
                if (sh) {
                    s2->popn -= amt;

                    if (!landed_on(s, sh)) {
                        s2->mass -= (amt * race->mass);
                    }

                    transfercrew = 1;
                } else if (sect->owner && (sect->owner != playernum)) {
                    sprintf(buf,
                            "That sector is already occupied by another player!\n");

                    notify(playernum, governor, buf);

                    /* Fight a land battle */
                    unload_onto_alien_sector(playernum,
                                             governor,
                                             p,
                                             s,
                                             sect,
                                             CIV,
                                             -amt);

                    putship(s);
                    putsector(sect, p, (int)s->land_x, (int)s->land_y);

                    putplanet(p,
                              Dir[playernum - 1][governor].snum,
                              Dir[playernum - 1][governor].pnum);

                    free(s);
                    free(sect);
                    free(p);

                    return;
                } else {
                    transfercrew = 1;

                    if (!sect->popn && !sect->troops && (amt < 0)) {
                        ++p->info[playernum - 1].numsectsowned;
                        p->info[playernum - 1].mob_points += sect->mobilization;
                        sect->owner = playernum;
                        sprintf(buf,
                                "sector %d,%d COLONIZED.\n",
                                s->land_x,
                                s->land_y);

                        notify(playernum, governor, buf);
                    }

                    sect->popn -= amt;
                    p->popn -= amt;
                    sect->owner = playernum;
                    p->info[playernum - 1].popn -= amt;

                    if (!sect->popn
                        && !sect->troops
                        && p->info[playernum - 1].numsectsowned) {
                        /*
                         * New code by Kharush. Check amt == 0 added to prevent
                         * bug that changed the amount of sectors in the planet
                         * to 64k. sect->owner = 0; is needed int he fix,
                         * because of sect->owner = playernum; ten rows above.
                         */
                        if (amt == 0) {
                            sect->owner = 0;
                            sprintf(buf,
                                    "You tried to load/unload civilians from empty sector/ship.\n");

                            notify(playernum, governor, buf);
                        } else {
                            --p->info[playernum - 1].numsectsowned;
                            p->info[playernum - 1].mob_points -= sect->mobilization;
                            sect->owner = 0;
                            sprintf(buf,
                                    "Sector %d,%d evacuated.\n",
                                    s->land_x,
                                    s->land_y);

                            notify(playernum, governor, buf);
                        }
                    }
                }

                if (transfercrew) {
                    s->popn += amt;
                    s->mass += (amt * race->mass);
                    sprintf(buf,
                            "Crew complement of %s is not %u.\n",
                            Ship(s),
                            s->popn);

                    notify(playernum, governor, buf);
                }

                break;
            case 'm':
                if (sh) {
                    s2->troops -= amt;

                    if (!landed_on(s, sh)) {
                        s2->mass -= (amt * race->mass);
                    }

                    transfercrew = 1;
                } else if (sect->owner && (sect->owner != playernum)) {
                    sprintf(buf,
                            "That sector is already occupied by another player!\n");

                    notify(playernum, governor, buf);
                    unload_onto_alien_sector(playernum,
                                             governor,
                                             p,
                                             s,
                                             sect,
                                             MIL,
                                             -amt);

                    putship(s);
                    putsector(sect, p, (int)s->land_x, (int)s->land_y);

                    putplanet(p,
                              Dir[playernum - 1][governor].snum,
                              Dir[playernum - 1][governor].pnum);

                    free(s);
                    free(sect);
                    free(p);

                    return;
                } else {
                    transfercrew = 1;

                    if (!(sect->popn + sect->troops) && (amt < 0)) {
                        ++p->info[playernum - 1].numsectsowned;
                        p->info[playernum - 1].mob_points += sect->mobilization;
                        sect->owner = playernum;
                        sprintf(buf,
                                "Sector %d,%d OCCUPIED.\n",
                                s->land_x,
                                s->land_y);

                        notify(playernum, governor, buf);
                    }

                    sect->troops -= amt;
                    p->troops -= amt;
                    p->info[playernum - 1].troops -= amt;

                    if (!(sect->troops + sect->popn)
                        && p->info[playernum - 1].numsectsowned) {
                        /*
                         * New code by Kharush. Check amt == 0 added to prevent
                         * bug that changed the amount of your sectors in the
                         * planet to 64k. No need for sect->owner = 0; in this
                         * fix.
                         */
                        if (amt == 0) {
                            sprintf(buf,
                                    "You tried to load/unload military from empty sector/ship.\n");

                            notify(playernum, governor, buf);
                        } else {
                            --p->info[playernum - 1].numsectsowned;
                            p->info[playernum - 1].mob_points -= sect->mobilization;
                            sect->owner = 0;
                            sprintf(buf,
                                    "Sector %d,%d evacuated.\n",
                                    s->land_x,
                                    s->land_y);

                            notify(playernum, governor, buf);
                        }
                    }
                }

                if (transfercrew) {
                    s->troops += amt;
                    s->mass += (amt * race->mass);

                    sprintf(buf,
                            "Troop complement of %s is not %u.\n",
                            Ship(s),
                            s->troops);

                    notify(playernum, governor, buf);
                }

                break;
            case 'd':
                if (sh) {
                    s2->destruct -= amt;

                    if (!landed_on(s, sh)) {
                        s2->mass -= (amt * MASS_DESTRUCT);
                    }
                } else {
                    if ((amt > 0) && (amt > p->info[playernum - 1].destruct)) {
                        p->info[playernum - 1].destruct = 0;
                    } else {
                        p->info[playernum - 1].destruct -= amt;
                    }
                }

                s->destruct += amt;
                s->mass += (amt * MASS_DESTRUCT);
                sprintf(buf, "%d destruct transferred.\n", amt);
                notify(playernum, governor, buf);

                if (s->type == OTYPE_FACTORY) {
                    crew = Shipdata[s->type][ABIL_MAXCREW] - s->troops;
                } else {
                    crew = s->max_crew - s->popn;
                }

                if (!crew) {
                    sprintf(buf, "\n%s", Ship(s));
                    notify(playernum, governor, buf);

                    if (s->destruct) {
                        sprintf(buf, "now boobytrapped.\n");
                    } else {
                        sprintf(buf, "no longer boobytrapped.\n");
                    }

                    notify(playernum, governor, buf);
                }

                break;
            case 'x':
                if (sh) {
                    s2->crystals -= amt;
                } else {
                    p->info[playernum - 1].crystals -= amt;
                }

                s->crystals += amt;
                sprintf(buf, "%d crystal(s) transferred.\n", amt);
                notify(playernum, governor, buf);

                break;
            case 'f':
                if (sh) {
                    s2->fuel -= (double)amt;

                    if (!landed_on(s, sh)) {
                        s2->mass -= ((double)amt * MASS_FUEL);
                    }
                } else {
                    if ((amt > 0) && (amt > p->info[playernum - 1].fuel)) {
                        p->info[playernum - 1].fuel = 0;
                    } else {
                        p->info[playernum - 1].fuel -= amt;
                    }
                }

                rcv_fuel(s, (double)amt);
                sprintf(buf, "%d fuel transferred.\n", amt);
                notify(playernum, governor, buf);

                break;
            case 'r':
                if (sh) {
                    s2->resource -= amt;

                    if (!landed_on(s, sh)) {
                        s2->mass -= (amt * MASS_RESOURCE);
                    }
                } else {
                    if ((amt > 0) && (amt > p->info[playernum - 1].resource)) {
                        p->info[playernum - 1].resource = 0;
                    } else {
                        p->info[playernum - 1].resource -= amt;
                    }
                }

                rcv_resource(s, amt);
                sprintf(buf, "%d resources transferred.\n", amt);
                notify(playernum, governor, buf);

                break;
            default:
                notify(playernum, governor, "No such commodity.\n");

                if (sh) {
                    free(s2);
                } else {
                    free(p);
                }

                free(s);
                do_shiplist(&s, &nextshipno);

                continue;
            }

            if (sh) {
                /* Ship to ship transfer */
                bufc[0] = '\0';
                bufd[0] = bufc[0];
                bufr[0] = bufd[0];
                buff[0] = bufr[0];

                switch (commod) {
                case 'r':
                    sprintf(buf, "%d resources transferred.\n", amt);
                    notify(playernum, governor, buf);
                    sprintf(bufr, "%d Resources\n", amt);

                    break;
                case 'f':
                    sprintf(buf, "%d fuel transferred.n", amt);
                    notify(playernum, governor, buf);
                    sprintf(buff, "%d Fuel\n", amt);

                    break;
                case 'd':
                    sprintf(buf, "%d destruct transferred.\n", amt);
                    notify(playernum, governor, buf);
                    sprintf(bufd, "%d Destruct\n", amt);

                    break;
                case 'x':
                case '&':
                    sprintf(buf, "%d crystals transferred.\n", amt);
                    notify(playernum, governor, buf);
                    sprintf(bufd, "%d Crystal(s)\n", amt);

                    break;
                case 'c':
                    sprintf(buf, "%d popn transferred.\n", amt);
                    notify(playernum, governor, buf);

                    if (race->Metamorph) {
                        sprintf(bufc, "%d tons of biomass\n", amt);
                    } else {
                        sprintf(bufc, "%d population\n", amt);
                    }

                    break;
                case 'm':
                    sprintf(buf, "%d military transferred.\n", amt);
                    notify(playernum, governor, buf);

                    if (race->Metamorph) {
                        sprintf(bufm, "%d tons of biomass\n", amt);
                    } else {
                        sprintf(bufm, "%d population\n", amt);
                    }

                    break;
                default:

                    break;
                }

                putship(s2);
                free(s2);
            } else {
                if ((commod == 'c') || (commod == 'm')) {
                    putsector(sect, p, (int)s->land_x, (int)s->land_y);
                    free(sect);
                }

                putplanet(p,
                          Dir[playernum - 1][governor].snum,
                          Dir[playernum - 1][governor].pnum);

                free(p);
            }

            /* Do transporting here */
            if ((s->type == OTYPE_TRANSDEV)
                && s->special.transport.target
                && s->on) {
                do_transporter(race, governor, s);
            }

            putship(s);
            free(s);
        } else {
            /* Make sure you do this! */
            free(s);
        }

        do_shiplist(&s, &nextshipno);
    }
}

void jettison(int playernum, int governor, int apcount)
{
    int mod = 0;
    int shipno;
    int nextshipno;
    int amt;
    char commod;
    shiptype *s;
    racetype *race;

    if (argn < 2) {
        notify(playernum, governor, "Jettison what?\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
            if ((s->owner != playernum) || !s->alive) {
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (landed(s)) {
                notify(playernum, governor, "Ship is landed, cannot jettison.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!s->active) {
                sprintf(buf, "%s is irradiated and inactive.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (s->whatorbits == LEVEL_UNIV) {
                if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }
            } else if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (argn > 3) {
                amt = atoi(args[3]);
            } else {
                amt = 0;
            }

            race = races[playernum - 1];
            commod = args[2][0];

            switch (commod) {
            case 'x':
                amt = jettison_check(playernum,
                                     governor,
                                     amt,
                                     (int)s->crystals);

                if (amt > 0) {
                    s->crystals -= amt;

                    if (amt == 1) {
                        sprintf(buf, "%d crystal jettisoned.\n", amt);
                    } else {
                        sprintf(buf, "%d crystals jettisoned.\n", amt);
                    }

                    notify(playernum, governor, buf);
                    mod = 1;
                }

                break;
            case 'c':
                amt = jettison_check(playernum,
                                     governor,
                                     amt,
                                     (int)s->popn);

                if (amt > 0) {
                    s->popn -= amt;
                    s->mass -= (amt * race->mass);

                    if (amt == 1) {
                        sprintf(buf,
                                "%d crew hurls itself into deep space.\n",
                                amt);
                    } else {
                        sprintf(buf,
                                "%d crew hurls themselves into deep space.\n",
                                amt);
                    }

                    notify(playernum, governor, buf);
                    sprintf(buf,
                            "Complement of %s is now %u.\n",
                            Ship(s),
                            s->popn);

                    notify(playernum, governor, buf);
                    mod = 1;
                }

                break;
            case 'm':
                amt = jettison_check(playernum,
                                     governor,
                                     amt,
                                     (int)s->troops);

                if (amt > 0) {
                    if (amt == 1) {
                        sprintf(buf,
                                "%d military hurls itself into deep space.\n",
                                amt);
                    } else {
                        sprintf(buf,
                                "%d military hurls themselves into deep space.\n",
                                amt);
                    }

                    notify(playernum, governor, buf);

                    sprintf(buf,
                            "Complement of ships #%d is not %u.\n",
                            shipno,
                            s->troops - amt);

                    notify(playernum, governor, buf);
                    s->troops -= amt;
                    s->mass -= (amt * race->mass);
                    mod = 1;
                }

                break;
            case 'd':
                amt = jettison_check(playernum,
                                     governor,
                                     amt,
                                     (int)s->destruct);

                if (amt > 0) {
                    use_destruct(s, amt);
                    sprintf(buf, "%d destruct jettisoned.\n", amt);
                    notify(playernum, governor, buf);

                    if (!((s->type == OTYPE_FACTORY) ? Shipdata[s->type][ABIL_MAXCREW] - s->troops : s->max_crew - s->troops)) {
                        sprintf(buf, "\n%s ", Ship(s));
                        notify(playernum, governor, buf);

                        if (s->destruct) {
                            notify(playernum,
                                   governor,
                                   "still boobytrapped.\n");
                        } else {
                            notify(playernum,
                                   governor,
                                   "no longer boobytrapped.\n");
                        }
                    }

                    mod = 1;
                }

                break;
            case 'f':
                amt = jettison_check(playernum, governor, amt, (int)s->fuel);

                if (amt > 0) {
                    use_fuel(s, (double)amt);
                    sprintf(buf, "%d fuel jettisoned.\n", amt);
                    notify(playernum, governor, buf);
                    mod = 1;
                }

                break;
            case 'r':
                amt = jettison_check(playernum,
                                     governor,
                                     amt,
                                     (int)s->resource);

                if (amt > 0) {
                    use_resource(s, amt);
                    sprintf(buf, "%d resources jettisoned.\n", amt);
                    notify(playernum, governor, buf);
                    mod = 1;
                }

                break;
            default:
                notify(playernum, governor, "No such commodity valid.\n");

                return;
            }

            if (mod) {
                putship(s);
            }

            free(s);
        } else {
            free(s);
        }

        shipno = do_shiplist(&s, &nextshipno);
    }
}

int jettison_check(int playernum, int governor, int amt, int max)
{
    if (amt == 0) {
        amt = max;
    }

    if (amt < 0) {
        notify(playernum, governor, "Nice try.\n");

        return -1;
    } else if (amt > max) {
        sprintf(buf, "You can jettison at most %d\n", max);
        notify(playernum, governor, buf);

        return 1;
    }

    return amt;
}

/*
 * Nonexistent dump command by Gardan 20.12.1996
 *
 * void dump(int playernum, int governor, int apcount)
 * {
 *     sprintf(buf, "Dump command is not in use.\n");
 *     notify(playernum, governor, buf);
 * }
 */

/* Gardan code 20.12.1996 Start of commented dump code */

void dump(int playernum, int governor, int apcount)
{
    int player;
    int star;
    int i;
    int j;
    int knew_all = 1;
    racetype *race;
    racetype *r;
    placetype where;

    if (!enufAP(playernum, governor,Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    player = GetPlayer(args[1]);

    if (!player) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    r = races[playernum - 1];

    if (r->Guest) {
        notify(playernum,
               governor,
               "Guests cannot receive dump exploration data...\n");

        return;
    }

    /*
     * Transfer all planet and star knowledge to the player, get all stars and
     * planets
     */

    race = races[playernum - 1];

    if (race->Guest) {
        notify(playernum, governor, "Guests cannot dump exploration data...\n");

        return;
    }

    getsdata(&Sdata);

    sprintf(buf,
            "%s [%d] has given you exploration data about ",
            race->name,
            playernum);

    if (argn < 3) {
        int comma = 0;
        int new_info = -1;

        for (star = 0; star < Sdata.numstars; ++star) {
            getstar(&Stars[star], star);

            if (isset(Stars[star]->explored, playernum)) {
                if (!isset(Stars[star]->explored, player)) {
                    new_info = 1;
                    setbit(Stars[star]->explored, player);
                }

                for (i = 0; i < Stars[star]->numplanets; ++i) {
                    new_info = 0;
                    getplanet(&planets[star][i], star, i);

                    if (planets[star][i]->info[playernum - 1].explored) {
                        if (!planets[star][i]->info[player - 1].explored) {
                            planets[star][i]->info[playernum - 1].explored = 1;
                            new_info = 1;
                            putplanet(planets[star][i], star, i);
                        }
                    }
                }

                if (new_info) {
                    putstar(Stars[star], star);

                    if (comma) {
                        strcat(buf, ",");
                    }

                    strcat(buf, Stars[star]->name);
                    comma = 1;
                    knew_all = 0;
                }
            }
        }
    } else {
        /* List of places given */
        int comma = 0;
        int new_info;

        for (i = 2; i < argn; ++i) {
            where = Getplace(playernum, governor, args[i], 1);

            if (!where.err
                && (where.level != LEVEL_UNIV)
                && (where.level != LEVEL_SHIP)) {
                new_info = 0;
                star = where.snum;
                getstar(&Stars[star], star);

                if (isset(Stars[star]->explored, playernum)) {
                    if (!isset(Stars[star]->explored, player)) {
                        new_info = 1;
                        setbit(Stars[star]->explored, player);
                    }

                    for (j = 0; j < Stars[star]->numplanets; ++j) {
                        getplanet(&planets[star][j], star, j);

                        if (planets[star][j]->info[playernum - 1].explored) {
                            if (!planets[star][j]->info[player - 1].explored) {
                                planets[star][j]->info[player - 1].explored = 1;
                                new_info = 1;
                                putplanet(planets[star][j], star, j);
                            }
                        }
                    }

                    if (new_info) {
                        putstar(Stars[star], star);

                        if (comma) {
                            strcat(buf, ",");
                        }

                        strcat(buf, Stars[star]->name);
                        comma = 1;
                        knew_all = 0;
                    }
                }
            }
        }
    }

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    if (knew_all) {
        strcat(buf, "Nothing new");
    }

    strcat(buf, ".\n");
    warn_race(player, buf);
    notify(playernum, governor, "Exploration data transferred.\n");
}

/* Gardan 20.12.1996 end of commented dump command */

void transfer(int playernum, int governor, int apcount)
{
    int player;
    int give;
    planettype *planet;
    char commod = 0;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf, "You need to be in planet scope to do this.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    player = GetPlayer(args[1]);

    if (!player) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    getplanet(&planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    sscanf(args[2], "%c", &commod);
    give = atoi(args[3]);

    if (give < 0) {
        notify(playernum, governor, "You must specify a positive amount.\n");
        free(planet);

        return;
    }

    sprintf(temp,
            "%s/%s:",
            Stars[Dir[playernum - 1][governor].snum]->name,
            Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

    switch (commod) {
    case 'r':
        if (give > planet->info[playernum - 1].resource) {
            sprintf(buf, "You don't have %d on this planet.\n", give);
            notify(playernum, governor, buf);
        } else {
            /*
             * New code by Kharush. Prevents receiving player's stockpiles'
             * overflow. Fitted in switch so that one can not check how much
             * stuff the other player has in his stockpiles.
             */
            if (give > (USHRT_MAX - planet->info[player - 1].resource)) {
                sprintf(buf,
                        "Race %d has room only for %d resources!\n",
                        player,
                        USHRT_MAX - planet->info[player - 1].resource);

                notify(playernum, governor, buf);
            } else {
                /* End of new code. */
                planet->info[playernum - 1].resource -= give;
                planet->info[player - 1].resource += give;

                sprintf(buf,
                        "%s %d resources transferred from player %d to player #%d\n",
                        temp,
                        give,
                        playernum,
                        player);

                notify(playernum, governor, buf);
                warn_race(player, buf);
            }
        }

        break;
    case 'x':
    case '&':
        if (give > planet->info[playernum - 1].crystals) {
            sprintf(buf, "You don't have %d on this planet.\n", give);
            notify(playernum, governor, buf);
        } else {
            planet->info[playernum - 1].crystals -= give;
            planet->info[player - 1].crystals += give;

            sprintf(buf,
                    "%s %d crystal(s) transferred from player %d to #%d\n",
                    temp,
                    give,
                    playernum,
                    player);

            notify(playernum, governor, buf);
            warn_race(player, buf);
        }

        break;
    case 'f':
        if (give > planet->info[playernum - 1].fuel) {
            sprintf(buf, "You don't have %d fuel on this planet.\n", give);
            notify(playernum, governor, buf);
        } else {
            /* New code by Kharush. */
            if (give > (USHRT_MAX - planet->info[player - 1].fuel)) {
                sprintf(buf,
                        "Race %d has room only for %d fuel!\n",
                        player,
                        USHRT_MAX - planet->info[player - 1].fuel);

                notify(playernum, governor, buf);
            } else {
                /* End of new code. */
                planet->info[playernum - 1].fuel -= give;
                planet->info[player - 1].fuel += give;

                sprintf(buf,
                        "%s %d fuel transferred from player %d to player #%d\n",
                        temp,
                        give,
                        playernum,
                        player);

                notify(playernum, governor, buf);
                warn_race(player, buf);
            }
        }

        break;
    case 'd':
        if (give > planet->info[playernum - 1].destruct) {
            sprintf(buf, "You don't have %d destruct on this planet.\n", give);
            notify(playernum, governor, buf);
        } else {
            /* New code by Kharush. */
            if (give > (USHRT_MAX - planet->info[player - 1].destruct)) {
                sprintf(buf,
                        "Race %d has room only for %d destruct!\n",
                        player,
                        USHRT_MAX - planet->info[player - 1].destruct);

                notify(playernum, governor, buf);
            } else {
                /* End of new code. */
                planet->info[playernum - 1].destruct -= give;
                planet->info[player - 1].destruct += give;

                sprintf(buf,
                        "%s %d destruct transferred from player %d to player #%d\n",
                        temp,
                        give,
                        playernum,
                        player);

                notify(playernum, governor, buf);
                warn_race(player, buf);
            }
        }

        break;
    default:
        sprintf(buf, "What?\n");
        notify(playernum, governor, buf);
    }

    putplanet(planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    free(planet);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);
}

void mount(int playernum, int governor, int apcount)
{
    _mount(playernum, governor, apcount, 1);
}

void dismount(int playernum, int governor, int apcount)
{
    _mount(playernum, governor, apcount, 0);
}

void _mount(int playernum, int governor, int apcount, int mnt)
{
    shiptype *ship;
    int shipno;
    int nextshipno;

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&ship, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], ship, &nextshipno)) {
            if (!ship->mount) {
                notify(playernum,
                       governor,
                       "This ship is not equipped with a crystal mount.\n");

                free(ship);
                shipno = do_shiplist(&ship, &nextshipno);

                continue;
            }

            if (ship->mounted && mnt) {
                notify(playernum,
                       governor,
                       "You already have a crystal mounted.\n");

                free(ship);
                shipno = do_shiplist(&ship, &nextshipno);

                continue;
            } else if (!ship->mounted && !mnt) {
                notify(playernum,
                       governor,
                       "You don't have a crystal mounted.\n");

                free(ship);
                shipno = do_shiplist(&ship, &nextshipno);

                continue;
            } else if (!ship->mounted && mnt) {
                if (!ship->crystals) {
                    notify(playernum,
                           governor,
                           "You have no crystals on board.\n");

                    free(ship);
                    shipno = do_shiplist(&ship, &nextshipno);
                }

                ship->mounted = 1;
                --ship->crystals;
                notify(playernum, governor, "Mounted.\n");
            } else if (ship->mounted && !mnt) {
                if (ship->crystals == 127) {
                    notify(playernum,
                           governor,
                           "You can't dismount the crystal. Max allowed already on board.\n");

                    free(ship);
                    shipno = do_shiplist(&ship, &nextshipno);

                    continue;
                }

                ship->mounted = 0;
                ++ship->crystals;
                notify(playernum, governor, "Dismounted.\n");

                if (ship->hyper_drive.charge || ship->hyper_drive.ready) {
                    ship->hyper_drive.charge = 0;
                    ship->hyper_drive.ready = 0;
                    notify(playernum, governor, "Discharged.\n");
                }

                if (ship->laser && ship->fire_laser) {
                    ship->fire_laser = 0;
                    notify(playernum, governor, "Laser deactivated.\n");
                }
            } else {
                notify(playernum, governor, "Weird error in 'mount'.\n");
                free(ship);
                shipno = do_shiplist(&ship, &nextshipno);

                continue;
            }

            putship(ship);
            free(ship);
        } else {
            free(ship);
        }

        shipno = do_shiplist(&ship, &nextshipno);
    }
}

void use_fuel(shiptype *s, double amt)
{
    s->fuel -= amt;
    s->mass -= (amt * MASS_FUEL);
}

void use_destruct(shiptype *s, int amt)
{
    s->destruct -= amt;
    s->mass -= ((double)amt * MASS_DESTRUCT);
}

void use_resource(shiptype *s, int amt)
{
    s->resource -= amt;
    s->mass -= ((double)amt * MASS_RESOURCE);
}

void use_popn(shiptype *s, int amt, double mass)
{
    s->popn -= amt;
    s->mass -= ((double)amt * mass);
}

void rcv_fuel(shiptype *s, double amt)
{
    s->fuel += amt;
    s->mass += (amt * MASS_FUEL);
}

void rcv_resource(shiptype *s, int amt)
{
    s->resource += amt;
    s->mass += ((double)amt * MASS_RESOURCE);
}

void rcv_destruct(shiptype *s, int amt)
{
    s->destruct += amt;
    s->mass += ((double)amt * MASS_DESTRUCT);
}

void rcv_popn(shiptype *s, int amt, double mass)
{
    s->popn += amt;
    s->mass += ((double)amt * mass);
}

void rcv_troops(shiptype *s, int amt, double mass)
{
    s->troops += amt;
    s->mass += ((double)amt * mass);
}

void do_transporter(racetype *race, int governor, shiptype *s)
{
    int playernum;
    shiptype *s2;

    playernum = race->Playernum;

    if (!landed(s) && !inship(s)) {
        notify(playernum, governor, "Origin ship not landed.\n");

        return;
    }

    if ((s->storbits != Dir[playernum - 1][governor].snum)
        || (s->pnumorbits != Dir[playernum - 1][governor].pnum)) {
        sprintf(buf, "Change scope to the planet the ship is landed on!\n");
        notify(playernum, governor, buf);

        return;
    }

    if (s->damage) {
        notify(playernum, governor, "Origin device is damaged.\n");

        return;
    }

    if (!getship(&s2, (int)s->special.transport.target)) {
        sprintf(buf, "The hopper seems to be blocked.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (!s2->alive || (s2->type != OTYPE_TRANSDEV) || !s2->on) {
        sprintf(buf, "The target device is not receiving.\n");
        notify(playernum, governor, buf);
        free(s2);

        return;
    }

    if (!landed(s2) && !inship(s2)) {
        notify(playernum, governor, "Target ship not landed.\n");
        free(s2);

        return;
    }

    if (s2->damage) {
        notify(playernum, governor, "Target device is damaged.\n");
        free(s2);

        return;
    }

    sprintf(buf, "Zap\07!\n"); /* ^G */
    notify(playernum, governor, buf);

    /* Send stuff to other ship (could be transport device) */
    if (s->resource) {
        rcv_resource(s2, (int)s->resource);
        sprintf(buf, "%d resources transferred.\n", s->resource);
        notify(playernum, governor, buf);
        sprintf(bufr, "%d Resources\n", s->resource);
        use_resource(s, (int)s->resource);
    } else {
        bufr[0] = '\0';
    }

    if (s->fuel) {
        rcv_fuel(s2, s->fuel);
        sprintf(buf, "%g fuel transferred.\n", s->fuel);
        notify(playernum, governor, buf);
        sprintf(buff, "%g Fuel\n", s->fuel);
        use_fuel(s, s->fuel);
    } else {
        buff[0] = '\0';
    }

    if (s->destruct) {
        rcv_destruct(s2, (int)s->destruct);
        sprintf(buf, "%d destruct transferred.\n", s->destruct);
        notify(playernum, governor, buf);
        sprintf(bufd, "%d Destruct\n", s->destruct);
        use_destruct(s, (int)s->destruct);
    } else {
        bufd[0] = '\0';
    }

    if (s->popn) {
        s2->mass += (s->popn * race->mass);
        s2->popn += s->popn;

        sprintf(buf, "%d populations transferred.\n", s->popn);
        notify(playernum, governor, buf);

        if (race->Metamorph) {
            sprintf(bufc, "%d tons of biomass\n", s->popn);
        } else {
            sprintf(bufc, "%d population\n", s->popn);
        }

        s->mass -= (s->popn * race->mass);
        s->popn -= s->popn;
    } else {
        bufc[0] = '\0';
    }

    if (s->crystals) {
        s2->crystals += s->crystals;
        sprintf(buf, "%d crystal(s) transferred.\n", s->crystals);
        notify(playernum, governor, buf);
        sprintf(bufx, "%d crystal(s)\n", s->crystals);
        s->crystals = 0;
    } else {
        bufx[0] = '\0';
    }

    if (s2->owner != s->owner) {
        sprintf(telegram_buf,
                "Audio-vibratory-physio-molecular transport device #");

        sprintf(buf,
                "%s gave your ship %s the following:\n",
                Ship(s),
                Ship(s2));

        strcat(telegram_buf, buf);
        strcat(telegram_buf, bufr);
        strcat(telegram_buf, bufd);
        strcat(telegram_buf, buff);
        strcat(telegram_buf, bufc);
        strcat(telegram_buf, bufm);
        strcat(telegram_buf, bufx);
        warn((int)s2->owner, (int)s2->governor, telegram_buf);
    }

    putship(s2);
    free(s2);
}

int landed_on(shiptype *s, int shipno)
{
    return ((s->whatorbits == LEVEL_SHIP) && (s->destshipno == shipno));
}

void unload_onto_alien_sector(int playernum,
                              int governor,
                              planettype *planet,
                              shiptype *ship,
                              sectortype *sect,
                              int what,
                              int people)
{
    double astrength;
    double dstrength;
    int oldowner;
    int oldgov;
    int oldpopn;
    int old2popn;
    int old3popn;
    int casualties;
    int casualties2;
    int casualties3;
    int absorbed;
    int defense;
    racetype *race;
    racetype *alien;

    if (people <= 0) {
        notify(playernum,
               governor,
               "You have to unload to assault alien sectors.\n");

        return;
    }

    ++ground_assaults[playernum - 1][sect->owner - 1][Dir[playernum - 1][governor].snum];

    race = races[playernum - 1];
    alien = races[sect->owner - 1];
    /* Races find out about each other */
    alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + 5, 100);
    race->translate[sect->owner - 1] = MIN(race->translate[sect->owner - 1] + 5, 100);

    oldowner = (int)sect->owner;
    oldgov = Stars[Dir[playernum - 1][governor].snum]->governor[sect->owner - 1];

    if (what == CIV) {
        ship->popn -= people;
    } else {
        ship->troops -= people;
    }

    ship->mass -= (people * race->mass);

    if (what == CIV) {
        sprintf(buf, "%d civ unloaded...\n", people);
    } else {
        sprintf(buf, "%d mil unloaded...\n", people);
    }

    notify(playernum, governor, buf);
    sprintf(buf, "Crew compliment %d civ  %d mil\n", ship->popn, ship->troops);
    notify(playernum, governor, buf);

    if (what == CIV) {
        sprintf(buf,
                "%d civ assault %d civ/%d mil\n",
                people,
                sect->popn,
                sect->troops);
    } else {
        sprintf(buf,
                "%d mil assault %d civ/%d mil\n",
                people,
                sect->popn,
                sect->troops);
    }

    notify(playernum, governor, buf);
    oldpopn = people;
    old2popn = sect->popn;
    old3popn = sect->troops;

    defense = Defensedata[sect->condition];

    ground_attack(race,
                  alien,
                  &people,
                  what,
                  &sect->popn,
                  &sect->troops,
                  (unsigned int)ship->armor,
                  (unsigned int)defense,
                  1.0 - ((double)ship->damage / 100.0),
                  alien->likes[sect->condition],
                  &astrength,
                  &dstrength,
                  &casualties,
                  &casualties2,
                  &casualties3);

    sprintf(buf, "Attack: %.2f   Defense: %.2f.\n", astrength, dstrength);
    notify(playernum, governor, buf);

    if (!(sect->popn + sect->troops)) {
        /* We got them */
        /* Mesomorphs absorb the bodies of their victims */
        absorbed = 0;

        if (race->absorb) {
            absorbed = int_rand(0, old2popn + old3popn);
            sprintf(buf, "%d alien bodies absorbed.\n", absorbed);
            notify(playernum, governor, buf);
            sprintf(buf, "Metamorphs have absorbed %d bodies!!!\n", absorbed);
            notify(oldowner, oldgov, buf);
        }

        if (what == CIV) {
            sect->popn = people + absorbed;
        } else if (what == MIL) {
            sect->popn = absorbed;
            sect->troops = people;
        }

        sect->owner = playernum;
        adjust_morale(race, alien, (int)alien->fighters);
    } else {
        /* Retreat */
        absorbed = 0;

        if (alien->absorb) {
            absorbed = int_rand(0, oldpopn - people);
            sprintf(buf, "%d alien bodies absorbed.\n", absorbed);
            notify(oldowner, oldgov, buf);
            sprintf(buf, "Metamorphs have absorbed %d bodies!!!\n", absorbed);
            notify(playernum, governor, buf);
            sect->popn += absorbed;
        }

        /* Load them back up */
        if (what == CIV) {
            sprintf(buf, "Loading %d civ\n", people);
        } else {
            sprintf(buf,"Loading %d mil\n", people);
        }

        notify(playernum, governor, buf);

        if (what == CIV) {
            ship->popn += people;
        } else {
            ship->troops += people;
        }

        /*
         * This caused negative ship mass bug (kse):
         * ship->mass -= (people * race->mass);
         */
        ship->mass += (people * race->mass);
        adjust_morale(alien, race, (int)race->fighters);
    }

    if (sect->owner == playernum) {
        sprintf(telegram_buf,
                "/%s/%s: %s [%d] %s assaults %s [%d] %c(%d,%d) VICTORY\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                race->name,
                playernum,
                Ship(ship),
                alien->name,
                alien->Playernum,
                Dessymbols[sect->condition],
                ship->land_x,
                ship->land_y);
    } else {
        sprintf(telegram_buf,
                "/%s/%s: %s [%d] %s assaults %s [%d] %c(%d,%d) DEFEAT\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                race->name,
                playernum,
                Ship(ship),
                alien->name,
                alien->Playernum,
                Dessymbols[sect->condition],
                ship->land_x,
                ship->land_y);
    }

    if (sect->owner == playernum) {
        sprintf(buf, "VICTORY! The sector is yours!\n");
        notify(playernum, governor, buf);
        sprintf(buf, "Sector CAPTURED!\n");
        strcat(telegram_buf, buf);

        if (people) {
            if (what == CIV) {
                sprintf(buf, "%d civilians move in.\n", people);
            } else {
                sprintf(buf, "%d troops move in.\n", people);
            }

            notify(playernum, governor, buf);
        }

        ++planet->info[playernum - 1].numsectsowned;
        planet->info[playernum - 1].mob_points += (int)sect->mobilization;
        /* Now planetary troop and civ counters shouldn't go bozo (kse) */
        planet->info[playernum - 1].popn += sect->popn;
        planet->info[playernum - 1].troops += sect->troops;

        if (planet->info[oldowner - 1].numsectsowned) {
            --planet->info[oldowner - 1].numsectsowned;
        }

        planet->info[oldowner - 1].mob_points -= (int)sect->mobilization;
    } else {
        sprintf(buf, "the invasion was repulsed; try again.\n");
        notify(playernum, governor, buf);
        sprintf(buf, "You fought them off!\n");
        strcat(telegram_buf, buf);
        /* Now planetary troop and civ counters shouldn't go bozo (kse) */
        planet->info[playernum - 1].popn -= (old2popn - sect->popn);
        planet->info[playernum - 1].troops -= (old3popn - sect->troops);
    }

    if (!(sect->popn + sect->troops + people)) {
        sprintf(buf, "You killed all of them!\n");
        strcat(telegram_buf, buf);
        /* Increase modifier */
        race->translate[oldowner - 1] = MIN(race->translate[oldowner - 1] + 5, 100);
    }

    if (!people) {
        sprintf(buf, "Oh no! They killed your party to the last man!\n");
        notify(playernum, governor, buf);
        /* Increase modifier */
        alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + 5, 100);
    }

    putrace(alien);
    putrace(race);

    if (what == CIV) {
        sprintf(buf,
                "Casualties: You: %d civ/%d mil, Them: %d civ\n",
                casualties2,
                casualties3,
                casualties);
    } else {
        sprintf(buf,
                "Casualties: You: %d civ/%d mil, them: %d mil\n",
                casualties2,
                casualties3,
                casualties);
    }

    strcat(telegram_buf, buf);
    warn(oldowner, oldgov, telegram_buf);

    if (what == CIV) {
        sprintf(buf,
                "Casualties: You: %d civ, Them: %d civ/%d mil\n",
                casualties,
                casualties2,
                casualties3);
    } else {
        sprintf(buf,
                "Casualties: You: %d mil, Them: %d civ/%d mil\n",
                casualties,
                casualties2,
                casualties3);
    }

    notify(playernum, governor, buf);
}
