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
 * build -- Build a ship
 *
 * Mon Apr 15 02:07:08 MDT 1991 Reformatted the 'make' command when at factory
 * scope. Evan Koffler
 *
 * Various bug fixes and mods by Tim Brown 1997-98
 *
 * #ident  "@(#)build.c  1.14 12/3/93 "
 *
 * $Header: /var/cvs/bgp/GB+/user/build.c,v 1.7 2007/07/06 18:09:34 gbp Exp $
 */
#include "build.h"

#include <ctype.h>
#include <curses.h>
#include <math.h>
#include <setjmp.h>
#include <stdlib.h> /* Added for free() and atoi() (kse) */
#include <string.h>

#include "buffers.h"
#include "GB_server.h"
#include "getplace.h"
#include "files_shl.h"
#include "lists.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "shlmisc.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

#include "fire.h"
#include "land.h"
#include "shootblast.h"
#include "tele.h"

extern int ShipVector[];

#ifdef MARKET
char const *Commod[] = {"resources", "destruct", "fuel", "crystals"};
#endif

int getcount(int, char *);
int can_build_at_planet(int, int, startype *, planettype *);
int get_build_type(char *);
int can_build_this(int, racetype *, char *);
int can_build_on_ship(int, racetype *, shiptype *, char *);
int can_build_on_sector(int, racetype *, planettype *, sectortype *, int, int, char *);
int build_at_ship(int, int, racetype *, shiptype *, int *, int *);
void autoload_at_planet(int, shiptype *, planettype *, sectortype *, int *, double *);
void autoload_at_ship(int, shiptype *, shiptype *, int *, double *);
void initialize_new_ship(int, int, racetype *, shiptype *, double, int, int);
int create_ship_by_planet(int, int, racetype *, shiptype *, planettype *, int, int, int, int);
int create_ship_by_ship(int, int, racetype *, int, startype *, planettype *, shiptype *, shiptype *);
double getmass(shiptype *);
int ship_size(shiptype *);
double cost(shiptype *);
void system_cost(double *, double *, int, int);
double complexity(shiptype *);
void Getship(shiptype *, int, racetype *);
void Getfactship(shiptype *, shiptype *);
int Shipcost(int, racetype *);
int shipping_cost(int, int, double *, int);
int inship(shiptype *); /* This was missing (kse) */

/* Upgrade ship characteristics */
void upgrade(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int value;
    int oldcost;
    int newcost;
    int netcost;
    int oldtech;
    int newtech = 0;
    shiptype ship;
    shiptype *dirship;
    shiptype *s2;
    double complex;
    racetype *race;
    float orig_complex;
    int techflag = 0;

    if (Dir[playernum - 1][governor].level != LEVEL_SHIP) {
        notify(playernum,
               governor,
               "You have to change scope to the ship you wish to upgrade.\n");

        return;
    }

    if (!getship(&dirship, Dir[playernum - 1][governor].shipno)) {
        sprintf(buf, "Illegal dir value.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (testship(playernum, governor, dirship)) {
        notify(playernum, governor, "Error in testship.\n");
        free(dirship);

        return;
    }

    if (dirship->damage) {
        notify(playernum, governor, "You cannot upgrade damaged ships.\n");
        free(dirship);

        return;
    }

    if (!dirship->popn) {
        notify(playernum, governor, "You cannot upgrade ships without crew.\n");
        free(dirship);

        return;
    }

    if (dirship->type == OTYPE_FACTORY) {
        notify(playernum, governor, "You cannot upgrade factories.\n");
        free(dirship);

        return;
    }

    orig_complex = complexity(dirship);
    race = races[playernum - 1];
    memcpy(&ship, dirship, sizeof(shiptype));

    if (argn == 3) {
        sscanf(args[2], "%d", &value);
    } else {
        value = 0;
    }

    if (value < 0) {
        notify(playernum, governor, "That's a ridiculous setting.\n");
        free(dirship);

        return;
    }

    if (Shipdata[dirship->build_type][ABIL_MOD]) {
        if (matchic(args[1], "armor")) {
            ship.armor = MAX(dirship->armor, MIN(value, 100));
        } else if (matchic(args[1], "cloak")
                   && Shipdata[dirship->build_type][ABIL_CLOAK]) {
            if (!Cloak(race)) {
                sprintf(buf, "Your race cannot build cloaking devices.\n");
                notify(playernum, governor, buf);
                free(dirship);

                return;
            }

            newtech = race->tech;
            oldtech = dirship->tech;
            netcost = (int)(((double)(newtech - oldtech) / (double)(2 * oldtech)) * (double)dirship->build_cost);
            ship.cloak = 1;
            sprintf(buf, "Cloaking device installed.\n");
            notify(playernum, governor, buf);
        } else if (matchic(args[1], "tech")) {
            newtech = race->tech;
            oldtech = dirship->tech;

            netcost = (int)(((double)(newtech - oldtech) / (double)(2 * oldtech)) * (double)dirship->build_cost) / (SHIP_TOO_OLD / 25);

            if (newtech <= oldtech) {
                notify(playernum,
                       governor,
                       "Ship's current tech is as good or higher than yours. Tech not modified.\n");

                free(dirship);

                return;
            }

            if (netcost > dirship->max_resource) {
                sprintf(buf,
                        "Not upgradable, technology too old, scrap and rebuild.\n");

                notify(playernum, governor, buf);
                free(dirship);

                return;
            } else if (netcost > dirship->resource) {
                sprintf(buf,
                        "You need %d resources on board to make this modification.\n",
                        netcost);

                notify(playernum, governor, buf);
                free(dirship);

                return;
            }

            sprintf(buf,
                    "Ship upgraded to %d tech at a cost of %d resources.\n",
                    newtech,
                    netcost);

            notify(playernum, governor, buf);

            techflag = MAX(1, ship.tech);
            ship.tech = newtech;
            ship.age /= 4;

            memcpy(dirship, &ship, sizeof(shiptype));
            dirship->resource -= netcost;
            putship(dirship);
            free(dirship);

            return;
        } else if (matchic(args[1], "crew")
                   && Shipdata[dirship->build_type][ABIL_MAXCREW]) {
            ship.max_crew = MAX(dirship->max_crew, MIN(value, 10000));
        } else if (matchic(args[1], "cargo")
                   && Shipdata[dirship->build_type][ABIL_CARGO]) {
            ship.max_resource = MAX(dirship->max_resource, MIN(value, 10000));
        } else if (matchic(args[1], "hanger")
                   && Shipdata[dirship->build_type][ABIL_HANGER]) {
            ship.max_hanger = MAX(dirship->max_hanger, MIN(value, 10000));
        } else if (matchic(args[1], "fuel")
                   && Shipdata[dirship->build_type][ABIL_FUELCAP]) {
            ship.max_fuel = MAX(dirship->max_fuel, MIN(value, 10000));
        } else if (matchic(args[1], "mount")
                   && Shipdata[dirship->build_type][ABIL_MOUNT]
                   && !dirship->mount) {
            if (!Crystal(race)) {
                notify(playernum,
                       governor,
                       "Your race does not know how to utilize crystal power yet.\n");
                free(dirship);

                return;
            }

            ship.mount = !ship.mount;
        } else if (matchic(args[1], "destruct")
                   && Shipdata[dirship->build_type][ABIL_DESTCAP]) {
            ship.max_destruct = MAX(dirship->max_destruct, MIN(value, 10000));
        } else if (matchic(args[1], "speed")
                   && Shipdata[dirship->build_type][ABIL_SPEED]) {
            ship.max_speed = MAX(dirship->max_speed, MAX(1, MIN(value, 9)));
        } else if (matchic(args[1], "hyperdrive")
                   && Shipdata[dirship->build_type][ABIL_JUMP]
                   && !dirship->hyper_drive.has
                   && Hyper_drive(race)) {
            ship.hyper_drive.has = 1;
        } else if (matchic(args[1], "primary")
                   && Shipdata[dirship->build_type][ABIL_PRIMARY]) {
            if (matchic(args[2], "strength")) {
                if (ship.primtype == NONE) {
                    notify(playernum, governor, "No caliber defined.\n");
                    free(dirship);

                    return;
                }

                ship.primary = atoi(args[3]);
                ship.primary = MAX(ship.primary, dirship->primary);
            } else if (matchic(args[2], "caliber")) {
                if (matchic(args[3], "light")) {
                    ship.primtype = MAX(LIGHT, dirship->primtype);
                } else if (matchic(args[3], "medium")) {
                    ship.primtype = MAX(MEDIUM, dirship->primtype);
                } else if (matchic(args[3], "heavy")) {
                    ship.primtype = MAX(HEAVY, dirship->primtype);
                } else {
                    notify(playernum, governor, "No such caliber.\n");
                    free(dirship);

                    return;
                }

                ship.primtype = MIN(Shipdata[dirship->build_type][ABIL_PRIMARY],
                                    ship.primtype);
            } else {
                notify(playernum, governor, "No such gun characteristic.\n");
                free(dirship);

                return;
            }
        } else if (matchic(args[1], "secondary")
                   && Shipdata[dirship->build_type][ABIL_SECONDARY]) {
            if (matchic(args[2], "strength")) {
                if (ship.sectype == NONE) {
                    notify(playernum, governor, "No caliber defined.\n");
                    free(dirship);

                    return;
                }

                ship.secondary = atoi(args[3]);
                ship.secondary = MAX(ship.secondary, dirship->secondary);
            } else if (matchic(args[2], "caliber")) {
                if (matchic(args[3], "light")) {
                    ship.sectype = MAX(LIGHT, dirship->sectype);
                } else if (matchic(args[3], "medium")) {
                    ship.sectype = MAX(MEDIUM, dirship->sectype);
                } else if (matchic(args[3], "heavy")) {
                    ship.sectype = MAX(HEAVY, dirship->sectype);
                } else {
                    notify(playernum, governor, "No such caliber.\n");
                    free(dirship);

                    return;
                }

                ship.sectype = MIN(Shipdata[dirship->build_type][ABIL_SECONDARY],
                                   ship.sectype);
            } else {
                notify(playernum, governor, "No such gun characteristic.\n");
                free(dirship);

                return;
            }
        } else if (matchic(args[1], "cew")
                   && Shipdata[dirship->build_type][ABIL_CEW]) {
            if (!Cew(race)) {
                sprintf(buf,
                        "Your race cannot build confined energy weapons.\n");

                notify(playernum, governor, buf);
                free(dirship);

                return;
            }

            if (!Shipdata[dirship->build_type][ABIL_CEW]) {
                notify(playernum,
                       governor,
                       "This kind of ship cannot mount confined energy weapons.\n");

                free(dirship);

                return;
            }

            value = atoi(args[3]);

            if (matchic(args[2], "strength")) {
                ship.cew = value;
            } else if (matchic(args[2], "range")) {
                ship.cew_range = value;
            } else {
                notify(playernum, governor, "No such option for CEWs.\n");
                free(dirship);

                return;
            }
        } else if (matchic(args[1], "laser")
                   && Shipdata[dirship->build_type][ABIL_LASER]) {
            if (!Laser(race)) {
                sprintf(buf, "Your race cannot build lasers.\n");
                notify(playernum, governor, buf);
                free(dirship);

                return;
            }

            if (Shipdata[dirship->build_type][ABIL_LASER]) {
                ship.laser = 1;
            } else {
                notify(playernum,
                       governor,
                       "That ship cannot be fitted with combat lasers.\n");

                free(dirship);

                return;
            }
        } else {
            notify(playernum,
                   governor,
                   "That characteristic either doesn't exist or can't be modified.\n");

            free(dirship);

            return;
        }
    } else {
        notify(playernum, governor, "This ship cannot be upgraded.\n");
        free(dirship);

        return;
    }

    /* Check to see whether this ship can actually be built by this player */
    complex = complexity(&ship);

    if (complex > race->tech) {
        sprintf(buf,
                "This upgrade requires an engineering technology of %.1f.\n",
                complex);

        notify(playernum, governor, buf);
        free(dirship);

        return;
    }

    /*
     * Check to see if the new ship will actually fit inside the hanger if it is
     * on another ship. Maarten
     */
    if (dirship->whatorbits == LEVEL_SHIP) {
        getship(&s2, dirship->destshipno);

        if ((s2->max_hanger - (s2->hanger - dirship->size)) < ship_size(&ship)) {
            sprintf(buf,
                    "Not enough free hanger space on %c%d.\n",
                    Shipltrs[s2->type],
                    dirship->destshipno);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "%d more needed.\n",
                    ship_size(&ship) - (s2->max_hanger - (s2->hanger - dirship->size)));

            notify(playernum, governor, buf);
            free(s2);
            free(dirship);

            return;
        }
    }

    /* Compute new ship costs and see if the player can afford it */
    if (race->God) {
        newcost = 0;
        oldcost = 0;
        netcost = 0;
    } else {
        newcost = (int)cost(&ship);
        oldcost = dirship->build_cost;
        netcost = 2 * (newcost - oldcost); /* Upgrade is expensive */
    }

    if (newcost < oldcost) {
        notify(playernum, governor, "You cannot downgrade ships!\n");
        free(dirship);

        return;
    }

    if (!race->God) {
        netcost = MAX(netcost, 1);
    }

    if (netcost > dirship->resource) {
        sprintf(buf, "Old value %dr   New value %dr\n", oldcost, newcost);
        notify(playernum, governor, buf);

        sprintf(buf,
                "You need %d resource on board to make this modification.\n",
                netcost);

        notify(playernum, governor, buf);
    } else if (netcost || race->God) {
        sprintf(buf, "Old value %dr   New value %dr\n", oldcost, newcost);
        notify(playernum, governor, buf);
        memcpy(dirship, &ship, sizeof(shiptype));
        dirship->resource -= netcost;

        if (dirship->whatorbits == LEVEL_SHIP) {
            s2->hanger -= (unsigned short)dirship->size;
            dirship->size = ship_size(dirship);
            s2->hanger += (unsigned short)dirship->size;
            putship(s2);
        }

        dirship->size = ship_size(dirship);
        dirship->base_mass = getmass(dirship);

        if (race->God) {
            dirship->build_cost = 0;
        } else {
            dirship->build_cost = cost(dirship);
        }

        dirship->complexity = complexity(dirship);

        /* From HAP -mfw */
        if (DAMAGE_TO_UPGRADE) {
            int up_dam;

            up_dam = (int)((100 * (complex / orig_complex)) - 100);

            if (techflag) {
                up_dam += (int)(2 * sqrt(newtech));
            }

            up_dam = MIN(99, up_dam);
            up_dam = MAX(1, up_dam);

            if (race->God) {
                up_dam = 0;
            }

            sprintf(buf,
                    "Old complexity: %.1f  New complexity: %.1f\n",
                    orig_complex,
                    complex);

            notify(playernum, governor, buf);
            sprintf(buf, "This upgrade does %d points of damage.\n", up_dam);
            notify(playernum, governor, buf);
            dirship->damage = MIN(99, up_dam + dirship->damage);
            sprintf(buf, "The ship is now at %d%% damage.\n", dirship->damage);
            notify(playernum, governor, buf);
        }

        if (techflag) {
            sprintf(buf, "The ship's tech is not %d.\n", (int)dirship->tech);
            notify(playernum, governor, buf);
        }

        /* End HAP */

        putship(dirship);
    } else {
        notify(playernum, governor, "You can not make this modification.\n");
    }

    free(dirship);
}

