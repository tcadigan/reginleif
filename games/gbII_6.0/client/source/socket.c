/*
 * socket.c: Handles the socket code including reading from socket.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "socket.h"

#include "action.h"
#include "args.h"
#include "buffer.h"
#include "command.h"
#include "crypt.h"
#include "csp.h"
#include "gb.h"
#include "icomm.h"
#include "imap.h"
#include "key.h"
#include "map.h"
#include "md5.h"
#include "proc.h"
#include "option.h"
#include "status.h"
#include "str.h"
#include "types.h"
#include "util.h"

#ifdef XMAP
#include "xmap.h"
#endif

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ENDHELP "Help on that subject unavailable"
#define ENDDOINGSEGMENT "DOING MOVEMENT..."
#define ENDDOINGUPDATE "DOING UPDATE..."
#define ENDLOGIN "Please enter your password"
#define ENDFINISH "Finished."
#define GBMFINISH "Segment finished"
#define GBUFINISH "Update %d finished"
#define HAPMFINISH "Movement finished."
#define HAPUFINISH "Update finished."
#define ENDREFUSED "Connection refused."
#define RACEGEN "Galactic Bloodshed Race Generator"
#define NO_LOGOUT_DELAY 600 /* 10 minutes */

extern int gb;
extern char pbuf[];
extern int cursor_display;
extern int do_queue; /* For queue */
extern int exit_now;
extern int kill_client_output;
extern int queue_sending;

BufferInfo gbsobuf; /* Socket buffer list ptrs */
char builtship[6] = "0"; /* Contains # of last ship built */
char lotnum[6] = "0"; /* Last lot # you put up for sale */
int command_has_output = false;
int end_msg = 0;
int hide_msg = false; /* Turn off kill_socket_output */
int kill_socket_output = false;
int notify = -2;
int password_invalid = false; /* Password prompt flag */
int server_help = false; /* Doing a server help */
long last_no_logout_time; /* For no_logout */
char last_prompt[MAXSIZ]; /* Holds last prompt found */

char *build_scope(void);
void (*check4_endprompt)(char *);
void connect_prompts(char *s);
void null_func(char *);
void oldcheck4_endprompts();
void check_for_special_formatting(char *s, int type);
void check_for_timecodes(char *s);
void close_gb(void);
int have_socket_output(void);
void loggedin(void);
int on_endprompt(int eprompt);
void parse_socket_output(char *s);
void process_gb(char *s);
void send_gb(char *s, int len);
int sendgb(char *buf, int len);
void set_no_logout(void);
void process_socket(char *s);
void socket_final_process(char *s, int type);
void chap_response(char *line);
void chap_abort(void);

