/*
 * status.c: Handles building up the status bar format
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1991
 *
 * See the COPYRIGHT file.
 */
#include "status.h"

#include "gb.h"
#include "key.h"
#include "option.h"
#include "str.h"
#include "term.h"
#include "types.h"

#include <pwd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern int gb;
extern char pbuf[];
extern char last_prompt[];
extern long map_time;
extern char post_buf[];

/* Set it way high so new mail notification does not occur at login */
int mcnt = 9999999;
char last_update_buf[NORMSIZ];

void update_status(void)
{
    long present;

    present = time(0);

    if ((status.last_time + 60) > present) {
        return;
    }

    force_update_status();
}

void force_update_status(void)
{
    char *getbuf;
    char *build_status(void);

    if (detached || (client_stats == L_NOTHING)) {
        return;
    }

    if (gb < 0) {
        strcpy(last_prompt, "Not Connected");
    }

    getbuf = build_status();
    strcpy(status.current_buf, getbuf);

    if (strcmp(status.last_buf, status.current_buf)) {
        put_status();
    }
}

void put_status(void)
{
    term_move_cursor(0, num_rows - 2);
    write_string(status.current_buf, num_columns);
    term_normal_mode();
    strcpy(status.last_buf, status.current_buf);
    cursor_to_window();
}

int check_mail(void)
{
    int cnt;
    int oldmcnt = mcnt;
    FILE *fmail;
    char mpath[NORMSIZ];
    static char *uname = NULL;
    struct passwd *pwname;

#ifdef MAILPATH
    /*
     * uname is static so this check is only made once instead of
     * everytime through here
     */
    if (!uname) {
        uname = getlogin();

        if (uname == NULL) {
            pwname = getpwuid(getuid());
            uname = pwname->pw_name;
        }
    }

    sprintf(mpath, "%s/%s", MAILPATH, uname);
    fmail = fopen(mpath, "r");

    if (fmail == NULL) {
        return -2;
    }

    cnt = 0;

    while (fgets(pbuf, NORMSIZ, fmail)) {
        if (strncmp(pbuf, MAIL_DELIMITER, MAIL_DELIMITER_LEN) == 0) {
            ++cnt;
        }
    }

    fclose(fmail);

    if ((cnt > oldmcnt) && (client_stats > L_NOTHING)) {
        msg("-- You have new mail.");
    }

    return cnt;

#else

    return -1;
#endif
}

char *print_time(long ptime)
{
    static char temp[6];
    struct tm *present_time;

    if (ptime == -1) {
        return "--:--";
    }

    present_time = localtime(&ptime);
    sprintf(temp, "%2d:%2d", present_time->tm_hour, present_time->tm_min);

    if (present_time->tm_hour < 10) {
        temp[0] = '0';
    }

    if (present_time->tm_min < 10) {
        temp[3] = '0';
    }

    temp[5] = '\0';

    return temp;
}

