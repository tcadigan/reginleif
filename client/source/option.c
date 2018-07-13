/*
 * option.c: Handles option setting characteristics for user
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "option.h"

#include "args.h"
#include "bind.h"
#include "key.h"
#include "socket.h"
#include "status.h"
#include "str.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern int end_msg;
extern int max_history;
extern int max_recall;
extern int reconnect_delay;
extern char *client_prompt;
extern char *input_prompt;
extern char *output_prompt;
extern char pbuf[];
extern int robo;
extern int notify;
extern int debug_level;

#ifdef IMAP
extern char cursor_sector; /* Imap */
#endif

#ifdef CLIENT_DEVEL
extern int client_devel;
#endif

char *entry_quote;
char *exit_quote;
int options[2] = { 0, 0 };

void display_set(void);

static CommandSet commandset_table[] = {
    {              "actions",               ACTIONS,                  toggle, 0 },
    {                 "ansi",             DISP_ANSI,                set_ansi, 0 },
    {            "autologin",     AUTOLOGIN_STARTUP,            doubletoggle, 0 },
    {                 "beep",                  BEEP,                  toggle, 0 },
    {                 "bell",              DO_BELLS,                  toggle, 0 },
    {            "bold_comm",             BOLD_COMM,                  toggle, 0 },
    {   "bold_communication",             BOLD_COMM,                  toggle, 0 },
    {             "brackets",              BRACKETS,                  toggle, 0 },
    {        "client_prompt",               OP_NONE,       set_client_prompt, 0 },
    {              "connect",       CONNECT_STARTUP,            doubletoggle, 0 },
    {        "connect_delay",               OP_NONE,       set_connect_delay, 0 },
#ifdef IMAP
    {        "cursor_sector",               OP_NONE,               set_debug, 0 },
#endif
    {                "debug",               OP_NONE,               set_debug, 0 },
#ifdef CLIENT_DEVEL
    {                "devel",               OP_NONE,               set_devel, 0 },
#endif
    {              "display",            DISPLAYING,                  toggle, 0 },
    {     "display_from_top",           DISPLAY_TOP,                  toggle, 0 },
    {              "encrypt",               OP_NONE,             set_encrypt, 0 },
    {          "entry_quote",               OP_NONE,         set_entry_quote, 0 },
    {           "exit_quote",               OP_NONE,          set_exit_quote, 0 },
    {          "full_screen",               OP_NONE,         set_full_screen, 0 },
    {                 "help",               OP_NONE,                set_help, 0 },
    {      "hide_end_prompt",       HIDE_END_PROMPT,                  toggle, 0 },
    {              "history",               OP_NONE,             set_history, 0 },
    {         "input_prompt",               OP_NONE,        set_input_prompt, 0 },
    {     "insert_edit_mode",               OP_NONE,    set_insert_edit_mode, 0 },
    {              "inverse",               OP_NONE,       term_standout_off, 0 },
    {       "login_suppress", LOGINSUPPRESS_STARTUP,            doubletoggle, 0 },
    {           "macro_char",               OP_NONE,          set_macro_char, 0 },
    {                  "map",               OP_NONE,            set_map_opts, 0 },
    {                 "more",               OP_NONE,                set_more, 0 },
    {           "more_delay",               OP_NONE,          set_more_delay, 0 },
    {            "more_rows",               OP_NONE,           set_more_rows, 0 },
    {            "no_logout",             NO_LOGOUT,                  toggle, 0 },
    {            "noclobber",             NOCLOBBER,                  toggle, 0 },
    {               "notify",               OP_NONE,              set_notify, 0 },
    {         "notify_beeps",               OP_NONE,        set_notify_beeps, 0 },
    {        "output_prompt",               OP_NONE,       set_output_prompt, 0 },
    {  "overwrite_edit_mode",               OP_NONE, set_overwrite_edit_mode, 0 },
    {        "partial_lines",         PARTIAL_LINES,                  toggle, 0 },
    {     "primary_password",               OP_NONE,    set_primary_password, 0 },
    {             "quit_all",              QUIT_ALL,                  toggle, 0 },
    {                  "raw",               RAWMODE,                  toggle, 0 },
    {               "recall",               OP_NONE,              set_recall, 0 },
    {       "repeat_connect",       CONNECT_STARTUP,            doubletoggle, 0 },
    {                 "robo",               OP_NONE,                set_robo, 0 },
    {                 "rwho",               OP_NONE,                set_rwho, 0 },
    {               "scroll",                SCROLL,                  toggle, 0 },
    {         "scroll_clear",            SCROLL_CLR,                  toggle, 0 },
    {   "secondary password",               OP_NONE,  set_secondary_password, 0 },
    {         "show_actions",          SHOW_ACTIONS,                  toggle, 0 },
    {           "show_clock",            SHOW_CLOCK,          set_show_clock, 0 },
    {            "show_mail",             SHOW_MAIL,           set_show_mail, 0 },
    {       "slash_commands",        SLASH_COMMANDS,                  toggle, 0 },
    {           "status_bar",               OP_NONE,          set_status_bar, 0 },
    {      "status_bar_char",               OP_NONE,     set_status_bar_char, 0 },
    { "status_bar_character",               OP_NONE,     set_status_bar_char, 0 }
};

