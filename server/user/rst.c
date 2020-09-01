/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
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
 * ship -- report -- stock -- tactical -- stuff on ship
 *
 * Command "factories" programmed by varneyml@gm.erc.clarkson.edu
 *
 * #ident  "@(#)rst.c        1.11 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/rst.c,v 1.5 2007/07/06 18:09:3334 gbp Exp $
 */

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

#define REPORT 0
#define STOCK 1
#define TACTICAL 2
#define SHIP 3
#define STATUS 4
#define WEAPONS 5
#define FACTORIES 6

#define PLANET 1

extern char Shipltrs[];

static char Caliber[] = { ' ', 'L', 'M', 'H' };
static char shiplist[256];
static unsigned char status;
static unsigned char ship;
static unsigned char stock;
static unsigned char report;
static unsigned char tactical;
static unsigned char weapons;
static unsigned char factories;
static unsigned char first;
static unsigned char found;
static racetype *race;
static int enemies_only;
static int armed_only;
static int dist_limit;
static int not_landed;
static int from_planet;
static int what_race;
static int who;
static int display;

/* Global; used in csp_dump.c */
reportdata *rd;

extern void rst(int, int, int, int);
extern void ship_report(int, int, int, unsigned char[], int);
extern void plan_getrships(int, int, int);
extern void star_getrships(int, int, int);
extern int Getrship(int, int, int);
extern void Free_rlist(void);
extern int listed(int, char *);
extern double rad2deg(double);
extern double deg2rad(double);