char *build_status(void)
{
    static char new_status_buf[NORMSIZ];
    char end_buf[NORMSIZ];
    char *p = status.format;
    char *q = new_status_buf;
    long present;
    char temp[SMABUF];
    int len;
    char *r;

    present = time(0);
    status.last_time = present;
    *end_buf = '\0';

    while (*p) {
        if (*p == '$') {
            switch (*++p) {
            case 'B':
                strcpy(q, print_time(boot_time));
                q += 5; /* HH:MM is 5 */

                break;
            case 'C':
                strcpy(q, print_time(connect_time));
                q += 5; /* HH:MM is 5 */

                break;
            case 'E':
                sprintf(temp,
                        "(%c)",
                        (input_mode.edit == EDIT_OVERWRITE ? 'O' : 'I'));

                strcpy(q, temp);
                q += strlen(temp);

                break;
            case 'H':
                if (*cur_game.game.host) {
                    strcpy(q, cur_game.game.host);
                    q += strlen(cur_game.game.host);
                }

                break;
            case 'I':
                sprintf(temp, "%d", profile.raceid);
                strcpy(q, temp);
                q += strlen(temp);

                break;
            case 'M':
                if (input_mode.map) {
                    sprintf(temp,
                            "(%c)",
                            (input_mode.say ? 'S' : 'M'));

                    strcpy(q, temp);
                    q += strlen(temp);
                }

                break;
            case 'N':
                strcpy(q, profile.racename);
                q += strlen(profile.racename);

                break;
            case 'P':
                if (input_mode.post) {
                    sprintf(temp, "(%ld)", MAX_POST_LEN - strlen(post_buf) - 1);
                    strcpy(q, temp);
                    q += strlen(temp);
                }

                break;
            case 'R':
                *q = '\0';
                q = end_buf;
                ++p;
                continue;

                break;
            case 'S':
                strcpy(q, last_prompt);

                if (options[BRACKETS / 32] & ((BRACKETS < 32) ?
                                              (1 << BRACKETS)
                                              : (1 << (BRACKETS % 32)))) {
                    r = strchr(q, '[');
                    *r = '(';
                    r = strchr(q, ']');
                    *r = ')';
                }

                q += strlen(last_prompt);

                break;
            case 'T':
                if (options[SHOW_CLOCK / 32] & ((SHOW_CLOCK < 32) ?
                                                (1 << SHOW_CLOCK)
                                                : (1 << (SHOW_CLOCK % 32)))) {
                    strcpy(q, print_time(present));
                    q += 5;
                }

                break;
            case 'c':
                *q++ = *status.schar;

                break;
            case 'g':
                if (cur_game.game.nick && *cur_game.game.nick) {
                    strcpy(q, cur_game.game.nick);
                    q += strlen(cur_game.game.nick);
                }

                break;
            case 'i':
                sprintf(temp, "%d", profile.govid);
                strcpy(q, temp);
                q += strlen(temp);

                break;
            case 'm':
                if (options[SHOW_MAIL / 32] & ((SHOW_MAIL < 32) ?
                                               (1 << SHOW_MAIL)
                                               : (1 << (SHOW_MAIL % 32)))) {
                    mcnt = check_mail();

                    if (mcnt > 0) {
                        sprintf(temp, "Mail: %d", mcnt);
                        strcpy(q, temp);
                        q += strlen(temp);
                    }
                }

                break;
            case 'n':
                strcpy(q, profile.govname);
                q += strlen(profile.govname);

                break;
            case 't':
                if (in_talk_mode()) {
                    strcpy(q, "(T)");
                    q += 3;
                }

                break;
            default:
                *q++ = '$';
                *q++ = *p;

                break;
            }

            ++p;
        }
        else {
            *q++ = *p++;
        }
    }

    *q = '\0';
    len = num_columns - strlen(new_status_buf) - strlen(end_buf);

    while (len > 0) {
        strcat(new_status_buf, status.schar);
        --len;
    }

    strcat(new_status_buf, end_buf);
    new_status_buf[num_columns] = '\0';

    return new_status_buf;
}

char *build_scope_prompt(void)
{
    static char sbuf[MAXSIZ];
    char shipbuf[NORMSIZ];
    char temp[SMABUF];
    int i;
    Ship *s;

    *shipbuf = '\0';

    /* Old type scope */
    if (scope.numships == -1) {
        scope.level = CSPD_PLAN;
        sprintf(shipbuf, "/../#%s", scope.shipc);
    } else if (scope.numships == -2) {
        scope.level = CSPD_PLAN;
        sprintf(shipbuf, "/#%s", scope.shipc);
    } else if (scope.numships) {
        for (s = scope.motherlist, i = 1; i < scope.numships; s = s->next, ++i) {
            sprintf(temp, "/#%d", s->shipno);
            strcat(shipbuf, temp);
        }

        sprintf(temp, "/#%d", scope.ship);
        strcat(shipbuf, temp);
    }

    switch (scope.level) {
    case CSPD_UNIV:
        sprintf(sbuf, "( [%d] /%s )", scope.aps, shipbuf);

        break;
    case CSPD_STAR:
        sprintf(sbuf, "( [%d] /%-s%s )", scope.aps, scope.star, shipbuf);

        break;
    case CSPD_PLAN:
        sprintf(sbuf, "( [%d] /%-s/%-s%s )", scope.aps, scope.star, scope.planet, shipbuf);

        break;
    default:

        break;
    }

    return sbuf;
}
