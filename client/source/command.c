/*
 * command.c -- Client commands.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file
 */
#include "command.h"

#include "action.h"
#include "args.h"
#include "bind.h"
#include "crypt.h"
#include "csp.h"
#include "fuse.h"
#include "gb.h"
#include "help.h"
#include "icomm.h"
#include "key.h"
#include "load.h"
#include "option.h"
#include "popn.h"
#include "proc.h"
#include "psmap.h"
#include "socket.h"
#include "status.h"
#include "stmt.h"
#include "str.h"
#include "util.h"
#include "xmap.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GBRC_SAVE_LINE "# Put your stuff below here -- Keep this line here\n"

extern int exit_now;
extern int hide_msg;
extern int kill_socket_output;
extern int max_recall;
extern int pipe_running;
extern int server_help;
extern int robo;

static Command command_table[] = {
    /*               Name           Function    Sub   Echo  Socket Cnt */
    {            "action",        cmd_action,  true, false, false, 0 },
    {           "addgame",          cmd_game,  true,  true, false, 0 },
    {            "assign",        cmd_assign,  true,  true, false, 0 },
    {              "bind",          cmd_bind,  true,  true, false, 0 },
    {                "cd",            cmd_cd,  true,  true,  true, 0 },
    {             "clear",         cmd_clear,  true, false, false, 0 },
    {          "cleardef",      cmd_cleardef,  true,  true, false, 0 },
    {          "cleargag",      cmd_cleargag,  true,  true, false, 0 },
    {           "connect",       cmd_connect,  true,  true, false, 0 },
    {             "convo",         cmd_convo,  true, false, false, 0 },
    {                "cr",            cmd_cr,  true,  true,  true, 0 },
    {             "crypt",         cmd_crypt,  true,  true, false, 0 },
    {               "def",           cmd_def, false,  true, false, 0 },
    {            "detach",        cmd_detach,  true,  true, false, 0 },
    {              "echo",          cmd_echo,  true, false, false, 0 },
    {               "for",           cmd_for, false,  true, false, 0 },
    {              "fuse",          cmd_fuse,  true,  true, false, 0 },
    {               "gag",           cmd_gag,  true,  true, false, 0 },
    {              "game",          cmd_game,  true,  true, false, 0 },
    {              "help",          cmd_help, false,  true,  true, 0 },
    {             "helpc",         cmd_helpc,  true, false, false, 0 },
    {             "helps",         cmd_helps,  true,  true, false, 0 },
    {              "hide",          cmd_hide,  true,  true, false, 0 },
    {                "if",            cmd_if, false,  true, false, 0 },
    {              "info",          cmd_info,  true, false, false, 0 },
    {          "internal",      cmd_internal,  true,  true, false, 0 },
    { "internal_pipe_off", internal_pipe_off, false, false, false, 0 },
    {              "last",        cmd_recall,  true, false, false, 0 },
    {        "listassign",    cmd_listassign,  true, false, false, 0 },
    {         "listcrypt",     cmd_listcrypt,  true, false, false, 0 },
    {           "listdef",       cmd_listdef,  true, false, false, 0 },
    {           "listgag",       cmd_listgag,  true, false, false, 0 },
    {          "listgame",      cmd_listgame,  true, false, false, 0 },
    {          "listloop",      cmd_listloop,  true, false, false, 0 },
    {             "loadf",         cmd_loadf,  true,  true, false, 0 },
    {               "log",           cmd_log,  true, false, false, 0 },
    {              "loop",          cmd_loop,  true,  true, false, 0 },
#ifdef XMAP
    {               "map",           cmd_map,  true,  true,  true, 0 },
#endif
    {               "msg",           cmd_msg,  true, false, false, 0 },
    {             "oldsh",      cmd_oldshell,  true,  true, false, 0 },
#ifdef OPTTECH
    {           "opttech",       cmd_opttech, false, false, false, 0 },
#endif
    {              "ping",          cmd_ping,  true,  true, false, 0 },
    {              "popn",          cmd_popn,  true,  true, false, 0 },
    {              "post",          cmd_post,  true,  true, false, 0 },
    {              "proc",          cmd_proc,  true, false, false, 0 },
    {             "psmap",         cmd_psmap,  true,  true, false, 0 },
    {             "queue",         cmd_queue,  true,  true, false, 0 },
    {              "quit",          cmd_quit,  true, false, false, 0 },
    {             "quote",         cmd_quote,  true,  true, false, 0 },
    {               "rea",          cmd_read, false,  true, false, 0 },
    {              "read",          cmd_read, false,  true, false, 0 },
    {            "recall",        cmd_recall,  true, false, false, 0 },
    {        "removeloop",    cmd_removeloop,  true,  true, false, 0 },
    {            "repeat",        cmd_repeat,  true,  true, false, 0 },
    {             "savef",         cmd_savef,  true,  true, false, 0 },
    {               "set",           cmd_set, false, false, false, 0 },
    {                "sh",          cmd_proc,  true, false, false, 0 },
    {             "shell",          cmd_proc,  true, false, false, 0 },
    {             "sleep",         cmd_sleep,  true,  true, false, 0 },
    {            "source",        cmd_source,  true,  true, false, 0 },
    {              "talk",          cmd_talk,  true, false, false, 0 },
    {           "uncrypt",       cmd_uncrypt,  true,  true, false, 0 },
    {             "undef",         cmd_undef,  true,  true, false, 0 },
    {             "ungag",         cmd_ungag,  true,  true, false, 0 },
    {            "ungame",        cmd_ungame,  true,  true, false, 0 },
    {            "unloop",    cmd_removeloop,  true,  true, false, 0 },
    {           "version",       cmd_version,  true, false, false, 0 },
    {              "wait",          cmd_wait,  true,  true, false, 0 },
    {            "watch4",        cmd_watch4,  true, false, false, 0 }
#ifdef XMAP
    ,{             "xmap",          cmd_xmap, false, false, false, 0}
#endif
};

