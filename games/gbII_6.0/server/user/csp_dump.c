/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, at al.
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
 * csp_dump.c -- Dump various information in CSP format.
 *
 * #ident  "@(#)csp_dump.c        1.2 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/csp_dump.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "csp_dump.h"

#include <ctype.h>
#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/csp.h"
#include "../server/csp_types.h"
#include "../server/files_shl.h"
#include "../server/first.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/log.h"
#include "../server/max.h"
#include "../server/misc.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "build.h"
#include "fire.h"
#include "rst.h"
#include "shootblast.h"

#define PLANET 1

extern char Shipltrs[];
extern reportdata *rd;

racetype *race;

void csp_ship_report(int, int, int, unsigned char[], int);

void CSP_sectors(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    int star;
    int i;
    int sectors = 0;
    int controlled = 0;

    for (star = 0; star < Sdata.numstars; ++star) {
        for (i = 0; i < Stars[star]->numplanets; ++i) {
            if ((planets[star][i]->type != TYPE_ASTEROID)
                && (planets[star][i]->info[playernum - 1].numsectsowned > ((planets[star][i]->Maxx * planets[star][i]->Maxy) / 2))) {
                /* Controlled planets count */
                ++controlled;
            }

            sectors += planets[star][i]->info[playernum - 1].numsectsowned;
        }
    }

    sprintf(buf, "Controlled Planets (>50%%): %d\n", controlled);
    notify(playernum, governor, buf);
    sprintf(buf, "Total occupied sectors: %d\n", sectors);
    notify(playernum, governor, buf);
}

void CSP_star_dump(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    placetype where;
    struct star *star;
    int i;

    if (argn < 2) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_FEW_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn > 3) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_MANY_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn == 2) {
        where = Getplace(playernum, governor, ":", 1);
    } else {
        where = Getplace(playernum, governor, args[2], 1);
    }

    if (where.err) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
        notify(playernum, governor, buf);

        return;
    }

    if (where.err) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
        notify(playernum, governor, buf);

        return;
    }

    if (!isset(Stars[where.snum]->explored, playernum)) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_STAR_UNEXPL, where.snum);
        notify(playernum, governor, buf);

        return;
    }

    star = Stars[where.snum];

    /* Intro */
    if (isset(star->inhabited, playernum)) {
        sprintf(buf,
                "%c %d %d %s %d %d %f %f\n",
                CSP_CLIENT,
                CSP_STARDUMP_INTRO,
                where.snum,
                star->name,
                1,
                1,
                star->xpos,
                star->ypos);
    } else {
        sprintf(buf,
                "%c %d %d %s %d %d %f %f\n",
                CSP_CLIENT,
                CSP_STARDUMP_INTRO,
                where.snum,
                star->name,
                1,
                0,
                star->xpos,
                star->ypos);
    }

    notify(playernum, governor, buf);

    /* Condition */
    sprintf(buf,
            "%c %d %d %d %d %d %f\n",
            CSP_CLIENT,
            CSP_STARDUMP_CONDITION,
            where.snum,
            star->stability,
            star->nova_stage,
            star->temperature,
            star->gravity);

    notify(playernum, governor, buf);

    /* Wormhole */
    sprintf(buf,
            "%c %d %d %d %d\n",
            CSP_CLIENT,
            CSP_STARDUMP_WORMHOLE,
            star->wh_has_wormhole,
            star->wh_dest_starnum,
            star->wh_stability);

    notify(playernum, governor, buf);

    /* Planets */
    for (i = 0; i < star->numplanets; ++i) {
        sprintf(buf,
                "%c %d %d %d %s\n",
                CSP_CLIENT,
                CSP_STARDUMP_PLANET,
                where.snum,
                i,
                star->pnames[i]);

        notify(playernum, governor, buf);
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_STARDUMP_END);
    notify(playernum, governor, buf);

    return;
}