void rst(int playernum, int governor, int apcount, int rst)
{
    int shipno;
    int shn;
    int i;
    int n_ships;
    int num;
    int snum;
    int pnum;
    int terse_out;
    unsigned char report_types[NUMSTYPES];

    for (i = 0; i < NUMSTYPES; ++i) {
        report_types[i] = 1;
    }

    enemies_only = 0;
    armed_only = 0;
    dist_limit = 0;
    not_landed = 0;
    from_planet = 0;
    what_race = 0;
    terse_out = 0;
    num_ships = 0;
    display = 1;
    first = 1;
    found = 0;

    switch (rst) {
    case REPORT:
        report = 1;
        factories = 0;
        tactical = factories;
        ship = tactical;
        stock = ship;
        status = stock;
        weapons = status;

        break;
    case STOCK:
        stock = 1;
        factories = 0;
        tactical = factories;
        ship = tactical;
        report = ship;
        status = report;
        weapons = status;

        break;
    case TACTICAL:
        tactical = 1;
        factories = 0;
        stock = factories;
        ship = stock;
        report = ship;
        status = report;
        weapons = status;

        break;
    case SHIP:
        stock = 1;
        ship = stock;
        report = 0;
        tactical = report;
        factories = 1;
        status = factories;
        weapons = status;

        break;
    case STATUS:
        status = 1;
        factories = 0;
        ship = factories;
        tactical = ship;
        stock = tactical;
        report = stock;
        weapons = report;

        break;
    case WEAPONS:
        weapons = 1;
        factories = 0;
        ship = factories;
        tactical = ship;
        stock = tactical;
        report = stock;
        status = report;

        break;
    case FACTORIES:
        factories = 1;
        weapons = 0;
        ship = weapons;
        tactical = ship;
        stock = tactical;
        report = stock;
        status = report;

        break;
    }

    /*
     * Players are whining about the spamminess of the tactical command so her's
     * some flags they can use to cut down on the display amount:
     *     -e : Enemies-only display
     *     -a : Armed-only ships
     *     -l : Ships that are NOT landed
     *     -p : From the planet at scope
     *     -s : Terse, summary only
     *     -d# : Distance limited to #
     *     -r# : Race restricted to #
     */
    if (tactical && optn) {
        /* Have flag */
        if (opts['e']) {
            enemies_only = 1;
        }

        if (opts['a']) {
            armed_only = 1;
        }

        if (opts['l']) {
            not_landed = 1;
        }

        if (opts['p']) {
            from_planet = 1;
        }

        if (opts['s']) {
            terse_out = 1;
        }

        if (opts['d']) {
            dist_limit = opts['d'];
        }

        if (opts['r']) {
            what_race = opts['r'];
        }
    }

    n_ships = Numships();
    rd = (reportdata *)malloc(sizeof(reportdata) * (n_ships + (Sdata.numstars * MAXPLANETS)));

    if (!rd) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failed [rst]");
        close_data_files();

        exit(1);
    }

    /* (One list entry for each ship, planet in universe) */
    race = races[playernum - 1];

    if (argn == 3) {
        if (isdigit((unsigned char)args[2][0])) {
            who = atoi(args[2]);
        } else {
            /* Treat args[2] as a list of ship types */
            who = -1;
            strncpy(shiplist, args[2], 255);
        }
    } else {
        who = 0;
    }

    if (tactical && from_planet) {
        planettype *p;

        if (Dir[playernum - 1][Governor].level != LEVEL_PLAN) {
            sprintf(buf, "You must be at planetary scope to run this.\n");
            notify(playernum, governor, buf);
            free(rd);

            return;
        } else {
            snum = Dir[playernum - 1][governor].snum;
            pnum = Dir[playernum - 1][governor].pnum;

            /* This allows us to do tactical reports from planets */
            getplanet(&rd[num_ships].p, snum, pnum);
            p = rd[num_ships].p;

            /* Add this planet into the ship list */
            rd[num_ships].star = snum;
            rd[num_ships].pnum = pnum;
            rd[num_ships].type = PLANET;
            rd[num_ships].n = 0;
            rd[num_ships].x = Stars[snum]->xpos + p->xpos;
            rd[num_ships].y = Stars[snum]->ypos + p->ypos;
            ++num_ships;

            num = num_ships;
            star_getrships(playernum, governor, snum);
            ship_report(playernum, governor, num - 1, report_types, terse_out);
            Free_rlist();

            return;
        }
    } else if (argn >= 2) {
        if ((*args[1] == '#') || isdigit((unsigned char)*args[1])) {
            /* Report on a couple ships */
            int l = 1;

            while ((l < MAXARGS)
                   && (*args[l] != '\0')
                   && ((*args[l] == '#') || isdigit((unsigned char)*args[l]))) {
                if (*args[l] == '#') {
                    sscanf(args[l] + 1, "%d", &shipno);
                } else {
                    sscanf(args[l], "%d", &shipno);
                }

                if ((shipno > n_ships) || (shipno < 1)) {
                    sprintf(buf, "rst: No such ship #%d\n", shipno);
                    notify(playernum, governor, buf);
                    free(rd);

                    return;
                }

                if (!Getrship(playernum, governor, shipno)) {
                    sprintf(buf,
                            "rst: Failed to get ship #%d from disk\n",
                            shipno);

                    notify(playernum, governor, buf);
                    free(rd);

                    return;
                }

                num = num_ships;

                if (rd[num_ships - 1].s->whatorbits != LEVEL_UNIV) {
                    star_getrships(playernum,
                                   governor,
                                   (int)rd[num - 1].s->storbits);

                    ship_report(playernum,
                                governor,
                                num - 1,
                                report_types,
                                terse_out);
                } else {
                    ship_report(playernum,
                                governor,
                                num - 1,
                                report_types,
                                terse_out);
                }

                ++l;
            }

            Free_rlist();

            return;
        } else {
            int l;

            l = strlen(args[1]);

            /* Display summary header */
            num = -1;

            for (i = 0; i < NUM_STYPES; ++i) {
                report_types[i] = 0;
            }

            while (l) {
                --l;
                i = NUMSTYPES;

                while (i && (Shipltrs[i] != args[1][l])) {
                    --i;
                }

                if (Shipltrs[i] != args[1][l]) {
                    sprintf(buf,
                            "'%c' -- No such ship letter\n",
                            args[1][l]);

                    notify(playernum, governor, buf);
                } else {
                    report_types[i] = 1;
                }
            }
        }
    }

    if (tactical && ((argn < 2) || (num < 0))) {
        /* Force terse for summary report */
        terse_out = 1;

        sprintf(buf, "Summary tactical report encompassing scope: /");
        notify(playernum, governor, buf);

        if ((Dir[playernum - 1][governor].level == LEVEL_STAR)
            || (Dir[playernum - 1][governor].level == LEVEL_PLAN)) {
            sprintf(buf, "%s", Stars[Dir[playernum - 1][governor].snum]->name);
            notify(playernum, governor, buf);
        }

        if (Dir[playernum - 1][governor].level = LEVEL_PLAN) {
            sprintf(buf,
                    "/%s",
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            notify(playernum, governor, buf);
        }

        notify(playernum, governor, "\n");

        sprintf(buf,
                "---------------------------------------------------------------\n");

        notify(playernum, governor, buf);
        sprintf(buf, " #    name             | sighted           | in range\n");
        notify(playernum, governor, buf);
    }

    switch (Dir[playernum, - 1][governor].level) {
    case LEVEL_UNIV:
        if ((rst != TACTICAL) || (argn >= 2)) {
            shn = Sdata.ships;

            while (shn && Getrship(playernum, governor, shn)) {
                shn = nextship(rd[num_ships - 1].s);
            }

            for (i = 0; i < Sdata.numstars; ++i) {
                star_getrships(playernum, governor, i);
            }

            for (i = 0; i < num_ships; ++i) {
                ship_report(playernum, governor, i, report_types, terse_out);
            }
        } else {
            notify(playernum,
                   governor,
                   "You can't do tactical option from universe level.\n");

            /* Nothing allocated */
            free(rd);

            return;
        }

        break;
    case LEVEL_PLAN:
        /* For tactical report it's important we get everything in range */
        if (!tactical) {
            plan_getrships(playernum,
                           governor,
                           Dir[playernum - 1][governor].snum,
                           Dir[playernum - 1][governor].pnum);
        } else {
            star_getrships(playernum,
                           governor,
                           Dir[playernum - 1][governor].snum);
        }

        for (i = 0; i < num_ships; ++i) {
            ship_report(playernum, governor, i, report_types, terse_out);
        }

        break;
    case LEVEL_STAR:
        star_getrships(playernum, governor, Dir[playernum - 1][governor].snum);

        for (i = 0; i < num_ships; ++i) {
            ship_report(playernum, governor, i, report_types, terse_out);
        }

        break;
    case LEVEL_SHIP:
        Getrship(playernum,
                 governor,
                 Dir[playernum - ][governor].shipno);

        /* First ship report */
        ship_report(playernum, governor, 0, report_types, terse_out);
        shn = rd[0].s->ships;
        num_ships = 0;

        while (shn && getrship(playernum, governor, shn)) {
            shn = nextship(rd[num_ships - 1].s);
        }

        for (i = 0; i < num_ships; ++i) {
            ship_report(playernum, governor, i, report_types, terse_out);
        }

        break;
    }

    if (!found) {
        /*
         * When we got first applicable ship first is set to 0 so if no ships
         * were shown tell it to the player (kse)
         */
        notify(playernum, governor, "No applicable ships pal.\n");
    }

    Free_rlist();
}