char tbuf[MAXSIZ];
char post_buf[MAXSIZ];

void add_to_post(char *s);
void robo_say(char *, int);

/* Return '1' if it is a client command else return '0' */
int client_command(char *s, int interactive)
{
    char cmd[NORMSIZ];
    char args[NORMSIZ];
    char buf[MAXSIZ];
    Command *handler;

    memset(cmd, '\0', sizeof(cmd));
    memset(args, '\0', sizeof(args));
    memset(buf, '\0', sizeof(buf));

    if (input_mode.post) {
        add_to_post(s);

        return true;
    }


    if (*s == '!') {
        cmd_proc(s + 1);

        return true;
    }

    split(s, cmd, args);

    if (*s == '^') {
        history_sub(s + 1);

        return true;
    }

    handler = binary_search(cmd);

    if (handler == NULL) {
        return false;
    }

    ++handler->cnt;

    debug(3,
          "handler client_command: '%s' %d/%d/%d echo/parse/inter",
          args,
          handler->echo_command,
          handler->parse_var,
          interactive);

    if (handler->echo_command && interactive) {
        print_key_string(handler->parse_var);
    }

    if (handler->parse_var) {
        parse_variables(args);
        debug(3, "PGS in client_command: %s", args);
    }

    sprintf(buf, "%s %s", cmd, args);
    argify(buf);

    if (handler->parse_var && parse_for_loops(buf)) {
        add_queue(buf, 0);

        return true;
    }

    handler->func(args);

    if (handler->send_to_socket) {
        send_gb(args, strlen(args));
    }

    return true;
}

Command *binary_search(char *cmd)
{
    int bottom = 0;
    int top = (sizeof(command_table) / sizeof(Command)) - 1;
    int mid;
    int value;

    while (bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = strcmp(cmd, command_table[mid].name);

        if (value == 0) {
            return &command_table[mid];
        } else if (value < 0) {
            top = mid - 1;
        } else {
            bottom = mid + 1;
        }
    }

    return NULL;
}