void csp_planet_dump(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    planettype *p;
    placetype where;
    struct plinfo *pl;

    if (argn < 2) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_FEW_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn > 23) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_MANY_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn == 2) {
        /* Ignore explored (last arg) was set to 1 in JH, for both lines -mfw */
        where = Getplace(playernum, governor, ":", 0);
    } else {
        where = Getplace(playernum, governor, args[2], 0);
    }

    if (where.err) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
        notify(playernum, governor, buf);

        return;
    }

    if (where.level != LEVEL_PLAN) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
        notify(playernum, governor, buf);

        return;
    }

    if (!isset(Stars[where.snum]->explored, playernum)) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_STAR_UNEXPL, where.snum);
        notify(playernum, governor, buf);

        return;
    }

    getplanet(&p, (int)where.snum, (int)where.pnum);

    sprintf(buf,
            "%c %d %d %d %s %s\n",
            CSP_CLIENT,
            CSP_PLANDUMP_INTRO,
            where.snum,
            where.pnum,
            Stars[where.snum]->pnames[where.pnum],
            Stars[where.snum]->name);

    notify(playernum, governor, buf);
    pl = &p->info[playernum - 1];

    if (pl->explored) {
        /* Everything but routes... */
        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PLANDUMP_CONDITIONS,
                where.snum,
                where.pnum,
                p->conditions[RTEMP],
                p->conditions[TEMP],
                p->conditions[METHANE],
                p->conditions[OXYGEN],
                p->conditions[CO2],
                p->conditions[HYDROGEN],
                p->conditions[NITROGEN],
                p->conditions[SULFUR],
                p->conditions[HELIUM],
                p->conditions[OTHER],
                p->conditions[TOXIC],
                (int)compatibility(p, races[playernum - 1]));

        notify(playernum, governor, buf);

        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %ld %ld\n",
                CSP_CLIENT,
                CSP_PLANDUMP_STOCK,
                where.snum,
                where.pnum,
                pl->fuel,
                pl->destruct,
                pl->resource,
                pl->crystals,
                pl->tech_invest,
                pl->numsectsowned,
                pl->popn,
                pl->troops);

        notify(playernum, governor, buf);

        sprintf(buf,
                "%c %d %d %d %ld %d %d %d %d %f\n",
                CSP_CLIENT,
                CSP_PLANDUMP_PROD,
                where.snum,
                where.pnum,
                pl->prod_money,
                pl->prod_res,
                pl->prod_fuel,
                pl->prod_dest,
                pl->prod_crystals,
                pl->prod_tech);

        notify(playernum, governor, buf);

        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %d %ld %d %d %f %f %d %ld %ld %ld %ld %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PLANDUMP_MISC,
                where.snum,
                where.pnum,
                pl->comread,
                pl->mob_set,
                pl->tox_thresh,
                pl->autorep,
                pl->tax,
                pl->newtax,
                pl->guns,
                pl->mob_points,
                p->Maxx,
                p->Maxy,
                p->xpos,
                p->ypos,
                p->xtals_left,
                p->popn,
                p->maxpopn,
                p->troops,
                p->total_resources,
                p->slaved_to,
                p->type,
                p->expltimer,
                p->explored);

        notify(playernum, governor, buf);
    } else {
        sprintf(buf,
                "%c %d %d %d\n",
                CSP_CLIENT,
                CSP_PLANDUMP_NOEXPL,
                where.snum,
                where.pnum);

        notify(playernum, governor, buf);
    }

    /* Notify done */
    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_PLANDUMP_END);
    notify(playernum, governor, buf);
    free(p);
}