/* Opens the socket and connects to specified host and port */
int connectgb(char *gbhost, char *charport, char *s)
{
    struct sockaddr_in sin;
    struct hostent *hp;
    int gbport;
    int fd;

    gbport = atoi(charport);
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_port = htons(gbport);

    if (isdigit(*gbhost)) {
        sin.sin_addr.s_addr = inet_addr(gbhost);

        if (sin.sin_addr.s_addr == (in_addr_t)-1) {
            msg("-- %s Unknown IP host.", s);

            return -1;
        }

        sin.sin_family = AF_INET;
    } else {
        hp = gethostbyname(gbhost);

        if (hp == 0) {
            msg("-- %s Unknown host(gethost) \'%s\'", s, gbhost);

            return -1;
        }

        memmove((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
        sin.sin_family = hp->h_addrtype;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        msg_error("-- %s", s);

        return -1;
    }

    if (connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        msg_error("-- %s", s);
        close(fd);

        return -1;
    }

    return fd;
}

/* Returns true if socket is active */
int is_connected(void)
{
    if (gb >= 0) {
        return true;
    }

    return false;
}

/* Handles the reading from the buffer and the processing of socket output */
void get_socket(void)
{
    char *gbbuf;

    gbbuf = remove_buffer(&gbsobuf);

    if (gbsobuf.is_partial) {
        if (racegen) {
            /* Fancy prompt stuff */
            update_input_prompt(gbbuf);
            refresh_input('\0');
            strfree(gbbuf);
            gbsobuf.is_partial = false;

            return;
        }

        gbsobuf.is_partial = false;
    }

    process_gb(gbbuf);
    strfree(gbbuf);
}

/* Take the given string and process it as socket output */
void process_gb(char *s)
{
    if (options[RAWMODE / 8] & (1 << (RAWMODE % 8))) {
        display_msg(s); /* No more needed here */

        return;
    }

    end_prompt = NOT_PROMPT;
    check4_endprompt(s); /* Dynamic call here */

    if (on_endprompt(end_prompt)) {
        return;
    }

    /* If line has not been cancelled -mfw */
    if (*s != 24) {
        switch (*s) {
        case '#':
            plot_orbit(s);

            break;
        case '$':
            plot_surface(s);

            break;
        case CSP_CLIENT:
            process_special(s + 2);

            if (wait_csp.have) {
                return;
            }

            break;
#ifdef TEST
        case 255:
            msg("TELNET - %s", s + 1);

            break;
#endif
        default:
            /* So CSP can catch end prompts this has to be here */
            if (icomm.num) {
                icomm_handling_command(s);

                if (icomm.list[0].ignore) {
                    return;
                }
            }

            process_socket(s);

            break;
        }
    }
}

void process_socket(char *s)
{
    int type = NONE; /* To signal for announce/braodcast */
    char *ptr;
    char *p;
    char racename[NORMSIZ];
    char govname[NORMSIZ];
    char raceid[NORMSIZ];
    char govid[NORMSIZ];
    char actualmsg[MAXSIZ];

    if (match_gag(s)) {
        return;
    }

    if (kill_socket_output) {
        if (logfile.redirect && logfile.on) {
            fprintf(logfile.fd, "%s\n", s);
        }

        if (pipe_running) {
            send_process(s);
        }

        return;
    }

    if (end_prompt == PASS_PROMPT) {
        client_stats = L_PASSWORD;

        if (options[AUTOLOGIN / 8] & (1 << (AUTOLOGIN % 8))) {
            if (!password_invalid) {
                send_password();
            }
        }

        return;
    } else {
        if ((client_stats < L_PASSWORD)
            && (options[LOGINSUPPRESS / 8] & (1 << (LOGINSUPPRESS % 8)))) {
            return;
        }
    }

    if (server_help) {
        check_for_special_formatting(s, FORMAT_HELP);
    }

    check_for_time_codes(s); /* mfw */
    parse_socket_output(s); /* For simple matches */

    if (pattern_match(s, "* \"*\" [*,*] *> *", pattern)) {
        type = GB_BROADCAST;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern6);
    } else if (pattern_match(s, "* \"*\" [*,*] > *", pattern)) {
        type = GB_BROADCAST;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "* \"*\" [*,*] = *", pattern)) {
        type = GB_THINK;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "* \"*\" [*,*] : *", pattern)) {
        type = GB_ANNOUNCE;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "* [*,*] * *", pattern)) {
        type = GB_EMOTE;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "(* [*.*]) *", pattern)) {
        type = HAP_THINK;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, profile.racename);
        strcpy(govname, pattern1);
        strcpy(raceid, pattern2);
        strcpy(govid, pattern3);
        strcpy(actualmsg, pattern4);
    } else if (pattern_match(s, "*:* [*.*] > *", pattern)) {
        type = HAP_BROADCAST;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "*:* [*.*] : *", pattern)) {
        type = HAP_ANNOUNCE;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        strcpy(govname, pattern2);
        strcpy(raceid, pattern3);
        strcpy(govid, pattern4);
        strcpy(actualmsg, pattern5);
    } else if (pattern_match(s, "* [*] > *", pattern)) {
        type = NORM_BROADCAST;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        *govname = '\0';
        strcpy(raceid, pattern2);
        *govid = '\0';
        strcpy(actualmsg, pattern3);
    } else if (pattern_match(s, "* [*] : *", pattern)) {
        type = NORM_ANNOUNCE;
        msg_type = MSG_COMMUNICATION;
        strcpy(racename, pattern1);
        *govname = '\0';
        strcpy(raceid, pattern2);
        *govid = '\0';
        strcpy(actualmsg, pattern3);
    }

    check_for_special_formatting(s, FORMAT_NORMAL);

    if (type != NONE) {
        check_for_special_formatting(actualmsg, FORMAT_NORMAL);
        debug(3, "Crypting! Sending: %s", actualmsg);
        ptr = check_crypt(skip_space(actualmsg), type);
        debug(3, "Crypting! Receive: %s", ptr);

        if (ptr) {
            if (type == GB_EMOTE) {
                p = s + strlen(s) - strlen(actualmsg);
            } else {
                p = s + strlen(s) - strlen(actualmsg) - 2;
            }

            strcpy(p, ptr);

            if (match_gag(s)) {
                return;
            }
        }
    } else {
        command_has_output = true;
    }

    if ((options[BEEP / 8] & (1 << (BEEP % 8)))
        && !end_prompt) {
        term_beep(1);
    }

    if (type != NONE) {
        process_spoken(racename, govname, atoi(raceid), atoi(govid), actualmsg);
    }

    socket_final_process(s, type);

    /* Reset for next loop - must be here */
    msg_type = MSG_ALL;
}

void socket_final_process(char *s, int type)
{
    if (handle_action_matches(s)) {
        return;
    }

    add_recall(s, type);

    if ((type != NONE)
        && (options[BOLD_COMM / 8] & (1 << (BOLD_COMM % 8)))) {
        display_bold_communication(s);
    } else {
        /* Write to screen */
        display_msg(s);
    }

    if (type) {
        cursor_to_window();
    }
}