void make_mod(int playernum, int governor, int apcount, int mode, orbitinfo *unused5)
{
    int i;
    int value;
    unsigned short size;
    char shipc;
    shiptype *dirship;
    racetype *race;
    double cost0;

    if (Dir[playernum - 1][governor].level != LEVEL_SHIP) {
        notify(playernum,
               governor,
               "You have to change scope to an installation.\n");

        return;
    }

    if (!getship(&dirship, Dir[playernum - 1][governor].shipno)) {
        sprintf(buf, "Illegal dir value.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (testship(playernum, governor, dirship)) {
        free(dirship);

        return;
    }

    if (dirship->type != OTYPE_FACTORY) {
        notify(playernum, governor, "That is not a factory.\n");
        free(dirship);

        return;
    }

    if (dirship->on && (argn > 1)) {
        notify(playernum, governor, "This factory is already online.\n");
        free(dirship);

        return;
    }

    race = races[playernum - 1];

    /*
     * Save size of the factory, and set it to the correct values for the
     * design. Maarten
     */
    size = dirship->size;
    dirship->size = ship_size(dirship);

    if (mode == 0) {
        if (argn < 2) {
            /* List the current settings for the factory */
            if (!dirship->build_type
                || (dirship->build_type == OTYPE_FACTORY)) {
                notify(playernum, governor, "No ship type specified.\n");
                free(dirship);

                return;
            }

            notify(playernum,
                   governor,
                   "  --- Current Production Specifications ---\n");

            sprintf(buf,
                    "%-23s Armor:    %4d          Guns",
                    (dirship->on ? "Online" : "Offline"),
                    dirship->armor);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_PRIMARY]
                && (dirship->primtype != NONE)) {
                if (dirship->primtype == LIGHT) {
                    sprintf(buf, "%3dL", dirship->primary);
                } else if (dirship->primtype == MEDIUM) {
                    sprintf(buf, "%3dM", dirship->primary);
                } else if (dirship->primtype == HEAVY) {
                    sprintf(buf, "%3dH", dirship->primary);
                } else {
                    sprintf(buf, "%3dN", dirship->primary);
                }

                notify(playernum, governor, buf);
            }

            if (Shipdata[dirship->build_type][ABIL_SECONDARY]
                && (dirship->sectype != NONE)) {
                if (dirship->sectype == LIGHT) {
                    sprintf(buf, "/%dL", dirship->secondary);
                } else if (dirship->sectype == MEDIUM) {
                    sprintf(buf, "/%dM", dirship->secondary);
                } else if (dirship->sectype == HEAVY) {
                    sprintf(buf, "/%dH", dirship->secondary);
                } else {
                    sprintf(buf, "/%dN", dirship->secondary);
                }

                notify(playernum, governor, buf);
            }

            notify(playernum, governor, "\n");

            sprintf(buf,
                    "Ship:  %-16.16s Crew:     %4d",
                    Shipnames[dirship->build_type],
                    dirship->max_crew);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_MOUNT]) {
                if (dirship->mount) {
                    sprintf(buf, "          Xtal Mount: yes\n");
                } else {
                    sprintf(buf, "          Xtal Mount: no\n");
                }

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            sprintf(buf,
                    "Class: %-16.16s Fuel:     %4d",
                    dirship->class,
                    dirship->max_fuel);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_JUMP]) {
                if (dirship->hyper_drive.has) {
                    sprintf(buf, "          Hyperdrive: yes\n");
                } else {
                    sprintf(buf, "          Hyperdrive: no\n");
                }

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            sprintf(buf,
                    "Cost:  %-5d            Cargo:    %4d",
                    dirship->build_cost,
                    dirship->max_resource);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_CLOAK]) {
                if (dirship->cloak) {
                    sprintf(buf, "          Cloak: yes\n");
                } else {
                    sprintf(buf, "          Cloak: no\n");
                }

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            sprintf(buf,
                    "Mas:  %-5.1f            Hanger:    %4u",
                    dirship->base_mass,
                    dirship->max_hanger);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_LASER]) {
                if (dirship->laser) {
                    sprintf(buf, "          Combat Lasers: yes\n");
                } else {
                    sprintf(buf, "          Combat Lasers: no\n");
                }

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            sprintf(buf,
                    "Size:  %-6d           Destruct: %4d",
                    dirship->size,
                    dirship->max_destruct);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_CEW]) {
                if (dirship->cew) {
                    sprintf(buf, "          CEW: yes\n");
                } else {
                    sprintf(buf, "          CEW: no\n");
                }

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            sprintf(buf,
                    "Tech:  %-5.1f (%-5.1f)    Speed:    %4d",
                    dirship->complexity,
                    race->tech,
                    dirship->max_speed);

            notify(playernum, governor, buf);

            if (Shipdata[dirship->build_type][ABIL_CEW] && dirship->cew) {
                sprintf(buf,
                        "            Opt Rng/Eng %4d/%4d\n",
                        dirship->cew_range,
                        dirship->cew);

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "\n");
            }

            if (race->tech < dirship->complexity) {
                notify(playernum,
                       governor,
                       "Your engineering capability is not advanced enough to produce this design.\n");
            }

            free(dirship);

            return;
        }

        shipc = args[1][0];

        i = 0;

        while ((Shipltrs[i] != shipc) && (i < NUMSTYPES)) {
            ++i;
        }

        if ((i >= NUMSTYPES)
            || ((i == STYPE_POD) && (!race->pods || !NORMAL_PODS))
            || ((i == STYPE_SUPERPOD) && (!race->pods || !SUPER_PODS))) {
            sprintf(buf, "Illegal ship letter.\n");
            notify(playernum, governor, buf);
            free(dirship);

            return;
        }

        if (!(Shipdata[i][ABIL_BUILD] & Shipdata[OTYPE_FACTORY][ABIL_CONSTRUCT])) {
            notify(playernum,
                   governor,
                   "This kind of ship does not require a factory to construct.\n");

            free(dirship);

            return;
        }

        dirship->build_type = i;
        dirship->armor = Shipdata[i][ABIL_ARMOR];
        dirship->guns = NONE; /* This keeps track of the factory status! */
        dirship->primary = Shipdata[i][ABIL_GUNS];
        dirship->primtype = Shipdata[i][ABIL_PRIMARY];
        dirship->secondary = Shipdata[i][ABIL_GUNS];
        dirship->sectype = Shipdata[i][ABIL_SECONDARY];
        dirship->max_crew = Shipdata[i][ABIL_MAXCREW];
        dirship->max_resource = Shipdata[i][ABIL_CARGO];
        dirship->max_hanger = Shipdata[i][ABIL_HANGER];
        dirship->max_fuel = Shipdata[i][ABIL_FUELCAP];
        dirship->max_destruct = Shipdata[i][ABIL_DESTCAP];
        dirship->max_speed = Shipdata[i][ABIL_SPEED];
        dirship->mount = Shipdata[i][ABIL_MOUNT] * Crystal(race);
        dirship->hyper_drive.has = Shipdata[i][ABIL_JUMP] * Hyper_drive(race);
        dirship->cloak = Shipdata[i][ABIL_CLOAK] * Cloak(race);
        dirship->laser = Shipdata[i][ABIL_LASER] * Laser(race);
        dirship->cew = 0;
        dirship->mode = 0;
        dirship->size = ship_size(dirship);
        dirship->complexity = complexity(dirship);

        sprintf(dirship->class, "mod %d", Dir[playernum - 1][governor].shipno);

        sprintf(buf, "Factory designated to produce %ss.\n", Shipnames[i]);
        notify(playernum, governor, buf);

        sprintf(buf,
                "Design complexity %.1f (%.1f).\n",
                dirship->complexity,
                race->tech);

        notify(playernum, governor, buf);

        if (dirship->complexity > race->tech) {
            notify(playernum, governor, "You can't produce this design yet!\n");
        }
    } else if (mode == 1) {
        if (!dirship->build_type || (dirship->build_type == OTYPE_FACTORY)) {
            notify(playernum,
                   governor,
                   "No ship design specified. Use 'make <ship type>' first.\n");

            free(dirship);

            return;
        }

        if (argn < 2) {
            notify(playernum,
                   governor,
                   "You have to specify the characteristic you wish to modify.\n");

            free(dirship);

            return;
        }

        if (argn == 3) {
            sscanf(args[2], "%d", &value);
        } else {
            value = 0;
        }

        if (value < 0) {
            notify(playernum, governor, "That's a ridiculous setting.\n");
            free(dirship);

            return;
        }

        if (Shipdata[dirship->build_type][ABIL_MOD]) {
            if (matchic(args[1], "armor")) {
                dirship->armor = MIN(value, 100);
            } else if (matchic(args[1], "crew")
                       && Shipdata[dirship->build_type][ABIL_MAXCREW]) {
                dirship->max_crew = MIN(value, 10000);
            } else if (matchic(args[1], "cargo")
                       && Shipdata[dirship->build_type][ABIL_CARGO]) {
                dirship->max_resource = MIN(value, 10000);
            } else if (matchic(args[1], "hanger")
                       && Shipdata[dirship->build_type][ABIL_HANGER]) {
                dirship->max_hanger = MIN(value, 10000);
            } else if (matchic(args[1], "fuel")
                       && Shipdata[dirship->build_type][ABIL_FUELCAP]) {
                dirship->max_fuel = MIN(value, 10000);
            } else if (matchic(args[1], "destruct")
                       && Shipdata[dirship->build_type][ABIL_DESTCAP]) {
                dirship->max_destruct = MIN(value, 10000);
            } else if (matchic(args[1], "speed")
                       && Shipdata[dirship->build_type][ABIL_SPEED]) {
                dirship->max_speed = MAX(1, MIN(value, 9));
            } else if (matchic(args[1], "mount")
                       && Shipdata[dirship->build_type][ABIL_MOUNT]
                       && Crystal(race)) {
                dirship->mount = !dirship->mount;
            } else if (matchic(args[1], "hyperdrive")
                       && Shipdata[dirship->build_type][ABIL_JUMP]
                       && Hyper_drive(race)) {
                dirship->hyper_drive.has = !dirship->hyper_drive.has;
            } else if (matchic(args[1], "primary")
                       && Shipdata[dirship->build_type][ABIL_PRIMARY]) {
                if (matchic(args[2], "strength")) {
                    dirship->primary = atoi(args[3]);
                } else if (matchic(args[2], "caliber")) {
                    if (matchic(args[3], "light")) {
                        dirship->primtype = LIGHT;
                    } else if (matchic(args[3], "medium")) {
                        dirship->primtype = MEDIUM;
                    } else if (matchic(args[3], "heavy")) {
                        dirship->primtype = HEAVY;
                    } else {
                        notify(playernum, governor, "No such caliber.\n");
                        free(dirship);

                        return;
                    }

                    dirship->primtype = MIN(Shipdata[dirship->build_type][ABIL_PRIMARY],
                                            dirship->primtype);
                } else {
                    notify(playernum,
                           governor,
                           "No such gun characteristic.n\n");

                    free(dirship);

                    return;
                }
            } else if (matchic(args[1], "secondary")
                       && Shipdata[dirship->build_type][ABIL_SECONDARY]) {
                if (matchic(args[2], "strength")) {
                    dirship->secondary = atoi(args[3]);
                } else if (matchic(args[2], "caliber")) {
                    if (matchic(args[3], "light")) {
                        dirship->sectype = LIGHT;
                    } else if (matchic(args[3], "medium")) {
                        dirship->sectype = MEDIUM;
                    } else if (matchic(args[3], "heavy")) {
                        dirship->sectype = HEAVY;
                    } else {
                        notify(playernum, governor, "No such caliber.\n");
                        free(dirship);

                        return;
                    }

                    dirship->sectype = MIN(Shipdata[dirship->build_type][ABIL_SECONDARY],
                                           dirship->sectype);
                } else {
                    notify(playernum,
                           governor,
                           "No such gun characteristic.\n");

                    free(dirship);

                    return;
                }
            } else if (matchic(args[1], "cew")
                       && Shipdata[dirship->build_type][ABIL_CEW]) {
                if (!Cew(race)) {
                    sprintf(buf,
                            "Your race does not understand confined energy weapons.\n");

                    notify(playernum, governor, buf);
                    free(dirship);

                    return;
                }

                if (!Shipdata[dirship->build_type][ABIL_CEW]) {
                    notify(playernum,
                           governor,
                           "This kind of ship cannot mount confined energy weapons.\n");

                    free(dirship);

                    return;
                }

                value = atoi(args[3]);

                if (matchic(args[2], "strength")) {
                    dirship->cew = value;
                } else if (matchic(args[2], "range")) {
                    dirship->cew_range = value;
                } else {
                    notify(playernum, governor, "No such option for CEWs.\n");
                    free(dirship);

                    return;
                }
            } else if (matchic(args[1], "laser")
                       && Shipdata[dirship->build_type][ABIL_LASER]) {
                if (!Laser(race)) {
                    sprintf(buf, "Your race does not understand lasers yet.\n");
                    notify(playernum, governor, buf);
                    free(dirship);

                    return;
                }

                if (Shipdata[dirship->build_type][ABIL_LASER]) {
                    dirship->laser = !dirship->laser;
                } else {
                    notify(playernum,
                           governor,
                           "That ship cannot be fitted with combat lasers.\n");

                    free(dirship);

                    return;
                }
            } else if (matchic(args[1], "cloak")
                       && Shipdata[dirship->build_type][ABIL_CLOAK]) {
                if (!Cloak(race)) {
                    sprintf(buf,
                            "Your race does not understand cloak technology yet.\n");

                    notify(playernum, governor, buf);
                    free(dirship);

                    return;
                }

                if (Shipdata[dirship->build_type][ABIL_CLOAK]) {
                    dirship->cloak = !dirship->cloak;
                } else {
                    notify(playernum,
                           governor,
                           "That ship cannot be fitted with a cloaking device.\n");

                    free(dirship);

                    return;
                }
            } else {
                notify(playernum,
                       governor,
                       "That characteristic either doesn't exist or can't be modified.\n");

                free(dirship);

                return;
            }
        } else if (Hyper_drive(race)) {
            if (matchic(args[1], "hyperdrive")) {
                dirship->hyper_drive.has = !dirship->hyper_drive.has;
            } else {
                notify(playernum,
                       governor,
                       "You may only modify hyperdrive installation on this kind of ship.\n");

                free(dirship);

                return;
            }
        } else {
            notify(playernum,
                   governor,
                   "Sorry, but you can't modify this ship right now.\n");

            free(dirship);

            return;
        }
    } else {
        notify(playernum, governor, "Weird error.\n");
        free(dirship);

        return;
    }

    /* Compute how much it's going to cost to build the ship */
    cost0 = cost(dirship);

    if (cost0 > 65535.0) {
        notify(playernum,
               governor,
               "Woah!! YOU CHEATER!!! The max cost allowed is 65536!!! I'm telling!!!\n");

        free(dirship);

        return;
    }

    if (race->God) {
        dirship->build_cost = 0;
    } else {
        dirship->build_cost = (int)cost0;
    }

    sprintf(buf,
            "The current cost of the ship is %d resources.\n",
            dirship->build_cost);

    notify(playernum, governor, buf);
    dirship->size = ship_size(dirship);
    dirship->base_mass = getmass(dirship);

    sprintf(buf,
            "The current base mass of the ship is %.1f - size is %d.\n",
            dirship->base_mass,
            dirship->size);

    notify(playernum, governor, buf);
    dirship->complexity = complexity(dirship);

    sprintf(buf,
            "Ship complexity is %.1f (you have %.1f engineering technology).\n",
            dirship->complexity,
            race->tech);

    notify(playernum, governor, buf);

    /* Restore size to what it was before. Maarten */
    dirship->size = size;

    putship(dirship);
    free(dirship);
}