/* Handle routines */
void cmd_info(char *args)
{
    int i;
    int numc;
    int total = 0;
    int total_only = true;
    char buf[SMABUF];

    msg("-- Info:");

    if (strcmp(args, "total")) {
        promptfor("Are you sure? (y/n) ", buf, PROMPT_CHAR);

        if ((*buf != 'Y') && (*buf != 'y')) {
            return;
        }

        total_only = false;
        set_column_maker(20);
    }

    numc = sizeof(command_table) / sizeof(Command);

    for (i = 0; i < numc; ++i) {
        if (!total_only) {
            sprintf(buf, "%s (%d)", command_table[i].name, command_table[i].cnt);
            do_column_maker(buf);
        }

        total += command_table[i].cnt;
    }

    msg("  Total client commands received: %d", total);
    msg("  Read: %lu bytes, %lu lines.", info.bytes_read, info.lines_read);
    msg("  Sent: %lu bytes, %lu lines.", info.bytes_sent, info.lines_sent);
}

void cmd_wait(char *args)
{
    if (!strcmp(args, "none") || !strcmp(args, "off")) {
        wait_status = WAIT_NONE;
    } else if (!strcmp(args, "booted")) {
        wait_status = WAIT_BOOTED;
    } else if (!strcmp(args, "connect")) {
        wait_status = WAIT_CONNECT;
    } else if (!strcmp(args, "password")) {
        wait_status = WAIT_PASSWORD;
    } else if (!strcmp(args, "login")) {
        wait_status = WAIT_LOGIN;
    }
}

/* Server command */
void cmd_help(char *args)
{
    char temp[MAXSIZ];

    sprintf(temp, "help %s", args);
    strcpy(args, temp);
    server_help = true;
}

void cmd_read(char *args)
{
    char junk[80];
    int flag = 0;

    /* Read news was typed */
    if (!strncmp(args, "n", strlen("n"))) {
        start_command(C_RNEWS, flag);
    } else if (!strncmp(args, "t", strlen("t"))) {
        start_command(C_TELEGRAM, flag);
    } else if (!strncmp(args, "d", strlen("d"))) {
        sscanf(args, "%s %d", junk, &flag);
        start_command(C_DISPATCH, flag);
    } else {
        start_command(C_READ, flag);
    }
}

void cmd_cd(char *args)
{
    char buf[MAXSIZ];

    sprintf(buf, "cs %s", args);
    strcpy(args, buf);
}

void cmd_hide(char *args)
{
    int cnt = 1;

    if (isdigit(*args)) {
        cnt = atoi(args);
    }

    if (cnt > 0) {
        hide_msg = cnt;
        ++kill_socket_output;
    } else if(cnt < 0) {
        msg("-- Hide: count must be a positive value.");
    }
}

void cmd_internal(char *args)
{
    if (!strcmp(args, "scope")) {
        strcpy(args, " ( [123] /Test/Scope )");
    }

    process_gb(args);
}

void cmd_detach(char *args)
{
#ifndef RESTRICTED_ACCESS
    int pid;
    char buf[SMABUF];

    pid = fork();

    switch (pid) {
    case -1:
        msg("-- Detach: Could not fork into background. Abort.");

        break;
    case 0: /* I'm the child */
        detached = true;

        break;
    default: /* I'm the parent */
        sprintf(buf, "-- I am detached. Process id (%d)", pid);
        quit_gb(0, buf, NULL, NULL);

        break;
    }

#else
    msg("-- Restricted Access: detach is not allowed for security.");
#endif
}

void cmd_post(char *args)
{
    input_mode.post = true;
    strcpy(post_buf, "post ");

    if (*args) {
        strcat(post_buf, args);
    }

    msg("-- You are not in posting mode. Type a single perdiod (.) to end.");
    force_update_status();
}

void cmd_echo(char *args)
{
    socket_final_process(args, 0);
}

void cmd_msg(char *args)
{
    char *p;
    char *q;

    q = get_args(1, 0);
    parse_variables(q);
    p = q;
    msg("%s %s", client_prompt, p);
    strfree(q);
}

void cmd_queue(char *args)
{
    char buf[MAXSIZ];

    sprintf(buf, "echo queue: '%s'", args);
    add_queue(buf, 0);
}