/* Used for finding simple matches and setting the appropriate markers */
void parse_socket_output(char *s)
{
    char dmbuf[MAXSIZ];
    char dmbuf2[SMABUF];
    int dmint;
    int dmint2;
    char dmch;

    if (!s && !*s) {
        return;
    }

    *dmbuf = '\0';
    *dmbuf2 = '\0';

    if ((sscanf(s, "Logged in as %[^:]:%[^[][%d.%d].", dmbuf, dmbuf2, &dmint, &dmint2) == 4)
        && (client_stats == L_PASSWORD)) {
        remove_space_at_end(dmbuf);
        remove_space_at_end(dmbuf2);
        strcpy(profile.racename, dmbuf);
        strcpy(profile.govname, dmbuf2);
        add_assign("racename", dmbuf);
        add_assign("govname", dmbuf2);
        profile.raceid = dmint;
        profile.govid = dmint2;
        sprintf(dmbuf, "%d", profile.raceid);
        add_assign("raceid", dmbuf);
        sprintf(dmbuf, "%d", profile.govid);
        add_assign("govid", dmbuf);
        loggedin();
        game_type = GAME_HAP;
        add_assign("game_type", "HAP");
        password_invalid = false;
    } else if (((sscanf(s, "%[^\"]\"%[^\"]\" [%d,%d] logged on.", dmbuf, dmbuf2, &dmint, &dmint2) == 4)
                || (sscanf(s, "%[^\"]\"\" [%d,%d]", dmbuf, &dmint, &dmint2) == 3))
               && (client_stats == L_PASSWORD)) {
        remove_space_at_end(dmbuf);
        remove_space_at_end(dmbuf2);
        strcpy(profile.racename, dmbuf);
        strcpy(profile.govname, dmbuf2);
        add_assign("racename", dmbuf);
        add_assign("govname", dmbuf2);
        profile.raceid = dmint;
        profile.govid = dmint2;
        sprintf(dmbuf, "%d", profile.raceid);
        add_assign("raceid", dmbuf);
        sprintf(dmbuf, "%d", profile.govid);
        add_assign("govid", dmbuf);
        loggedin();
        password_invalid = false;
        on_endprompt(LEVEL_PROMPT);
    } else if (sscanf(s, "Name changed to `%s[^']'.", dmbuf) == 1) {
        strcpy(profile.racename, dmbuf);
        add_assign("racename", dmbuf);
    } else if (pattern_match(s, "Ship $* built at a cost *", pattern)) {
        strcpy(builtship, pattern1);
        add_assign("builtship", builtship);
    } else if (sscanf(s, "%c%d build at a cost of %d resources.", &dmch, &dmint, &dmint2) == 3) {
        sprintf(builtship, "%d", dmint);
        add_assign("builtship", builtship);
    } else if (pattern_match(s, "Lot #* - *", pattern)) {
        strcpy(lotnum, pattern1);
        add_assign("lotnum", lotnum);
    } else if (!strcmp(s, "Deity has kicked you off")) {
        close_gb();
    }
}

