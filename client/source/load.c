/*
 * load.c: Loading a file
 *         Loading of init file (.gbrc)
 *         Saving of init file (.gbrc)
 *         Logging files
 *         Shell escaping (old version)
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "load.h"

#include "gb.h"
#include "key.h"
#include "option.h"
#include "str.h"
#include "types.h"
#include "util.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define GBRC_SAVE_LINE "# Put your stuff below here -- Keep this line here\n"

extern int end_msg;
extern int hide_msg;
extern int kill_socket_output;

void expand_file(char *filename);
void load_init_file(FILE *fd);
void load_predefined(char *fname);
void log_file(char *args);

/*
 * loadf: Has several functions.
 * No arguments: It prompts for loading of the defined GBRC file.
 * Arguments trip certain output flags and load the file and take
 * appropriate action base on the flags.
 * If no flags are given, but the file is recognized as a GBRC file
 * it will load the file through the init file function
 */
void cmd_loadf(char *args)
{
#ifndef RESTRICTED_ACCESS

    int type = SCREEN_ONLY;
    int show = 1;
    int hidden = 0;
    char *c;
    char *p = args;
    char buf[NORMSIZ];
    char buf2[MAXSIZ];
    FILE *fd;
    extern char gbrc_path[];

    /* No args...ask to loadf GBRC */
    if (*args == '\0') {
        expand_file(gbrc_path);
        sprintf(buf, "Really load %s (y/n)? ", gbrc_path);
        promptfor(buf, buf2, PROMPT_STRING);

        if ((*buf2 == 'Y') || (*buf2 == 'y')) {
            load_predefined(gbrc_path);
        } else {
            msg("-- Ussage: loadf [-a|b|c|d|e|h|q|s|t|D] [-p<hrase>] filename");
            msg("           loadf without any arguments will prompt you to load your specified GBRC.");
            msg("           Your GBRC file is: \'%s\'", gbrc_path);
        }

        return;
    } else if (*args == '-') {
        c = args + 1;
        args = rest(args);

        while (*c != ' ') {
            switch (*c++) {
            case 'a':
                type = NORM_ANNOUNCE;

                break;
            case 'b':
                type = NORM_BROADCAST;

                break;
            case 'e':
                type = GB_EMOTE;

                break;
            case 't':
                type = GB_THINK;

                break;
            case 'c':
                type = ENCRYPTED;
                p = first(c);

                while (*c != ' ') {
                    ++c;
                }

                break;
            case 'h':
                hidden = 1;

                break;
            case 'p':
                type = USER_DEFINED;
                p = first(c);

                while (*c != ' ') {
                    ++c;
                }

                break;
            case 's':
            case 'd':
                type = SERVER;

                break;
            case 'D':
                type = DO_SCREEN_ONLY;

                break;
            case 'x':
            case 'q':
                show = 0;

                break;
            default:
                msg("-- Usage: loadf [-a|b|c|d|h|q|s|t] [-p<hrase>] filename");

                return;
            }
        }
    }

    fd = fopen(args, "r");

    if (fd == NULL) {
        msg("-- Error. Could not open %s for reading.", args);

        return;
    }

    while (fgets(buf, MAXSIZ, fd)) {
        if ((*buf == ';') || (*buf == '#')) {
            if ((type == SCREEN_ONLY)
                && !strcmp(buf, "# Galactic Bloodshed Client II Initialization File\n")) {
                msg("-- loadf Loading GB II Init File: \'%s\'", args);
                load_init_file(fd);

                return;
            }

            continue;
        }

        c = strchr(buf, '\n');

        if (c) {
            *c = '\0';
        }

        switch (type) {
        case NORM_ANNOUNCE:
            sprintf(buf2, "announce %s", buf);

            break;
        case NORM_BROADCAST:
            sprintf(buf2, "broadcast %s", buf);

            break;
        case GB_EMOTE:
            sprintf(buf2, "emote %s", buf);

            break;
        case GB_THINK:
            sprintf(buf2, "think %s", buf);

            break;
        case SCREEN_ONLY:
        case DO_SCREEN_ONLY:
        case SERVER:
            strcpy(buf2, buf);

            break;
        case ENCRYPTED:
            sprintf(buf2, "cr %s %s", p, buf);

            break;
        case USER_DEFINED:
            sprintf(buf2, "%s %s", p, buf);

            break;
        default:
            msg("-- error in loadf of load.c");

            break;
        }

        if ((type != SCREEN_ONLY) || (type != DO_SCREEN_ONLY)) {
            ++end_msg;
            add_queue(buf2, 0);

            if (hidden) {
                ++hide_msg;
                kill_socket_output = true;
            }
        }

        if (show) {
            strcpy(buf, "echo ");
            strcat(buf, buf2);
            add_queue(buf, 0);
        }
    }

    if ((type != SCREEN_ONLY) || (type != DO_SCREEN_ONLY)) {
        --end_msg;
    }

    if (hidden) {
        --hide_msg;
    }

    fclose(fd);

#else

    msg("-- Restricted Access: loading files not available.");
#endif
}

/*
 * This is OLD and here only to facilitate my development of the
 * client when I need a secondary way to test proc commands.
 */
