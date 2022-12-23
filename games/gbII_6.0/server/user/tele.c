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
 * *****************************************************************************
 *
 * tele.c
 *
 * Created: ??
 * Author: Robert Chansky
 *
 * Version: 1.10 09:06:00
 *
 * Contains: purge()
 *           post()
 *           push_telegram_race()
 *           push_telegram()
 *           teleg_read()
 *           news_read()
 *
 * #ident  "@(#)tele.c  1.13 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/tele.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 *
 * *****************************************************************************
 */
#include "tele.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>

#include "../server/buffers.h"
#include "../server/files.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/log.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ships.h"
#include "../server/vars.h"

static time_t tm;
static FILE *teleg_read_fd;
static char telegram_file[PATHLEN];
static struct stat telestat;
static struct tm *current_tm; /* For watching for next update */

/*
 * purge:
 *
 * arguments: none
 *
 * called by: process_commands
 *
 * description: Used to purge the News files.
 */
void purge(int playernum, int governor)
{
    fclose(fopen(DECLARATIONFL, "w+"));
    newslength[0] = 0;
    fclose(fopen(COMBATFL, "w+"));
    newslength[1] = 0;
    fclose(fopen(ANNOUNCEFL, "w+"));
    newslength[2] = 0;
    fclose(fopen(TRANSFERFL, "w+"));
    newslength[3] = 0;

    notify(playernum, governor, "News files purged.\n");
}

/*
 * post:
 *
 * arguments:
 *   msg   The actual message type
 *   type  Type of message. Valid types are DECLARATION, TRANSFER, COMBAT and
 *         ANNOUNCE.
 *
 * called by:
 *   fire
 *   name
 *   declare
 *   dock
 *   land
 *   dissolve
 *   doship
 *   doturn
 *
 * description: Does the actual posting of message to the news files
 */
void post(char const *msg, int type)
{
    char telefl[100];
    char pbuf[1024]; /* This is needed, don't use global pointer! */
    FILE *news_fd;
    char *p;

    memset((char *)telefl, 0, sizeof(telefl));

    switch (type) {
    case DECLARATION:
        sprintf(telefl, "%s", DECLARATIONFL);

        break;
    case TRANSFER:
        sprintf(telefl, "%s", TRANSFERFL);

        break;
    case COMBAT:
        sprintf(telefl, "%s", COMBATFL);

        break;
    case ANNOUNCE:
        sprintf(telefl, "%s", ANNOUNCEFL);

        break;
    default:

        return;
    }

    /* Look for special symbols */
    for (p = (char *)msg; *p; ++p) {
        if (*p == ';') {
            *p = '\n';
        } else if (*p == '|') {
            *p = '\t';
        }
    }

    news_fd = fopen(telefl, "a");

    if (news_fd != NULL) {
        tm = time(0);
        current_tm = localtime(&tm);
        sprintf(pbuf,
                "%2d/%2d %02d:%02d:%02d %s",
                current_tm->tm_mon + 1,
                current_tm->tm_mday,
                current_tm->tm_hour,
                current_tm->tm_min,
                current_tm->tm_sec,
                msg);

        fprintf(news_fd, "%s", pbuf);
        fclose(news_fd);
        newslength[type] += strlen(pbuf);
    }
}

/*
 * push_telegram:
 *
 * arguments:
 *   recipient
 *   gov
 *   msg
 *
 * called by:
 *
 * description: Sends a message to everyone from person to person
 */
void push_telegram(int recipient, int gov, char const *msg)
{
    char telefl[100];
    FILE *telegram_fd;

    memset((char *)telefl, 0, sizeof(telefl));
    sprintf(telefl, "%s.%d.%d", TELEGRAMFL, recipient, gov);

    telegram_fd = fopen(telefl, "a");

    if (telegram_fd == NULL) {
        telegram_fd = fopen(telefl, "w+");

        if (telegram_fd == NULL) {
            perror("teleg_send");

            return;
        }
    }

    tm = time(0);
    current_tm = localtime(&tm);

    fprintf(telegram_fd,
            "%2d/%2d %02d:%02d:%02d %s\n",
            current_tm->tm_mon + 1,
            current_tm->tm_mday,
            current_tm->tm_hour,
            current_tm->tm_min,
            current_tm->tm_sec,
            msg);

    fclose(telegram_fd);
}

/*
 * push_telegram_race:
 *
 * arguments:
 *   recipient
 *   msg
 *
 * called by:
 *
 * description: Sends a message to everyone in the race
 */