void oldcheck4_endprompt(char *s)
{
    char *p;
    int dummyint;
    char dummybuf[SMABUF];

    if (!s || !*s) {
        end_prompt = NOT_PROMPT;

        return;
    }

    debug(4, "In oldcheck4 on a prompt: %s", s);

    if ((*(s + 1) == '(') && (*(s + 3) == '[')) {
        /* We have a prompt */
        *scope.star = '\0';
        *scope.planet = '\0';
        *scope.shipc = '\0';
        *scope.mothership = '\0';

        if (strchr(s, '#')) {
            /* We are at ship scope somewhere, flag old style ship scope */
            scope.level = CSPD_PLAN;

            if ((sscanf(s, " ( [%d] / /../#%[^/]/#%s )", &scope.aps, scope.mothership, scope.shipc) == 3)
                || (sscanf(s, " ( [%d] /#%[^/]/#%s )", &scope.aps, scope.mothership, scope.shipc) == 3)
                || (sscanf(s, " ( [%d] /%[^/]/%[^/]/ /../#%[^/]#%s )", &scope.aps, scope.star, scope.planet, scope.mothership, scope.shipc) == 5)
                || (sscanf(s, " ( [%d] /%[^/]/%[^/]/#%[^/]/#%s )", &scope.aps, scope.star, scope.planet, scope.mothership, scope.shipc) == 5)
                || (sscanf(s, " ( [%d] /%[^/]/ /../#%[^/]/#%s )", &scope.aps, scope.star, scope.mothership, scope.shipc) == 4)
                || (sscanf(s, " ( [%d] /%[^/]/#%[^/]/#%s )", &scope.aps, scope.star, scope.mothership, scope.shipc) == 4)) {
                scope.numships = -1;
            } else if ((sscanf(s, " ( [%d] /%[^/]/%[^/]/#%s )", &scope.aps, scope.star, scope.planet, scope.shipc) == 4)
                       || (sscanf(s, " ( [%d] /%[^/]/#%s )", &scope.aps, scope.star, scope.shipc) == 3)
                       || (sscanf(s, " ( [%d] /#%s )", &scope.aps, scope.shipc) == 2)) {
                scope.numships = -2;
            } else {
                scope.level = CSPD_LOCATION_UNKNOWN;
            }
        } else {
            if (sscanf(s, " ( [%d] /%[^/]/%s )", &scope.aps, scope.star, scope.planet) == 3) {
                scope.level = CSPD_PLAN;
            } else {
                p = strchr(s, '/');

                if (!p) {
                    end_prompt = NOT_PROMPT;

                    return;
                }

                if (*(p + 1) == ' ') {
                    *scope.star = '\0';
                    scope.level = CSPD_UNIV;
                } else {
                    if (sscanf(s, " ( [%d] /%s )", &scope.aps, scope.star) == 2) {
                        scope.level = CSPD_STAR;
                    }
                }
            }
        }

        if (game_type == GAME_NOTGB) {
            game_type = GAME_UNKNOWN;
            add_assign("game_type", "UNKNOWN");
        }

        end_prompt = LEVEL_PROMPT;
        add_assign("scope", build_scope());
        sprintf(dummybuf, "%d", scope.aps);
        add_assign("aps", dummybuf);
        sprintf(dummybuf, "%d", scope.level);
        add_assign("scope_level", dummybuf);
        add_assign("star", scope.star);
        add_assign("planet", scope.planet);
        add_assign("ship", scope.shipc);
        add_assign("mothership", scope.mothership);
        *s = 24; /* Cancel the line -mfw */
    } else if (!strncmp(s, ENDREFUSED, strlen(ENDREFUSED))) {
        end_prompt = END_PROMPT;
    } else if (!strncmp(s, ENDHELP, strlen(ENDHELP))) {
        end_prompt = END_PROMPT;
    } else if (!strncmp(s, GBMFINISH, strlen(GBMFINISH))
               || (sscanf(s, GBUFINISH, &dummyint) == 1)
               || !strncmp(s, HAPMFINISH, strlen(HAPMFINISH))
               || !strncmp(s, HAPUFINISH, strlen(HAPUFINISH))) {
        end_prompt = ENDDOING_PROMPT;

        if (client_stats == L_SEGMENT) {
            if (notify > 0) {
                term_beep(notify);
            }

            init_start_commands(2);
            client_stats = L_REINIT;
        } else if (client_stats == L_UPDATE) {
            if (notify > 0) {
                term_beep(notify);
            }

            init_start_commands(1);
            client_stats = L_REINIT;
        }
    } else if (!strncmp(s, ENDDOINGSEGMENT, strlen(ENDDOINGSEGMENT))) {
        end_prompt = DOING_PROMPT;
        client_stats = L_SEGMENT;
    } else if (!strncmp(s, ENDDOINGUPDATE, strlen(ENDDOINGUPDATE))) {
        end_prompt = DOING_PROMPT;
        client_stats = L_UPDATE;
    } else if (!strncmp(s, ENDFINISH, strlen(ENDFINISH))) {
        end_prompt = FINISHED_PROMPT;
        server_help = false;
    } else if (!strncmp(s, ENDLOGIN, strlen(ENDLOGIN))
               || pattern_match(s, "Please * password*", pattern)
               || pattern_match(s, "please * password*", pattern)
               || pattern_match(s, "Please * Password*", pattern)
               || pattern_match(s, "Password: *", pattern)) {
        end_prompt = PASS_PROMPT;
    } else if (!strcmp(s, "Enter Password <race> <gov>:")) {
        end_prompt = PASS_PROMPT;
        game_type = GAME_GBDT;
        add_assign("game_type", "GB+");

        options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));

        if (!(options[AUTOLOGIN / 8] & (1 << (AUTOLOGIN % 8)))) {
            msg("%s", s);
            hide_input = true;
            promptfor("Password> ", dummybuf, PROMPT_STRING);
            hide_input = false;
            send_gb(dummybuf, strlen(dummybuf));
        }
    } else if ((!strcmp(s, "Invalid: Bad Password.")
                || !strcmp(s, "Invalid: Player already logged on!"))
               && (client_stats == L_PASSWORD)) {
        end_prompt = NODIS_PROMPT;
        client_stats = L_CONNECTED;
        password_invalid = true;

        if (game_type == GAME_GBDT) {
            options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));
            options[AUTOLOGIN / 8] &= ~(1 << (AUTOLOGIN % 8));
            options[CONNECT / 8] &= ~(1 << (CONNECT % 8));
        }

        msg("%s", s);
        msg("-- Please enter a new password or 'quit' to disconnect from the server.");
        hide_input = true;
        promptfor("Password> ", dummybuf, PROMPT_STRING);
        hide_input = false;

        if (!strcmp(dummybuf, "quit")) {
            cmd_quote(dummybuf);
        } else {
            send_gb(dummybuf, strlen(dummybuf));
        }
    } else if (strstr(s, RACEGEN)) {
        racegen = true;
        msg("-- Client is in RACEGEN mode.");
        strcpy(last_prompt, "[ GB Racegen ]");

        options[PARTIAL_LINES / 8] |= (1 << (PARTIAL_LINES % 8));
        options[AUTOLOGIN / 8] &= ~(1 << (AUTOLOGIN % 8));
        options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));

        force_update_status();
    } else {
        end_prompt = NOT_PROMPT;
    }
}