void display_set(void)
{
    char dsbuf[NORMSIZ];

    set_column_maker(num_columns / 2);
    msg("-- set:");
    sprintf(dsbuf,
            "actions: %s",
            (options[ACTIONS / 32] & ((ACTIONS < 32) ?
                                      (1 << ACTIONS)
                                      : (1 << (ACTIONS % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "ansi: %s",
            (options[DISP_ANSI / 32] & ((DISP_ANSI < 32) ?
                                        (1 << DISP_ANSI)
                                        : (1 << (DISP_ANSI % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "more_delay: %d secs", more_val.delay);

    sprintf(dsbuf,
            "autologin: %s",
            (options[AUTOLOGIN_STARTUP / 32] & ((AUTOLOGIN_STARTUP < 32) ?
                                                (1 << AUTOLOGIN_STARTUP)
                                                : (1 << (AUTOLOGIN_STARTUP % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "beep: %s",
            (options[BEEP / 32] & ((BEEP < 32) ?
                                   (1 << BEEP)
                                   : (1 << (BEEP % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "bell: %s",
            (options[DO_BELLS / 32] & ((DO_BELLS < 32) ?
                                       (1 << DO_BELLS)
                                       : (1 << (DO_BELLS % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "bold_communcation: %s",
            (options[BOLD_COMM / 32] & ((BOLD_COMM < 32) ?
                                        (1 << BOLD_COMM)
                                        : (1 << (BOLD_COMM % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "client_prompt: \'%s\'", client_prompt);
    do_column_maker(dsbuf);
    sprintf(dsbuf, "connect_delay: %3d secs", reconnect_delay);
    do_column_maker(dsbuf);

#ifdef IMAP
    sprintf(dsbuf, "cursor_sector: %c", cursor_sector);
    do_column_maker(dsbuf);
#endif

    sprintf(dsbuf,
            "display_from_top: %s",
            (options[DISPLAY_TOP / 32] & ((DISPLAY_TOP < 32) ?
                                          (1 << DISPLAY_TOP)
                                          : (1 << (DISPLAY_TOP % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "edit mode: %s",
            (input_mode.edit == EDIT_INSERT ? "insert" : "overwrite"));

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "encrypt: %s messages",
            (options[ENCRYPT / 32] & ((ENCRYPT < 32) ?
                                      (1 << ENCRYPT)
                                      : (1 << (ENCRYPT % 32)))) ?
            "hidden"
            : "shown");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "entry_quote: \'%s\'",
            (entry_quote ? entry_quote : "none"));

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "exit_quote: \'%s\'",
            (exit_quote ? exit_quote : "none"));

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "full_screen: %s",
            (options[FULLSCREEN / 32] & ((FULLSCREEN < 32) ?
                                         (1 << FULLSCREEN)
                                         : (1 << (FULLSCREEN % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "help: %s", help_client);
    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "hide_end_prompt: %s",
            (options[HIDE_END_PROMPT/ 32] & ((HIDE_END_PROMPT < 32) ?
                                             (1 << HIDE_END_PROMPT)
                                             : (1 << (HIDE_END_PROMPT % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "history: %d lines", max_history);
    do_column_maker(dsbuf);
    sprintf(dsbuf, "input_prompt: \'%s\'", input_prompt);
    do_column_maker(dsbuf);
    sprintf(dsbuf, "logging: %s", (logfile.on ? "on " : "off"));
    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "login_suppress: %s",
            (options[LOGINSUPPRESS_STARTUP / 32] & ((LOGINSUPPRESS_STARTUP < 32) ?
                                                    (1 << LOGINSUPPRESS_STARTUP)
                                                    : (1 << (LOGINSUPPRESS_STARTUP % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "map double: %s",
            (options[MAP_DOUBLE / 32] & ((MAP_DOUBLE < 32) ?
                                         (1 << MAP_DOUBLE)
                                         : (1 << (MAP_DOUBLE % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "map space: %s",
            (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                        (1 << MAP_SPACE)
                                        : (1 << (MAP_SPACE % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "more: %s", more_val.on ? "on " : "off");
    do_column_maker(dsbuf);
    sprintf(dsbuf, "more_delay: %d secs", more_val.delay);
    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "no_logout: %s",
            (options[NO_LOGOUT / 32] & ((NO_LOGOUT < 32) ?
                                        (1 << NO_LOGOUT)
                                        : (1 << (NO_LOGOUT % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "noclobber: %s",
            (options[NOCLOBBER / 32] & ((NOCLOBBER < 32) ?
                                        (1 << NOCLOBBER)
                                        : (1 << (NOCLOBBER % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "notify: %s (beeps: %d)",
            (notify > 0 ? "on " : "off"),
            (notify > 0 ? notify : -notify));

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "password: %s (%s)",
            (cur_game.game.pripassword == NULL ? "none" : cur_game.game.pripassword),
            (cur_game.game.secpassword == NULL ? "none" : cur_game.game.secpassword));

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "quit_all: %s",
            (options[QUIT_ALL / 32] & ((QUIT_ALL < 32) ?
                                       (1 << QUIT_ALL)
                                       : (1 << (QUIT_ALL % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "recall: %d lines", max_recall);
    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "repeat_connect: %s",
            (options[CONNECT_STARTUP / 32] & ((CONNECT_STARTUP < 32) ?
                                              (1 << CONNECT_STARTUP)
                                              : (1 << (CONNECT_STARTUP % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "scroll: %s",
            (options[SCROLL / 32] & ((SCROLL < 32) ?
                                     (1 << SCROLL)
                                     : (1 << (SCROLL % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "scroll_clear: %s",
            (options[SCROLL_CLR / 32] & ((SCROLL_CLR < 32) ?
                                         (1 << SCROLL_CLR)
                                         : (1 << (SCROLL_CLR % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);
    sprintf(dsbuf, "server_version: %d", servinfo.version);
    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "show_actions: %s",
            (options[SHOW_ACTIONS / 32] & ((SHOW_ACTIONS < 32) ?
                                           (1 << SHOW_ACTIONS)
                                           : (1 << (SHOW_ACTIONS % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "show_mail: %s",
            (options[SHOW_MAIL / 32] & ((SHOW_MAIL < 32) ?
                                        (1 << SHOW_MAIL)
                                        : (1 << (SHOW_MAIL % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "show_clock: %s",
            (options[SHOW_CLOCK / 32] & ((SHOW_CLOCK < 32) ?
                                         (1 << SHOW_CLOCK)
                                         : (1 << (SHOW_CLOCK % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    sprintf(dsbuf,
            "slash_commands: %s",
            (options[SLASH_COMMANDS / 32] & ((SLASH_COMMANDS < 32) ?
                                             (1 << SLASH_COMMANDS)
                                             : (1 << (SLASH_COMMANDS % 32)))) ?
            "on "
            : "off");

    do_column_maker(dsbuf);

    if (options[BRACKETS / 32] & ((BRACKETS < 32) ?
                                  (1 << BRACKETS)
                                  : (1 << (BRACKETS % 32)))) {
        do_column_maker("brackets: on");
    }

    flush_column_maker();
    msg("shell: \'%s\'", (shell ? shell : "NONE SET"));

    if (debug_level) {
        msg("DEBUG ON -- Level: %s", debug_level);
    }

    if (options[RAWMODE / 32] & ((RAWMODE < 32) ?
                                 (1 << RAWMODE)
                                 : (1 << (RAWMODE)))) {
        msg("RAWMODE ON.");
    }

    msg("-- end of set.");
}

/*
 * Takes the args and if on or off set/clears the bits. Otherwise it
 * flips the bit from which ever way it was.
 */
void toggle(int *args, int type, char *name)
{
    if (*args && !strcmp((char *)args, "on")) {
        if (*name) {
            msg("-- %s: on", name);
        }

        if (type < 32) {
            options[type / 32] |= (1 << type);
        } else {
            options[type / 32] |= (1 << (type % 32));
        }
    } else if (*args && !strcmp((char *)args, "off")) {
        if (*name) {
            msg("-- %s: off", name);
        }

        if (type < 32) {
            options[type / 32] &= ~(1 << type);
        } else {
            options[type / 32] &= ~(1 << (type % 32));
        }
    } else if (options[type / 32] & ((type < 32) ?
                                     (1 << type)
                                     : (1 << (type % 32)))) {
        if (*name) {
            msg("-- %s: off", name);
        }

        if (type < 32) {
            options[type / 32] &= ~(1 << type);
        } else {
            options[type / 32] &= ~(1 << (type % 32));
        }
    }
    else {
        if (*name) {
            msg("-- %s: on", name);
        }

        if (type < 32) {
            options[type / 32] |= (1 << type);
        } else {
            options[type / 32] |= (1 << (type % 32));
        }
    }
}

void doubletoggle(int *args, int type, char *name)
{
    toggle(args, type, name);
    toggle(args, type - 1, "");
}

void cmd_set(char *s)
{
    CommandSet *handler;
    char cmd[MAXSIZ];
    char args[BUFSIZ];
    char *c;

    memset(args, '\0', sizeof(args));
    memset(cmd, '\0', sizeof(cmd));

    c = first(s);

    if (c && *c) {
        strcpy(cmd, c);
    } else {
        display_set();

        return;
    }

    c = rest(s);

    if (c) {
        strcpy(args, c);
    } else {
        *args = '\0';
    }

    handler = binary_set_search(cmd);

    if (handler == NULL) {
        msg("-- set: no such option %s.", cmd);

        return;
    }

    handler->has_changed = true;
    handler->func(args, handler->value, handler->name);
}

CommandSet *binary_set_search(char *cmd)
{
    int bottom = 0;
    int top = (sizeof(commandset_table) / sizeof(CommandSet)) - 1;
    int mid;
    int value;

    while (bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = strcmp(cmd, commandset_table[mid].name);

        if(value == 0) {
            return &commandset_table[mid];
        } else if (value < 0) {
            top = mid - 1;
        } else {
            bottom = mid - 1;
        }
    }

    return (CommandSet *)NULL;
}

/* Handle routines */
void set_client_prompt(char *args)
{
    char *p;

    p = get_args(2, 0);

    if (!strcmp(p, "default")) {
        strfree(client_prompt);
        client_prompt = string(DEFAULT_CLIENT_PROMPT);
        strfree(p);
    } else if (*p) {
        strfree(client_prompt);
        client_prompt = p;
    } else {
        *client_prompt = '\0';
        strfree(p);
    }

    msg("-- client_prompt is: \'%s\'", client_prompt);
}

void set_connect_delay(char *args)
{
    if (isdigit(*args)) {
        reconnect_delay = atoi(args);
        msg("-- reconnect delay set at %d", atoi(args));
    } else {
        msg("-- reconnect delay must be an integer.");
    }
}

#ifdef IMAP
void set_cursor_sector(char *args, int val)
{
    if (*args) {
        cursor_sector = *args;
        msg("-- cursor_sector set to: %c", *args);
    } else {
        msg("-- you must specify a character.");
    }
}
#endif

void set_debug(char *args)
{
    int i;

    if (isdigit(*args)) {
        i = atoi(args);

        if (i < 0) {
            msg("-- debug: level must be positive");
        } else {
            debug_level = i;
            msg("-- debug level: %d", debug_level);
        }
    } else {
        msg("-- debug: value must be a positive integer.");
    }
}

void set_encrypt(char *args)
{
    if (!strcmp(args, "on") || !strncmp(args, "hide", strlen(args))) {
        if (ENCRYPT < 32) {
            options[ENCRYPT / 32] |= (1 << ENCRYPT);
        } else {
            options[ENCRYPT / 32] |= (1 << (ENCRYPT % 32));
        }

        msg("-- encrypt: hidden.");
    } else if (!strcmp(args, "off") || !strncmp(args, "shown", strlen(args))) {
        if (ENCRYPT < 32) {
            options[ENCRYPT / 32] &= ~(1 << ENCRYPT);
        } else {
            options[ENCRYPT / 32] &= ~(1 << (ENCRYPT % 32));
        }

        msg("-- encrypt: shown.");
    } else {
        msg("-- encrypt options: on | off.");
    }
}

void set_entry_quote(char *args)
{
    if (!strcmp(args, "none")) {
        strfree(entry_quote);
        entry_quote = (char *)NULL;
        msg("-- entry_quote: cleared.");
    } else {
        strfree(entry_quote);
        entry_quote = string(args);
        msg("-- entry_quote: %s", entry_quote);
    }
}

void set_exit_quote(char *args)
{
    if (!strcmp(args, "none")) {
        strfree(exit_quote);
        exit_quote = (char *)NULL;
        msg("-- exit_quote: cleared.");
    } else {
        strfree(exit_quote);
        exit_quote = string(args);
        msg("-- exit_quote: %s", exit_quote);
    }
}

void set_full_screen(char *args)
{
    scroll_output_window();
    toggle((int *)args, FULLSCREEN, "full_screen");
}

void set_help(char *args)
{
    if (*args) {
        strfree(help_client);

        if (!strcmp(args, "default")) {
#ifdef HELP_CLIENT
            help_client = string(HELP_CLIENT);

#else

            help_client = string("./Help");
#endif
        } else {
            help_client = string(args);
        }

        msg("-- help: %s", help_client);
    } else {
        msg("-- help is currently found at: %s", help_client);
    }
}

void set_history(char *args)
{
    if (isdigit(*args)) {
        max_history = atoi(args);
        msg("-- history set at %d", atoi(args));
    } else {
        msg("-- history value must be an integer.");
    }
}

void set_input_prompt(char *args)
{
    char *p;

    p = get_args(2, 0);

    if (!strcmp(p, "default")) {
        strfree(input_prompt);
        input_prompt = string(DEFAULT_INPUT_PROMPT);
        strfree(p);
    } else if (*p) {
        strfree(input_prompt);
        input_prompt = p;
    } else {
        *input_prompt = '\0';
        strfree(p);
    }

    update_input_prompt(input_prompt);
    cancel_input('\0');
    msg("-- input_prompt is: \'%s\'", input_prompt);
}

void set_insert_edit_mode(char *args)
{
    input_mode.edit = EDIT_INSERT;
    msg("-- edit mode: insert");
    force_update_status();
}

void set_macro_char(char *args)
{
    if (!strcmp(args, "default")) {
#ifdef DEFAULT_MACRO_CHAR
        macro_char = DEFAULT_MACRO_CHAR;

#else

        macro_char = '/';
#endif
    } else {
        macro_char = *args;
    }

    msg("-- macro_char: %s", display_char(macro_char, 0));
}

void set_map_opts(char *args)
{
    if (!strncmp(args, "double", strlen(args))) {
        if (options[MAP_DOUBLE / 32] & ((MAP_DOUBLE < 32) ?
                                        (1 << MAP_DOUBLE)
                                        : (1 << (MAP_DOUBLE % 32)))) {
            if (MAP_DOUBLE < 32) {
                options[MAP_DOUBLE / 32] &= ~(1 << MAP_DOUBLE);
            } else {
                options[MAP_DOUBLE / 32] &= ~(1 << (MAP_DOUBLE % 32));
            }

            msg("-- map: double numbers turned off.");
        } else {
            if (MAP_DOUBLE < 32) {
                options[MAP_DOUBLE / 32] |= (1 << MAP_DOUBLE);
            } else {
                options[MAP_DOUBLE / 32] |= (1 << (MAP_DOUBLE % 32));
            }

            msg("-- map: double numbers turned on.");
        }
    } else if (!strncmp(args, "space", strlen(args))) {
        if (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                       (1 << MAP_SPACE)
                                       : (1 << (MAP_SPACE % 32)))) {
            if (MAP_SPACE < 32) {
                options[MAP_SPACE / 32] &= ~(1 << MAP_SPACE);
            } else {
                options[MAP_SPACE / 32] &= ~(1 << (MAP_SPACE % 32));
            }

            msg("-- map: spacing disabled.");
        } else {
            if (MAP_SPACE < 32) {
                options[MAP_SPACE / 32] |= (1 << MAP_SPACE);
            } else {
                options[MAP_SPACE / 32] |= (1 << (MAP_SPACE % 32));
            }

            msg("-- map options: spacing enabled.");
        }
    } else {
        msg("-- map options: double | space");
    }
}

void set_more(char *args)
{
    if (*args && !strcmp(args, "on")) {
        more_val.on = true;
    } else if (*args && !strcmp(args, "off")) {
        more_val.on = false;
    } else {
        more_val.on = (more_val.on + 1) % 2;
    }

    msg("-- more: %s", (more_val.on ? "on" : "off"));
}

void set_more_delay(char *args)
{
    if (isdigit(*args)) {
        more_val.delay = atol(args);
        msg("-- more_delay set at %d", atoi(args));
    } else {
        msg("-- more_delay must be an integer.");
    }
}

void set_more_rows(char *args)
{
    if (isdigit(*args)) {
        more_val.num_rows = atoi(args);
        msg("-- more_rows set at %d", atoi(args));
    } else {
        msg("-- more_rows must be an integer.");
    }
}

void set_notify(char *args)
{
    if (((notify > 0) || !strcmp(args, "off")) && strcmp(args, "on")) {
        if (notify > 0) {
            notify = 0 - notify;
        }

        msg("-- notify: off");
    } else if ((notify < 0) || !strcmp(args, "on")) {
        if (notify < 0) {
            notify = 0 - notify;
        }

        msg("-- notify: on");
    }
}

void set_notify_beeps(char *args)
{
    if (isdigit(*args) && (atoi(args) > 0)) {
        if (notify < 0) {
            notify = 0 - atoi(args);
        } else {
            notify = atoi(args);
        }

        msg("-- notify_beeps set at %d", atoi(args));
    } else {
        msg("-- notify_beeps must be a positive integer.");
    }
}

void set_output_prompt(char *args)
{
    char *p;

    p = get_args(2, 0);

    if (!strcmp(p, "default")) {
        strfree(output_prompt);

#ifdef DEFAULT_OUTPUT_PROMPT
        output_prompt = string(DEFAULT_OUTPUT_PROMPT);

#else

        output_prompt = string("");
#endif

        strfree(p);
    } else if (*p) {
        strfree(output_prompt);
        output_prompt = p;
    } else {
        *output_prompt = '\0';
        strfree(p);
    }

    msg("-- output_prompt is: \'%s\'", output_prompt);
}

void set_overwrite_edit_mode(char *args)
{
    input_mode.edit = EDIT_OVERWRITE;
    msg("-- edit mode: overwrite");
    force_update_status();
}

void set_primary_password(char *args)
{
    strfree(cur_game.game.pripassword);
    cur_game.game.pripassword = string(args);
    msg("-- Primary_password set to: %s", cur_game.game.pripassword);
}

void set_recall(char *args)
{
    if (isdigit(*args)) {
        max_recall = atoi(args);
        msg("-- recall set at %d", atoi(args));
    } else {
        msg("-- recall value must be an integer.");
    }
}

void set_rwho(char *args)
{
    rwho.on = (rwho.on + 1) % 2;
    msg("-- set: rwho %s", (rwho.on ? "on" : "off"));
}

void set_secondary_password(char *args)
{
    strfree(cur_game.game.secpassword);
    cur_game.game.secpassword = string(args);
    msg("-- Secondary_password set to: %s", cur_game.game.secpassword);
}

void set_show_clock(char *args, int val, char *name)
{
    toggle((int *)args, val, name);
    force_update_status();
}

void set_show_mail(char *args, int val, char *name)
{
    toggle((int *)args, val, name);
    force_update_status();
}

void set_status_bar(char *args)
{
    if (!strcmp(args, "default")) {
#ifdef DEFAULT_STATUS_BAR
        strcpy(status.format, DEFAULT_STATUS_BAR);

#else

        strcpy(status.format, "-");
#endif
    } else {
        strncpy(status.format, args, SMABUF);
    }

    msg("-- set: status_bar has been changed.");
    force_update_status();
}

void set_status_bar_char(char *args)
{
    if (*args) {
        status.schar[0] = *args;
        force_update_status();
    } else {
        msg("-- set status_bar_char: Specify a character.");
    }
}

void set_robo(char *args)
{
    robo = (robo + 1) % 2;
    msg("-- set: robo %s", (robo ? "on" : "off"));
}

#ifdef CLIENT_DEVEL
void set_devel(char *args)
{
    client_devel = (client_devel + 1) % 2;
    msg(":: Developer Output: %s", (client_devel ? "on" : "off"));
}
#endif

void set_ansi(char *args, int val, char *name)
{
    toggle((int *)args, val, name);
    force_update_status();
}

void save_settings(FILE *fd)
{
    fprintf(fd, "\n#\n# Settings (set <option> <args>)\n#\n");

    if (options[ACTIONS / 32] & ((ACTIONS < 32) ?
                                 (1 << ACTIONS)
                                 : (1 << (ACTIONS % 32)))) {
        fprintf(fd, "set actions on\n");
    }

    if (options[DISP_ANSI / 32] & ((DISP_ANSI < 32) ?
                                   (1 << DISP_ANSI)
                                   : (1 << (DISP_ANSI % 32)))) {
        fprintf(fd, "set ansi on\n");
    }

    if (options[AUTOLOGIN_STARTUP / 32] & ((AUTOLOGIN_STARTUP < 32) ?
                                           (1 << AUTOLOGIN_STARTUP)
                                           : (1 << (AUTOLOGIN_STARTUP % 32)))) {
        fprintf(fd, "set autologin\n");
    }

    if (options[BEEP / 32] & ((BEEP < 32) ?
                              (1 << BEEP)
                              : (1 << (BEEP % 32)))) {
        fprintf(fd, "set beep on\n");
    }

    if (options[DO_BELLS / 32] & ((DO_BELLS < 32) ?
                                  (1 << DO_BELLS)
                                  : (1 << (DO_BELLS % 32)))) {
        fprintf(fd, "set bell on\n");
    }

    if (options[BOLD_COMM / 32] & ((BOLD_COMM < 32) ?
                                   (1 << BOLD_COMM)
                                   : (1 << (BOLD_COMM % 32)))) {
        fprintf(fd, "set bold_communication on\n");
    }

    if (options[BRACKETS / 32] & ((BRACKETS < 32) ?
                                  (1 << BRACKETS)
                                  : (1 << (BRACKETS % 32)))) {
        fprintf(fd, "set brackets on\n");
    }

    if (strcmp(client_prompt, DEFAULT_CLIENT_PROMPT)) {
        fprintf(fd, "set client_prompt \"%s\"\n", client_prompt);
    }

    if (options[CONNECT_STARTUP / 32] & ((CONNECT_STARTUP < 32) ?
                                         (1 << CONNECT_STARTUP)
                                         : (1 << (CONNECT_STARTUP)))) {
        fprintf(fd, "set connect on\n");
    }

    if (reconnect_delay) {
        fprintf(fd, "set connect_delay %d\n", reconnect_delay);
    }

#ifdef IMAP
    if (cursor_sector != DEFAULT_CURSOR_SECTOR) {
        fprintf(fd, "set cursor_sector %c\n", cursor_sector);
    }
#endif

    if (options[DISPLAY_TOP / 32] & ((DISPLAY_TOP < 32) ?
                                     (1 << DISPLAY_TOP)
                                     : (1 << (DISPLAY_TOP % 32)))) {
        fprintf(fd, "set display_from_top on\n");
    }

    if (options[ENCRYPT / 32] & ((ENCRYPT < 32) ?
                                 (1 << ENCRYPT)
                                 : (1 << (ENCRYPT % 32)))) {
        fprintf(fd, "set encrypt on\n");
    }

    if (entry_quote) {
        fprintf(fd, "set entry_quote %s\n", fstring(entry_quote));
    }

    if (exit_quote) {
        fprintf(fd, "set exit_quote %s\n", fstring(exit_quote));
    }

    if (options[FULLSCREEN / 32] & ((FULLSCREEN < 32) ?
                                    (1 << FULLSCREEN)
                                    : (1 << (FULLSCREEN % 32)))) {
        fprintf(fd, "set full_screen on\n");
    }

    if (strcmp(help_client, HELP_CLIENT)) {
        fprintf(fd, "set help %s\n", help_client);
    }

    if (options[HIDE_END_PROMPT / 32] & ((HIDE_END_PROMPT < 32) ?
                                         (1 << HIDE_END_PROMPT)
                                         : (1 << (HIDE_END_PROMPT % 32)))) {
        fprintf(fd, "set hide_end_prompt on\n");
    }

    if (max_history) {
        fprintf(fd, "set history %d\n", max_history);
    }

    if (strcmp(input_prompt, DEFAULT_INPUT_PROMPT)) {
        fprintf(fd, "set input_prompt \"%s\"\n", input_prompt);
    }

    if (options[LOGINSUPPRESS_STARTUP / 32] & ((LOGINSUPPRESS_STARTUP < 32) ?
                                               (1 << LOGINSUPPRESS_STARTUP)
                                               : (1 << (LOGINSUPPRESS_STARTUP % 32)))) {
        fprintf(fd, "set login_suppress on\n");
    }

    if (options[MAP_DOUBLE / 32] & ((MAP_DOUBLE < 32) ?
                                    (1 << MAP_DOUBLE)
                                    : (1 << (MAP_DOUBLE % 32)))) {
        fprintf(fd, "set map double on\n");
    }

    if (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                   (1 << MAP_SPACE)
                                   : (1 << (MAP_SPACE % 32)))) {
        fprintf(fd, "set map space on\n");
    }

    fprintf(fd, "set more %s\n", (more_val.on ? "on" : "off"));

    if (more_val.delay) {
        fprintf(fd, "set more_delay %d\n", more_val.delay);
    }

    if (options[NO_LOGOUT / 32] & ((NO_LOGOUT < 32) ?
                                   (1 << NO_LOGOUT)
                                   : (1 << (NO_LOGOUT % 32)))) {
        fprintf(fd, "set no_logout on\n");
    }

    if (options[NOCLOBBER / 32] & ((NOCLOBBER < 32) ?
                                   (1 << NOCLOBBER)
                                   : (1 << (NOCLOBBER % 32)))) {
        fprintf(fd, "set noclobber on\n");
    }

    if (notify > 0) {
        fprintf(fd, "set notify on\n");
    }

    if (notify) {
        fprintf(fd, "set notify_beeps %d\n", (notify > 0 ? notify : -notify));
    }

    if (strcmp(output_prompt, DEFAULT_OUTPUT_PROMPT)) {
        fprintf(fd, "set output_prompt \"%s\"\n", output_prompt);
    }

    if (input_mode.edit != EDIT_INSERT) {
        fprintf(fd, "set overwrite_edit_mode\n");
    }

    if (options[PARTIAL_LINES / 32] & ((PARTIAL_LINES < 32) ?
                                       (1 << PARTIAL_LINES)
                                       : (1 << (PARTIAL_LINES % 32)))) {
        fprintf(fd, "#set partial_lines on\n");
    }

    if (options[QUIT_ALL / 32] & ((QUIT_ALL < 32) ?
                                  (1 << QUIT_ALL)
                                  : (1 << (QUIT_ALL % 32)))) {
        fprintf(fd, "set quit_all on\n");
    }

    if (max_recall != DEFAULT_RECALL) {
        fprintf(fd, "set recall %d\n", max_recall);
    }

    if (options[SCROLL / 32] & ((SCROLL < 32) ?
                                (1 << SCROLL)
                                : (1 << (SCROLL % 32)))) {
        fprintf(fd, "set scroll on\n");
    }

    if (options[SCROLL_CLR / 32] & ((SCROLL_CLR < 32) ?
                                    (1 << SCROLL_CLR)
                                    : (1 << (SCROLL_CLR % 32)))) {
        fprintf(fd, "set scroll_clear on\n");
    }

    if (options[SHOW_ACTIONS / 32] & ((SHOW_ACTIONS < 32) ?
                                      (1 << SHOW_ACTIONS)
                                      : (1 << (SHOW_ACTIONS % 32)))) {
        fprintf(fd, "set show_actions on\n");
    }

    if (options[SHOW_CLOCK / 32] & ((SHOW_CLOCK < 32) ?
                                    (1 << SHOW_CLOCK)
                                    : (1 << (SHOW_CLOCK % 32)))) {
        fprintf(fd, "set show_clock on\n");
    }

    if (options[SHOW_MAIL / 32] & ((SHOW_MAIL < 32) ?
                                   (1 << SHOW_MAIL)
                                   : (1 << (SHOW_MAIL % 32)))) {
        fprintf(fd, "set show_mail on\n");
    }

    if (strcmp(status.format, DEFAULT_STATUS_BAR)) {
        fprintf(fd, "set status_bar %s\n", status.format);
    }

    if (strcmp(status.schar, DEFAULT_STATUS_BAR_CHAR)) {
        fprintf(fd, "set status_bar_char %c\n", status.schar[0]);
    }
}