void push_telegram_race(int recipient, char const *msg)
{
    racetype *race;
    int j;

    race = races[recipient - 1];

    for (j = 0; j <= MAXGOVERNORS; ++j) {
        if (race->governor[j].active) {
            push_telegram(recipient, j, msg);
        }
    }
}

/*
 * read_teleg.c -- (try to) read telegrams. The first byte in each telegram is
 *                 the sending player #, or 254 to denote autoreport. Then the
 *                 time sent, then the message itself, terminated by
 *                 TELEG_DELIM.
 */

/*
 * teleg_read:
 *
 * arguments:
 *   playernum
 *   governor
 *
 * called by:
 *   process_commands
 *
 * description: Read the telegrams for the player. The first byte in each
 *              telegram is the sending player number or 22544 to denote an
 *              autoreport. then the time send, then the message, then
 *              terminated by TELEG_DELIM
 */
void teleg_read(int playernum, int governor)
{
    char *p;

    memset((char *)telegram_file, 0, sizeof(telegram_file));
    sprintf(telegram_file, "%s.%d.%d", TELEGRAMFL, playernum, governor);

    teleg_read_fd = fopen(telegram_file, "r");

    if (teleg_read_fd != 0) {
        notify(playernum, governor, "Telegrams:");
        stat(telegram_file, &telestat);

        if (telestat.st_size > 0) {
            notify(playernum, governor, "\n");

            while (fgets(buf, sizeof(buf), teleg_read_fd)) {
                for (p = buf; *p; ++p) {
                    if (*p == '\n') {
                        *p = '\0';

                        break;
                    }
                }

                strcat(buf, "\n");
                notify(playernum, governor, buf);
            }
        } else {
            notify(playernum, governor, " None.\n");
        }

        fclose(teleg_read_fd);

        /*
         * if (races[playernum - 1]->governor[governor].toggle.autopurge) {
         *     purge_telegrams(playernum, governor);
         * }
         */
        purge_telegrams(playernum, governor);
    } else {
        sprintf(buf, "\nTelegram file %s non-existent.\n", telegram_file);
        notify(playernum, governor, buf);

        loginfo(ERRORLOG,
                NOERRNO,
                "Telegram file for %s non-existent",
                telegram_file);

        return;
    }
}

/*
 * news_read:
 *
 * arguments
 *   playernum
 *   governor
 *   type
 *
 * description: Read the news file
 */
void news_read(int playernum, int governor, int type)
{
    char *p;
    racetype *race;

    memset((char *)telegram_file, 0, sizeof(telegram_file));

    switch (type) {
    case DECLARATION:
        sprintf(telegram_file, "%s", DECLARATIONFL);

        break;
    case TRANSFER:
        sprintf(telegram_file, "%s", TRANSFERFL);

        break;
    case COMBAT:
        sprintf(telegram_file, "%s", COMBATFL);

        break;
    case ANNOUNCE:
        sprintf(telegram_file, "%s", ANNOUNCEFL);

        break;
    default:

        return;
    }

    teleg_read_fd = fopen(telegram_file, "r");

    if (teleg_read_fd != 0) {
        race = races[playernum - 1];

        if (race->governor[governor].newspos[type] > newslength[type]) {
            race->governor[governor].newspos[type] = 0;
        }

        fseek(teleg_read_fd, race->governor[governor].newspos[type], 0);

        while (fgets(buf, sizeof(buf), teleg_read_fd)) {
            for (p = buf; *p; ++p) {
                if (*p == '\n') {
                    *p = '\0';

                    break;
                }
            }

            strcat(buf, "\n");
            notify(playernum, governor, buf);
        }

        fclose(teleg_read_fd);
        race->governor[governor].newspos[type] = newslength[type];
        putrace(race);
    } else {
        sprintf(buf, "\nNews file %s non-existent.\n", telegram_file);
        notify(playernum, governor, buf);

        loginfo(ERRORLOG,
                NOERRNO,
                "Telegram file for %s non-existent",
                telegram_file);

        return;
    }
}

void purge_telegrams(int playernum, int governor)
{
    memset((char *)telegram_file, 0, sizeof(telegram_file));
    sprintf(telegram_file, "%s.%d.%d", TELEGRAMFL, playernum, governor);
    teleg_read_fd = fopen(telegram_file, "w+"); /* Trunc file */
    fclose(teleg_read_fd);
}