void connect_prompts(char *s)
{
    char buf[SMABUF];

    debug(4, "In connect_prompts on a prompt: %s", s);

    if (!strncmp(s, ENDLOGIN, strlen(ENDLOGIN))
        || pattern_match(s, "Please * password*", pattern)
        || pattern_match(s, "please * password*", pattern)
        || pattern_match(s, "Please * Password*", pattern)
        || pattern_match(s, "Password: *", pattern)) {
        end_prompt = PASS_PROMPT;
    } else if (!strcmp(s, "Enter Password <race> <gov>:")) {
        end_prompt = PASS_PROMPT;
        game_type = GAME_GBDT;
        add_assign("game_type", "GB+");

        options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));

        if (!(options[AUTOLOGIN / 8] & (1 << (AUTOLOGIN % 8)))) {
            msg("%s", s);
            hide_input = true;
            promptfor("PASSWORD> ", buf, PROMPT_STRING);
            hide_input = false;

            /* Allow the user to quit -mfw */
            if (!strncmp(buf, "quit", strlen("quit"))) {
                cmd_quote(buf);
            } else {
                send_gb(buf, strlen(buf));
            }
        }
    } else if ((!strcmp(s, "Invalid: Bad Password.")
                || !strcmp(s, "Invalid: Player already logged on!"))
               && (client_stats == L_PASSWORD)) {
        end_prompt = NODIS_PROMPT;

        /*
         * It seems to me after looking at the code this should be set
         * to L_PASSWORD. I'm commenting out L_CONNECTED for now -mfw
         */
        client_stats = L_PASSWORD;
        password_invalid = true;

        if (game_type == GAME_GBDT) {
            options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));
            options[AUTOLOGIN / 8] &= ~(1 << (AUTOLOGIN % 8));
            options[CONNECT / 8] &= ~(1 << (CONNECT % 8));
        }

        msg("%s", s);
        strcpy(s, ""); /* To blank out the message -mfw */

        /* Hmmm, the quit command isn't working -mfw */
        /* msg("-- Please enter a new password or 'quit' to disconnect from the server."); */
        msg("-- Please enter a new password or 'ctrl-c' to quit.");
        hide_input = true;
        promptfor("PASSWORD> ", buf, PROMPT_STRING);
        hide_input = false;

        if (!strncmp(buf, "quit", strlen("quit"))) {
            cmd_quote(buf);
        } else {
            send_gb(buf, strlen(buf));
        }
    } else if (!strncmp(buf, "CHAP CHALLENGE", strlen("CHAP CHALLENGE"))) {
        /* Mike's CHAP login hack -mfw */
        end_prompt = PASS_PROMPT;
        game_type = GAME_GBDT;
        add_assign("game_type", "GB+");

        if (options[AUTOLOGIN / 8] & (1 << (AUTOLOGIN % 8))) {
            if (!strcmp(cur_game.game.type, "chap")) {
                strcpy(race_name, cur_game.game.racename);
                strcpy(govn_name, cur_game.game.govname);
                strcpy(race_pass, cur_game.game.pripassword);
                strcpy(govn_pass, cur_game.game.secpassword);
            } else {
                msg("-- autologin failed, server requires CHAP login method.");
            }
        }

        chap_response(s);
    } else if (!strcmp(s, "CHAP FAILURE")) {
        end_prompt = INTERNAL_PROMPT;
        client_stats = L_PASSWORD;
        password_failed = 1;

        strcpy(s, ""); /* To blank out the message -mfw */
        msg("-- CHAP login failed, try again or 'ctrl-c' to quit.");

        if (game_type == GAME_GBDT) {
            options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));
            options[AUTOLOGIN / 8] &= ~(1 << (AUTOLOGIN % 8));
            options[CONNECT / 8] &= ~(1 << (CONNECT % 8));
        }
    } else if (!strcmp(s, "CHAP SUCCESS")) {
        strcpy(s, ""); /* To blank out the message -mfw */
    } else if (strstr(s, RACEGEN)) {
        racegen = true;
        msg("-- Client is in RACEGEN mode.");
        strcpy(last_prompt, "[ GB Racegen ]");

        options[PARTIAL_LINES / 8] |= (1 << (PARTIAL_LINES % 8));
        options[AUTOLOGIN / 8] &= ~(1 << (AUTOLOGIN % 8));
        options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));

        force_update_status();
    } else {
        end_prompt = NOT_PROMPT;
    }
}

void init_endprompt_connect(void)
{
    check4_endprompt = connect_prompts;
}

/* Send to the socket */
void send_gb(char *s, int len)
{
    char *p;
    char outbuf[MAXSIZ];
    char *q = s;
    int olen = 0;
    int splen;

    if((!queue_sending && have_queue())
       || ((client_stats != L_ACTIVE)
           && !(options[PARTIAL_LINES / 8] & (1 << (PARTIAL_LINES % 8)))
           && !((client_stats == L_PASSWORD)
                || (client_stats == L_INTERNALINIT)
                || (client_stats == L_REINIT)))) {
        add_queue(s, 0);

        return;
    }

    if (options[NO_LOGOUT / 8] & (1 << (NO_LOGOUT % 8))) {
        set_no_logout();
    }

    do_queue = false;

    if(gb < 0) {
        return;
    }

    splen = 0;
    p = outbuf;
    q = s;

    while (*q) {
        switch (*q) {
        case BELL_CHAR:
        case BOLD_CHAR:
        case INVERSE_CHAR:
        case UNDERLINE_CHAR:
            *p++ = SEND_QUOTE_CHAR;
            strcpy(p, SEND_QUOTE_PHRASE);
            splen = strlen(SEND_QUOTE_PHRASE);
            p += splen;
            *p++ = *q + 'A' - 1;
            olen += (2 + splen);
            ++q;

            break;
        case '\n':
            ++info.lines_sent;
            *p++ = *q++;
            ++olen;

            break;
        default:
            *p++ = *q++;
            ++olen;

            break;
        }
    }

    outbuf[olen] = '\0';
    debug(4, "send_gb: %s", outbuf);

    if (sendgb(outbuf, olen) == -1) {
        msg("Send to gb failed -- %s", s);
    } else {
        info.bytes_sent += olen;
    }
}

/*
 * Final raw sending of the data to the server. Retruns a -1 upon
 * failure. Otherwise #bytes sent.
 */
int sendgb(char *buf, int len)
{
    buf[len++] = '\x0d';
    buf[len++] = '\x0a';
    buf[len] = '\0';

    debug(4, "sending: %s", buf);

    return send(gb, buf, len, 0);
}

void cursor_output_window(void)
{
    if (options[DISPLAY_TOP / 8] & (1 << (DISPLAY_TOP % 8))) {
        term_move_cursor(0, last_output_row);
    } else {
        term_move_cursor(0, output_row);
    }

    cursor_display = false;
}

void scroll_output_window(void)
{
    if ((last_output_row >= output_row)
        || !(options[DISPLAY_TOP / 8] & (1 << (DISPLAY_TOP % 8)))) {
        term_scroll(0, output_row, 1);
        cursor_display = false;

        if (last_output_row > output_row) {
            --last_output_row;
        }
    }
}