void cmd_helpc(char *args)
{
    FILE *fhelp;

    fhelp = fopen(help_client, "r");

    if (fhelp == NULL) {
        msg("-- Could not open %s for reading. Help unavailable.", help_client);
        msg("   Use 'set help <path_to_help_file>' and try again.");

        return;
    }

    help(args, fhelp);
}

void cmd_helps(char *args)
{
    FILE *fhelp;

    fhelp = fopen(HELP_SERVER, "r");

    if (fhelp == NULL) {
        msg("-- Could not open %s for reading. Help unavailable.", HELP_SERVER);

        return;
    }

    help(args, fhelp);
}

void cmd_watch4(char *args)
{
    int id;
    char temp[NORMSIZ];

#ifdef RWHO
    if (!strcmp(args, "all")) {
        for (id = 0; id <= cur_game.maxplayers; ++id) {
            rwho.info[id].watch4 = true;
        }

        msg("-- watch4 placed on all players.");

        return;
    } else if (*args == '-') {
        if (isdigit(*(args + 1))) {
            id = atoi(args + 1);

            if (id > cur_game.maxplayers) {
                msg("-- watch4: invalid player number.");

                return;
            }

            rwho.info[id].watch4 = false;
            msg("-- watch4: turned off for player #%d", id);
        }
    } else if (isdigit(*args)) {
        id = atoi(args);

        if (id > cur_game.maxplayers) {
            msg("-- Invalid player number.");

            return;
        }

        rwho.info[id].watch4 = true;
        msg("-- watch4 placed on %d.", id);

        return;
    } else {
        set_column_maker(25);
        msg("-- Watch4 List:");

        for (id = 1; id <= cur_game.maxplayers; ++id) {
            if (rwho.info[id].watch4) {
                if (rwho.info[id].on) {
                    sprintf(temp, "  Player #%-2d - ON", id);
                } else {
                    sprintf(temp, "  Player #%-2d - not on", id);
                }

                do_column_maker(temp);
            }
        }

        flush_column_maker();

        return;
    }

#else
    msg("-- You must compile with the RWHO defined for watch4 to work.");
#endif
}

void done_rwho(void)
{
    int i;

    for (i = 0; i <= cur_game.maxplayers; ++i) {
        if ((rwho.info[i].watch4 == RWHO_ON) && rwho.info[i].on) {
            msg("-- watch4: %s [%d] is on or is now visible.",
                rwho.info[i].name,
                i);

            rwho.info[i].watch4 = RWHO_NOTIFIED;
        } else if ((rwho.info[i].watch4 == RWHO_NOTIFIED) && !rwho.info[i].on) {
            msg("-- watch4: %s [%d] is no longer visible or signed off.",
                rwho.info[i].name,
                i);

            rwho.info[i].watch4 = RWHO_ON;
        }
    }
}

void start_rwho(void)
{
    int i;

    for (i = 0; i <= cur_game.maxplayers; ++i) {
        rwho.info[i].on = false;
    }
}

/*
 * Displays the version and other useful information to aid in discussions about
 * what version people are or aren't using. -b flag will broadcast the
 * information out as well as display it.
 */
void cmd_version(char *args)
{
    char opts[SMABUF];
    char broad[NORMSIZ];

    csp_send_request(CSP_VERSION_COMMAND, NULL);
    msg("-- Client version %s.", VERSION);
    strcpy(opts, "-- Client options set:");

#ifdef ARRAY
    strcat(opts, " ARRAY");
#endif

#ifdef CTIX
    strcat(opts, " CTIX");
#endif

#ifdef IMAP
    strcat(opts, " IMAP");
#endif

#ifdef OPTTECH
    strcat(opts, " OPTTECH");
#endif

#ifdef POPN
    strcat(opts, " POPN");
#endif

#ifdef SMART_CLIENT
    strcat(opts, " SMART_CLIENT");
#endif

#ifdef SYSV
    strcat(opts, " SYSV");
#endif

#ifdef XMAP
    strcat(opts, " XMAP");
#endif

#ifdef TERMIO
    strcat(opts, " TERMIO");
#endif

    msg(opts);

    if (!strcmp(args, "-b")) {
        sprintf(broad,
                "broadcast Version (%s), Term (%s)\nbroadcast %s\n",
                VERSION,
                getenv("TERM"),
                opts);

        send_gb(broad, strlen(broad));
    }

#ifdef RESTRICTED_ACCESS
    if (!strcmp(args, "-b")) {
        send_gb("broadcast Client is running in RESTRICTED_ACCESS MODE\n", 54);
    }

    msg("-- Client is running in RESTRICTED_ACCESS MODE");
#endif

    msg(GBSAYING);
}

