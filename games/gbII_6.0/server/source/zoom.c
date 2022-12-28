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
 * The GNU General Public License is contained int the file LICENSE.
 * -----------------------------------------------------------------------------
 * *****************************************************************************
 *
 * zoom.c
 *
 * Created:
 * Author: ???
 *
 * Version: 1.9 17:35:12
 *
 * Contains:
 *   zoom()
 *
 * #ident  "@(#)zoom.c  1.13 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/zoom.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 * *****************************************************************************
 */
#include "zoom.h"

#include <stdlib.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "GB_server.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

/*
 * GB_schedule:
 *
 * arguments:
 *   playernum  Playernum who called it
 *   governor   Governor who called it
 *
 * called by: process_command
 */
void zoom(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    double num;
    double denom;
    int i;

    if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
        i = 1;
    } else {
        i = 0;
    }

    if (argn > 1) {
        if (sscanf(args[1], "%lf/%lf", &num, &denom) == 2) {
            /* num/denom format */
            if (denom == 0.0) {
                sprintf(buf, "Illegal denominator value.\n");
                notify(playernum, governor, buf);
            } else {
                /* if (denom) */
                Dir[playernum -1][governor].zoom[i] = num / denom;
            }
        } else {
            /* One number */
            Dir[playernum - 1][governor].zoom[i] = num;
        }
    }

    sprintf(buf,
            "Zoom value %G, lastx = %G, lasty = %G.\n",
            Dir[playernum - 1][governor].zoom[i],
            Dir[playernum - 1][governor].lastx[i],
            Dir[playernum - 1][governor].lasty[i]);

    notify(playernum, governor, buf);
}

void csp_zoom(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    double num;
    double denom;
    int i;
    int ns;

    if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
        i = 1;
    } else {
        i = 0;
    }

    if (argn > 2) {
        i = atoi(args[2]);

        if (i) {
            i = 1;
        }
    }

    if (argn > 3) {
        ns = sscanf(args[3], "%lf,%lf", &num, &denom);

        if (ns == 2) {
            /* num/denom format */
            if (denom == 0.0) {
                sprintf(buf, "Illegal denominator value.\n");
                notify(playernum, governor, buf);
            } else {
                /* if (denom) */
                Dir[playernum - 1][governor].zoom[i] = num / denom;
            }
        } else if (ns == 1) {
            /* One number */
            Dir[playernum - 1][governor].zoom[i] = num;
        } else {
            sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR);
            notify(playernum, governor, buf);
        }
    }

    sprintf(buf,
            "%c %d %d %f\n",
            CSP_CLIENT,
            CSP_ZOOM,
            i,
            Dir[playernum -1][governor].zoom[i]);

    notify(playernum, governor, buf);
}