void cmd_connect(char *s)
{
    Game *p = (Game *)NULL;
    char host_try[NORMSIZ];
    char port_try[NORMSIZ];
    int fd;
    int unknown = false;
    int dup_game = false;

    if (!*s) {
        msg("Usage: connect [nick | host] [port]");

        return;
    }

    p = find_game(s);

    if (p) {
        if (cur_game.game.host /* Added this, was segfaulting on next line -mfw */
            && !strcmp(cur_game.game.host, p->host)
            && !strcmp(cur_game.game.port, p->port)
            && !strcmp(cur_game.game.nick, p->nick)
            && !strcmp(cur_game.game.pripassword, p->pripassword)
            && !strcmp(cur_game.game.secpassword, p->secpassword)) {
            dup_game = true;
        }

        strcpy(host_try, p->host);
        strcpy(port_try, p->port);
    } else {
        split(s, host_try, port_try);

        if (!*port_try) {
#ifdef DEFAULT_GBPORT
            sprintf(port_try, "%d", DEFAULT_GBPORT);

#else

            strcpy(port_try, "2010");
#endif
        }

        unknown = true;
    }

    msg("-- Trying new host (%s %s)", host_try, port_try);
    fd = connectgb(host_try, port_try, "Connect: ");
    debug(1, "cmd_connect() fd: %d gb: %d", fd, gb);

    if (fd > 0) {
        if (gb > 0) {
            close_gb();
            gb = dup(fd);
            close(fd);
        } else {
            gb = fd;
        }

        /* Connecting to same site and race? If so, don't re-init everything */
        if (!dup_game) {
            init_assign(true);
        }

        cur_game.game.pripassword = strfree(cur_game.game.pripassword);
        cur_game.game.secpassword = strfree(cur_game.game.secpassword);
        cur_game.game.host = strfree(cur_game.game.host);
        cur_game.game.port = strfree(cur_game.game.port);
        cur_game.game.nick = strfree(cur_game.game.nick);
        cur_game.game.type = strfree(cur_game.game.type);
        cur_game.game.racename = strfree(cur_game.game.racename);
        cur_game.game.govname = strfree(cur_game.game.govname);

        cur_game.game.host = string(host_try);
        cur_game.game.port = string(port_try);

        if (!unknown) {
            cur_game.game.pripassword = string(p->pripassword);
            cur_game.game.secpassword = string(p->secpassword);
            cur_game.game.nick = string(p->nick);
            cur_game.game.type = string(p->type);
            cur_game.game.racename = string(p->racename);
            cur_game.game.govname = string(p->govname);
        } else {
            cur_game.game.pripassword = string("");
            cur_game.game.secpassword = string("");
            cur_game.game.nick = string("unknown");
            cur_game.game.type = string("unknown");
            cur_game.game.racename = string("");
            cur_game.game.govname = string("");
        }

        add_assign("game_nick", cur_game.game.nick);
        add_assign("host", cur_game.game.host);
        add_assign("port", cur_game.game.port);
        add_assign("type", cur_game.game.type);
        add_assign("racename", cur_game.game.racename);
        add_assign("pripassword", cur_game.game.pripassword);
        add_assign("govname", cur_game.game.govname);
        add_assign("secpassword", cur_game.game.secpassword);

        if (options[AUTOLOGIN_STARTUP / 8] & (1 << (AUTOLOGIN_STARTUP % 8))) {
            options[AUTOLOGIN / 8] |= (1 << (AUTOLOGIN % 8));
        }

        if (options[LOGINSUPPRESS_STARTUP / 8] & (1 << (LOGINSUPPRESS_STARTUP % 8))) {
            options[LOGINSUPPRESS / 8] |= (1 << (LOGINSUPPRESS % 8));
        }

        msg("-- Connected to new host.");
        client_stats = L_CONNECTED;
        connect_time = time(0);
    } else {
        msg("-- Could not reach new host.");
    }
}

void set_values_on_end_prompt(void)
{
    more_val.non_stop = false;
    more_val.forward = false;

    if (!hide_msg) {
        kill_socket_output = false;
    }

    kill_client_output = false;
}

void set_no_logout(void)
{
    last_no_logout_time = time(0);
}

void check_no_logout(void)
{
    long now;
    char tbuf[NORMSIZ];

    if (!(options[NO_LOGOUT / 8] & (1 << (NO_LOGOUT % 8)))) {
        return;
    }

    now = time(0);

    if ((now - NO_LOGOUT_DELAY) < last_no_logout_time) {
        return;
    }

    last_no_logout_time = now;
    sprintf(tbuf, "cs %s\n", build_scope());
    send_gb(tbuf, strlen(tbuf));
}

char *build_scope(void)
{
    static char scopebuf[NORMSIZ];

    if (scope.level == CSPD_UNIV) {
        sprintf(scopebuf, "/");
    } else if (scope.level == CSPD_STAR) {
        sprintf(scopebuf, "/%s", scope.star);
    } else if (scope.level == CSPD_PLAN) {
        sprintf(scopebuf, "/%s/%s", scope.star, scope.planet);
    } else {
        if ((scope.numships == -1) || (scope.numships == -2)) {
            sprintf(scopebuf, "#%s", scope.shipc);
        } else {
            sprintf(scopebuf, "#%d", scope.ship);
        }
    }

    return scopebuf;
}