void cmd_clear(char *args)
{
    clear_screen(' ');
}

void cmd_quote(char *args)
{
    if (*args) {
        send_gb(args, strlen(args));
    }
}

void cmd_savef(char *args)
{
#ifdef RESTRICTED_ACCESS
    msg("-- Restricted access: savef not available.");
#else
    char str[MAXSIZ];
    char askbuf[MAXSIZ];
    char *gbrc_save_buf;
    int gbrc_buf_size;
    int first_blanks = true;

    if (!*args) {
        expand_file(gbrc_path);
        sprintf(askbuf, "Really save to %s (y/n)? ", gbrc_path);
        promptfor(askbuf, str, PROMPT_STRING);

        if ((*str != 'Y') && (*str != 'y')) {
            return;
        }

        strcpy(askbuf, gbrc_path);
    } else {
        strcpy(askbuf, args);
        expand_file(askbuf);
    }

    /* Open to read for the GBRC_SAVE_LINE stuff */
    FILE *fd = fopen(askbuf, "r");

    if (fd != NULL) {
        /* Using it as flag for now */
        gbrc_buf_size = false;

        while (fgets(str, MAXSIZ - 1, fd)) {
            if (!strcmp(str, GBRC_SAVE_LINE)) {
                gbrc_save_buf = (char *)malloc(MAXSIZ);
                *gbrc_save_buf = '\0';
                gbrc_buf_size = MAXSIZ;

                continue;
            } else if (!gbrc_buf_size) {
                continue;
            }

            /* Skip blank lines */
            if ((*str == '\n') && first_blanks) {
                continue;
            }

            /* Realloc the buffer */
            if (strlen(str) > (gbrc_buf_size - strlen(gbrc_save_buf))) {
                char *p = (char *)realloc(gbrc_save_buf, MAXSIZ);
                gbrc_save_buf = p;
                gbrc_buf_size += MAXSIZ;
            }

            strcat(gbrc_save_buf, str);
            first_blanks = false;
        }

        fclose(fd);
    }

    fd = fopen(askbuf, "w");

    if (fd == NULL) {
        msg("-- savef: could not open \'%s\' for writing.", askbuf);
        strfree(gbrc_save_buf);

        return;
    }

    time_t clk = time(0);
    fprintf(fd, "#\n# Galactic Bloodshed client II Initialization File\n");
    fprintf(fd, "# Created: %s#\n\n", ctime(&clk));
    fprintf(fd, "set display off\n\n");

    save_actions(fd);
    save_assigns(fd);
    save_binds(fd);
    save_crypts(fd);
    save_defs(fd);
    save_gags(fd);
    save_games(fd);
    save_settings(fd);

    fprintf(fd, "\nset display on\n\n");
    fprintf(fd, "# Do NOT put anything below here that is SAVED above.\n");
    fprintf(fd, "# Commands (like connect) NOT saved by the client\n");
    fprintf(fd, "# should got below these comment lines.\n");
    fprintf(fd, "%s\n", GBRC_SAVE_LINE);

    if (grbc_buf_size) {
        fprintf(fd, gbrc_save_buf);
    }

    strfree(gbrc_save_buf);
    fclose(fd);
    msg("-- savef: Saved settings to \'%s\'", askbuf);
#endif
}

void cmd_sleep(char *args)
{
    if (isdigit(*args)) {
        sleep((unsigned)atoi(args));
    }
}

