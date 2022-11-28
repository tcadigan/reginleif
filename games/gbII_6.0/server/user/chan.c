/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
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
 * chan.c
 *
 * Created: Wed Jan 27 20:56:42 EST 1993
 * Author:  John Paul Deragon
 *
 * Version: 1.5 23:05:36
 *
 * Contains:
 *
 * #ident  "@(#)chan.c  1.7 12/3/93 "
 * $Header: /var/cvs/gbp/GB+/user/chan.c,v 1.4 2007/07/-6 18:09:34 gbp Exp $
 * *****************************************************************************
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffers.h"
#include "config.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

extern void channel(int, int, int, int);

void channel(int playernum, int governor, int ap, int startup)
{
    int want;

    if (startup) {
        if (isclr(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL1)
            && isclr(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL2)
            && isclr(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL3)) {
            setbit(races[playernum - 1]->governor[governor].channel,
                   COMM_RCV_CHANNEL1);

            setbit(races[playernum - 1]->governor[governor].channel,
                   COMM_XMIT_CHANNEL1);
        }

        return;
    }

#ifndef MULTIPLE_COMM_CHANNELS
    notify(playernum, governor, "Command disabled.\n");

    return;
#endif

    if (argn >= 2) {
        if (argn == 2) {
            want = atoi(args[1]);

            if ((want < 1) || (want > 3)) {
                notify(playernum, governor, "Invalid channel.\n");

                return;
            }

            if (isset(races[playernum - 1]->governor[governor].channel, want)) {
                clrbit(races[playernum - 1]->governor[governor].channel, want);

                clrbit(races[playernum - 1]->governor[governor].channel,
                       want + COMM_CHANNEL_MASK);
            } else {
                setbit(races[playernum - 1]->governor[governor].channel, want);
            }

            if (isset(races[playernum - 1]->governor[governor].channel, want + COMM_CHANNEL_MASK)) {
                clrbit(races[playernum - 1]->governor[governor].channel,
                       want + COMM_CHANNEL_MASK);

                setbit(races[playernum -1]->governor[governor].channel,
                       COMM_XMIT_CHANNEL1);

                notify(playernum, governor, "Setting xmit comm channel to 1\n");
            }
        } else if (argn == 3) {
            want = atoi(args[1]);

            if ((want < 1) || (want > 3)) {
                notify(playernum, governor, "Invalid channel.\n");

                return;
            }

            if (match(args[2], "xmit")) {
                clrbit(races[playernum - 1]->governor[governor].channel,
                       COMM_XMIT_CHANNEL1);

                clrbit(races[playernum - 1]->governor[governor].channel,
                       COMM_XMIT_CHANNEL2);

                clrbit(races[playernum - 1]->governor[governor].channel,
                       COMM_XMIT_CHANNEL3);

                if (isclr(races[playernum - 1]->governor[governor].channel, want)) {
                    setbit(races[playernum - 1]->governor[governor].channel,
                           want);

                    setbit(races[playernum - 1]->governor[governor].channel,
                           want + COMM_CHANNEL_MASK);
                } else {
                    setbit(races[playernum - 1]->governor[governor].channel,
                           want + COMM_CHANNEL_MASK);
                }
            }
        } else {
            notify(playernum, governor, "Syntax: channel <channel> xmit\n");

            return;
        }
    }

    /* Make sure we have an xmit channel set */
    if (isclr(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL1)
        && isclr(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL2)
        && isclr(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL3)) {
        /* No xmit channel set, pick one */
        if (isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL!)) {
            setbit(races[playernum - 1]->governor[governor].channel,
                   COMM_XMIT_CHANNEL1);

            notify(playernum, governor, "Xmit channel set to 1.\n");
        } else if (isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL2)) {
            setbit(races[playernum - 1]->governor[governor].channel,
                   COMM_XMIT_CHANNEL2);

            notify(playernum, governor, "Xmit channel set to 2.\n");
        } else if (isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL3)) {
            setbit(races[playernum - 1]->governor[governor].channel,
                   COMM_XMIT_CHANNEL3);

            notify(playernum, governor, "Xmit channel set to 3.\n");
        }
    }

    notify(playernum, governor, "Channel Status:\n");

    sprintf(buf,
            "    One: %s %s\n",
            isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL1) ? "RCV" : "OFF",
            isset(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL1) ? "XMIT" : "   ");

    notify(playernum, governor, buf);

    sprintf(buf,
            "   Two: %s %s\n",
            isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL2) ? "RCV" : "OFF",
            isset(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL2) ? "XMIT" : "   ");

    notify(playernum, governor, buf);

    sprintf(buf,
            "   Three: %s %s\n",
            isset(races[playernum - 1]->governor[governor].channel, COMM_RCV_CHANNEL3) ? "RCV" : "OFF",
            isset(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL2) ? "XMIT" : "   ");

    notify(playernum, governor, buf);
}