void shell_out(char *args)
{
#ifndef RESTRICTED_ACCESS
    char buf[MAXSIZ];
    char fname[NORMSIZ];
    int flag = 0;

    strcpy(fname, "~/.gbtemp");
    expand_file(fname);

    if (*args == '-') {
        sprintf(buf, "%s > %s", rest(args), fname);
        system(buf);

        if (*(args + 1) == 'a') {
            sprintf(buf, "-a %s", fname);
            cmd_loadf(buf);
            ++flag;
        } else if (*(args + 1) == 'b') {
            sprintf(buf, "-b %s", fname);
            cmd_loadf(buf);
            ++flag;
        }
    } else {
        sprintf(buf, "%s > %s", args, fname);
        system(buf);
        cmd_loadf(fname);
        ++flag;
    }

    sprintf(buf, "rm -f %s", fname);
    system(buf);

    if (!flag) {
        msg("-- shell done.");
    }

#else

    msg("-- Restricted Access: shell escapes not available.");
#endif
}

/* Loads the file .gbrc in users $HOME if present and executes line by line. */
void load_predefined(char *fname)
{
    FILE *fd;
    char buf[NORMSIZ];

    strcpy(buf, fname);
    expand_file(buf);
    fd = fopen(buf, "r");

    if (fd == NULL) {
        return;
    }

    load_init_file(fd);
}

void load_init_file(FILE *fd)
{
    char buf[NORMSIZ];
    char *p;

    while (fgets(buf, NORMSIZ, fd)) {
        p = strchr(buf, '\n');

        if (!p) {
            continue;
        }

        *p = '\0';

        if (*buf == '#') {
            continue;
        }

        process_key(buf, false);
    }

    fclose(fd);
}

void cmd_source(char *args)
{
    char buf[MAXSIZ];

    sprintf(buf, "-d %s", args);
    cmd_loadf(buf);
}

void cmd_oldshell(char *args)
{
#ifndef RESTRICTED_ACCESS
    shell_out(args);

#else

    msg("-- Restricted Access: shell escape not available.");
#endif
}

void cmd_log(char *args)
{
#ifndef RESTRICTED_ACCESS
    log_file(args);

#else

    msg("-- Restricted Access: logging not available.");
#endif
}

/* If no filename is given, then gb.log in $HOME is used. */
void log_file(char *args)
{
#ifdef RESTRICTED_ACCESS
    msg("-- Restricted Access: Logging not available.");

    return;

#else
    long clk;
    char mode[SMABUF];
    struct stat statbuf;

    strcpy(mode, "a+");

    /* Turn off logging, if appropriate */
    if (!strcmp(args, "off")
        || (*args == '\0')
        || !strcmp(args, "off no msg")) {
        if (logfile.on) {
            fclose(logfile.fd);
        }

        logfile.on = false;
        logfile.redirect = false;
        logfile.level = LOG_OFF;

        /* noclobber sets name to null is error */
        if (*logfile.name == '\0') {
            if (strcmp(args, "off not msg")) {
                msg("-- Logging turned off.");
            }
        } else {
            msg("-- Log file %s closed.", logfile.name);
        }

        *logfile.name = '\0';

        return;
    }

    logfile.level = LOG_ALL;
    debug(1, "log str: %s", args);

    while (*args == '-') {
        ++args;
        debug(1, "log hyphen: %s", args);

        while (!isspace(*args)) {
            debug(1, "log !space: %c", *args);

            switch (*args++) {
                case 'a':
                    strcpy(mode, "a+");

                    break;
            case 'c':
                logfile.level = LOG_COMMUNICATION;

                break;
            case 'w':
                strcpy(mode, "w");

                break;
            default: /* Error to be here */

                break;
            }
        }

        while (isspace(*args)) {
            ++args;
        }
    }

    /* Else we have a new log file about to be opened, so close the old one */
    if (logfile.on && *logfile.name) {
        msg("-- Log file %s closed.", logfile.name);
        fclose(logfile.fd);
    }

    /* If filename is on, use ~/gb.log */
    if (!strcmp(args, "on")) {
        strcpy(args, "~/gb.log");
        expand_file(args);
    } else {
        expand_file(args);
    }

    strcpy(logfile.name, args);

    if ((options[NOCLOBBER / 32] & ((NOCLOBBER < 32) ?
                                    (1 << NOCLOBBER)
                                    : (1 << (NOCLOBBER % 32))))
        && (*mode == 'w')
        && (stat(logfile.name, &statbuf) == 0)) {
        logfile.on = false;
        msg("-- Log: noclobber is set and file \'%s\' exists. Not writing.", logfile.name);
        *logfile.name = '\0';

        return;
    }

    logfile.fd = fopen(logfile.name, mode);

    if (logfile.fd == NULL) {
        msg("-- Log: Could not open \'%s\' for writing (%s)", logfile.name, mode);
        logfile.on = false;

        return;
    }

    clk = time(0);
    fprintf(logfile.fd, "GBII Log File: %s\n", ctime(&clk));
    msg("-- Log file %s opened.", logfile.name);
    logfile.on = true;
#endif
}

/*
 * If ~/ is the first prat of the file string, it expands to the
 * user's $HOME. Otherwise it is left alone.
 */
void expand_file(char *fname)
{
    char *env;
    char temp[NORMSIZ];

    if (fname[0] != '~') {
        return;
    }

    if (fname[1] == '/') {
        env = getenv("HOME");

        if (env == NULL) {
            temp[0] = '\0';
        } else {
            strcpy(temp, env);
            strcat(temp, fname + 1);
            strcpy(fname, temp);
        }
    }
}