void build(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    racetype *race;
    char c;
    char head[320];
    int i;
    int j;
    int k = 1;
    int m;
    int n;
    int x;
    int y;
    int count;
    int level;
    int what = -1;
    int outside = -1;
    int shipcost;
    int load_crew;
    int snum;
    int pnum;
    int build_level = -1;
    int high = 99999;
    int low = -1;
    double load_fuel;
    double tech;
    FILE *fd;
    planettype *planet = NULL;
    sectortype *sector = NULL;
    shiptype *builder = NULL;
    shiptype newship;
    char *cptr;

    if ((argn > 1) && (args[1][0] == '?')) {
        /* Information request. Account for probe char */
        if ((argn == 2)
            || isdigit(args[2][0])
            || ((args[2][0] == ':') && isdigit(args[2][1]))) {
            if (argn >= 3) {
                low = atoi(args[2]);
                cptr = strchr(args[2], ':');

                if (cptr) {
                    ++cptr;
                    high = atoi(cptr);
                }

                if (!high) {
                    high = 99999;
                }

                if (low > high) {
                    /* Retard user, set it back to default */
                    low = -1;
                    high = 99999;
                }
            }

            /* Ship parameter list */
            notify(playernum, governor, "     - Default ship parameters -\n");
            sprintf(head,
                    "%1s %-15s %5s %5s %3s %4s %3s %3s %3s %4s %4s %2s %3s %4s %4s\n- --------------- ----- ----- --- ---- --- --- --- ---- ---- -- --- ---- ----\n",
                    "?",
                    "name",
                    "cargo",
                    "hang",
                    "arm",
                    "dest",
                    "gun",
                    "pri",
                    "sec",
                    "fuel",
                    "crew",
                    "sp",
                    "clk",
                    "tech",
                    "cost");

            race = races[playernum - 1];
            notify(playernum, governor, head);

            for (j = 1; j < NUMSTYPES; ++j) {
                if ((k % 21) == 0) {
                    notify(playernum, governor, head);
                }

                i = ShipVector[j];

                if (race->God
                    || race->pods
                    || ((i == STYPE_POD) || (i == STYPE_SUPERPOD))) {
                    if (Shipdata[i][ABIL_PROGRAMMED]
                        && ((i != STYPE_POD) || NORMAL_PODS)
                        && ((i != STYPE_SUPERPOD) || SUPER_PODS)
                        && ((Shipdata[i][ABIL_TECH] < 9999) || race->God)
                        && ((Shipdata[i][ABIL_TECH] >= low)
                            && (Shipdata[i][ABIL_TECH] <= high))) {
                        sprintf(buf,
                                "%1c %-15.15s %5ld %5ld %3ld %4ld %3ld %3ld %3ld %4ld %4ld %2ld %3s %4.0f %4d\n",
                                Shipltrs[i],
                                Shipnames[i],
                                Shipdata[i][ABIL_CARGO],
                                Shipdata[i][ABIL_HANGER],
                                Shipdata[i][ABIL_ARMOR],
                                Shipdata[i][ABIL_DESTCAP],
                                Shipdata[i][ABIL_GUNS],
                                Shipdata[i][ABIL_PRIMARY],
                                Shipdata[i][ABIL_SECONDARY],
                                Shipdata[i][ABIL_FUELCAP],
                                Shipdata[i][ABIL_MAXCREW],
                                Shipdata[i][ABIL_SPEED],
                                (Shipdata[i][ABIL_CLOAK] ? "yes" : "no"),
                                (double)Shipdata[i][ABIL_TECH],
                                Shipcost(i, race));

                        notify(playernum, governor, buf);
                        ++k;
                    }
                }
            }

            return;
        } else {
            /* Description of specific ship type */
            i = 0;

            while ((Shipltrs[i] != args[2][0]) && (i < NUMSTYPES)) {
                ++i;
            }

            if ((i < 0) || (i >= NUMSTYPES)) {
                notify(playernum, governor, "No such ship type.\n");
            } else if (!Shipdata[i][ABIL_PROGRAMMED]) {
                notify(playernum,
                       governor,
                       "This ship type has not been programmed.\n");
            } else {
                fd = fopen(EXAM_FL, "r");

                if (fd == NULL) {
                    perror(EXAM_FL);

                    return;
                } else {
                    /* Look through ship description file */
                    sprintf(buf, "\n");

                    for (j = 0; j <= i; ++j) {
                        c = fgetc(fd);

                        while (c != '~') {
                            c = fgetc(fd);
                        }
                    }

                    c = fgetc(fd);

                    while (c != '~') {
                        sprintf(temp, "%c", c);
                        strcat(buf, temp);
                    }

                    fclose(fd);

                    /* Built where? */
                    if ((Shipdata[i][ABIL_BUILD] % 2) == 1) {
                        sprintf(temp, "\nCan be constructed on planet.");
                        strcat(buf, temp);
                    }

                    n = 0;
                    sprintf(temp, "\nCan be built by ");

                    for (j = 0; j <= NUMSTYPES; ++j) {
                        if (Shipdata[i][ABIL_BUILD] & Shipdata[j][ABIL_CONSTRUCT]) {
                            ++n;
                        }
                    }

                    if (n) {
                        m = 0;
                        strcat(buf, temp);

                        for (j = 0; j < NUMSTYPES; ++j) {
                            if (Shipdata[i][ABIL_BUILD] & Shipdata[j][ABIL_CONSTRUCT]) {
                                ++m;

                                if ((n - m) > 1) {
                                    sprintf(temp, "%c, ", Shipltrs[j]);
                                } else if ((n - m) > 0) {
                                    sprintf(temp, "%c and ", Shipltrs[j]);
                                } else {
                                    sprintf(temp, "%c ", Shipltrs[j]);
                                }

                                strcat(buf, temp);
                            }
                        }

                        sprintf(temp, "type ships.\n");
                        strcat(buf, temp);
                    }

                    /* Default parameters */
                    sprintf(temp,
                            "\n%1s %-15s %5s %5s %3s %4s %3s %3s %3s %4s %4s %2s %3s %4s %4s\n- --------------- ----- ----- --- ---- --- --- --- ---- ---- -- --- ---- ----\n",
                            "?",
                            "name",
                            "cargo",
                            "hang",
                            "arm",
                            "dest",
                            "gun",
                            "pri",
                            "sec",
                            "fuel",
                            "crew",
                            "sp",
                            "clk",
                            "tech",
                            "cost");

                    strcat(buf, temp);
                    race = races[playernum - 1];

                    sprintf(temp,
                            "%1c %-15.15s %5ld %5ld %3ld %4ld %3ld %3ld %3ld %4ld %4ld %2ld %3s %4.0f %4d\n",
                            Shipltrs[i],
                            Shipnames[i],
                            Shipdata[i][ABIL_CARGO],
                            Shipdata[i][ABIL_HANGER],
                            Shipdata[i][ABIL_ARMOR],
                            Shipdata[i][ABIL_DESTCAP],
                            Shipdata[i][ABIL_GUNS],
                            Shipdata[i][ABIL_PRIMARY],
                            Shipdata[i][ABIL_SECONDARY],
                            Shipdata[i][ABIL_FUELCAP],
                            Shipdata[i][ABIL_MAXCREW],
                            Shipdata[i][ABIL_SPEED],
                            (Shipdata[i][ABIL_CLOAK] ? "yes" : "no"),
                            (double)Shipdata[i][ABIL_TECH],
                            Shipcost(i, race));

                    strcat(buf, temp);
                    notify(playernum, governor, buf);
                }
            }
        }

        return;
    }

    /* Here begins the actual build code */

    /*
     * HUTm (kse) check if ship slots are all used up. NOTE: This is just a hack
     * to prevent server crashes until reliable ship number reuse ode is
     * implemented
     */
    if (!shipSlotsAvail()) {
        notify(playernum, governor, "All ship slots are already used.\n");

        return;
    }

    /* Temp kludge until these ships are removed */
    if ((args[1][0] == 'Y') || (args[1][0] == 'a') || (args[1][0] == 'K')) {
        notify(playernum,
               governor,
               "This ship superceeded by the INF machines (A)\n");

        return;
    }

    level = Dir[playernum - 1][governor].level;

    if ((level != LEVEL_SHIP) && (level != LEVEL_PLAN)) {
        notify(playernum,
               governor,
               "You must change scope to a ship or a planet to build.\n");

        return;
    }

    snum = Dir[playernum - 1][governor].snum;
    pnum = Dir[playernum - 1][governor].pnum;
    race = races[playernum - 1];
    count = 0; /* This is used to reset count in the loop */

    switch (level) {
    case LEVEL_PLAN:
        if (!count) {
            /* Initialize loop variables */
            if (argn < 2) {
                notify(playernum, governor, "Build what?\n");

                return;
            }

            what = get_build_type(args[1]);

            if (what < 0) {
                notify(playernum, governor, "No such ship type.\n");

                return;
            }

            if (!can_build_this(what, race, buf) && !race->God) {
                notify(playernum, governor, buf);

                return;
            }

            if (((Shipdata[what][ABIL_BUILD] % 2) == 0) && !race->God) {
                notify(playernum,
                       governor,
                       "This ship cannot be built by a planet.\n");

                return;
            }

            if (argn < 3) {
                notify(playernum, governor, "Build where?\n");

                return;
            }

            getplanet(&planet, snum, pnum);

            if (!can_build_at_planet(playernum, governor, Stars[snum], planet)
                && !race->God) {
                notify(playernum, governor, "You can't build that here.\n");
                free(planet);

                return;
            }

            sscanf(args[2], "%d,%d", &x, &y);

            if ((x < 0)
                || (x >= planet->Maxx)
                || (y < 0)
                || (y >= planet->Maxy)) {
                notify(playernum, governor, "Illegal sector.\n");
                free(planet);

                return;
            }

            if (!getsector(&sector, planet, x, y)) {
                notify(playernum,
                       governor,
                       "Error in sector database, notify deity.\n");

                return;
            }

            if (!can_build_on_sector(what, race, planet, sector, x, y, buf)
                && !race->God) {
                notify(playernum, governor, buf);
                free(planet);
                free(sector);

                return;
            }

            if (argn < 4) {
                count = getcount(1, args[3]);
            } else {
                count = getcount(0, args[3]);
            }

            if (!count) {
                notify(playernum,
                       governor,
                       "Give a positive number of builds.\n");

                free(planet);
                free(sector);

                return;
            }

            Getship(&newship, what, race);
        } /* if (!count) */

        /*
         * HUTm (kse) Check if ship slots are all used up. NOTE: This is just a
         * hack to prevent server crashes until reliable ship number reuse code
         * is implemented
         */
        if (!shipSlotsAvail()) {
            notify(playernum, governor, "All ships slots are already used.\n");

            putsector(sector, planet, x, y);
            putplanet(planet, snum, pnum);
            free(sector);
            free(planet);

            return;
        }

        shipcost = newship.build_cost;

        if (shipcost > planet->info[playernum - 1].resource) {
            sprintf(buf, "You need %dr to construct this ship.\n", shipcost);
            notify(playernum, governor, buf);

            putsector(sector, planet, x, y);
            putplanet(planet, snum, pnum);
            free(sector);
            free(planet);

            return;
        }

        if (!create_ship_by_planet(playernum, governor, race, &newship, planet, snum, pnum, x, y)) {
            return;
        }

        if (race->governor[governor].toggle.autoload
            && (what != OTYPE_TRANSDEV)
            && !race->God) {
            autoload_at_planet(playernum,
                               &newship,
                               planet,
                               sector,
                               &load_crew,
                               &load_fuel);
        } else {
            load_crew = 0;
            load_fuel = 0.0;
        }

        initialize_new_ship(playernum,
                            governor,
                            race,
                            &newship,
                            load_fuel,
                            load_crew,
                            0);

        putship(&newship);

        break;
    case LEVEL_SHIP:
        if (!count) {
            /* Initialize loop variables */
            getship(&builder, Dir[playernum - 1][governor].shipno);
            outside = 0;
            build_level = build_at_ship(playernum,
                                        governor,
                                        race,
                                        builder,
                                        &snum,
                                        &pnum);

            if (build_level < 0) {
                notify(playernum, governor, "You can't build here.\n");
                free(builder);

                return;
            }

            if (builder->type == OTYPE_FACTORY) {
                if (argn < 2) {
                    count = getcount(1, args[1]);
                } else {
                    count = getcount(0, args[1]);
                }

                if (!count) {
                    notify(playernum,
                           governor,
                           "Give a positive number of builds.\n");

                    free(builder);

                    return;
                }

                if (!landed(builder) && !inship(builder)) {
                    notify(playernum,
                           governor,
                           "Factories can only build when landed on a planet.\n");

                    free(builder);

                    return;
                }

                Getfactship(&newship, builder);

                if (landed(builder)) {
                    outside = 1;
                }
            } else {
                if (Shipdata[builder->type][ABIL_CONSTRUCT] & 4) {
                    if (landed(builder)) {
                        notify(playernum,
                               governor,
                               "This ship cannot build when landed.\n");

                        free(builder);

                        return;
                    }
                }

                if (argn < 2) {
                    notify(playernum, governor, "Build what?\n");
                    free(builder);

                    return;
                }

                what = get_build_type(args[1]);

                if (what < 0) {
                    notify(playernum, governor, "No such ship type.\n");
                    free(builder);

                    return;
                }

                if (!can_build_on_ship(what, race, builder, buf)) {
                    notify(playernum, governor, buf);
                    free(builder);

                    return;
                }

                if (Shipdata[what][ABIL_BUILD] & 4) {
                    outside = 1;
                }

                if (argn < 3) {
                    count = getcount(1, args[2]);
                } else {
                    count = getcount(0, args[2]);
                }

                if (!count) {
                    notify(playernum,
                           governor,
                           "Give a positive number of builds.\n");

                    free(builder);

                    return;
                }

                Getship(&newship, what, race);
            }

            if (builder->type == OTYPE_FACTORY) {
                tech = complexity(builder);
            } else {
                tech = Shipdata[what][ABIL_TECH];
            }

            if ((tech > race->tech) && !race->God) {
                sprintf(buf,
                        "You are not advanced enough to build this ship.\n%.1f engineering technology. You have %.1f.\n",
                        tech,
                        race->tech);

                notify(playernum, governor, buf);
                free(builder);

                return;
            }

            if (outside && (build_level == LEVEL_PLAN)) {
                getplanet(&planet, snum, pnum);

                if (builder->type == OTYPE_FACTORY) {
                    if (!can_build_at_planet(playernum, governor, Stars[snum], planet)) {
                        notify(playernum, governor, "You can't build that here.\n");
                        free(planet);
                        free(builder);

                        return;
                    }

                    x = builder->land_x;
                    y = builder->land_y;
                    what = builder->build_type;

                    if (!getsector(&sector, planet, x, y)) {
                        notify(playernum,
                               governor,
                               "Error in sector database, notify deity.\n");

                        return;
                    }

                    if (!can_build_on_sector(what, race, planet, sector, x, y, buf)) {
                        notify(playernum, governor, buf);
                        free(planet);
                        free(sector);
                        free(builder);

                        return;
                    }
                }
            }
        }

        /* Build them */
        switch (builder->type) {
        case OTYPE_FACTORY:
            if (!inship(builder)) {
                /*
                 * HUTm (kse) Check if ship slots are all used up. NOTE: This is
                 * just a hack to prevent server crashes until reliable ship
                 * number reuse code is implemented
                 */
                if (!shipSlotsAvail()) {
                    notify(playernum,
                           governor,
                           "All ships slots are already used.\n");

                    if (outside) {
                        switch (build_level) {
                        case LEVEL_PLAN:
                            putplanet(planet, snum, pnum);

                            if (landed(builder)) {
                                putsector(sector, planet, x, y);
                                free(sector);
                            }

                            free(planet);

                            break;
                        case LEVEL_STAR:
                            putstar(Stars[snum], snum);

                            break;
                        case LEVEL_UNIV:
                            putsdata(&Sdata);

                            break;
                        }
                    }

                    putship(builder);
                    free(builder);

                    return;
                }

                shipcost = newship.build_cost;

                if (shipcost > planet->info[playernum - 1].resource) {
                    sprintf(buf,
                            "You need %dr to construct this ship.\n",
                            shipcost);

                    notify(playernum, governor, buf);

                    if (outside) {
                        switch (build_level) {
                        case LEVEL_PLAN:
                            putplanet(planet, snum, pnum);

                            if (landed(builder)) {
                                putsector(sector, planet, x, y);
                                free(sector);
                            }

                            free(planet);

                            break;
                        case LEVEL_STAR:
                            putstar(Stars[snum], snum);

                            break;
                        case LEVEL_UNIV:
                            putsdata(&Sdata);

                            break;
                        }
                    }

                    putship(builder);
                    free(builder);

                    return;
                }

                if (!create_ship_by_planet(playernum, governor, race, &newship, planet, snum, pnum, x, y)) {
                    return;
                }

                if (race->governor[governor].toggle.autoload
                    && (what != OTYPE_TRANSDEV)
                    && !race->God) {
                    autoload_at_planet(playernum,
                                       &newship,
                                       planet,
                                       sector,
                                       &load_crew,
                                       &load_fuel);
                } else {
                    load_crew = 0;
                    load_fuel = 0.0;
                }

                break;
            } else {
                shiptype *t = builder;
                getship(&builder, t->destshipno);
                free(t);
            }

            /* Don't break here, we want to fall-through -mfw */
            /* FALL THROUGH */
        default:
            if (outside) {
                if (docked(builder)) {
                    notify(playernum, governor, "Undock this ship first!\n");

                    switch (build_level) {
                    case LEVEL_PLAN:
                        putplanet(planet, snum, pnum);

                        if (landed(builder)) {
                            putsector(sector, planet, x, y);
                            free(sector);
                        }

                        free(planet);

                        break;
                    case LEVEL_STAR:
                        putstar(Stars[snum], snum);

                        break;
                    case LEVEL_UNIV:
                        putsdata(&Sdata);

                        break;
                    }

                    putship(builder);
                    free(builder);

                    return;
                }
            } else {
                if ((builder->hanger + ship_size(&newship)) > builder->max_hanger) {
                    notify(playernum, governor, "Not enough hanger space.\n");
                    putship(builder);
                    free(builder);

                    return;
                }
            }

            /*
             * HUTm (kse) Check if ship slots are all used up. NOTE: This is
             * just a hack to prevent server crashes until reliable ship number
             * reuse code is implemented.
             */
            if (!shipSlotsAvail()) {
                notify(playernum,
                       governor,
                       "All ship slots are already used.\n");

                if (outside) {
                    switch (build_level) {
                    case LEVEL_PLAN:
                        putplanet(planet, snum, pnum);

                        if (landed(builder)) {
                            putsector(sector, planet, x, y);
                            free(sector);
                        }

                        free(planet);

                        break;
                    case LEVEL_STAR:
                        putstar(Stars[snum], snum);

                        break;
                    case LEVEL_UNIV:
                        putsdata(&Sdata);

                        break;
                    }
                }

                putship(builder);
                free(builder);

                return;
            }

            shipcost = newship.build_cost;

            if (builder->resource < shipcost) {
                sprintf(buf, "You need %dr to construct the ship.\n", shipcost);
                notify(playernum, governor, buf);

                if (outside) {
                    switch (build_level) {
                    case LEVEL_PLAN:
                        putplanet(planet, snum, pnum);

                        if (landed(builder)) {
                            putsector(sector, planet, x, y);
                            free(sector);
                        }

                        free(planet);

                        break;
                    case LEVEL_STAR:
                        putstar(Stars[snum], snum);

                        break;
                    case LEVEL_UNIV:
                        putsdata(&Sdata);

                        break;
                    }
                }

                putship(builder);
                free(builder);

                return;
            }

            if (!create_ship_by_ship(playernum, governor, race, outside, Stars[builder->storbits], planet, &newship, builder)) {
                return;
            }

            if (race->governor[governor].toggle.autoload
                && (what != OTYPE_TRANSDEV)
                && !race->God) {
                autoload_at_ship(playernum,
                                 &newship,
                                 builder,
                                 &load_crew,
                                 &load_fuel);
            } else {
                load_crew = 0;
                load_fuel = 0.0;
            }
        }

        initialize_new_ship(playernum,
                            governor,
                            race,
                            &newship,
                            load_fuel,
                            load_crew,
                            0);

        putship(&newship);

        break;
    } /* level */

    --count;

    while (count) {
        switch (level) {
        case LEVEL_PLAN:
            if (!count) {
                /* Initialize loop variables */
                if (argn < 2) {
                    notify(playernum, governor, "Build what?\n");

                    return;
                }

                what = get_build_type(args[1]);

                if (what < 0) {
                    notify(playernum, governor, "No such ship type.\n");

                    return;
                }

                if (!can_build_this(what, race, buf) && !race->God) {
                    notify(playernum, governor, buf);

                    return;
                }

                if (((Shipdata[what][ABIL_BUILD] % 2) == 0) && !race->God) {
                    notify(playernum,
                           governor,
                           "This ship cannot be built by a planet.\n");

                    return;
                }

                if (argn < 3) {
                    notify(playernum, governor, "Build where?\n");

                    return;
                }

                getplanet(&planet, snum, pnum);

                if (!can_build_at_planet(playernum, governor, Stars[snum], planet)
                    && !race->God) {
                    notify(playernum, governor, "You can't build that here.\n");
                    free(planet);

                    return;
                }

                sscanf(args[2], "%d,%d", &x, &y);

                if ((x < 0)
                    || (x >= planet->Maxx)
                    || (y < 0)
                    || (y >= planet->Maxy)) {
                    notify(playernum, governor, "Illegal sector.\n");
                    free(planet);

                    return;
                }

                if (!getsector(&sector, planet, x, y)) {
                    notify(playernum,
                           governor,
                           "Error in sector database, notify deity.\n");

                    return;
                }

                if (argn < 4) {
                    count = getcount(1, args[3]);
                } else {
                    count = getcount(0, args[3]);
                }

                if (!count) {
                    notify(playernum,
                           governor,
                           "Give a positive number of builds.\n");

                    free(planet);
                    free(sector);

                    return;
                }

                Getship(&newship, what, race);
            } /* if (!count) */

            /*
             * HUTm (kse) Check if ship slots are all used up. NOTE: This is
             * just a hack to prevent server crashes until reliable ship number
             * reuse code is implemented.
             */
            if (!shipSlotsAvail()) {
                notify(playernum,
                       governor,
                       "All ships slots are already used.\n");

                putsector(sector, planet, x, y);
                putplanet(planet, snum, pnum);
                free(sector);
                free(planet);

                return;
            }

            shipcost = newship.build_cost;

            if (shipcost > planet->info[playernum - 1].resource) {
                sprintf(buf,
                        "You need %dr to construct this ship.\n",
                        shipcost);

                notify(playernum, governor, buf);
                putsector(sector, planet, x, y);
                putplanet(planet, snum, pnum);
                free(sector);
                free(planet);

                return;
            }

            if (!create_ship_by_planet(playernum, governor, race, &newship, planet, snum, pnum, x, y)) {
                return;
            }

            if (race->governor[governor].toggle.autoload
                && (what != OTYPE_TRANSDEV)
                && !race->God) {
                autoload_at_planet(playernum,
                                   &newship,
                                   planet,
                                   sector,
                                   &load_crew,
                                   &load_fuel);
            } else {
                load_crew = 0;
                load_fuel = 0.0;
            }

            initialize_new_ship(playernum,
                                governor,
                                race,
                                &newship,
                                load_fuel,
                                load_crew,
                                0);

            putship(&newship);

            break;
        case LEVEL_SHIP:
            if (!count) {
                /* Initialize loop variables */
                getship(&builder, Dir[playernum - 1][governor].shipno);
                outside = 0;
                build_level = build_at_ship(playernum,
                                            governor,
                                            race,
                                            builder,
                                            &snum,
                                            &pnum);

                if (build_level < 0) {
                    notify(playernum, governor, "You can't build here.\n");
                    free(builder);

                    return;
                }

                if (builder->type == OTYPE_FACTORY) {
                    if (argn < 2) {
                        count = getcount(1, args[1]);
                    } else {
                        count = getcount(0, args[1]);
                    }

                    if (!count) {
                        notify(playernum,
                               governor,
                               "Give a positive number of builds.\n");

                        free(builder);

                        return;
                    }

                    if (!landed(builder) && !inship(builder)) {
                        notify(playernum,
                               governor,
                               "Factories can only build when landed on a planet.\n");

                        free(builder);

                        return;
                    }

                    Getfactship(&newship, builder);

                    if (landed(builder)) {
                        outside = 1;
                    }
                } else {
                    if (Shipdata[builder->type][ABIL_CONSTRUCT] & 4) {
                        if (landed(builder)) {
                            notify(playernum,
                                   governor,
                                   "This ship cannot build when landed.\n");

                            free(builder);

                            return;
                        }
                    }

                    if (argn < 2) {
                        notify(playernum, governor, "Build what?\n");
                        free(builder);

                        return;
                    }

                    what = get_build_type(args[1]);

                    if (what < 0) {
                        notify(playernum, governor, "No such ship type.\n");
                        free(builder);

                        return;
                    }

                    if (!can_build_on_ship(what, race, builder, buf)) {
                        notify(playernum, governor, buf);
                        free(builder);

                        return;
                    }

                    if (Shipdata[what][ABIL_BUILD] & 4) {
                        outside = 1;
                    }

                    if (argn < 3) {
                        count = getcount(1, args[2]);
                    } else {
                        count = getcount(0, args[2]);
                    }

                    if (!count) {
                        notify(playernum,
                               governor,
                               "Give a positive number of builds.\n");

                        free(builder);

                        return;
                    }

                    Getship(&newship, what, race);
                }

                if (builder->type == OTYPE_FACTORY) {
                    tech = complexity(builder);
                } else {
                    tech = Shipdata[what][ABIL_TECH];
                }

                if ((tech > race->tech) && !race->God) {
                    sprintf(buf,
                            "You are not advanced enough to build this ship.\n%.1f engineering technology. You have %.1f.\n",
                            tech,
                            race->tech);

                    notify(playernum, governor, buf);
                    free(builder);

                    return;
                }

                if (outside && (build_level == LEVEL_PLAN)) {
                    getplanet(&planet, snum, pnum);

                    if (builder->type == OTYPE_FACTORY) {
                        if (!can_build_at_planet(playernum, governor, Stars[snum], planet)) {
                            notify(playernum,
                                   governor,
                                   "You can't build that here.\n");

                            free(planet);
                            free(builder);

                            return;
                        }

                        x = builder->land_x;
                        y = builder->land_y;
                        what = builder->build_type;

                        if (!getsector(&sector, planet, x, y)) {
                            notify(playernum,
                                   governor,
                                   "Error in sector database, notify deity.\n");

                            return;
                        }

                        if (!can_build_on_sector(what, race, planet, sector, x, y, buf)) {
                            notify(playernum, governor, buf);
                            free(planet);
                            free(sector);
                            free(builder);

                            return;
                        }
                    }
                }
            }

            /* Build them */
            switch (builder->type) {
            case OTYPE_FACTORY:
                if (!inship(builder)) {
                    /*
                     * HUTm (kse) Check if ship slots are all used up. NOTE:
                     * This is just a hack to prevent server crashes until
                     * reliable ship number reuse code is implemented.
                     */
                    if (!shipSlotsAvail()) {
                        notify(playernum,
                               governor,
                               "All ships slots are already used.\n");

                        if (outside) {
                            switch (build_level) {
                            case LEVEL_PLAN:
                                putplanet(planet, snum, pnum);

                                if (landed(builder)) {
                                    putsector(sector, planet, x, y);
                                    free(sector);
                                }

                                free(planet);

                                break;
                            case LEVEL_STAR:
                                putstar(Stars[snum], snum);

                                break;
                            case LEVEL_UNIV:
                                putsdata(&Sdata);

                                break;
                            }
                        }

                        putship(builder);
                        free(builder);

                        return;
                    }

                    shipcost = newship.build_cost;

                    if (shipcost > planet->info[playernum - 1].resource) {
                        sprintf(buf,
                                "You need %dr to construct this ship.\n",
                                shipcost);

                        notify(playernum, governor, buf);

                        if (outside) {
                            switch (build_level) {
                            case LEVEL_PLAN:
                                putplanet(planet, snum, pnum);

                                if (landed(builder)) {
                                    putsector(sector, planet, x, y);
                                    free(sector);
                                }

                                free(planet);

                                break;
                            case LEVEL_STAR:
                                putstar(Stars[snum], snum);

                                break;
                            case LEVEL_UNIV:
                                putsdata(&Sdata);

                                break;
                            }
                        }

                        putship(builder);
                        free(builder);

                        return;
                    }

                    if (!create_ship_by_planet(playernum, governor, race, &newship, planet, snum, pnum, x, y)) {
                        return;
                    }

                    if (race->governor[governor].toggle.autoload
                        && (what != OTYPE_TRANSDEV)
                        && !race->God) {
                        autoload_at_planet(playernum,
                                           &newship,
                                           planet,
                                           sector,
                                           &load_crew,
                                           &load_fuel);
                    } else {
                        load_crew = 0;
                        load_fuel = 0.0;
                    }

                    break;
                } else {
                    shiptype *t = builder;
                    getship(&builder, t->destshipno);
                    free(t);
                }

                /* Don't break here, we want to fall-through -mfw */
                /* FALL THROUGH */
            default:
                if (outside) {
                    if (docked(builder)) {
                        notify(playernum, governor, "Undock this ship first!\n");

                        switch (build_level) {
                        case LEVEL_PLAN:
                            putplanet(planet, snum, pnum);

                            if (landed(builder)) {
                                putsector(sector, planet, x, y);
                                free(sector);
                            }

                            free(planet);

                            break;
                        case LEVEL_STAR:
                            putstar(Stars[snum], snum);

                            break;
                        case LEVEL_UNIV:
                            putsdata(&Sdata);

                            break;
                        }

                        putship(builder);
                        free(builder);

                        return;
                    }
                } else {
                    if ((builder->hanger + ship_size(&newship)) > builder->max_hanger) {
                        notify(playernum,
                               governor,
                               "Not enough hanger space.\n");

                        putship(builder);
                        free(builder);

                        return;
                    }
                }

                /*
                 * HUTm (kse) Check if ship slots are all used up. NOTE: This is
                 * just a hack to prevent server crashes until reliable ship
                 * number reuse code is implemented.
                 */
                if (!shipSlotsAvail()) {
                    notify(playernum,
                           governor,
                           "All ship slots are already used.\n");

                    if (outside) {
                        switch (build_level) {
                        case LEVEL_PLAN:
                            putplanet(planet, snum, pnum);

                            if (landed(builder)) {
                                putsector(sector, planet, x, y);
                                free(sector);
                            }

                            free(planet);

                            break;
                        case LEVEL_STAR:
                            putstar(Stars[snum], snum);

                            break;
                        case LEVEL_UNIV:
                            putsdata(&Sdata);

                            break;
                        }
                    }

                    putship(builder);
                    free(builder);

                    return;
                }

                shipcost = newship.build_cost;

                if (builder->resource < shipcost) {
                    sprintf(buf,
                            "You need %dr to construct the ship.\n",
                            shipcost);

                    notify(playernum, governor, buf);

                    if (outside) {
                        switch (build_level) {
                        case LEVEL_PLAN:
                            putplanet(planet, snum, pnum);

                            if (landed(builder)) {
                                putsector(sector, planet, x, y);
                                free(sector);
                            }

                            free(planet);

                            break;
                        case LEVEL_STAR:
                            putstar(Stars[snum], snum);

                            break;
                        case LEVEL_UNIV:
                            putsdata(&Sdata);

                            break;
                        }
                    }

                    putship(builder);
                    free(builder);

                    return;
                }

                if (!create_ship_by_ship(playernum, governor, race, outside, Stars[builder->storbits], planet, &newship, builder)) {
                    return;
                }

                if (race->governor[governor].toggle.autoload
                    && (what != OTYPE_TRANSDEV)
                    && !race->God) {
                    autoload_at_ship(playernum,
                                     &newship,
                                     builder,
                                     &load_crew,
                                     &load_fuel);
                } else {
                    load_crew = 0;
                    load_fuel = 0.0;
                }
            }

            initialize_new_ship(playernum,
                                governor,
                                race,
                                &newship,
                                load_fuel,
                                load_crew,
                                0);

            putship(&newship);

            break;
        }

        --count;
    }

    switch (level) {
    case LEVEL_PLAN:
        putsector(sector, planet, x, y);
        putplanet(planet, snum, pnum);
        free(sector);
        free(planet);

        break;
    case LEVEL_SHIP:
        if (outside) {
            switch (build_level) {
            case LEVEL_PLAN:
                putplanet(planet, snum, pnum);

                if (landed(builder)) {
                    putsector(sector, planet, x, y);
                    free(sector);
                }

                free(planet);

                break;
            case LEVEL_STAR:
                putstar(Stars[snum], snum);

                break;
            case LEVEL_UNIV:
                putsdata(&Sdata);

                break;
            }
        }

        putship(builder);
        free(builder);

        break;
    }
}

