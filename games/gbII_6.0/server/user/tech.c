/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * foundation; either version 2 of the License, or (at your option) any later
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
 * *****************************************************************************
 *
 * tech.c
 *
 * Created: ??
 * Author: ??
 * Version: @)$_tech.c  1.7 2/22/93
 *
 * #ident  "@(#)tech.c  1.9 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/tech.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 *
 * *****************************************************************************
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

/* Prototypes */
double tech_prod(int, int);

void technology(int playernum, int governor, int apcount)
{
    short int invest;
    planettype *p;

    /* CWL */
    shiptype *seti;
    int sh;
    int setihere;
    float seticrew;
    float setimaxcrew;
    float setidamage;
    float setimult;
    int got;
    float seti_fact = 0.0;
    float nseti_fact = 0.0;
    /* End CWL */

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf,
                "scope must be a planet (%d).\n",
                Dir[playernum - 1][governor].level);

        notify(playernum, governor, buf);

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    /* CWL code to support setis */
    sh = p->ships;
    setidamage = 0;
    seticrew = setidamage;
    setimaxcrew = seticrew;
    setihere = setimaxcrew;
    got = 1;

    while (sh && got) {
        got = getship(&seti, sh);

        if (got) {
            if ((seti->type == OTYPE_SETI)
                && seti->on
                && seti->alive
                && (seti->owner == playernum)) {
                ++setihere;

                if (seti->on) {
                    nseti_fact = ((float)seti->popn / (float)set->max_crew) * ((float)(100 - set->damage) / 100.0);
                } else {
                    nseti_fact = 0;
                }

                set_fact += nseti_fact;
            }

            sh = nextship(seti);
            free(seti);
        }
    }

    setimult = 1.0 + (log10(1.0 + seti_fact) / 5);
    /* End CWL */

    if (argn < 2) {
        float tech_display; /* CWL */

        tech_display = tech_prod((int)p->info[playernum - 1].tech_invest,
                                 (int)p->info[playernum - 1].popn);

        sprintf(buf,
                "Current investment : %d    Technology production/update: %.3f\n",
                p->info[playernum - 1].tech_invest,
                tech_display);

        notify(playernum, governor, buf);

        /* CWL */
        if (setihere) {
            tech_display *= setimult;
            sprintf(buf, "Active science research centers: %d.\n", setihere);
            notify(playernum, governor, buf);

            sprintf(buf,
                    "Scientific research center factor: %2.4f.\nTotal tech production: %2.4f.\n",
                    setimult,
                    tech_display);

            notify(playernum, governor, buf);
        }

        free(p);

        return;
    }

    invest = atoi(args[1]);

    if (invest < 0) {
        sprintf(buf, "Illegal value.\n");
        notify(playernum, governor, buf);
        free(p);

        return;
    }

    p->info[playernum - 1].tech_invest = invest;

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    sprintf(buf,
            "   New (ideal) tech production: %.3f (this planet)\n",
            tech_prod((int)p->info[playernum - 1].tech_invest, (int)p->info[playernum - 1].popn));

    notify(playernum, governor, buf);

    free(p);
}

void techlevel(int playernum, int governor, int apcount)
{
    racetype *r;

    r = races[playernum - 1];

    sprintf(buf, "You have discovered the following technologies:");

    if (Hyper_drive(r)) {
        strcat(buf,
               "\n  Ability to install hyper drive engines on most ships (HYPERSPACE), ");
    }

    if (Crystal(r)) {
        strcat(buf,
               "\n  Ability to identify crystal sectors and mine them. Also can\nnow build things which require this, like laser and hyperdrive, ");
    }

    if (Atmos(r)) {
        strcat(buf,
               "\n  Ability to use atsmopheric process features (ATMOS), ");
    }

    if (Laser(r)) {
        strcat(buf, "\n  Ability to install laser weapons on ships (LASER), ");
    }

    if (Wormhole(r)) {
        strcat(buf, "\n  Ability to identify Worm Holes (WORMHOLE), ");
    }

#ifdef USE_VN
    if (Vn(r)) {
        strcat(buf, "\n  Von Neumann Machines (VN), ");
    }
#endif

    if (Cew(r)) {
        strcat(buf, "\n  Confined Energy Weapons (CEW), ");
    }

    if (Cloak(r)) {
        strcat(buf, "\n  Ability to cloak warships (CLOAK), ");
    }

    if (Avpm(r)) {
        strcat(buf, "\n  Ability to transport between star systems (AVPM), ");
    }

    if (Tractor_beam(r)) {
        strcat(buf, "\n  TRACTOR_BEAM (not implemented), ");
    }

    if (Transporter(r)) {
        strcat(buf, "\n  TRANSPORTER (not implemented), ");
    }

    notify(playernum, governor, buf);

    sprintf(buf,
            "\n\nList of available technologies and level of tech needed:\n");

    notify(playernum, governor, buf);
    sprintf(buf, "HYPER_DRIVE    %f\n", TECH_HYPER_DRIVE);
    notify(playernum, governor, buf);
    sprintf(buf, "CRYSTAL        %f\n", TECH_CRYSTAL);
    notify(playernum, governor, buf);
    sprintf(buf, "ATMOS          %f\n", TECH_ATMOS);
    notify(playernum, governor, buf);
    sprintf(buf, "LASER          %f\n", TECH_LASER);
    notify(playernum, governor, buf);
    sprintf(buf, "WORMHOLE       %f\n", TECH_WORMHOLE);
    notify(playernum, governor, buf);

#ifdef USE_VN
    sprintf(buf, "VN_MACHINES    %f\n", TECH_VN);
    notify(playernum, governor, buf);
#endif

    sprintf(buf, "CEW            %f\n", TECH_CEW);
    notify(playernum, governor, buf);
    sprintf(buf, "CLOAK          %f\n", TECH_CLOAK);
    notify(playernum, governor, buf);
    sprintf(buf, "AVPM           %f\n", TECH_AVPM);
    notify(playernum, governor, buf);
    sprintf(buf, "TRACTOR_BEAM   %f\n", TECH_TRACTOR_BEAM);
    notify(playernum, governor, buf);
    sprintf(buf, "TRANSPORTER    %f\n", TECH_TRANSPORTER);
    notify(playernum, governor, buf);
}

double tech_prod(int investment, int popn)
{
    double scale;

    scale = (double)popn / 10000.0;

    return (TECH_INVEST * log10(((double)investment * scale) + 1.0));
}