void check_for_time_codes(char *s)
{
    char *p = (char *)NULL;
    char *q = (char *)NULL;
    char *t = (char *)NULL;
    char tmpbuf[MAXSIZ];
    char timestr[MAXSIZ];
    time_t ts;

    p = strstr(s, OPEN_TIME_TAG);

    if (p) {
        q = strstr(s, CLOSE_TIME_TAG);

        if (q) {
            debug(1, "found time codes");
            q += strlen(CLOSE_TIME_TAG);
            strcpy(tmpbuf, q);
            t = p;
            t += strlen(OPEN_TIME_TAG);
            *q = '\0';
            ts = atol(t);
            strftime(timestr, sizeof(timestr), "%s %b %e %X %Z %Y", localtime(&ts));
            strcpy(p, timestr);
            strcat(p, tmpbuf);
        }
    }
}

void check_for_special_formatting(char *s, int type)
{
    char *p;
    char outbuf[MAXSIZ];
    char *q;
    char ch;

    if (!*s) {
        return;
    }

    for (p = s, q = outbuf; *p; ++p, ++q) {
        if ((*p == SEND_QUOTE_CHAR) && (type == FORMAT_NORMAL)) {
            ++p;

            if (!strncmp(p, SEND_QUOTE_PHRASE, strlen(SEND_QUOTE_PHRASE))) {
                p += strlen(SEND_QUOTE_PHRASE);

                if (*p == SEND_QUOTE_CHAR) {
                    *q = SEND_QUOTE_CHAR;
                } else {
                    *q = *p - 64;
                }
            } else if (!strncmp(p, SEND_OLD_QUOTE_PHRASE, strlen(SEND_OLD_QUOTE_PHRASE))) {
                p += strlen(SEND_OLD_QUOTE_PHRASE);
                *q = *p - 64;
            } else {
                --p;
                *q = *p;
            }
        } else if (type == FORMAT_HELP) {
            switch (*p) {
            case '{':
            case '}':
                ch = *p++;

                if (*p == ch) {
                    *q = BOLD_CHAR;
                } else {
                    *q++ = ch;
                    *q = *p;
                }

                break;
            case '_':
                ++p;

                if (*p == '_') {
                    *q = UNDERLINE_CHAR;
                } else {
                    *q++ = '_';
                    *q = *p;
                }

                break;
            default:
                *q = *p;

                break;
            }
        } else {
            *q = *p;
        }
    }

    *q = '\0';
    strcpy(s, outbuf);
}

/* Close the socket and clean up variables */
void close_gb(void)
{
    while (have_socket_output() && !paused) {
        get_socket();
    }

    client_stats = L_BOOTED;
    password_invalid = false;
    game_type = GAME_NOTGB;
    csp_server_vers = 0;
    init_assign(false);
    icomm.num = 0;

    if (gb > 0) {
        msg("-- Game closed.");
        close(gb);
        strcpy(last_prompt, "Not Connected");
        force_update_status();
    }

    gb = -1;
}

/*
 * Read a chunk from the socket and separate on newlines and null
 * terminate after removing the newline
 */
int read_socket(void)
{
    int cnt;
    char buf[MAXSIZ];
    char *p;
    char *q;

    /* Read a chunk */
    cnt = recv(gb, buf, MAXSIZ - 1, 0);

    if (cnt == 0) {
        close_gb();
    } else if (cnt < 0) {
        msg_error("-- Read_socket: ");

        return -1;
    }

    buf[cnt] = '\0'; /* Line isn't nulled unless we do it */
    info.bytes_read += cnt;

    debug(3, "read_socket buf is:");
    debug(3, "%s", buf);
    debug(3, "end read_socket buf");

    q = buf;

    /* Break into newlines if possible */
    p = strchr(q, '\n');

    while (p) {
        *p = '\0';
        add_buffer(&gbsobuf, q, 0);
        q = p + 1;
    }

    /*
     * We ended on a newline so we are fine, otherwise we need to put
     * the partial part on the buffer
     */
    if ((q - buf) != cnt) {
        add_buffer(&gbsobuf, q, 1);
    }

    return cnt;
}

int have_socket_output(void)
{
    return have_buffer(&gbsobuf);
}

void loggedin(void)
{
    extern char *entry_quote;

    client_stats = L_LOGGEDIN;
    connect_time = time(0);

    if (game_type == GAME_GBDT) {
        /* Arg of 0 for color off, 1 for color on */
        csp_send_request(CSP_LOGIN_COMMAND, NULL);

        if(csp_server_vers <= 1) {
            check4_endprompt = oldcheck4_endprompt;
        } else {
            check4_endprompt = null_func;
        }
    } else {
        game_type = GAME_GB;
        add_assign("game_type", "GB");
        check4_endprompt = oldcheck4_endprompt;
    }

    if (options[CONNECT_STARTUP / 8] & (1 << (CONNECT_STARTUP % 8))) {
        options[CONNECT / 8] |= (1 << (CONNECT % 8));
    }

    if (options[AUTOLOGIN_STARTUP / 8] & (1 << (AUTOLOGIN_STARTUP % 8))) {
        options[AUTOLOGIN / 8] |= (1 << (AUTOLOGIN % 8));
    }

    if (options[LOGINSUPPRESS_STARTUP / 8] & (1 << (LOGINSUPPRESS_STARTUP % 8))) {
        options[LOGINSUPPRESS / 8] |= (1 << (LOGINSUPPRESS % 8));
    }

    init_start_commands(0); /* Set client_stats below */

    if ((game_type != GAME_GBDT) && entry_quote) {
        send_gb(entry_quote, strlen(entry_quote));
    }

    /* For init_start_comm, but after entry_quote */
    client_stats = L_INTERNALINIT;
    more_val.num_lines_scrolled = 0;
}