int getcount(int mode, char *string)
{
    int count;

    if (mode) {
        count = 1;
    } else {
        count = atoi(string);
    }

    if (count <= 0) {
        count = 0;
    }

    return count;
}

int can_build_at_planet(int playernum,
                        int governor,
                        startype *star,
                        planettype *planet)
{
    if (planet->slaved_to && (planet->slaved_to != playernum)) {
        sprintf(buf,
                "This planet is enslaved by player %d.\n",
                planet->slaved_to);

        notify(playernum, governor, buf);

        return 0;
    }

    return 1;
}

int get_build_type(char *string)
{
    char shipc;
    int i = 0;

    shipc = string[0];

    while ((i < NUMSTYPES) && (Shipltrs[i] != shipc)) {
        ++i;
    }

    if ((i < 0) || (i >= NUMSTYPES)) {
        return -1;
    }

    return i;
}

int can_build_this(int what, racetype *race, char *string)
{
    if (((what == STYPE_POD) || (what == STYPE_SUPERPOD)) && !race->pods) {
        sprintf(string, "Only Metamorphic races can build Spore Pods.\n");

        return 0;
    }

    if (!Shipdata[what][ABIL_PROGRAMMED]) {
        sprintf(string, "This ship type has not been programmed.\n");

        return 0;
    }

#ifdef USE_VN
    if ((what == OTYPE_VN) && !Vn(race)) {
        sprintf(string, "You have not discovered VN technology.\n");

        return 0;
    }
#endif

    if ((what == OTYPE_TRANSDEV) && !Avpm(race)) {
        sprintf(string, "You have not discovered AVPM technology.\n");

        return 0;
    }

    if ((Shipdata[what][ABIL_TECH] > race->tech) && !race->God) {
        sprintf(string,
                "You have not advanced enough to build this ship.\n%.1f engineering technology needed. You have %.1f.\n",
                (double)Shipdata[what][ABIL_TECH],
                race->tech);

        return 0;
    }

    return 1;
}