void ship_report(int playernum,
                 int governor,
                 int indx,
                 unsigned char rep_on[],
                 int terse)
{
    shiptype *s;
    planettype *p;
    int shipno;
    int i;
    int sight;
    int caliber;
    placetype where;
    char strng[COMMANDSIZE];
    char locstrn[COMMANDSIZE];
    char tmpbuf1[10];
    char tmpbuf2[10];
    char tmpbuf3[10];
    char tmpbuf4[10];
    double dist;
    double bear;
    double head;
    int planet_cnt;
    int war_rng_cnt;
    int ally_rng_cnt;
    int neut_rng_cnt;
    int war_see_cnt;
    int ally_see_cnt;
    int neut_see_cnt;

    /* Last ship retrieved from disk */
    s = rd[indx].s;
    p = rd[indx].p;
    shipno = rd[indx].n;

    /* Launched cansiter, non-owned ships don't show up */
    if (((rd[indx].type == PLANET) && p->info[playernum - 1].numsectsowned)
        || ((rd[indx].type != PLANET)
            && s->alive
            && (s->owner == playernum)
            && authorized(governor, s)
            && ((s->type != OTYPE_CANIST) || s->docked)
            && ((s->type != OTYPE_GREEN) || s->docked))) {
        if ((rd[indx].type != PLANET) && stock) {
            if (first) {
                sprintf(buf,
                        "    #       name        x  hanger   res        des         fuel      crew/mil\n");

                notify(playernum, governor, buf);

                if (!ship) {
                    first = 0;
                }

                found = 1;
            }

            sprintf(buf,
                    "%5u %c %14.14s%3u%4u:%-3u%5u:%-5ld%5u:%-5ld%7.1f:%-6ld%u/%u:%d\n",
                    shipno,
                    Shipltrs[s->type],
                    s->active ? s->name : "INACTIVE",
                    s->crystals,
                    s->hanger,
                    s->max_hanger,
                    s->resource,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_CARGO]
                    : s->max_resource,
                    s->destruct,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][DESTCAP]
                    : s->max_destruct,
                    s->fuel,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_FUELCAP]
                    : s->max_fuel,
                    s->popn,
                    s->troops,
                    s->max_crew);

            notify(playernum, governor, buf);
        }

        if ((rd[indx].type != PLANET) && status) {
            if (first) {
                sprintf(buf,
                        "    #       name       las cew hyp    guns   arm tech spd cost  mass size cloak\n");

                notify(playernum, governor, buf);

                if (!ship) {
                    first = 0;
                }

                found = 1;
            }

            sprintf(buf,
                    "%5u %c %14.14s %s%s%s%3u%c/%3u%c%4lu%5.0f%4lu%5lu%7.1f%4u %c",
                    shipno,
                    Shipltrs[s->type],
                    s->active ? s->name : "INACTIVE",
                    s->laser ? "yes " : "    ",
                    s->cew ? "yes " : "    ",
                    s->hyper_drive.has ? "yes " : "    ",
                    s->primary,
                    Caliber[s->primtype],
                    s->secondary,
                    Caliber[s->sectype],
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_ARMOR]
                    : (s->armor * (100 - s->damage)) / 100,
                    s->tech,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_SPEED]
                    : s->max_speed,
                    s->type == OTYPE_FACTORY
                    ? (2 * s->build_cost * s->on) + Shipdata[s->type][ABIL_COST]
                    : s->build_cost,
                    s->mass,
                    s->size,
                    (char)(s->cloak ? 'Y' : 'N'));

            notify(playernum, governor, buf);

            if (s->type) {
                sprintf(buf, " (%d)", s->special.pod.temperature);
                notify(playernum, governor, buf);
            }

            notify(playernum, governor, "\n");
        }

        if ((rd[indx].type != PLANET) && weapons) {
            if (first) {
                sprintf(buf,
                        "    #       name      age  laser   cew     safe     guns    damage   class\n");

                notify(playernum, governor, buf);

                if (!ship) {
                    first = 0;
                }

                found = 1;
            }

            sprintf(buf,
                    "%5u %c %14.14s %-4d %s  %3d/%-4d  %4d  %3d%c/%3dc    %3d%%  %c %s\n",
                    shipno,
                    Shipltrs[s->type],
                    s->active ? s->name : "INACTIVE",
                    s->age,
                    s->laser ? "yes " : "    ",
                    s->cew,
                    s->cew_range,
                    (int)(((1.0 - (0.01 * s->damage)) * s->tech) / 4.0),
                    s->primary,
                    Caliber[s->primtype],
                    s->secondary,
                    Caliber[s->sectype],
                    s->damage,
                    s->type == OTYPE_FACTORY ? Shipltrs[s->build_type] : ' ',
                    ((s->type == OTYPE_TERRA) || (s->type == OTYPE_PLOW)) ? "Standard" : s->class);

            notify(playernum, governor, buf);
        }

        if ((rd[indx].type != PLANET)
            && factories
            && (s->type == OTYPE_FACTORY)) {
            if (first) {
                sprintf(buf,
                        "   #    Cost Tech Mass Sz A Crw Ful Crg Hng Dst Sp Weapons Lsr CEWs Range Dmg\n");

                notify(playernum, governor, buf);

                if (!ship) {
                    first = 0;
                }

                found = 1;
            }

            if ((s->build_type == 0) || (s->build_type == OTYPE_FACTORY)) {
                sprintf(buf,
                        "%5d               (No ship type specified yet)                     (OFF)\n",
                        shipno);

                notify(playernum, governor, buf);
            } else {
                if (s->primtype) {
                    sprintf(tmpbuf1,
                            "%2d%s",
                            s->primary,
                            s->primtype == LIGHT ? "L"
                            : s->primtype == MEDIUM ? "M"
                            : s->primtype == HEAVY ? "H" : "N");
                } else {
                    strncpy(tmpbuf, "---", 9);
                }

                if (s->sectype) {
                    sprintf(tmpbuf2,
                            "%2d%s",
                            s->secondary,
                            s->sectype == LIGHT ? "L"
                            : s->sectype == MEDIUM ? "M"
                            : s->sectype == HEAVY ? "H" : "N");
                } else {
                    strncpy(tmpbuf2, "---", 9);
                }

                if (s->cew) {
                    sprintf(tmpbuf3, "%4d", s->cew);
                } else {
                    strncpy(tmpbuf3, "----", 9);
                }

                if (s->cew) {
                    sprintf(tmpbuf4, "%5d", s->cew_range);
                } else {
                    strncpy(tmpbuf4, "-----", 9);
                }

                sprintf(buf,
                        "%5u %c%4d%6.1f%5.1f%3d%2d%4d%4d%4d%4d%4d %c%1d %s/%s %s %s %s %s %02d%%%s\n",
                        shipno,
                        Shipltrs[s->build_type],
                        s->build_cost,
                        s->complexity,
                        s->base_mass,
                        ship_size(s),
                        s->armor,
                        s->max_crew,
                        s->max_fuel,
                        s->max_resource,
                        s->max_hanger,
                        s->max_destruct,
                        s->hyper_drive.has ?
                        s->mount ?
                        s->mounted ? '+' : '-'
                        : '*'
                        : ' ',
                        s_max_speed,
                        tmpbuf1,
                        tmpbuf2,
                        tmpbuf3,
                        tmpbuf4,
                        s->damage,
                        s->damage ? s->on ? "" : "*" : "");

                notify(playernum, governor, buf);
            }
        }

        if ((rd[indx].type != PLANET) && report) {
            if (first) {
                sprintf(buf,
                        " #      name       gov dam crew mil  des fuel sp orbits     destination\n");

                notify(playernum, governor, buf);

                if (!ship) {
                    first = 0;
                }

                found = 1;
            }

            if (s->docked) {
                if (s->whatdest == LEVEL_SHIP) {
                    sprintf(locstrn, "D#%d", s->destshipno);
                } else {
                    sprintf(locstrn, "L%2d,%-2d", s->land_x, s->land_y);
                }
            } else if (s->navigate.on) {
                sprintf(locstrn,
                        "nav:%d (%d)",
                        s->navigate.bearing,
                        s->navigate.turns);
            } else {
                strncpy(locstrn,
                        prin_ship_dest(playernum, governor, s),
                        COMMAND_SIZE - 1);
            }

            if (!s->active) {
                sprintf(strng, "INACTIVE(%d)", s->rad);
            }

            sprintf(buf,
                    "%c%-5u %12.12s %2d %3u%5u%4u%5u%5.0f %c%1u %-10s %-18s\n",
                    Shipltrs[s->type],
                    shipno,
                    s->active ? s->name : strng,
                    s->governor,
                    s->damage,
                    s->popn,
                    s->troops,
                    s->destruct,
                    s->fuel,
                    s->hyper_drive.has ? s->mount ? s->mounted ? '+' : '-' : '*' : ' ',
                    s->speed,
                    Dispshiploc_brief(s),
                    locstrn);

            notify(playernum, governor, buf);
        }

        if (tactical) {
            int fev = 0;
            int fspeed = 0;
            int defense;
            int fdam = 0;
            double tech;

            sight = 0;

            if (rd[indx].type == PLANET) {
                sight = 1;
            } else if ((s->type == OTYPE_PROBE) || s->popn) {
                sight = 1;
            }

            if (rd[indx].type == PLANET) {
                tech = race->tech;
                caliber = MEDIUM;

                if (sight && !terse) {
                    /* Tac report from planet */
                    sprintf(buf,
                            "Tactical report from planet /%s/%s:\n",
                            Stars[rd[indx].star]->name,
                            Stars[rd[indx].star]=>pnames[rd[indx].pnum]);

                    notify(playernum, governor, buf);
                    sprintf(buf, "\ntech    guns             dest   fuel\n");
                    notify(playernum, governor, buf);

                    sprintf(buf,
                            "%4.0f   %4dM            %5u %6u\n",
                            tech,
                            p->info[playernum - 1].guns,
                            p->info[playernum - 1].destruct,
                            p->info[playernum - 1].fuel);

                    notify(playernum, governor, buf);
                }
            } else {
                if (sight && !terse) {
                    sprintf(buf,
                            "Tactical report from ship %c%d%c %s%c\n",
                            Shipltrs[s->type],
                            shipno,
                            strlen(s->name) ? ',' : ':',
                            s->name,
                            strlen(s->name) ? ':' : ' ');

                    notify(playernum, governor, buf);

                    sprintf(buf,
                            "\ntech    guns  armor size dest   fuel dam spd evad orbits\n");

                    notify(playernum, governor, buf);
                }

                where.level = s->whatorbits;
                where.snum = s->storbits;
                where.pnum = s->pnumorbits;
                tech = s->tech;
                caliber = current_caliber(s);

                if (((s->whatdest != LEVEL_UNIV) || s->navigate.on)
                    && !s->docked
                    && s->active) {
                    fspeed = s->speed;
                    fev = s->protect.evade;
                }

                fdam = s->damage;

                if (sight && !terse) {
                    sprintf(buf,
                            "%4.0f%3d%c/%3d%c%6d%5d%5u%7.1f%3d%%  %1d  %3s  %-30.30s",
                            s->tech,
                            s->primary,
                            Caliber[s->primtype],
                            s->secondary,
                            Caliber[s->sectype],
                            s->armor,
                            s->size,
                            s->destruct,
                            s->fuel,
                            s->damage,
                            fspeed,
                            fev ? "yes" : "    ",
                            Displace(playernum, governor, &where));

                    notify(playernum, governor, buf);

                    if (landed(s)) {
                        sprintf(buf, " (%d,%d)", s->land_x, s->land_y);
                        notify(playernum, governor, buf);
                    }

                    if (!s->active) {
                        sprintf(buf, " INACTIVE(%d)", s->rad);
                        notify(playernum, governor, buf);
                    }

                    sprintf(buf, "\n");
                    notify(playernum, governor, buf);
                }
            }

            if (sight) {
                if (!terse) {
                    /* Tactical display */
                    sprintf(buf,
                            "\n  #      own typ      name  sz  rng bng spd hdn ev (50%%) hit  dam  pot  loc\n");

                    notify(playernum, governor, buf);
                }

                planet_cnt = 0;
                war_see_cnt = 0;
                war_rng_cnt = 0;
                ally_see_cnt = 0;
                ally_rng_cnt = 0;
                nuet_see_cnt = 0;
                neut_rng_cnt = 0;

                for (i = 0; i < num_ships; ++i) {
                    if (i != indx) {
                        dist = sqrt(Distsq(rd[indx].x, rd[indx].y, rd[i].x, rd[i].y));

                        if (dist < gun_range(race, rd[indx].s, rd[indx].type == PLANET)) {
                            bear = rad2deg(M_PI - atan2(rd[i].x - rd[indx].x, rd[i].y - rd[indx].y));

                            if (rd[i].type == PLANET) {
                                ++planet_cnt;

                                if (!terse && (rd[i].pnum != rd[indx].pnum)) {
                                    /* Tac report at planet */
                                    sprintf(buf,
                                            "  (planet) %17.17s %8.0f %3.0f\n",
                                            Stars[rd[i].star]->pnames[rd[i].pnum],
                                            dist,
                                            bear);

                                    notify(playernum, governor, buf);
                                }
                            } else {
                                /*
                                 * We use this for arg 2, this determines
                                 * whether we're scanning for all ships, ships
                                 * owned by a certain race, or ships of a
                                 * particular type
                                 */
                                if (!who
                                    || (who == rd[i].s->owner)
                                    || ((who == -1) && listed((int)rd[i].s->type, shiplist))) {
                                    /* Tac report at ship */
                                    if (((rd[i].s->owner != playernum)
                                         || !authorized(governor, rd[i].s))
                                        && rd[i].s->alive
                                        && (rd[i].s->type != OTYPE_CANIST)
                                        && (rd[i].s->type 1= OTYPE_GREEN)) {
                                        int tev = 0;
                                        int tspeed = 0;
                                        int body = 0;
                                        int prob = 0;
                                        int factor = 0;
                                        int xdst;
                                        int ydst;
                                        double potdmg = 0.0;

                                        /* See if target is cloaked */
                                        if (!see_cloaked(&rd[i], &rd[indx], dist)) {
                                            continue;
                                        }

                                        if (((rd[i].s->whatdest != LEVEL_UNIV)
                                             || rd[i].s->navigate.on)
                                            && !rd[i].s->docked
                                            && rd[i].s->active) {
                                            tspeed = rd[i].s->speed;
                                            tev = rd[i].s->protect.evade;
                                        }

                                        body = rd[i].s->size;
                                        defense = getdefense(rd[i].s);

                                        /* Calculate ship's heading */
                                        if (!terse) {
                                            xdst = 0;
                                            ydst = 0;

                                            if (((rd[i].s->whatdest == LEVEL_STAR)
                                                 || (rd[i].s->whatdest == LEVEL_PLAN))
                                                && (rd[i].s->deststar < Sdata.numstars)) {
                                                xdst += Stars[rd[i].s->deststar]->xpos;
                                                ydst += Stars[rd[i].s->deststar]->ypos;
                                            }

                                            if ((rd[i].s->whatdest == LEVEL_PLAN)
                                                && (rd[i].s->destpnum < Stars[rd[i].s->deststar]->numplanets)) {
                                                xdst += planets[rd[i].s->deststar][rd[i].s->destpnum]->xpos;
                                                ydst += planets[rd[i].s->deststar][rd[i].s->destpnum]->ypos;
                                            }

                                            head = rad2deg(M_PI - atan2(xdst - rd[i].s->xpos, -ydst + rd[i].s->ypos));
                                        }

                                        /*
                                         * New code by Kharush to make sure tact
                                         * gives correct probabilities for
                                         * sweepers against mines and
                                         * minefields. Corresponding change in
                                         * shootblast.c, shoot_ship_to_ship
                                         * function.
                                         */
                                        if (rd[indx].type != PLANET) {
                                            if ((rd[indx].s->type == STYPE_SWEEPER)
                                                && (rd[i].s->type == STYPE_MINEF)) {
                                                dist *= (dist / 2400.0);
                                            }
                                        }

                                        prob = hit_odds(dist,
                                                        &factor,
                                                        tech,
                                                        fdam,
                                                        fev,
                                                        tev,
                                                        fspeed,
                                                        tspeed,
                                                        body,
                                                        caliber,
                                                        defense);

                                        if ((rd[indx].type != PLANET)
                                            && laser_on(rd[indx].s)
                                            && rd[indx].s->focus) {
                                            prob = (prob * prob) / 100;
                                        }

                                        /* Maximum potential damage */
                                        if (!terse) {
                                            if (rd[indx].type == PLANET) {
                                                potdmg = (SHIP_DAMAGE
                                                          * MEDIUM
                                                          * p->inf[playernum - 1].guns)
                                                    / sqrt((double)(0.1
                                                                    * (rd[i].s->size
                                                                       - rd[i].s->max-hanger)));
                                            } else {
                                                potdmg = (SHIP_DAMAGE
                                                          * current_caliber(rd[indx].s)
                                                          * MAX(rd[indx].s->primary,
                                                                rd[indx].s->secondary))
                                                    / sqrt((double)(0.1
                                                                    * (rd[i].s->size
                                                                       - rd[i].s->max_hanger)));
                                            }
                                        }

                                        sprintf(buf,
                                                "%6d %s %2d,%1d %c %14.14s%4d %4.0f %3.0f %3d %3.0f %1s %4d %3d%% %3u%%%s %3.0f%%",
                                                rd[i].n,
                                                isset(race[playernum - 1]->atwar, rd[i].s->owner) ? "-"
                                                : isset(races[playernum -1]->allied, rd[i].s->owner) ? "+" : " ",
                                                rd[i].s->owner,
                                                rd[i].s->governor,
                                                Shipltrs[rd[i].s->type],
                                                rd[i].s->name,
                                                body,
                                                dist,
                                                bear,
                                                tspeed,
                                                head,
                                                tev ? "y " : "  ",
                                                factor,
                                                prob,
                                                rd[i].s->damage,
                                                rd[i].s->active ? "" : " INACTIVE",
                                                potdmg);

                                        if (isset(races[playernum - 1]->atwar, rd[i].s->owner)) {
                                            ++war_see_cnt;

                                            if (prob >= 1) {
                                                ++war_rng_cnt;
                                            }
                                        } else if (isset(races[playernum - 1]->allied, rd[i].s->owner)) {
                                            ++ally_see_cnt;

                                            if (prob >= 1) {
                                                ++ally_rng_cnt;
                                            }
                                        } else {
                                            ++neut_see_cnt;

                                            if (prob >= 1) {
                                                ++neut_rng_cnt;
                                            }
                                        }

                                        /* Take care of our flag options */
                                        display = 1;

                                        if (enemies_only
                                            && !isset(races[playernum -1]->atwar, rd[i].s->owner)) {
                                            display = 0;
                                        }

                                        if (armed_only
                                            && (!rd[i].s->primary
                                                && !rd[i].s->secondary
                                                && !rd[i].s->laser
                                                && !rd[i].s->cew
                                                && (rd[i].s->type != STYPE_MISSILE)
                                                && (rd[i].s->type != STYPE_MINEF))) {
                                            display = 0;
                                        }

                                        if (dist_limit && (dist > dist_limit)) {
                                            display = 0;
                                        }

                                        if (not_landed && rd[i].s->docked) {
                                            display = 0;
                                        }

                                        if (what_race
                                            && (rd[i].s->owner != what_race)) {
                                            display = 0;
                                        }

                                        if (display) {
                                            if (!terse) {
                                                notify(playernum,
                                                       governor,
                                                       buf);

                                                if (landed(rd[i].s)) {
                                                    sprintf(buf,
                                                            " (%d,%d)",
                                                            rd[i].s->land_x,
                                                            rd[i].s->land_y);

                                                    notify(playernum,
                                                           governor,
                                                           buf);
                                                } else {
                                                    sprintf(buf, "        ");
                                                    notify(playernum,
                                                           governor,
                                                           buf);
                                                }

                                                sprintf(buf, "\n");
                                                notify(playernum,
                                                       governor,
                                                       buf);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (!terse) {
                    sprintf(buf, "-----\n");
                    notify(playernum, governor, buf);
                } else {
                    if (rd[indx].type == PLANET) {
                        sprintf(buf,
                                "(planet) %13.13s | e:%3d a:%3d n:%3d | e:%3d a:%3d n:%3d\n",
                                Stars[rd[indx].star]->pnames[rd[indx].pnum],
                                war_see_cnt,
                                ally_see_cnt,
                                neut_see_cnt,
                                war_rng_cnt,
                                ally_rng_cnt,
                                neut_rng, cnt);

                        notify(playernum, governor, buf);
                    } else if (shipno) {
                        sprintf(buf,
                                "%c%-4u %16.16s | e:%3d a:%3d n:%3d | e:%3d a:%3d n:%3d\n",
                                Shipltrs[rd[indx].s->type],
                                shipno,
                                rd[indx].s->active ? rd[indx].s->name : "INACTIVE",
                                war_see_cnt,
                                ally_see_cnt,
                                neut_see_cnt,
                                war_rng_cnt,
                                ally_rng_cnt,
                                neut_rng_cnt);

                        notify(playernum, governor, buf);
                    }
                }
            }
        } /* End tactical */
    }
}

void plan_getrships(int playernum, int governor, int snum, int pnum)
{
    int shn;
    planettype *p;

    getplanet(&rd[num_ships].p, snum, pnum);
    p = rd[num_ships].p;
    /* Add this planet into the ship list */
    rd[num_ships].star = snum;
    rd[num_ships].pnum = pnum;
    rd[num_ships].type = PLANET;
    rd[num_ships].n = 0;
    rd[num_ships].x = Stars[snum]->xpos + p->xpos;
    rd[num_ships].y = Stars[snum]->ypos + p->ypos;
    ++num_ships;

    if (p->info[playernum - 1].explored) {
        shn = p->ships;

        while (shn && Getrship(playernum, governor, shn)) {
            shn = nextship(rd[num_ships - 1].s);
        }
    }
}

void star_getrships(int playernum, int governor, int snum)
{
    int shn;
    int i;

    if (isset(Stars[snum]->explored, playernum)) {
        shn = Stars[snum]->ships;

        while (shn && Getrship(playernum, governor, shn)) {
            shn = nextship(rd[num_ships - 1].s);
        }

        for (i = 0; i < Stars[snum]->numplanets; ++i) {
            plan_getrships(playernum, governor, snum, i);
        }
    }
}

/* Get a ship from the disk and add it to the ship list we're maintaining. */
int Getrship(int playernum, int governor, int shipno)
{
    if (getship(&rd[num_ships].s, shipno)) {
        rd[num_ships].type = 0;
        rd[num_ships].n = shipno;
        rd[num_ships].x = rd[num_ships].s->xpos;
        rd[num_ships].y = rd[num_ships].s->ypos;
        ++num_ships;

        return 1;
    } else {
        sprintf(buf, "Getrship: Error on ship get (%d).\n", shipno);
        notify(playernum, governor, buf);

        return 0;
    }
}

void Free_rlist(void)
{
    int i;

    for (i = 0; i < num_ships; ++i) {
        if (rd[i].type == PLANET) {
            free(rd[i].p);
        } else {
            free(rd[i].s);
        }
    }

    free(rd);
}

int listed(int type, char *string)
{
    char *p;

    for (p = string; *p; ++p) {
        if (Shipltrs[type] == *p) {
            return 1;
        }
    }

    return 0;
}

int see_cloaked(reportdata *target, reportdata *orig, double dist)
{
    shiptype *s = orig->s;
    shiptype *t = target->s;

    if (target->type == PLANET) {
        return 1;
    }

    /*
     * If ship is not cloaked or ship can see it anyway, return true, else zero
     */
    if (!t->cloaked) {
        return 1;
    }

    if (orig->type != PLANET) {
        if (s->type != OTYPE_PROBE) {
            if (dist > (200 * (s->tech / 200))) {
                return 0;
            } else {
                return 1;
            }
        }

        /* Is a probe, better chance */
        if (dist > (500 & (s->tech / 200))) {
            return 0;
        } else {
            return 1;
        }
    } else {
        /* Planet */
        if (dist > 200) {
            return 0;
        } else {
            return 1;
        }
    }
}

double rad2deg(double radians)
{
    return ((radians * 180.0f) / M_PI);
}

double deg2rad(double degrees)
{
    return ((degrees * M_PI) / 180);
}