void CSP_ship_list(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    shiptype *s;
    int starnum;
    unsigned short shipno;
    int i;
    int n_ships;
    unsigned char Report_types[NUMSTYPES];

    if (argn < 2) {
        return;
    }

    for (i = 0; i < NUMSTYPES; ++i) {
        Report_types[i] = 1;
    }

    Num_ships = 0;
    n_ships = Numships();

    rd = (reportdata *)malloc(sizeof(reportdata) * (n_ships + (Sdata.numstars * MAXPLANETS)));

    if (!rd) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failed [csp_ship_dump]");
        close_data_files();

        exit(1);
    }

    /* One list entry for each ship, planet in universe */
    race = races[playernum - 1];

    /* Get ship here */
    sscanf(args[2], "%d", &starnum);
    star_getrships(playernum, governor, starnum);
    sprintf(buf, "| %d %d \n", CSP_SHIPLIST_INTRO, starnum);
    notify(playernum, governor, buf);

    for (i = 0; i < Num_ships; ++i) {
        /* Last ship gotten from disk */
        s = rd[i].s;
        shipno = rd[i].n;

        if ((shipno > 0) && (s->owner == playernum)) {
            sprintf(buf, "| %d %d \n", CSP_SHIPLIST_DATA, shipno);
            notify(playernum, governor, buf);
        }
    }

    sprintf(buf, "%c %d \n", CSP_CLIENT, CSP_SHIPLIST_END);
    notify(playernum, governor, buf);
}

void csp_ship_dump(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    int shipno;
    int shn;
    int i;
    int n_ships;
    int num;
    unsigned char Report_types[NUMSTYPES];
    int first_arg;
    int tact_mode;

    for (i = 0; i < NUMSTYPES; ++i) {
        Report_types[i] = 1;
    }

    Num_ships = 0;
    n_ships = Numships();

    rd = (reportdata *)malloc(sizeof(reportdata) * (n_ships + (Sdata.numstars * MAXPLANETS)));

    if (!rd) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failed [csp_ship_dump]");
        close_data_files();

        exit(1);
    }

    /* One list entry for each ship, planet in universe */
    race = races[playernum - 1];
    tact_mode = 1;
    first_arg = 2;

    if (argn >= 3) {
        if (*args[2] == 'N') {
            first_arg = 3;
            /* No tact data wanted */
            tact_mode = 0;
        }

        /* sprintf(buf, "tactmode=%d, f=%d\n", tact_mode, first_arg); */
        /* notify(playernum, governor, buf); */

        if ((*args[first_arg] == '#')
            || isdigit((unsigned char)*args[first_arg])) {
            /* Report on a couple ships */
            int l = first_arg;

            while ((l < MAXARGS) && (*args[l] != '\0')) {
                if (*args[l] == '#') {
                    sscanf(args[l] + 1, "%d", &shipno);
                } else {
                    sscanf(args[l], "%d", &shipno);
                }

                if ((shipno > n_ships) || (shipno < 1)) {
                    sprintf(buf, "rst: No such ship #%d\n", shipno);
                    notify(playernum, governor, buf);
                    free(rd);
                    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SHIPDUMP_END);
                    notify(playernum, governor, buf);

                    return;
                }

                Getrship(playernum, governor, shipno);
                num = Num_ships;

                if (rd[Num_ships - 1].s->whatorbits != LEVEL_UNIV) {
                    star_getrships(playernum,
                                   governor,
                                   (int)rd[num - 1].s->storbits);

                    csp_ship_report(playernum,
                                    governor,
                                    num - 1,
                                    Report_types,
                                    tact_mode);
                } else {
                    csp_ship_report(playernum,
                                    governor,
                                    num - 1,
                                    Report_types,
                                    tact_mode);
                }

                ++l;
            }

            Free_rlist();
            sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SHIPDUMP_END);
            notify(playernum, governor, buf);

            return;
        }
    }

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        notify(playernum,
               governor,
               "All ship dumps are not supported at universe level.\n");

        notify(playernum,
               governor,
               "You must specific specific ship numbers to do this.\n");

        free(rd); /* Nothing allocated */
        sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SHIPDUMP_END);
        notify(playernum, governor, buf);

        return;
    case LEVEL_STAR:
    case LEVEL_PLAN:
        star_getrships(playernum, governor, Dir[playernum - 1][governor].snum);

        for (i = 0; i < Num_ships; ++i) {
            csp_ship_report(playernum, governor, i, Report_types, tact_mode);
        }

        break;
    case LEVEL_SHIP:
        Getrship(playernum, governor, Dir[playernum - 1][governor].shipno);
        /* First ship report */
        csp_ship_report(playernum, governor, 0, Report_types, tact_mode);

        /* And then report on the ships it is carrying. */
        shn = rd[0].s->ships;
        Num_ships = 0;

        while (shn && Getrship(playernum, governor, shn)) {
            shn = nextship(rd[Num_ships - 1].s);
        }

        for (i = 0; i < Num_ships; ++i) {
            csp_ship_report(playernum, governor, i, Report_types, tact_mode);
        }

        break;
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SHIPDUMP_END);
    notify(playernum, governor, buf);
    Free_rlist();
}