int can_build_on_ship(int what, racetype *race, shiptype *builder, char *string)
{
    if (!(Shipdata[what][ABIL_BUILD] & Shipdata[builder->type][ABIL_CONSTRUCT])
        && !race->God) {
        sprintf(string,
                "This ship type cannot be built by a %s.\n",
                Shipnames[builder->type]);

        sprintf(temp,
                "Use 'build ? %c' to find out where it can be built.\n",
                Shipltrs[what]);

        strcat(string, temp);

        return 0;
    }

    return 1;
}

int can_build_on_sector(int what,
                        racetype *race,
                        planettype *planet,
                        sectortype *sector,
                        int x,
                        int y,
                        char *string)
{
    shiptype *s;
    char shipc;

    shipc = Shipltrs[what];

    if (!sector->popn) {
        sprintf(string, "You have no more civs in the sector!\n");

        return 0;
    }

    if (sector->condition == WASTED) {
        sprintf(string, "You can't build on wasted sectors.\n");

        return 0;
    }

    if ((sector->owner != race->Playernum) && !race->God) {
        sprintf(string, "You don't own that sector.\n");

        return 0;
    }

    if (((Shipdata[what][ABIL_BUILD] % 2) == 0) && !race->God) {
        sprintf(string, "This ship type cannot be built on a planet.\n");

        sprintf(temp,
                "Use 'build ? %c' to find out where it can be built.\n",
                shipc);

        strcat(string, temp);

        return 0;
    }

    if (what == OTYPE_QUARRY) {
        int sh;

        sh = planet->ships;

        while (sh) {
            getship(&s, sh);

            if (s->alive
                && (s->type == OTYPE_QUARRY)
                && (s->land_x == x)
                && (s->land_y == y)) {
                sprintf(string, "There already is a quarry here.\n");
                free(s);

                return 0;
            }

            sh = nextship(s);
            free(s);
        }
    }

    return 1;
}