void cmd_repeat(char *args)
{
    char *c;
    char cmd[MAXSIZ];
    int i;

    if (isdigit(*args)) {
        c = first(args);

        if (c) {
            strcpy(cmd, c);
        } else {
            *cmd = '\0';
        }

        c = rest(args);

        if (c) {
            strcpy(args, c);
        } else {
            *args = '\0';
        }

        for (i = atoi(cmd); i; --i) {
            add_queue(args, 0);
        }
    }
}

void cmd_quit(char *args)
{
    char c;
    extern char *exit_quote;

    if (is_connected()) {
        if (exit_quote) {
            send_gb(exit_quote, strlen(exit_quote));
            send_gb("", 0);
        }

        gb_close_socket = 1;
        send_gb("quit", 4);

        if (CONNECT < 32) {
            options[CONNECT / 32] &= ~(1 << CONNECT);
        } else {
            options[CONNECT / 32] &= ~(1 << (CONNECT % 32));
        }

        if ((options[QUIT_ALL / 32] & ((QUIT_ALL < 32) ?
                                       (1 << QUIT_ALL)
                                       : (1 << (QUIT_ALL % 32))))
            || !strcmp(args, "all")) {
            quit_all = true;
        } else {
            promptfor("Quit client (y/n)? ", &c, PROMPT_CHAR);

            if ((c == 'Y') || (c == 'y')) {
                quit_all = true;
            } else {
                update_input_prompt(input_prompt);
            }
        }
    } else {
        exit_now = true;
    }
}

void cmd_recall(char *args)
{
    if (!args) {
        msg("Usage: recall [num | *pat* | all]");
    } else if (!strcmp(args, "all")) {
        recall(max_recall, 0);
    } else if (pattern_match(args, "*,*", pattern)
               || pattern_match(args, "*, *", pattern)
               || pattern_match(args, "*-*", pattern)
               || pattern_match(args, "* *", pattern)) {
        if (isdigit(*pattern1) && isdigit(*pattern2)) {
            recall_n_m(atoi(pattern1), atoi(pattern2), 0);
        } else {
            msg("-- Usage: recall n,m");
        }
    } else if (isdigit(args[0])) {
        recall(atoi(args), 0);
    } else {
        recall_match(args, 0);
    }
}

void cmd_convo(char *args)
{
    if (!args) {
        msg("Usage: convo [num | *pat* | all]");
    } else if (!strcmp(args, "all")) {
        recall(max_recall, 1);
    } else if (pattern_match(args, "*,*", pattern)
               || pattern_match(args, "*, *", pattern)
               || pattern_match(args, "*-*", pattern)
               || pattern_match(args, "* *", pattern)) {
        if (isdigit(*pattern1) && isdigit(*pattern2)) {
            recall_n_m(atoi(pattern1), atoi(pattern2), 1);
        } else {
            msg("-- Usage: convo n,m");
        }
    } else if (isdigit(args[0])) {
        recall(atoi(args), 1);
    } else {
        recall_match(args, 1);
    }
}

int can_log(char *s)
{
    if (input_mode.post) {
        return 1;
    }

    if ((*s == '\'') /* Short broadcast */
        || (*s == '\"')
        || (strncmp(s, "announce", strlen(s)) == 0)
        || (strncmp(s, "broadcast", 2 < strlen(s) ? strlen(s) : 2) == 0)
        || (strncmp(s, "think", strlen(s)) == 0)
        || !strncmp(s, "sen", strlen("sen")) /* Send */
        || !strncmp(s, "cr", strlen("cr")) /* Crypt */
        || !strncmp(s, "set", strlen("set")) /* Settings */
        || !strncmp(s, "gag", strlen("gag")) /* Gag */
        || !strncmp(s, "ungag", strlen("ungag")) /* Ungag */
        || !strncmp(s, "def", strlen("def")) /* Def */
        || !strncmp(s, "bind", strlen("bind")) /* Bind */
        || !strncmp(s, "sh", strlen("sh")) /* Shell escape */
        || (*s == '!') /* Shell escape */
        || !strncmp(s, "if", strlen("if")) /* If command */
        || !strncmp(s, "action", strlen("action")) /* Action */
        || !strncmp(s, "pos", strlen("pos"))) { /* Post */
        return 0;
    }

    return 1;
}