void csp_ship_report(int playernum,
                     int governor,
                     int indx,
                     unsigned char rep_on[],
                     int tact_mode)
{
    shiptype *s;
    planettype *p;
    int shipno;
    int i;
    int sight;
    int caliber;
    placetype where;
    double Dist;
    int fev = 0;
    int fspeed = 0;
    int defense;
    int fdam = 0;
    double tech;
    char buf2[BUFSIZ];

    /* Last ship gotten from disk */
    s = rd[indx].s;
    p = rd[indx].p;
    shipno = rd[indx].n;

    /* Launched canister, non-owned ships don't show up */
    if (((rd[indx].type == PLANET) && p->info[playernum - 1].numsectsowned)
        || ((rd[indx].type != PLANET)
            && s->alive
            && (s->owner == playernum)
            && authorized(governor, s)
            && rep_on[s->type]
            && ((s->type != OTYPE_CANIST) || s->docked)
            && ((s->type != OTYPE_GREEN) || s->docked))) {
        if (rd[indx].type != PLANET) {
            if ((s->type == OTYPE_TERRA) || (s->type == OTYPE_PLOW)) {
                sprintf(buf,
                        "%c %d %d %d %d %d %d %d %ld %d \"Standard\" \"%s\"\n",
                        CSP_CLIENT,
                        CSP_SHIPDUMP_GEN,
                        shipno,
                        s->type,
                        s->active,
                        s->damage,
                        s->rad,
                        s->governor,
                        Shipdata[s->type][ABIL_HASSWITCH],
                        s->build_type,
                        s->name);
            } else {
                sprintf(buf,
                        "%c %d %d %d %d %d %d %d %ld %d \"%s\" \"%s\"\n",
                        CSP_CLIENT,
                        CSP_SHIPDUMP_GEN,
                        shipno,
                        s->type,
                        s->active,
                        s->damage,
                        s->rad,
                        s->governor,
                        Shipdata[s->type][ABIL_HASSWITCH],
                        s->build_type,
                        s->class,
                        s->name);
            }

            notify(playernum, governor, buf);

            sprintf(buf,
                    "%c %d %d %d %d %d %d %ld %d %ld %f %ld %d %d %d %d\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_STOCK,
                    shipno,
                    s->crystals,
                    s->hanger,
                    s->max_hanger,
                    s->resource,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_CARGO]
                    : s->max_resource,
                    s->destruct,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_DESTCAP]
                    : s->max_destruct,
                    s->fuel,
                    s->type == OTYPE_FACTORY
                    ? Shipdata[s->type][ABIL_FUELCAP]
                    : s->max_fuel,
                    s->popn,
                    s->troops,
                    s->max_crew,
                    s->cloak);

            notify(playernum, governor, buf);

            if (s->type == STYPE_POD) {
                sprintf(buf,
                        "%c %d %d %d %d %d %d %ld %f %ld %ld %f %d\n",
                        CSP_CLIENT,
                        CSP_SHIPDUMP_STATUS,
                        shipno,
                        s->hyper_drive.has,
                        s->mount,
                        s->mounted,
                        s->special.pod.temperature,
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
                        s->size);
            } else {
                sprintf(buf,
                        "%c %d %d %d %d %d 0 %ld %f %ld %ld %f %d\n",
                        CSP_CLIENT,
                        CSP_SHIPDUMP_STATUS,
                        shipno,
                        s->hyper_drive.has,
                        s->mount,
                        s->mounted,
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
                        s->size);
            }

            notify(playernum, governor, buf);

            sprintf(buf,
                    "%c %d %d %d %d %d %d %d %d %d %d\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_WEAPONS,
                    shipno,
                    s->laser,
                    s->cew,
                    s->cew_range,
                    (int)(((1.0 - (0.01 * s->damage)) * s->tech) / 4.0),
                    s->primary,
                    s->primtype,
                    s->secondary,
                    s->sectype);

            notify(playernum, governor, buf);

#ifdef AUTOSCRAP
            sprintf(buf,
                    "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_ORDERS,
                    shipno,
                    s->hyper_drive.on,
                    s->hyper_drive.ready,
                    s->hyper_drive.charge,
                    s->protect.self,
                    s->fire_laser,
                    s->focus,
                    s->retaliate,
                    s->protect.planet,
                    s->protect.on,
                    s->protect.ship,
                    s->merchant,
                    s->on,
                    s->autoscrap,
                    s->protect.evade,
                    s->bombard,
                    s->cloaked,
                    s->wants_reports);

#else
            sprintf(buf,
                    "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d 0 %d %d %d %d\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_ORDER,
                    shipno,
                    s->hyper_drive.on,
                    s->hyper_drive.ready,
                    s->hyper_drive.charge,
                    s->protect.self,
                    s->fire_laser,
                    s->focus,
                    s->retaliate,
                    s->protect.planet,
                    s->protect.on,
                    s->protect.ship,
                    s->merchant,
                    s->on,
                    s->protect.evade,
                    s->bombard,
                    s->cloaked,
                    s->wants_reports);
#endif

            notify(playernum, governor, buf);

#ifdef THRESHOLDING
            sprintf(buf, "%c %d %d ", CSP_CLIENT, CSP_SHIPDUMP_THRESH, shipno);

            for (i = 0; i <= TH_CRYSTALS; ++i) {
                sprintf(buf2, "%d %d ", i, s->threshold[i]);
                strcat(buf, buf2);
            }

            strcat(buf, "\n");
            notify(playernum, governor, buf);
#endif

            sprintf(buf,
                    "%c %d %d %d",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_SPECIAL,
                    shipno,
                    s->type);

            buf2[0] = '\0';

            switch (s->type) {
            case OTYPE_TERRA:
            case OTYPE_PLOW:
                strcpy(buf2, &(s->class[s->special.terraform.index]));
                i = strlen(buf2) - 1;

                if (buf2[i] == 'c') {
                    char c = s->class[s->special.terraform.index];
                    s->class[s->special.terraform.index] = '\0';
                    sprintf(buf2 + i, "%sc", s->class);
                    s->class[s->special.terraform.index] = c;
                }

                break;
            case STYPE_MISSILE:
                sprintf(buf2,
                        "%d %d %d %d",
                        s->whatdest,
                        s->special.impact.scatter,
                        s->special.impact.x,
                        s->special.impact.y);

                break;
            case STYPE_MINEF:
                sprintf(buf2, "%d", s->special.trigger.radius);

                break;
            case OTYPE_TRANSDEV:
                sprintf(buf2, "%d", s->special.transport.target);

                break;
            case STYPE_MIRROR:
                sprintf(buf2,
                        "%d %d %d %d %d",
                        s->special.aimed_at.level,
                        s->special.aimed_at.snum,
                        s->special.aimed_at.pnum,
                        s->special.aimed_at.shipno,
                        s->special.aimed_at.intensity);

                break;
            }

            strcat(buf, buf2);
            strcat(buf, "\n");
            notify(playernum, governor, buf);

            if (s->hyper_drive.on) {
                double dist;
                double fuse;

                dist = sqrt(Distsq(s->xpos, s->ypos, Stars[s->deststar]->xpos, Stars[s->deststar]->ypos));

                fuse = calc_fuse(s, dist);

                /*
                 * Something wrong? (kse)
                 *
                 * sprintf(buf,
                 *         "%c %d %f %f %f\n",
                 *         CSP_CLIENT,
                 *         CSP_SHIPDUMP_HYPER,
                 *         shipno,
                 *         dist,
                 *         fuse,
                 *         mfuse);
                 */

                sprintf(buf,
                        "%c %d %d %f %f\n",
                        CSP_CLIENT,
                        CSP_SHIPDUMP_HYPER,
                        shipno,
                        dist,
                        fuse);

                notify(playernum, governor, buf);
            }

            if (s->type == OTYPE_FACTORY) {
                if ((s->build_type != 0) && (s->build_type != OTYPE_FACTORY)) {
                    sprintf(buf,
                            "%c %d %d %d %f %f %d %d %d %d %d %d %d %d\n",
                            CSP_CLIENT,
                            CSP_SHIPDUMP_FACTORY,
                            shipno,
                            s->build_cost,
                            s->complexity,
                            s->base_mass,
                            ship_size(s),
                            s->armor,
                            s->max_speed,
                            s->max_crew,
                            s->max_fuel,
                            s->max_resource,
                            s->max_destruct,
                            s->max_hanger);

                    notify(playernum, governor, buf);
                }
            }

            sprintf(buf,
                    "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_DEST,
                    shipno,
                    s->docked,
                    s->land_x,
                    s->land_y,
                    s->navigate.on,
                    s->navigate.bearing,
                    s->navigate.turns,
                    s->whatdest,
                    s->deststar,
                    s->destpnum,
                    s->destshipno,
                    s->speed,
                    s->whatorbits,
                    s->storbits,
                    s->pnumorbits,
                    s->xpos,
                    s->ypos);

            notify(playernum, governor, buf);
        }

        /* Tactical information... */
        if (rd[indx].type == PLANET) {
            tech = race->tech;
            caliber = MEDIUM;

            sprintf(buf,
                    "%c %d %d %d %f %d %d %d\n",
                    CSP_CLIENT,
                    CSP_SHIPDUMP_PTACT_GEN,
                    rd[indx].star,
                    rd[indx].pnum,
                    tech,
                    p->info[playernum - 1].guns,
                    p->info[playernum - 1].destruct,
                    p->info[playernum - 1].fuel);

            notify(playernum, governor, buf);
        } else {
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
        }

        sight = 0;

        if (rd[indx].type == PLANET) {
            sight = 1;
        } else if ((s->type == OTYPE_PROBE) || s->popn) {
            sight = 1;
        }

        /* Tactical display */
        /* sprintf(buf, "tactmode=%d\n", tact_mode); */
        /* notify(playernum, governor, buf); */

        if (sight && tact_mode) {
            for (i = 0; i < Num_ships; ++i) {
                if (i != indx) {
                    Dist = sqrt(Distsq(rd[indx].x, rd[indx].y, rd[i].x, rd[i].y));

                    if (Dist < gun_range(race, rd[indx].s, (rd[indx].type == PLANET))) {
                        if (rd[i].type == PLANET) {
                            /* tact report at planet */
                            if (rd[indx].type == PLANET) {
                                sprintf(buf,
                                        "%c %d %d %d %d %d %f\n",
                                        CSP_CLIENT,
                                        CSP_SHIPDUMP_PTACT_PDIST,
                                        rd[indx].star,
                                        rd[indx].pnum,
                                        rd[i].star,
                                        rd[i].pnum,
                                        Dist);

                                notify(playernum, governor, buf);
                            } else {
                                sprintf(buf,
                                        "%c %d %d %d %d %f\n",
                                        CSP_CLIENT,
                                        CSP_SHIPDUMP_STACT_PDIST,
                                        shipno,
                                        rd[i].star,
                                        rd[i].pnum,
                                        Dist);

                                notify(playernum, governor, buf);
                            }
                        } else {
                            if (!see_cloaked(&rd[i], &rd[indx], Dist)) {
                                continue;
                            }

                            /* tac report at ship */
                            if (((rd[i].s->owner != playernum)
                                 || !authorized(governor, rd[i].s))
                                && rd[i].s->alive
                                && (rd[i].s->type != OTYPE_CANIST)
                                && (rd[i].s->type != OTYPE_GREEN)) {
                                int tev = 0;
                                int tspeed = 0;
                                int body = 0;
                                int prob = 0;
                                int factor = 0;

                                if (((rd[i].s->whatdest != LEVEL_UNIV)
                                     || rd[i].s->navigate.on)
                                    && !rd[i].s->docked
                                    && rd[i].s->active) {
                                    tspeed = rd[i].s->speed;
                                    tev = rd[i].s->protect.evade;
                                }

                                body = rd[i].s->size;
                                defense = getdefense(rd[i].s);
                                prob = hit_odds(Dist,
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

                                if (rd[indx].type == PLANET) {
                                    sprintf(buf,
                                            "%c %d %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d %d %d %d \"%s\" %f %f\n",
                                            CSP_CLIENT,
                                            CSP_SHIPDUMP_PTACT_INFO,
                                            rd[indx].star,
                                            rd[indx].pnum,
                                            caliber,
                                            rd[i].n,
                                            rd[i].s->owner,
                                            rd[i].s->governor,
                                            rd[i].s->type,
                                            rd[i].s->active,
                                            Dist,
                                            factor,
                                            body,
                                            tspeed,
                                            tev,
                                            prob,
                                            rd[i].s->damage,
                                            landed(rd[i].s),
                                            rd[i].s->land_x,
                                            rd[i].s->land_y,
                                            rd[i].s->name,
                                            rd[i].s->xpos,
                                            rd[i].s->ypos);
                                } else {
                                    sprintf(buf,
                                            "%c %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d %d %d %d \"%s\" %f %f\n",
                                            CSP_CLIENT,
                                            CSP_SHIPDUMP_STACT_INFO,
                                            shipno,
                                            caliber,
                                            rd[i].n,
                                            rd[i].s->owner,
                                            rd[i].s->governor,
                                            rd[i].s->type,
                                            rd[i].s->active,
                                            Dist,
                                            factor,
                                            body,
                                            tspeed,
                                            tev,
                                            prob,
                                            rd[i].s->damage,
                                            landed(rd[i].s),
                                            rd[i].s->land_x,
                                            rd[i].s->land_y,
                                            rd[i].s->name,
                                            rd[i].s->xpos,
                                            rd[i].s->ypos);
                                }

                                notify(playernum, governor, buf);
                            }
                        }
                    }
                }
            }
        }
    }
}

