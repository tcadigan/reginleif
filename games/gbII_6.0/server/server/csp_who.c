/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_Copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin StreeT, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * $Header: /var/cvs/gbp/GB+/server/csp_who.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: csp_who.c,v $ $Revision: 1.3 $";
 */
#include "csp_who.h"

#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "buffers.h"
#include "config.h"
#include "csp.h"
#include "csp_types.h"
#include "debug.h"
#include "GB_server.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

void csp_who(int playernum, int governor)
{
    time_t now;
    racetype *r;
    int god = 0;
    int coward_count = 0;
    int whoami;
    int i;

    time(&now);
    sprintf(buf, "%c %d %s", CSP_CLIENT, CSP_WHO_INTRO, ctime(&now));
    notify(playernum, governor, buf);

    debug(LEVEL_CSP, "CSP: Entering CSP_who\n");
    debug(LEVEL_CSP_DETAILED, "CSP: CSP_WHO, output on %d\n", playernum);

    whoami = playernum;
    r = races[whoami - 1];
    god = r->God;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if (des[i].Playernum) {
            r = races[des[i].Playernum - 1];

            if (!r->governor[des[i].Governor].toggle.invisible
                || (playernum == des[i].Playernum)
                || god) {
                if (!strlen(r->governor[des[i].Governor].name)) {
                    sprintf(temp, "\"Unknown\"");
                } else {
                    sprintf(temp, "\"%s\"", r->governor[des[i].Governor].name);
                }

                char *star_name;
                int gag;
                int invisible;

                if (god) {
                    star_name = Stars[Dir[des[i].Playernum - 1][des[i].Governor].snum]->name;
                } else {
                    star_name = "????";
                }

                if (r->governor[des[i].Governor].toggle.gag) {
                    gag = TRUE;
                } else {
                    gag = FALSE;
                }

                if (r->governor[des[i].Governor].toggle.invisible) {
                    invisible = TRUE;
                } else {
                    invisible = FALSE;
                }

                sprintf(buf,
                        "%c %d %s %s %ld %ld %ld %s %d %d\n",
                        CSP_CLIENT,
                        CSP_WHO_DATA,
                        r->name,
                        temp,
                        des[i].Playernum,
                        des[i].Governor,
                        now - actives[((des[i].Playernum - 1) * 5) + des[i].Governor + 1].idle_time,
                        star_name,
                        gag,
                        invisible);

                notify(playernum, governor, buf);
            } else if (!god) {
                /* Deity lurks around */
                ++coward_count;
            }
        }
    }

#ifdef SHOW_COWARDS
    sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_WHO_COWARDS, coward_count);

#else

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_WHO_END);
#endif

    notify(playernum, governor, buf);

    debug(LEVEL_CSP, "CSP: Leaving CSP_WHO\n");
}