#ifdef RWHO
void icomm_rwho(char *s)
{
    char name[200];
    int id;
    long unow;
    char *p;

    if (!rwho.on) {
        return;
    }

    debug(2, "icomm_rwho()");

    if (icomm.list[0].state == S_WAIT) {
        if (!strncmp(s, "Current Players:", strlen("Current Players:"))) {
            icomm.list[0].state = S_PROC;
            icomm.list[0].ignore = true;
        }

        return;
    }

    unow = time(0);

    if (((game_type == GAME_HAP)
         && pattern_match(s, "*:* [*.*] seconds idle", pattern))
        || pattern_match(s, "* \"*\" [*] * idle", pattern)) {
        p = skip_space(pattern1);
        remove_space_at_end(pattern1);
        strcpy(name, p);
        id = atoi(pattern3);
        rwho.info[id].last_on = unow;
        rwho.info[id].on = true;

        if (strcmp(rwho.info[id].name, name)) {
            rwho.info[id].changed_names = unow;
            strcpy(rwho.info[id].last_name, name);
            strcpy(rwho.info[id].name, name);
        }
    }
}
#endif

void init_rwho(void)
{
#ifdef RWHO
    int i;

    for (i = 0; i < MAX_NUM_PLAYERS; ++i) {
        rwho.info[i].last_on = -1;
        rwho.info[i].last_spoke = -1;
        rwho.info[i].changed_names = -1;
        rwho.info[i].watch4 = false;
        strcpy(rwho.info[i].name, races[i].name);
        strcpy(rwho.info[i].last_name, races[i].name);
    }
#endif
}

void process_spoken(char *race, char *gov, int rid, int gid, char *message)
{
    char name[SMABUF];
    char fword[MAXSIZ];
    char rword[MAXSIZ];
    char psbuf[MAXSIZ];
    long now;
    int player;

    strcpy(name, race);
    split(message, fword, rword);

    if (!strcmp(fword, profile.racename)
        && (!strcmp(race, "Q continuum") || !strcmp(gov, "Priam"))
        && !strcmp(rword, "GBII_CLIENT_INFORMATION")) {
        sprintf(psbuf,
                "send %s %s Info %s@%s -- version (%s) -- term (%s)\n",
                race,
                gov,
                getenv("LOGNAME"),
                getenv("HOSTNAME"),
                VERSION,
                getenv("TERM"));

        send_gb(psbuf, strlen(psbuf));

        return;
    }

    if (!robo) {
        return;
    }

    now = time(0);

#ifdef RWHO
    if (strcmp(rwho.info[rid].last_name, name)) {
        strcpy(rwho.info[rid].last_name, rwho.info[rid].name);
        strcpy(rwho.info[rid].name, name);
        rwho.info[rid].changed_names = now;
    }

    rwho.info[rid].last_spoke = now;
    rwho.info[rid].last_on = now;
#endif

#define PHRASE1 ROBONAME

    if (strcmp("Robby", fword)
        && strcmp("robby", fword)
        && strcmp("Robby:", fword)
        && strcmp("robby:", fword)
        && strcmp("Robby,", fword)
        && strcmp("robby,", fword)) {
        return;
    }

    if (!strcmp("help", rword) || !strcmp("Help", rword)) {
        robo_say("Help: fortune; info on #", 0);

        return;
    }

    if (!strcmp("fortune", rword)) {
        robo_say("-b fortune", 1);

        return;
    }

#ifdef RWHO
    if (pattern_match(rword, "*when was * on*", pattern)
        || pattern_match(rword, "*info on *", pattern)) {
        if (isdigit(*pattern2)) {
            player = atoi(pattern2);
        } else {
            player = 1;

            while (strcmp(pattern2, rwho.info[player].name)
                   && (player < MAX_NUM_PLAYERS)) {
                ++player;
            }
        }

        if (player >= MAX_NUM_PLAYERS) {
            sprintf(tbuf,
                    "broadcast %s [%d] - I do not know of such a player.",
                    name,
                    rid);

            robo_say(tbuf, 0);

            return;
        }

        sprintf(tbuf,
                "broadcast %s [%d] - I last saw %s [%d] %s ago and last heard %s speak %s ago.",
                name,
                rid,
                rwho.info[player].name,
                player,
                time_dur(rwho.info[player].last_on),
                rwho.info[player].name,
                time_dur(rwho.info[player].last_spoke));

        robo_say(tbuf, 0);

        return;
    }
#endif

    robo_say("broadcast If you want something, then ask.", 0);
}

