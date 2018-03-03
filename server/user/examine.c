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
 * examine -- Check out an object
 *
 * #ident  "@(#)examine.c       1.7 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/examine.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */

#include <stdlib.h>
#include <string.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

extern long Shipdata[NUMSTYPES][NUMABILS];
extern char const *Shipnames[];

void examine(int, int, int);

void examine(int playernum, int governor, int apcount)
{
    shiptype *ship;
    int t;
    int shipno;
    FILE *fd;
    char ch;

    if (argn < 2) {
        notify(playernum, governor, "Examine what?\n");

        return;
    }

    if (*args[1] == '#') {
        sscanf(args[1] + 1, "%d", &shipno);
    } else {
        sscanf(args[1], "%d", &shipno);
    }

    if (!getship(&ship, shipno)) {
        notify(playernum, governor, "No applicable ship.\n");

        return;
    }

    if (!ship->alive) {
        sprintf(buf, "That ship is dead.\n");
        notify(playernum, governor, buf);
        free(ship);

        return;
    }

    if ((ship->whatorbits == LEVEL_UNIV)
        || isclr(Stars[ship->storbits]->inhabited, playernum)) {
        sprintf(buf, "That ship is not visible to you.\n");
        notify(playernum, governor, buf);
        free(ship);

        return;
    }

    fd = fopen(EXAM_FL, "r");

    if (fd == NULL) {
        perror(EXAM_FL);
        free(ship);

        return;
    }

    /* Look through ship data file */
    for (t = 0; t <= ship->type; ++t) {
        ch = fgetc(fd);

        while (ch != '~') {
            ch = fgetc(fd);
        }
    }

    /* Look through ship data file */
    sprintf(buf, "\n");

    /* Give report */
    ch = fgetc(fd);

    while (ch != '~') {
        sprintf(temp, "%c", ch);
        strcat(buf, temp);

        ch = fgetc(fd);
    }

    notify(playernum, governor, buf);
    fclose(fd);

    if (!ship->examined) {
        if (ship->whatorbits == LEVEL_UNIV) {
            /* Deduct from sdata */
            deductAPs(playernum, governor, apcount, 0, 1);
        } else {
            deductAPs(playernum, governor, apcount, (int)ship->storbits, 0);
        }

        ship->examined = 1;
        putship(ship);
    }

    if (has_switch(ship)) {
        sprintf(buf,
                "This device has an on/off switch that can be set with order.\n");

        notify(playernum, governor, buf);
    }

    if (!ship->active) {
        sprintf(buf,
                "This device has been irradiated;\nit's crew is dying and it cannot move for the time being.\n");

        notify(playernum, governor, buf);
    }

    free(ship);
}
