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
 * *****************************************************************************
 *
 * togg.c
 *
 * Created: ???
 * Author: Robert Chansky
 *
 * Version 1.6 23:05:33
 *
 * Contains: toggle()
 *           highlight()
 *           tog()
 *
 * #ident  "@(#)togg.c  1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/togg.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 *
 * *****************************************************************************
 */
#include "togg.h"

#include <string.h>

#include "../server/buffers.h"
#include "../server/csp_dispatch.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

/* Prototypes */
void tog(int, int, char *, char const *);

/*
 * toggle:
 *
 * arguments:
 *   playernum
 *   governor
 *
 * called by:
 *   process_commands
 *
 * description: Called from process_commands, allows you to toggle some
 *              options. Currently inverse, double_digits, gag, autoload,
 *              autopurge, color and monitor
 */
void toggle(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    racetype *race;

    race = races[playernum - 1];

    if (argn > 1) {
        if (match(args[1], "inverse")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.inverse,
                "inverse");
        } else if (match(args[1], "double_digits")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.double_digits,
                "double_digits");
        } else if (match(args[1], "geography")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.geography,
                "geography");
        } else if (match(args[1], "gag")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.gag,
                "gag");
        } else if (match(args[1], "autoload")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.autoload,
                "autoload");
        } // else if (match(args[1], "autopurge")) {
        //   tog(playernum,
        //       governor,
        //       &race->governor[governor].toggle.autopurge,
        //       "autopurge");
        // }
        else if (match(args[1], "color")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.color,
                "color");
        } else if (match(args[1], "visible")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.invisible,
                "invisible");
        } else if (race->God && match(args[1], "monitor")) {
            tog(playernum, governor, &race->monitor, "monitor");
        } else if (match(args[1], "compatibility")) {
            tog(playernum,
                governor,
                &race->governor[governor].toggle.compat,
                "compatibility");
        } else if (match(args[1], "client")) {
            CSP_client_toggle(playernum, governor, 0, -1, NULL);
            notify(playernum, governor, "client toggled\n");
        } else {
            sprintf(buf, "No such option \'%s\'\n", args[1]);
            notify(playernum, governor, buf);

            return;
        }

        putrace(race);
    } else {
        sprintf(buf,
                "gag is %s\n",
                race->governor[governor].toggle.gag ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "inverse is %s\n",
                race->governor[governor].toggle.inverse ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "double_digits is %s\n",
                race->governor[governor].toggle.double_digits ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "geography is %s\n",
                race->governor[governor].toggle.geography ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "autoload is %s\n",
                race->governor[governor].toggle.autoload ? "ON": "OFF");

        notify(playernum, governor, buf);

        /*
         * sprintf(buf,
         *         autopurge is %s\n",
         *         race->governor[governor].toggle.autopurge ? "ON" : "OFF");
         *
         * notify(playernum, governor, buf);
         */

        sprintf(buf,
                "color is %s\n",
                race->governor[governor].toggle.color ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "compatibility is %s\n",
                race->governor[governor].toggle.compat ? "ON" : "OFF");

        notify(playernum, governor, buf);

        sprintf(buf,
                "%s\n",
                race->governor[governor].toggle.invisible ? "INVISIBLE" : "VISIBLE");

        notify(playernum, governor, buf);

        sprintf(buf,
                "highlight player %d\n",
                race->governor[governor].toggle.highlight);

        notify(playernum, governor, buf);

        sprintf(buf,
                "client mode is %s\n",
                race->governor[governor].CSP_client_info.csp_user ? "ON" : "OFF");

        notify(playernum, governor, buf);

        if (race->God) {
            sprintf(buf, "monitor is %s\n", race->monitor ? "ON" : "OFF");
            notify(playernum, governor, buf);
        }
    }
}

/*
 * highlight:
 *
 * arguments:
 *   playernum
 *   governor
 *
 * description: Called process commands.
 */

void highlight(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    int n;
    racetype *race;

    n = GetPlayer(args[1]);

    if (!n) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    race = races[playernum - 1];
    race->governor[governor].toggle.highlight = n;
    putrace(race);
}

/*
 * tog:
 *
 * arguments:
 *   playernum
 *   governor
 *   op         What we are toggling name
 *
 * called by: toggle (local to this file)
 *
 * description: this function does the actual toggle of the bits
 */
void tog(int playernum, int governor, char *op, char const *name)
{
    *op = !*op;
    sprintf(buf, "%s is not %s\n", name, *op ? "on" : "off");
    notify(playernum, governor, buf);
}