void robo_say(char *message, int proc)
{
    char myname[80];
    char saybuf[MAXSIZ];

    strcpy(myname, "Priam");

    /* Rename ourselves temporarily */
    sprintf(saybuf, "name gov %s", ROBONAME);
    send_gb(saybuf, sizeof(saybuf));

    if (proc) {
        sprintf(saybuf, "%s", message);
        cmd_proc(saybuf);
        sprintf(saybuf, "--Robo ran: %s", message);
        msg(saybuf);
    } else {
        /* Say what we need to say. */
        sprintf(saybuf, "broadcast %s\n", message);
        send_gb(saybuf, sizeof(saybuf));
        sprintf(saybuf, "--Robo said: %s", message);
        msg(saybuf);
    }

    /* Restore our good name */
    sprintf(saybuf, "name gov %s", myname);
    send_gb(saybuf, sizeof(saybuf));
}

void cancel_post(void)
{
    input_mode.post = false;
    *post_buf = '\0';
    force_update_status();
}

void add_to_post(char *s)
{
    char *p;

    if (!strcmp(s, ".")) {
        parse_variables(post_buf);
        p = post_buf;
        send_gb(p, strlen(p));
        msg("-- Posting message.");
        input_mode.post = false;
        *post_buf = '\0';
        force_update_status();

        return;
    }

    if ((game_type == GAME_GB) || (game_type == GAME_GBDT)) {
        strcat(post_buf, ";|");
    } else {
        strcat(post_buf, " ");
    }

    strcat(post_buf, s);
    msg("POST> %s", s);
    force_update_status();
}

int handle_pipes_and_redirects(char *str)
{
    char cmd[MAXSIZ];
    char s[MAXSIZ];
    char *c;

    split(str, cmd, s);
    strcpy(s, str);
    debug(3, "pipes/redirects: %s (%s)", str, cmd);

    c = strchr(s, '|');

    if (c && (*(c - 1) != '\\') && can_log(cmd)) {
        *c = '\0';

#ifndef RESTRICTED_ACCESS
        ++c;

        if (*c == '!') {
            ++c;
            kill_socket_output = true;
        }

        c = skip_space(c);
        pipe_running = true;
        cmd_proc(c);
        add_queue(s, 0);
        add_queue("internal_pipe_off", 0);

#else
        msg("-- Restricted Access: piping not available.");
#endif

        return 1;
    }

    c = strchr(s, '>');

    if (c && (*(c - 1) != '\\') && can_log(cmd)) {
        *c = '\0';

#ifndef RESTRICTED_ACCESS
        char mode[5];
        char buf[NORMSIZ];

        /* hmmm append it */
        if (*(c + 1) == '>') {
            strcpy(mode, "a");

            if (*(c + 2) == '!') {
                c += 3;
                kill_socket_output = true;
            } else {
                c += 2;
            }
        } else {
            /* Overwrite it */
            strcpy(mode, "w");

            if (*(c + 1) == '!') {
                c += 2;
                kill_socket_output = true;
            } else {
                ++c;
            }
        }

        logfile.redirect = true;
        logfile.level = LOG_ALL;

        c = skip_space(c);
        sprintf(buf, "log -%s %s", mode, c);
        add_queue(buf, 0);
        add_queue(s, 0);
        add_queue("log off no msg", 0);

#else
        msg("-- Restricted Access: redirection not available.");
#endif

        return 1;
    }

    return 0;
}