void cmd_ping(char *s)
{
    Game *p = (Game *)NULL;
    char host_try[NORMSIZ];
    char port_try[NORMSIZ];
    int fd;

    if (!*s) {
        msg("Usage: ping [nick | host] [port]");

        return;
    }

    p = find_game(s);

    if (p) {
        strcpy(host_try, p->host);
        strcpy(port_try, p->port);
    } else {
        split(s, host_try, port_try);

        if (!*port_try) {
#ifdef DEFAULT_GBPORT
            sprintf(port_try, "%d", DEFAULT_GBPORT);

#else

            sprintf(port_try, "%d", 2010);
#endif
        }
    }

    msg("-- Ping: (%s %s", host_try, port_try);
    fd = connectgb(host_try, port_try, "Ping:");

    if (fd > 0) {
        msg("-- Ping: Host (%s %s) reached.", host_try, port_try);
        close(fd);
    }
}

void null_func(char *s)
{
    return;
}

int on_endprompt(int eprompt)
{
    char *p;

    if (!eprompt) {
        return false;
    }

    if (icomm.num
        && (icomm.list[0].state = S_PROC)
        && (icomm.list[0].prompt == eprompt)) {
        icomm_command_done('\0');
    }

    set_values_on_end_prompt();
    do_queue = true; /* Special case...must be socket */

    if (eprompt == LEVEL_PROMPT) {
        if ((client_stats == L_PASSWORD)
            && !(options[PARTIAL_LINES / 8] & (1 << (PARTIAL_LINES % 8)))) {
            msg("-- WARNING: The client does NOT know your race id number.");

            while ((client_stats != L_LOGGEDIN) && (client_stats >= L_PASSWORD)) {
                promptfor("Race Id#? ", pbuf, PROMPT_STRING);

                if (isdigit(*pbuf)) {
                    profile.raceid = atoi(pbuf);

                    if (profile.raceid > 0) {
                        loggedin();
                    }
                }
            }
        }

        if (command_has_output) {
            command_has_output = false;
        }

#ifdef IMAP
        if (input_mode.map && (scope.level == CSPD_PLAN)) {
            map_prompt_force_redraw();
        }
#endif
    }

    if (hide_msg) {
        --hide_msg;

        if (!hide_msg) {
            kill_socket_output = false;
        } else {
            /* Should this be false? */
            return true;
        }
    }

    if (end_msg) {
        --end_msg;

        return true;
    }

    if (eprompt == FINISHED_PROMPT) {
        server_help = false;
    }

    if (end_prompt == LEVEL_PROMPT) {
        p = build_scope_prompt();

        if (strcmp(last_prompt, p)) {
            strcpy(last_prompt, p);
            force_update_status();
        }
    }

    if ((eprompt >= NODIS_PROMPT)
        && (options[HIDE_END_PROMPT / 8] & (1 << (HIDE_END_PROMPT % 8)))) {
        cursor_to_window();

        return true;
    }

    return false;
}

void get_pass_info(void)
{
    promptfor("Race Name: ", pbuf, PROMPT_STRING);
    strcpy(race_name, pbuf);

    if (!strncasecmp(race_name, "quit", 4)) {
        chap_abort();

        return;
    }

    hide_input = true;
    promptfor("Race Password: ", pbuf, PROMPT_STRING);
    hide_input = false;
    strcpy(race_pass, pbuf);

    promptfor("Governor Name: ", pbuf, PROMPT_STRING);
    strcpy(govn_name, pbuf);

    if (!strncasecmp(govn_name, "quit", 4)) {
        chap_abort();

        return;
    }

    hide_input = true;
    promptfor("Governor Password: ", pbuf, PROMPT_STRING);
    hide_input = false;
    strcpy(govn_pass, pbuf);

    return;
}

void chap_response(char *line)
{
    char auth_string[MAXSIZ];
    char client_hash[NORMSIZ];
    char pass_cat[NORMSIZ];
    char key[NORMSIZ];
    char garb1[NORMSIZ];
    char garb2[NORMSIZ];

    debug(2, "Received CHAP Challenge.");
    sscanf(line, "%s %s %s", garb1, garb2, key);
    debug(4, "Received key: %s", key);

    if (!race_name[0]
        || !govn_name[0]
        || !race_pass[0]
        || !govn_pass[0]
        || password_failed) {
        options[LOGINSUPPRESS / 8] &= ~(1 << (LOGINSUPPRESS % 8));

        get_pass_info();
    }

    sprintf(pass_cat, "%s%s%s", race_pass, govn_pass, key);
    memset(client_hash, '\0', sizeof(NORMSIZ));
    MD5String(pass_cat, client_hash);

    sprintf(auth_string,
            "CHAP RESPONSE %s %s %s",
            race_name,
            govn_name,
            client_hash);

    debug(3, "chap_response sending: %s", auth_string);
    send_gb(auth_string, strlen(auth_string));

    return;
}

void chap_abort(void)
{
    char abort[22];
    char c;

    strcpy(abort, "CHAP ABORT");
    debug(3, "chap_abort sending abort string");
    sendgb(abort, 10);
    promptfor("Quit client (y/n)? ", &c, PROMPT_CHAR);

    if ((c == 'Y') || (c == 'y')) {
        quit_all = true;
    } else {
        update_input_prompt(input_prompt);
    }
}