int build_at_ship(int playernum,
                  int governor,
                  racetype *race,
                  shiptype *builder,
                  int *snum,
                  int *pnum)
{
    if (testship(playernum, governor, builder)) {
        return -1;
    }

    if (!Shipdata[builder->type][ABIL_CONSTRUCT]) {
        notify(playernum,
               governor,
               "This ship cannot construct other ships.\n");

        return -1;
    }

    if (!builder->popn) {
        notify(playernum, governor, "This ship has no crew.\n");

        return -1;
    }

    if (builder->damage) {
        notify(playernum, governor, "This ship is damage can cannot build.\n");

        return -1;
    }

    if ((builder->type == OTYPE_FACTORY) && !builder->on) {
        notify(playernum, governor, "This factory is not online.\n");

        return -1;
    }

    if ((builder->type == OTYPE_FACTORY)
        && !landed(builder)
        && !inship(builder)) {
        notify(playernum, governor, "Factories must be landed on a planet.\n");

        return -1;
    }

    *snum = builder->storbits;
    *pnum = builder->pnumorbits;

    return builder->whatorbits;
}

void autoload_at_planet(int playernum,
                        shiptype *s,
                        planettype *planet,
                        sectortype *sector,
                        int *crew,
                        double *fuel)
{
    *crew = MIN(s->max_crew, sector->popn);
    *fuel = MIN((double)s->max_fuel, (double)planet->info[playernum - 1].fuel);
    sector->popn -= *crew;

    if (!sector->popn && !sector->troops) {
        sector->owner = 0;
    }

    planet->info[playernum - 1].fuel -= (int)*fuel;
}

void autoload_at_ship(int playernum,
                      shiptype *s,
                      shiptype *b,
                      int *crew,
                      double *fuel)
{
    *crew = MIN(s->max_crew, b->popn);
    *fuel = MIN((double)s->max_fuel, (double)b->fuel);
    b->popn -= *crew;
    b->fuel -= *fuel;
}

void initialize_new_ship(int playernum,
                         int governor,
                         racetype *race,
                         shiptype *newship,
                         double load_fuel,
                         int load_crew,
                         int quiet)
{
    int i;

#ifdef AUTOSCRAP
    newship->autoscrap = 0;
#endif

#ifdef THRESHOLDING
    newship->threshold[RESOURCE] = 0;
    newship->threshold[DESTRUCT] = 0;
    newship->threshold[FUEL] = 0;
    newship->threshold[CRYSTAL] = 0;
#endif

    newship->speed = newship->max_speed;
    newship->owner = playernum;
    newship->governor = governor;
    newship->troops = 0;

    if (race->God) {
        newship->fuel = newship->max_fuel;
        newship->popn = newship->max_crew;
        newship->resource = newship->max_resource;
        newship->destruct = newship->max_destruct;
        newship->mounted = newship->mount;
        newship->damage = 0;
    } else {
        newship->fuel = load_fuel;
        newship->popn = load_crew;
        newship->resource = 0;
        newship->destruct = 0;
        newship->mounted = 0;
        newship->damage = Shipdata[newship->type][ABIL_DAMAGE];
    }

    newship->crystals = 0;
    newship->hanger = 0;
    newship->mass = newship->base_mass
        + ((double)newship->popn * race->mass)
        + ((double)newship->fuel * MASS_FUEL)
        + ((double)newship->resource * MASS_RESOURCE)
        + ((double)newship->destruct * MASS_DESTRUCT);

    newship->alive = 1;
    newship->active = 1;
    newship->protect.self = newship->guns ? 1 : 0;
    newship->hyper_drive.on = 0;
    newship->hyper_drive.ready = 0;
    newship->hyper_drive.charge = 0;
    newship->cloak = Shipdata[newship->type][ABIL_CLOAK] * Cloak(race);
    newship->cloaked = 0;
    newship->cloaking = 0;
    newship->smart_strength = 0;
    newship->smart_gun = NONE;
    newship->smart_list[0] = 0;
    newship->fire_laser = 0;
    newship->mode = 0;
    newship->rad = 0;
    newship->retaliate = newship->primary;
    newship->ships = 0;
    newship->on = 0;

    /* HUTm (kse) */
    newship->use_stock = 1;
    newship->hop = 1;
    newship->limit = 100;

    switch (newship->type) {
#ifdef USE_VN
    case OTYPE_VN:
        newship->special.mind.busy = 1;
        newship->special.mind.progenitor = playernum;
        newship->special.mind.generation = 1;
        newship->special.mind.target = 0;
        newship->special.mind.tampered = 0;

        break;
#endif
    case STYPE_MINEF:
        newship->special.trigger.radius = 100; /* Trigger radius */
        /* Disperse the mine when empty */
        newship->special.trigger.disperse = TRUE;

        if (!quiet) {
            notify(playernum,
                   governor,
                   "Mine disarmed.\nTrigger radius set at 100.\nMine set to disperse when empty.\n");
        }

        newship->retaliate = 0;
        newship->protect.self = 0;

        break;
    case OTYPE_TRANSDEV:
        newship->special.transport.target = 0;
        newship->on = 0;

        if (!quiet) {
            notify(playernum, governor, "Receive OFF. Change with order.\n");
        }

        break;
    case OTYPE_AP:
        if (!quiet) {
            notify(playernum, governor, "Processor OFF.\n");
        }

        break;
    case OTYPE_STELE:
    case OTYPE_GTELE:
        sprintf(buf,
                "Telescope range is %.2f.\n",
                tele_range(newship->type, newship->tech));

        if (!quiet) {
            notify(playernum, governor, buf);
        }

        break;
    case STYPE_MISSILE: /* HUTm (kse) */
        /* Set scatter on by default */
        newship->special.impact.scatter = 1;

        break;
    case OTYPE_FACTORY:
        newship->build_type = 0;

        break;
    default:

        break;
    }

    for (i = 0; i <= INF_MAX_TARGETS; ++i) {
        newship->inf.eff_targets[i].x = 999;
        newship->inf.fert_targets[i].x = 999;
    }

    newship->inf.wants_reports = 0;
    newship->inf.eff_setting = 0;
    newship->inf.fert_setting = 0;
    newship->inf.atmos_setting = 0;

    if (!quiet) {
        if (newship->damage) {
            sprintf(buf,
                    "Warning: this ship is construct with a %d%% damage level.\n",
                    newship->damage);

            notify(playernum, governor, buf);

            if (!Shipdata[newship->type][ABIL_REPAIR] && newship->max_crew) {
                notify(playernum,
                       governor,
                       "It will need resource to become fully operational.\n");
            }
        }

        if (Shipdata[newship->type][ABIL_REPAIR] && newship->max_crew) {
            notify(playernum,
                   governor,
                   "This ship does not need resources to repair.\n");
        }

        if (newship->type == OTYPE_FACTORY) {
            notify(playernum,
                   governor,
                   "This factory may not begin repairs until it has been activated.\n");
        }

        if (!newship->max_crew) {
            notify(playernum,
                   governor,
                   "This ship is robotic, and may not repair itself.\n");
        }

        sprintf(buf,
                "Loaded with %d crew and %.1f fuel.\n",
                load_crew,
                load_fuel);

        notify(playernum, governor, buf);
    }
}