void csp_univ_dump(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    int i;
    double dist;
    double x;
    double y;
    int max;

    if (argn < 2) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_FEW_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn > 3) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_MANY_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn > 3) {
        max = atoi(args[3]);
    } else {
        max = 999999;
    }

    /* Intro */
    sprintf(buf,
            "%c %d %d %d %s\n",
            CSP_CLIENT,
            CSP_UNIVDUMP_INTRO,
            Sdata.numstars,
            UNIVSIZE,
            GAL_NAME);

    notify(playernum, governor, buf);
    x = Dir[playernum - 1][governor].lastx[1];
    y = Dir[playernum - 1][governor].lasty[1];

    /* Star */
    for (i = 0; i < Sdata.numstars; ++i) {
        dist = sqrt(Distsq(Stars[i]->xpos, Stars[i]->ypos, x, y));

        if ((int)dist <= max) {
            sprintf(buf,
                    "%c %d %d %s %.0f %.0f %.0f\n",
                    CSP_CLIENT,
                    CSP_UNIVDUMP_STAR,
                    i,
                    Stars[i]->name,
                    Stars[i]->xpos,
                    Stars[i]->ypos,
                    dist);

            notify(playernum, governor, buf);
        }
    }

    /* End */
    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_UNIVDUMP_END);
    notify(playernum, governor, buf);
}
