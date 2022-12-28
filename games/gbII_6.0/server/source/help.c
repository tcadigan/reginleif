/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_Copyright.h for additional authors and details.
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
 * Galactic Bloodshed Help File Indexer
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *  help() called from GB_server.c
 *  ancillary functions
 *
 * These functions display files indexed with the makeindex command.
 *
 * #ident  "@(#)help.c  1.13 12/3/93 "
 *
 * *****************************************************************************
 */
#include "help.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "buffers.h"
#include "GB_server.h"
#include "idx.h"
#include "log.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

/*
 * help:
 *
 * Arguments: Who pointer to the descriptor data
 *
 * Called by: main
 *
 * Description: Called from the process_commands loop.
 */
void help(int who)
{
    char indx[80];
    char text[80];
    int idx_found;
    int len;
    int i;
    int mode;
    int counter;
    idx_mark entry;
    FILE *tfile;
    FILE *ifile;
    char line[LINE_SIZE + 1];
    char head[LINE_SIZE + 1];
    char padding[LINE_SIZE + 1];
    char title[80];
    char *topic;
    char *subject;
    char *indxfile;
    char *textfile;

    /* Determine mode by number of arguments */
    if (argn == 3) {
        /* Specific topic */
        mode = 2;
    } else if (argn == 2) {
        /* List of topics */
        mode = 1;
    } else {
        /* General help */
        mode = 0;
    }

    switch (mode) {
    case 0:
        subject = args[0];
        topic = args[0];

        break;
    case 1:
        subject = args[1];
        topic = args[1];

        break;
    case 2:
        subject = args[1];
        topic = args[2];

        break;
    }

    help_dir(subject);

    if (strlen(subject) <= 0) {
        sprintf(buf, "No such subject.\n");
        outstr(who, buf);

        return;
    }

    sprintf(indx, "%s/%s.idx", HELPDIR, subject);
    sprintf(text, "%s/%s.txt", HELPDIR, subject);

    indxfile = indx;
    textfile = text;

    ifile = fopen(indxfile, "r");

    if (ifile == NULL) {
        tfile = fopen(textfile, "r");

        if (tfile == NULL) {
            if (errno == ENOENT) {
                /* No such file or directory. No such text bundle. */
                sprintf(buf, "No such file or directory: %s\n", textfile);
                outstr(who, buf);

                return;
            } else {
                loginfo(ERRORLOG, NOERRNO, "%s: errno %d", textfile, errno);
                sprintf(buf, "%s: error number %d\n", textfile, errno);
                outstr(who, buf);
                perror(textfile);

                return;
            }
        } else {
            loginfo(ERRORLOG, NOERRNO, "%s: errno %d", indxfile, errno);
            sprintf(buf, "%s isn't indexed.\n", indxfile);
            outstr(who, buf);
            perror(indxfile);
            fclose(tfile);

            return;
        }
    }

    idx_found = fread(&entry, sizeof(idx_mark), 1, ifile);

    while (idx_found == 1) {
        if (string_prefix(entry.topic, topic)) {
            /* Search for single topic, found it */
            break;
        }

        idx_found = fread(&entry, sizeof(idx_mark), 1, ifile);
    }

    if (idx_found <= 0) {
        sprintf(buf, "No '%s' in %s.\n", topic, subject);
        outstr(who, buf);

        return;
    }

    tfile = fopen(textfile, "r");

    if (tfile == NULL) {
        sprintf(buf, "%s: Sorry, temporarily not available.\n", subject);
        outstr(who, buf);
        loginfo(ERRORLOG, NOERRNO, "Can't open %s for reading", textfile);

        return;
    }

    if (fseek(tfile, entry.pos, 0) < 0L) {
        sprintf(buf, "%s: Sorry, temporarily not available.\n", subject);
        outstr(who, buf);
        loginfo(ERRORLOG, NOERRNO, "Seek error in file %s", textfile);
        fclose(tfile);

        return;
    }

    switch (mode) {
    case 0:
        strcpy(title, "HELP");

        break;
    case 1:
        sprintf(title, "%s", subject);

        break;
    case 2:
        sprintf(title, "%s:%s", subject, entry.topic);

        break;
    }

    to_upper(title);
    len = (50 - (strlen(title) * 2)) / 2;

    for (i = 0; i < len; ++i) {
        padding[i] = ' ';
    }

    padding[len] = '\0';

    sprintf(head,
            "__{{%s}}%sGalactic Bloodshed Help%s{{%s}}__\n",
            title,
            padding,
            padding,
            title);

    outstr(who, head);

    while (1) {
        if (fgets(line, LINE_SIZE, tfile) == NULL) {
            break;
        }

        if (line[0] == '&') {
            break;
        }

        /*
         * for (p = line; *p != '\0'; ++p) {
         *     if (*p == '\n') {
         *         *p = '\0';
         *     }
         * }
         */

        outstr(who, line);
    }

    if (mode == 1) {
        strcpy(buf, "  ");
        counter = 0;
        outstr(who, "{{TOPICS}}\n\n");

        idx_found = fread(&entry, sizeof(idx_mark), 1, ifile);

        while (idx_found == 1) {
            /* List entries */
            sprintf(temp, "%-14s ", entry.topic);
            strcat(buf, temp);
            ++counter;

            if (counter == 5) {
                strcat(buf, "\n");
                outstr(who, buf);
                strcpy(buf, "  ");
                counter = 0;
            }

            idx_found = fread(&entry, sizeof(idx_mark), 1, ifile);
        }

        /* Print the leftovers */
        if (counter > 0) {
            strcat(buf, "\n");
            outstr(who, buf);
        }

        outstr(who, "\n");
    }

    outstr(who, "-----\nFinished.\n");

    fclose(ifile);
    fclose(tfile);
}

int string_prefix(char *string, char *prefix)
{
    while (*string && *prefix && (to_lower(*string) == to_lower(*prefix))) {
        ++string;
        ++prefix;
    }

    return (*prefix == '\0');
}

char to_lower(int x)
{
    if ((x < 'A') || (x > 'Z')) {
        return x;
    }

    return ('a' + (x - 'A'));
}

void to_upper(char *str)
{
    int i;
    char *x;

    x = str;

    for (i = 0; i < strlen(str); ++i) {
        if ((*x >= 'a') && (*x <= 'z')) {
            *x = 'A' + (*x - 'a');
        }

        ++x;
    }
}

void help_dir(char *subject)
{
    DIR *dirp;
    struct dirent *dp;
    char *pos;

    dirp = opendir(HELPDIR);

    if (dirp == NULL) {
        loginfo(ERRORLOG,
                NOERRNO,
                "help_dir(): Error opening directory %s",
                HELPDIR);

        strcpy(subject, "");

        return;
    }

    errno = 0;
    dp = readdir(dirp);

    if (dp != NULL) {
        if (strncmp(dp->d_name, subject, strlen(subject)) == 0) {
            strcpy(subject, dp->d_name);
            pos = strchr(subject, '.');
            *pos = '\0';
            closedir(dirp);

            return;
        }
    }

    while (dp != NULL) {
        errno = 0;

        dp = readdir(dirp);

        if (dp != NULL) {
            if (strncmp(dp->d_name, subject, strlen(subject)) != 0) {
                continue;
            }

            strcpy(subject, dp->d_name);
            pos = strchr(subject, '.');
            *pos = '\0';
            closedir(dirp);

            return;
        }
    }

    if (errno != 0) {
        loginfo(ERRORLOG,
                NOERRNO,
                "help_dir(): Error reading directory %s",
                HELPDIR);
    }

    strcpy(subject, "");
    closedir(dirp);
}
