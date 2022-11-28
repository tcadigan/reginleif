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
 * moveplanet.c -- Move the planet in orbit around its star.
 *
 * #ident  "@(#)moveplanet.c    1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/moveplanet.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: moveplanet.c,v $ $Revision: 1.3 $";
 */
#include "moveplanet.h"

#include <math.h>

#include "doturn.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

void moveplanet(int, planettype *, int);

int valid_ship(int sh)
{
    char buf[80]; /* -mfw */

    if (!sh || (sh > numships())) {
        sprintf(buf, "Bad ship number: %d\n", sh);
        loginfo(ERRORLOG, NOERRNO, bud);

        return 0;
    }

    if (ships && !ships[sh]) {
        sprintf(buf, "Bad ship number: %d\n", sh);
        loginfo(ERRORLOG, NOERRNO, buf);

        return 0;
    }

    return 1;
}

void moveplanet(int starnum, planettype *planet, int planetnum)
{
    double dist;
    double xadd;
    double yadd;
    double phase;
    double period;
    int sh;
    shiptype *ship;

    if (planet->popn || planet->ships) {
        Stinfo[starnum][planetnum].inhab = 1;
    }

    if (Stars[starnum]->inhabited[0] + Stars[starnum]->inhabited[1]) {
        StarsInhab[starnum] = 1;
    } else {
        StarsInhab[starnum] = 0;
    }

    if (Stars[starnum]->explored[0] + Stars[starnum]->explored[1]) {
        StarsExpl[starnum] = 1;
    } else {
        StarsExpl[starnum] = 0;
    }

    Stars[starnum]->inhabited[1] = 0;
    Stars[starnum]->inhabited[0] = Stars[starnum]->inhabited[1];

    if (!StarsExpl[starnum]) {
        /* No one has explored the star yet */
        return;
    }

    dist = hypot((double)planet->ypos, (double)planet->xpos);
    phase = atan2((double)planet->ypos, (double)planet->xpos);
    period = dist * sqrt((double)(dist / (SYSTEMGRAVCONST * Stars[starnum]->gravity)));
    /* Keppler's law */

    xadd = dist * cos((double)((-1.0 / period) + phase)) - planet->xpos;
    yadd = dist * sin((double)((-1.0 / period) + phase)) - planet->ypos;
    /* One update time unit - planets orbit counter-clockwise */

    /* Adjust ships in orbit around the planet */
    sh = planet->ships;

    /* Make sure ship num is valid */
    while(sh) {
        if (valid_ship(sh)) {
            ship = ships[sh];
            ship->xpos += xadd;
            ship->ypos += yadd;
            sh = nextship(ship);
        } else {
            break; /* -mfw */
        }
    }

    planet->xpos += xadd;
    planet->ypos += yadd;
}