int create_ship_by_planet(int playernum,
                          int governor,
                          racetype *race,
                          shiptype *newship,
                          planettype *planet,
                          int snum,
                          int pnum,
                          int x,
                          int y)
{
    int i;
    int shipno;

    newship->tech = race->tech;
    newship->xpos = Stars[snum]->xpos + planet->xpos;
    newship->ypos = Stars[snum]->ypos + planet->ypos;
    newship->land_x = x;
    newship->land_y = y;

    if ((newship->type == OTYPE_TERRA) || (newship->type == OTYPE_PLOW)) {
        sprintf(newship->class, "5");
    } else {
        sprintf(newship->class, "Standard");
    }
    
    newship->whatorbits = LEVEL_PLAN;
    newship->whatdest = LEVEL_PLAN;
    newship->deststar = snum;
    newship->destpnum = pnum;
    newship->storbits = snum;
    newship->pnumorbits = pnum;
    newship->docked = 1;
    newship->cloak = Shipdata[newship->type][ABIL_CLOAK] * Cloak(race);
    newship->cloaked = 0;
    planet->info[playernum - 1].resource -= newship->build_cost;

    shipno = getFreeShip(newship->type, playernum);

    if (shipno) {
        newship->number = shipno;
        newship->owner = playernum;
        newship->governor = governor;
        newship->ships = 0;
        insert_sh_plan(planet, newship);

        if (newship->type == OTYPE_TOXIC) {
            sprintf(buf,
                    "Toxin concentration on planet was %d%%,",
                    planet->conditions[TOXIC]);

            notify(playernum, governor, buf);

            if (planet->conditions[TOXIC] > TOXMAX) {
                newship->special.waste.toxic = TOXMAX;
            } else {
                newship->special.waste.toxic = planet->conditions[TOXIC];
            }

            planet->conditions[TOXIC] -= newship->special.waste.toxic;
            sprintf(buf, " now %d%%.\n", planet->conditions[TOXIC]);
            notify(playernum, governor, buf);
        }

        for (i = 0; i < INF_MAX_TARGETS; ++i) {
            newship->inf.eff_targets[i].x = 999;
            newship->inf.fert_targets[i].x = 999;
        }

        newship->inf.wants_reports = 0;
        newship->inf.eff_setting = 0;
        newship->inf.fert_setting = 0;
        newship->inf.atmos_setting = 0;

        sprintf(buf,
                "%s built at a cost of %d resources.\n",
                Ship(newship),
                newship->build_cost);

        notify(playernum, governor, buf);
        sprintf(buf, "Technology %.1f.\n", newship->tech);
        notify(playernum, governor, buf);

        sprintf(buf,
                "%s is on sector %d,%d.\n",
                Ship(newship),
                newship->land_x,
                newship->land_y);

        notify(playernum, governor, buf);

        return 1;
    } else {
        notify(playernum,
               governor,
               "All ship slots used up (or free ship list corrupted)\n");

        return 0;
    }
}

int create_ship_by_ship(int playernum,
                        int governor,
                        racetype *race,
                        int outside,
                        startype *star,
                        planettype *planet,
                        shiptype *newship,
                        shiptype *builder)
{
    int i;
    int shipno;

    shipno = getFreeShip(newship->type, playernum);

    if (shipno) {
        newship->number = shipno;
        newship->owner = playernum;
        newship->governor = governor;

        if (outside) {
            newship->whatorbits = builder->whatorbits;
            newship->whatdest = LEVEL_UNIV;
            newship->deststar = builder->deststar;
            newship->destpnum = builder->destpnum;
            newship->storbits = builder->storbits;
            newship->pnumorbits = builder->pnumorbits;
            newship->docked = 0;

            switch (builder->whatorbits) {
            case LEVEL_PLAN:
                if (planet == NULL) {
                    sprintf(buf, "No such planet\n");
                    notify(playernum, governor, buf);

                    break;
                }

                insert_sh_plan(planet, newship);

                break;
            case LEVEL_STAR:
                insert_sh_star(Stars[builder->storbits], newship);

                break;
            case LEVEL_UNIV:
                insert_sh_univ(&Sdata, newship);

                break;
            }

            newship->docked = 1;
            newship->whatdest = LEVEL_SHIP;
            newship->destshipno = builder->number;

            builder->docked = 1;
            builder->whatdest = LEVEL_SHIP;
            builder->destshipno = newship->number;
        } else {
            newship->whatorbits = LEVEL_SHIP;
            newship->whatdest = LEVEL_SHIP;
            newship->deststar = builder->deststar;
            newship->destpnum = builder->destpnum;
            newship->destshipno = builder->number;
            newship->storbits = builder->storbits;
            newship->pnumorbits = builder->pnumorbits;
            newship->docked = 1;
            newship->cloak = Shipdata[newship->type][ABIL_CLOAK] * Cloak(race);
            newship->cloaked = 0;
            insert_sh_ship(newship, builder);
            builder->mass += getmass(newship);
            builder->hanger += ship_size(newship);
        }

        for (i = 0; i < INF_MAX_TARGETS; ++i) {
            newship->inf.eff_targets[i].x = 999;
            newship->inf.fert_targets[i].x = 999;
        }

        newship->inf.wants_reports = 0;
        newship->inf.eff_setting = 0;
        newship->inf.fert_setting = 0;
        newship->inf.atmos_setting = 0;
        newship->tech = race->tech;
        newship->xpos = builder->xpos;
        newship->ypos = builder->ypos;
        newship->land_x = builder->land_x;
        newship->land_y = builder->land_y;

        if ((newship->type == OTYPE_TERRA) || (newship->type == OTYPE_PLOW)) {
            sprintf(newship->class, "5");
        } else {
            sprintf(newship->class, "Standard");
        }

        builder->resource -= newship->build_cost;

        sprintf(buf,
                "%s built at a cost of %d resources.\n",
                Ship(newship),
                newship->build_cost);

        notify(playernum, governor, buf);
        sprintf(buf, "Technology, %.1f.\n", newship->tech);
        notify(playernum, governor, buf);

        return 1;
    } else {
        notify(playernum,
               governor,
               "All ship slots used up (or free ship list corrupted)\n");

        return 0;
    }
}

double getmass(shiptype *s)
{
    return (1.0
            + (MASS_ARMOR * s->armor)
            + (MASS_SIZE * (s->size - s->max_hanger))
            + (MASS_HANGER * s->max_hanger)
            + (MASS_GUNS * s->primary * s->primtype)
            + (MASS_GUNS * s->secondary * s->sectype));
}

int ship_size(shiptype *s)
{
    double size;

    size = 1.0
        + (SIZE_GUNS * s->primary)
        + (SIZE_GUNS * s->secondary)
        + (SIZE_CREW * s->max_crew)
        + (SIZE_RESOURCE * s->max_resource)
        + (SIZE_FUEL * s->max_fuel)
        + (SIZE_DESTRUCT * s->max_destruct)
        + s->max_hanger;

    return (int)size;
}

double cost (shiptype *s)
{
    int i;
    double factor = 0.0;
    double advantage = 0.0;

    i = s->build_type;
    /* Compute how much it costs to build this ship */
    factor += (double)Shipdata[i][ABIL_COST];
    factor += (GUN_COST * (double)s->primary);
    factor += (GUN_COST * (double)s->secondary);
    factor += (CREW_COST * (double)s->max_crew);
    factor += (CARGO_COST * (double)s->max_resource);
    factor += (FUEL_COST * (double)s->max_fuel);
    factor += (AMMO_COST * (double)s->max_destruct);
    factor += (SPEED_COST * (double)s->max_speed * (double)sqrt((double)s->max_speed));
    factor += (HANGER_COST * (double)s->max_hanger);

    if (i != OTYPE_INF) {
        factor += (ARMOR_COST * (double)s->armor * (double)sqrt((double)s->armor));
    }

    factor += (CEW_COST * (double)(s->cew * s->cew_range));

    /* Additional advantages/disadvantages */
    if (s->hyper_drive.has) {
        advantage += 0.5;
    }

    if (s->laser) {
        advantage += 0.5;
    }

    if (s->cloak) {
        advantage += 0.5;
    }

    if (s->mount) {
        advantage += 0.5;
    }

    factor *= sqrt(1.0 + advantage);

    return factor;
}

void system_cost(double *advantage, double *disadvantage, int value, int base)
{
    double factor;

    factor = (((double)value + 1.0) / (base + 1.0)) - 1.0;

    if (factor >= 0.0) {
        *advantage += factor;
    } else {
        *disadvantage -= factor;
    }
}

double complexity(shiptype *s)
{
    int i;
    double advantage;
    double disadvantage;
    double factor;
    double temp;

    i = s->build_type;
    advantage = 0.0;
    disadvantage = 0.0;

    system_cost(&advantage,
                &disadvantage,
                (int)s->primary,
                Shipdata[i][ABIL_GUNS]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->secondary,
                Shipdata[i][ABIL_GUNS]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_crew,
                Shipdata[i][ABIL_MAXCREW]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_resource,
                Shipdata[i][ABIL_CARGO]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_fuel,
                Shipdata[i][ABIL_FUELCAP]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_destruct,
                Shipdata[i][ABIL_DESTCAP]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_speed,
                Shipdata[i][ABIL_SPEED]);

    system_cost(&advantage,
                &disadvantage,
                (int)s->max_hanger,
                Shipdata[i][ABIL_HANGER]);

    if (i != OTYPE_INF) {
        system_cost(&advantage,
                    &disadvantage,
                    (int)s->armor,
                    Shipdata[i][ABIL_ARMOR]);
    }

    /* Additional advantages/disadvantages */
    factor = sqrt((1.0 + advantage) * exp(-(double)disadvantage / 10.0));
    temp = ((COMPLEXITY_FACTOR * (factor - 1.0)) / sqrt((double)Shipdata[i][ABIL_TECH] + 1)) + 1.0;

    factor = temp * temp;

    return (factor * (double)Shipdata[i][ABIL_TECH]);
}

void Getship(shiptype *s, int i, racetype *r)
{
    memset(s, 0, sizeof(shiptype));
    s->reuse = 0;
    s->type = i;
    s->armor = Shipdata[i][ABIL_ARMOR];
    s->guns = Shipdata[i][ABIL_PRIMARY] ? PRIMARY : NONE;
    s->primary = Shipdata[i][ABIL_GUNS];
    s->primtype = Shipdata[i][ABIL_PRIMARY];
    s->secondary = Shipdata[i][ABIL_GUNS];
    s->sectype = Shipdata[i][ABIL_SECONDARY];
    s->max_crew = Shipdata[i][ABIL_MAXCREW];
    s->max_resource = Shipdata[i][ABIL_CARGO];
    s->max_hanger = Shipdata[i][ABIL_HANGER];
    s->max_destruct = Shipdata[i][ABIL_DESTCAP];
    s->max_fuel = Shipdata[i][ABIL_FUELCAP];
    s->max_speed = Shipdata[i][ABIL_SPEED];
    s->build_type = i;
    s->cew = 0;
    s->cew_range = 0;

    if (r->God) {
        s->mount = Shipdata[i][ABIL_MOUNT];
        s->hyper_drive.has = Shipdata[i][ABIL_JUMP];
        s->cloak = Shipdata[i][ABIL_CLOAK];
        s->laser = Shipdata[i][ABIL_LASER];
    } else {
        s->mount = 0;
        s->hyper_drive.has = 0;
        s->cloak = 0;
        s->laser = 0;
    }

    s->size = ship_size(s);
    s->base_mass = getmass(s);
    s->mass = getmass(s);

    if (r->God) {
        s->build_cost = 0;
    } else {
        s->build_cost = (int)cost(s);
    }

    if ((s->type == OTYPE_VN) || (s->type == OTYPE_BERS)) {
        s->special.mind.progenitor = r->Playernum;
    }
}

void Getfactship(shiptype * s, shiptype *b)
{
    memset(s, 0, sizeof(shiptype));
    s->type = b->build_type;
    s->armor = b->armor;
    s->primary = b->primary;
    s->primtype = b->primtype;
    s->secondary = b->secondary;
    s->sectype = b->sectype;
    s->guns = s->primary ? PRIMARY : NONE;
    s->max_crew = b->max_crew;
    s->max_resource = b->max_resource;
    s->max_hanger = b->max_hanger;
    s->max_destruct = b->max_destruct;
    s->max_fuel = b->max_fuel;
    s->max_speed = b->max_speed;
    s->build_type = b->build_type;
    s->build_cost = b->build_cost;
    s->mount = b->mount;
    s->hyper_drive.has = b->hyper_drive.has;
    s->cloak = b->cloak;
    s->laser = b->laser;
    s->cew = b->cew;
    s->cew_range = b->cew_range;
    s->size = ship_size(s);
    s->base_mass = getmass(s);
    s->mass = getmass(s);
}

int Shipcost(int i, racetype *r)
{
    shiptype s;

    Getship(&s, i, r);

    return (int)cost(&s);
}

int inship(shiptype *s)
{
    return (s->docked
            && (s->whatdest == LEVEL_SHIP)
            && (s->whatorbits == LEVEL_SHIP));
}

