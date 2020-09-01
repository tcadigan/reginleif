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
 * maxsupport() -- Return how many people one sector can support
 * compatibility() -- Return how much race is compatible with planet
 * gravity() -- Return gravity for planet
 * prin_ship_orbits() -- Prints place ship orbits
 *
 * #ident  "@(#)max.c   1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/max.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: max.c,v $ $Revision: 1.3 $";
 */
#include "max.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

char Dispshiporbits_buf[PLACENAMESIZE + 13];

int maxsupport(racetype *, sectortype *, double, int);
double compatibility(planettype *, racetype *);
double gravity(planettype *);
char *prin_ship_orbits(shiptype *);

int maxsupport(racetype *r, sectortype *s, double c, int toxic)
{
    int val;
    double a;
    double b;

    if (!r->likes[s->condition]) {
        return 0.0;
    }

    a = ((double)s->eff + 1.0) * (double)s->fert;
    b = 0.01 * c;
    val = (int)(a * b * 0.01 * (100.0 - (double)toxic));

    return val;
}

double compatibility(planettype *planet, racetype *race)
{
    int i;
    double add;
    double sum;
    double atmosphere = 1.0;

    /* Make an adjustment for planetary temperature */
    add = 0.1 * ((double)planet->conditions[TEMP] - (double)race->conditions[TEMP]);
    sum = 1.0 - ((double)abs(add) / 100.0);

    /* Step through and report compatibility of each planetary gas */
    for (i = TEMP + 1; i <= OTHER; ++i) {
        add = (double)planet->conditions[i] - (double)race->conditions[i];
        atmosphere *= (1.0 - ((double)abs(add) / 100.0));
    }

    sum *= atmosphere;
    sum *= (100.0 - planet->conditions[TOXIC]);

    if (sum < 0.0) {
        return 0.0;
    }

    return sum;
}

double gravity(planettype *p)
{
    return ((double)p->Maxx * (double)p->Maxy * GRAV_FACTOR);
}

char *prin_ship_orbits(shiptype *s)
{
    shiptype *mothership;
    char *motherorbits;

    switch (s->whatorbits) {
    case LEVEL_UNIV:
        sprintf(Dispshiporbits_buf, "/(%.0f,%.0f)", s->xpos, s->ypos);

        break;
    case LEVEL_STAR:
        sprintf(Dispshiporbits_buf, "/%s", Stars[s->storbits]->name);

        break;
    case LEVEL_PLAN:
        sprintf(Dispshiporbits_buf,
                "/%s/%s",
                Stars[s->storbits]->name,
                Stars[s->storbits]->pnames[s->pnumorbits]);

        break;
    case LEVEL_SHIP:
        if (getship(&mothership, s->destshipno)) {
            motherorbits = prin_ship_orbits(mothership);
            strcpy(Dispshiporbits_buf, motherorbits);
            free(mothership);
        } else {
            strcpy(Dispshiporbits_buf, "/");
        }

        break;
    default:

        break;
    }

    return Dispshiporbits_buf;
}