#ifdef MARKET
void sell(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    racetype *race;
    planettype *p;
    shiptype *s;
    commodtype c;
    int commodno;
    int quantity;
    int minbid;
    int item;
    int ok = 0;
    int sh;
    int snum;
    int pnum;
    int i;
    char commod;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum,
               governor,
               "You have to be in a planet scope to sell.\n");

        return;
    }

    snum = Dir[playernum - 1][governor].snum;
    pnum = Dir[playernum - 1][governor].pnum;

    if (argn < 4) {
        notify(playernum,
               governor,
               "Syntax: sell <commodity> <quantity> <minimum bid>\n");

        return;
    }

    race = races[playernum - 1];

    if (race->Guest) {
        notify(playernum, governor, "Guest races can't sell anything.\n");

        return;
    }

    /* Get information on sale */
    commod = args[1][0];
    quantity = atoi(args[2]);
    minbid = atoi(args[3]);

    if ((quantity <= 0) || (minbid <= 0)) {
        notify(playernum, governor, "Try using positive values.\n");

        return;
    }

    apcount = MIN(apcount, quantity);

    if (!enufAP(playernum, governor, Stars[snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p, snum, pnum);

    if (p->slaved_to && (p->slaved_to != playernum)) {
        sprintf(buf, "This planet is enslaved to player %d.\n", p->slaved_to);
        notify(playernum, governor, buf);
        free(p);

        return;
    }

    /* Check to see if there is an undamaged gov center or space port here */
    sh = p->ships;

    while (sh && !ok) {
        getship(&s, sh);

        if (s->alive
            && (s->owner == playernum)
            && !s->damage
            && Shipdata[s->type][ABIL_PORT]) {
            ok = 1;
        }

        sh = nextship(s);
        free(s);
    }

    if (!ok) {
        notify(playernum,
               governor,
               "You don't have an undamaged space port or government center here.\n");

        free(p);

        return;
    }

    switch (commod) {
    case 'r':
        if (!p->info[playernum - 1].resource) {
            notify(playernum,
                   governor,
                   "You don't have any resources here to sell!\n");

            free(p);

            return;
        }

        quantity = MIN(quantity, p->info[playernum - 1].resource);
        p->info[playernum - 1].resource -= quantity;
        item = RESOURCE;

        break;
    case 'd':
        if (!p->info[playernum - 1].destruct) {
            notify(playernum,
                   governor,
                   "You don't have any destruct here to sell!\n");

            free(p);

            return;
        }

        quantity = MIN(quantity, p->info[playernum - 1].destruct);
        p->info[playernum - 1].destruct -= quantity;
        item = DESTRUCT;

        break;
    case 'f':
        if (!p->info[playernum - 1].fuel) {
            notify(playernum,
                   governor,
                   "You don't have any fuel here to sell!\n");

            free(p);

            return;
        }

        quantity = MIN(quantity, p->info[playernum - 1].fuel);
        p->info[playernum - 1].fuel -= quantity;
        item = FUEL;

        break;
    case 'x':
        if (!p->info[playernum - 1].crystals) {
            notify(playernum,
                   governor,
                   "You don't have any crystals here to sell!\n");

            free(p);

            return;
        }

        quantity = MIN(quantity, p->info[playernum - 1].crystals);
        p->info[playernum - 1].crystals -= quantity;
        item = CRYSTAL;

        break;
    default:
        notify(playernum,
               governor,
               "Permitted commodities are r, d, f, and x.\n");

        free(p);

        return;
    }

    c.owner = playernum;
    c.governor = governor;
    c.type = item;
    c.quantity = quantity;
    c.minbid = minbid;
    c.deliver = 0;
    c.bid = 0;
    c.bidder = 0;
    c.star_from = snum;
    c.planet_from = pnum;
    c.star_to = snum;
    c.planet_to = pnum;

    commodno = getdeadcommod();

    while (commodno == 0) {
        commodno = getdeadcommod();
    }

    if (commodno == -1) {
        commodno = Numcommods() + 1;
    }

    sprintf(buf,
            "Lot #%d - %d units of %s starting at %d.\n",
            commodno,
            quantity,
            Commod[item],
            minbid);

    notify(playernum, governor, buf);

    sprintf(buf,
            "Lot #%d - %d units of %s for sale by %s [%d] starting at %d.\n",
            commodno,
            quantity,
            Commod[item],
            races[playernum - 1]->name,
            playernum,
            minbid);

    post(buf, TRANSFER);

    for (i = 1; i <= Num_races; ++i) {
        notify_race(i, buf);
    }

    putcommod(&c, commodno);
    putplanet(p, snum, pnum);
    free(p);
    deductAPs(playernum, governor, apcount, snum, 0);
}

void bid(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    racetype *race;
    planettype *p;
    commodtype *c;
    commodtype *cc;
    shiptype *s;
    char commod;
    int i;
    int item;
    int bid0;
    int lot;
    int shipping;
    int ok = 0;
    int sh;
    int u;
    int shipcost = 0;
    int minbid;
    int money_owed = 0;
    int snum;
    int pnum;
    double dist;
    double rate;

    if (argn == 1) {
        /* List all market blocks for sale */
        notify(playernum,
               governor,
               "+++ Galactic Bloodshed Commodities Market +++\n\n");

        notify(playernum,
               governor,
               "  Lot Stock      type  Owner  MinBid  Bidder  Amount Cost/Unit  Ship  Destn");

        for (i = 1; i <= Numcommods(); ++i) {
            getcommod(&c, i);

            if (c->owner && c->quantity) {
                rate = (double)c->bid / (double)c->quantity;

                if (c->bidder == playernum) {
                    sprintf(temp,
                            "%4.4s/%-4.4s",
                            Stars[c->star_to]->name,
                            Stars[c->star_to]->pnames[c->planet_to]);
                } else {
                    temp[0] = '\0';
                }

                minbid = (int)((double)c->bid * (1.0 + UP_BID));

                if (minbid <= 0) {
                    minbid = c->minbid;
                }

                /*
                 * (char) might convert in undesired way -- char might be signed
                 * or not Commod[c->type], c->owner, c->bidder, c->bid, rate
                 */
                sprintf(buf,
                        " %4d%c%5d%10s%7d%8d%8d%8ld%10.2f%6d %10s\n",
                        i,
                        c->deliver ? '*' : ' ',
                        c->quantity,
                        Commod[(unsigned int)c->type],
                        c->owner,
                        minbid,
                        c->bidder,
                        c->bid,
                        rate,
                        shipping_cost((int)c->star_from, (int)Dir[playernum - 1][governor].snum, &dist, (int)c->bid),
                        temp);

                notify(playernum, governor, buf);
            }

            free(c);
        }
    } else if (argn == 2) {
        /* List all market blocks for sale of the requested type */
        commod = args[1][0];

        switch (commod) {
        case 'r':
            item = RESOURCE;

            break;
        case 'd':
            item = DESTRUCT;

            break;
        case 'f':
            item = FUEL;

            break;
        case 'x':
            item = CRYSTAL;

            break;
        default:
            notify(playernum, governor, "No such type of commodity.\n");

            return;
        }

        notify(playernum,
               governor,
               "+++ Galactic Bloodshed Commodities Market +++\n\n");

        notify(playernum,
               governor,
               "  Lot Stock      type  Owner  Bidder  Amount Cost/Unit  Ship  Dest\n");

        for (i = 1; i <= Numcommods(); ++i) {
            getcommod(&c, i);

            if (c->owner && c->quantity && (c->type == item)) {
                rate = (double)c->bid / (double)c->quantity;

                if (c->bidder == playernum) {
                    sprintf(temp,
                            "%4.4s/%-4.4s",
                            Stars[c->star_to]->name,
                            Stars[c->star_to]->pnames[c->planet_to]);
                } else {
                    temp[0] = '\0';
                }

                minbid = (int)((double)c->bid * (1.0 + UP_BID));

                if (minbid <= 0) {
                    minbid = c->minbid;
                }

                /*
                 * (char) might convert in undesired way -- char might be signed
                 * or not Commod[c->type], c->owner, c->bidder, c->bid, rate
                 */
                sprintf(buf,
                        " %4d%c%5d%10s%7d%8d%8d%8ld%10.2f%6d %10s\n",
                        i,
                        c->deliver ? '*' : ' ',
                        c->quantity,
                        Commod[(unsigned int)c->type],
                        c->owner,
                        minbid,
                        c->bidder,
                        c->bid,
                        rate,
                        shipping_cost((int)c->star_from, (int)Dir[playernum - 1][governor].snum, &dist, (int)c->bid),
                        temp);

                notify(playernum, governor, buf);
            }

            free(c);
        }
    } else {
        if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
            notify(playernum,
                   governor,
                   "You have to be in a planet scope to buy.\n");

            return;
        }

        snum = Dir[playernum - 1][governor].snum;
        pnum = Dir[playernum - 1][governor].pnum;

        getplanet(&p, snum, pnum);

        /* Can't bid from an enslaved planet */
        if (p->slaved_to && (p->slaved_to != playernum)) {
            sprintf(buf,
                    "This planet is enslaved to player %d.\n",
                    p->slaved_to);

            notify(playernum, governor, buf);
            free(p);

            return;
        }

        /*
         * Check to see if there is an undamaged gov center or space port here
         */
        sh = p->ships;

        while (sh && !ok) {
            getship(&s, sh);

            if (s->alive
                && (s->owner == playernum)
                && !s->damage
                && Shipdata[s->type][ABIL_PORT]) {
                ok = 1;
            }

            sh = nextship(s);
            free(s);
        }

        if (!ok) {
            notify(playernum,
                   governor,
                   "You don't have an undamaged space port or government center here.\n");

            free(p);

            return;
        }

        lot = atoi(args[1]);
        bid0 = atoi(args[2]);

        if ((lot <= 0) || (lot > Numcommods())) {
            notify(playernum, governor, "Illegal lot number.\n");
            free(p);

            return;
        }

        /* What the HELL does this do!? */
        getcommod(&c, lot);

        if (!c->owner) {
            notify(playernum, governor, "No such lot for sale.\n");
            free(p);
            free(c);

            return;
        }

        /*
         * Don't allow players to cheat by using the market to move supplies
         * around. That is what AVPMs are for. At tech 250.
*/
        if ((c->owner == playernum)
            && ((c->star_from != Dir[(unsigned int)playernum - 1][(unsigned int)c->governor].snum)
                || (c->star_from != Dir[(unsigned int)playernum - 1][(unsigned int)c->governor].pnum))) {
            notify(playernum,
                   governor,
                   "You can only set a minimum price for your lot from the location it was sold.\n");

            free(p);
            free(c);

            return;
        }

        minbid = (int)((double)c->bid * (1.0 + UP_BID));

        if (minbid <= 0) {
            minbid = c->minbid;
        }

        if (bid0 < minbid) {
            sprintf(buf, "You have to bid %d or more.\n", minbid);
            notify(playernum, governor, buf);
            free(p);
            free(c);

            return;
        }

        /* Don't allow guests to screw things up */
        race = races[playernum - 1];

        if (race->Guest) {
            notify(playernum, governor, "Guest races cannot bid.\n");
            free(p);
            free(c);

            return;
        }

        /* Make sure they have the bucks */
        if (bid0 > MONEY(race, governor)) {
            notify(playernum,
                   governor,
                   "Sorry, no buying on credit allowed.\n");

            free(p);
            free(c);

            return;
        }

        /*
         * Make sure we check to see if we have enough for shipping costs as
         * well. --JPD--
         */
        shipcost = shipping_cost((int)snum, (int)c->star_from, &dist, (int)bid0);

        if ((bid0 + shipcost) > MONEY(race, governor)) {
            sprintf(buf,
                    "The bid (%d) + shipping costs (%d) exceed your money (%ld)\n",
                    bid0,
                    shipcost,
                    MONEY(race, governor));

            notify(playernum, governor, buf);
            free(p);
            free(c);

            return;
        }

        /*
         * Also make sure they have not bid on other things and used money
         * earmarked for other lots. --JPD--
         */
        for (u = 1; u <= Numcommods(); ++u) {
            getcommod(&cc, u);

            if (cc->bidder == playernum) {
                money_owed += cc->bid;
            }
        }

        free(cc);

        if ((bid0 + shipcost + money_owed) > MONEY(race, governor)) {
            notify(playernum,
                   governor,
                   "All you money is used in bids already\n");

            free(p);
            free(c);

            return;
        }

        /*
         * They passed the credit check, notify the previous bidder that he was
         * just outbidded
         */
        if (c->bidder) {
            sprintf(buf,
                    "The bid on lot #%d (%d %s) has been upped to %d by  %s [%d].\n",
                    lot,
                    c->quantity,
                    Commod[(unsigned int)c->type],
                    bid0,
                    race->name,
                    playernum);

            notify((int)c->bidder, (int)c->bidder_gov, buf);
        }

        /* Set the commod structs to the new values */
        c->bid = bid0;
        c->bidder = playernum;
        c->bidder_gov = governor;
        c->star_to = snum;
        c->planet_to = pnum;
        shipping = shipcost;

        sprintf(buf,
                "There will be an additional %d charged to you for shipping costs.\n",
                shipping);

        notify(playernum, governor, buf);
        putcommod(c, lot);
        notify(playernum, governor, "Bid accepted.\n");
        free(p);
        free(c);
    }
}

int shipping_cost(int to, int from, double *dist, int value)
{
    double factor;
    double fcost;
    int junk;

    *dist = sqrt(Distsq(Stars[to]->xpos, Stars[to]->ypos, Stars[from]->xpos, Stars[from]->ypos));

    junk = (int)(*dist / 10000.0);
    junk *= 10000;

    factor = 1.0 - exp(-(double)junk / MERCHANT_LENGTH);
    fcost = factor * (double)value;

    return (int)fcost;
}

#endif
