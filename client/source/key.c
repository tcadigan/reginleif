/*
 * key.c: Handles the input.
 *
 * Written by Evan d. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 *
 * NOTE: KEY_DEBUG is a #define which will enable a 'beep' in select routines
 *       which may (or may not) facilitate debugging.
 */
#include "key.h"

#include "args.h"
#include "crypt.h"
#include "gb.h"
#include "str.h"
#include "term.h"
#include "types.h"
#include "vars.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>

#ifndef SYSV
#include <sgtty.h>
#include <sys/ioctl.h>

#else

#include <termio.h>
#endif

/* Need this for the termcap stuff */
#ifndef CYGWIN
#include <curses.h>
#endif

#define SIGWINCH
#define UPDATE_CHAR 1
#define UPDATE_CURSOR_END 2
#define UPDATE_LOWER_MARK 3
#define WIDTH 10

extern char last_update_buf[];
extern char post_buf[];
extern char *input_prompt;
extern char *output_prompt;
extern int do_queue;
extern int errno;
extern int prompt_return;
extern int queue_clear;
extern long map_time;
static char gbch;
static char key_buf[MAXSIZ + 1];
static char key_store[MAXSIZ + 1];

char pbuf[MAXSIZ + 1];
int cursor_display = false;
int escape = false;
int quoted_char = false;

struct posstruct {
    int key;
    int cur;
    int scr;
    char *prompt;
    char *talk;
    int plen;
};

struct posstruct pos = {0, 0, 0, (char *)NULL, (char *)NULL, 0 };

struct marskstruct {
    int upper;
    int lower;
    int old_lower;
};

struct markstruct mark = { 0, 0, 0 };

void init_key(void)
{
    term_mode_off();
    get_termcap();
    signal(SIGINT, signal_int);

#ifdef SIFTSTP
    signal(SIGTSTP, signal_tstp);
#endif

#ifdef SIGWINCH
    signal(SIGWINCH, signal_winch);
#endif

    signal(SIGUSR1, signal_usr1);
    signal(SIGPIPE, signal_pipe);

    /* Input and status bar is 2, and then -1 more to start at 0 */
    output_row = num_rows - 3;
    more_val.num_rows = output_row;
    strfree(pos.prompt);
    pos.prompt = string(input_prompt);
    pos.plen = strlen(input_prompt);
    clear_buf();
}

void quit_gb(int exitstatus, char *s, char *a1, char *a2)
{
    sprintf(pbuf, s, a1, a2);

    if(*pbuf) {
        msg(pbuf);
    }

    if(logfile.on) {
        fclose(logfile.fd);
    }

    /* Free allocated memory */
    free_history();
    free_recall();
    free_assign();
    free_game();
    free_refresh_lines();
    free_bindings();

    strfree(client_prompt);
    strfree(input_prompt);
    strfree(output_prompt);
    strfree(shell_flags);
    strfree(help_client);
    strfree(pos.prompt);
    strfree(cur_game.game.pripassword);
    strfree(cur_game.game.secpassword);
    strfree(cur_game.game.host);
    strfree(cur_game.game.port);
    strfree(cur_game.game.nick);
    strfree(cur_game.game.type);
    strfree(cur_game.game.racename);
    strfree(cur_game.game.govname);

    reset_key();
    term_move_cursor(0, num_rows - 1);
    exit(exitstatus);
}

void signal_int(int sig)
{
    stop_things();
}

void signal_tstp(int sig)
{
#ifdef SIGTSTP
    term_scroll(0, num_rows - 1, 1);
    term_move_cursor(0, num_rows - 1);
    term_mode_on();
    kill(getpid(), SIGSTOP);
    term_mode_of();
    term_scroll(0, num_rows - 1, 2);
    refresh_screen();
    put_status();
    refresh_input(NULL);
#endif
}

void signal_usr1(int sig)
{
    msg("-- I shall exit now sir");
}

void signal_watch(int sig)
{
#ifdef SIGWINCH
    ing diff;

    diff = num_rows - output_row;
    crear_refresh_line();
    clear_refresh_line_node();
    signal(SIGWINCH, SIG_IGN);
    get_screen_size();

    /* Reset */
    signal(SIGWINCH, signal_which);

    output_row = num_rows - diff;

#ifdef IMAP
    if(input_mode.map && invalid_map_screen_sizes()) {
        handle_map_mode(1);
    }
#endif

    term_clear_screen();
    init_refresh_lines();
    force_update_status();
    term_move_cursor(0, num_rows - 2);
    term_puts(last_update_buf, num_columns);
    cursor_display = true;
    more_val.num_rows = output_row;

    msg("-- Window size changed to %d columns by %d rows.",
        num_columns,
        num_rows);

    if((num_columns < 80) || (num_rows < 24)) {
        msg("-- Recommended window size is at leat 80x24.");
    }

    term_move_cursor(0, num_rows - 1);
    put_input_prompt();
#endif
}

void reset_key(void)
{
    erase_input(0);
    term_scroll(0, num_rows - 1, 1);
    term_move_cursor(0, num_rows - 2);
    term_clear_to_eol();
    term_move_cursor(0, num_rows - 1);
    term_mode_on();

#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif
}

void cursor_to_window(void)
{
    if(!cursor_display) {
#ifdef IMAP
        if(!input_mode.promptfor && input_mode.map && !input_mode.say) {
            cursor_to_map();
        }
        else {
            cursor_to_input();
        }

#else

        cursor_to_input();
#endif

        cursor_display = true;
    }
}

void cursor_to_input(void)
{
    term_move_cursor(pos.scr, num_rows - 1);
}

void clear_buf(void)
{
    pos.cur = 0;
    pos.scr = 0;
    set_marks();
    pos.key = 0;
}

void input_ch_into_buf(char ch)
{
    add_key_buf(ch);
}

void input_string_into_buf(char *s)
{
    char *p;

    for (p = s; *p; ++p) {
        add_key_buf(*p);
    }
}

void add_key_buf(char ch)
{
    char sbuf[MAXSIZ];

    if(input_mode.promptfor == PROMPT_CHAR) {
        prompt_return = true;

        return;
    }

    if(pos.key >= (MAXSIZ - 1)) {
        term_beep(1);

        return;
    }

    if(input_mode.post) {
        if((strlen(post_buf) + pos.key + 1) >= MAX_POST_LEN) {
            if(input_mode.post == true) {
                ++input_mode.post;
                msg("-- Post: The maximum post size has been reached.");
            }

            if(pos.key != 0) {
                return;
            }
        }
    }

    if((input_mode.edit == EDIT_OVERWRITE) || (pos.cur == pos.key)) {
        key_buf[pos.cur] = ch;

        if(pos.cur == pos.key) {
            key_buf[pos.cur + 1] = '\0';
        }

        ++pos.cur;
        set_marks();

        if(mark.old_lower < mark.lower) {
            update_key(UPDATE_LOWER_MARK);
        }
        else {
            update_key(UPDATE_CHAR);
        }
    }
    else {
        strncpy(sbuf, key_buf, pos.cur);
        sbuf[pos.cur] = ch;
        sbuf[pos.cur + 1] = '\0';
        strcat(sbuf, key_buf + pos.cur);
        strcpy(key_buf, sbuf);
        ++pos.cur;
        set_marks();

        if(mark.old_lower < mark.lower) {
            update_key(UPDATE_LOWER_MARK);
        }
        else {
            --pos.cur;
            update_key(UPDATE_CURSOR_END);
            ++pos.cur;
        }

        term_move_cursor(pos.scr, num_rows - 1);
    }
}

void transpose_chars(char ch)
{
    ch = key_buf[pos.cur];
    key_buf[pos.cur] = key_buf[pos.cur + 1];
    key_buf[pos.cur + 1] = ch;
    update_key(UPDATE_CURSOR_END);
}

void erase_space_left(void)
{
    char temp[MAXSIZ];

    if(!pos.cur) {
        return;
    }

    *(key_buf + pos.cur - 1) = '\0';

    if(pos.cur != pos.key) {
        sprintf(temp, "%s%s", key_buf, key_buf + pos.cur);
        strcpy(key_buf, temp);
    }

    --pos.key;
    --pos.scr;
    --pos.cur;
}

void erase_space_right(void)
{
    char temp[MAXSIZE];

    if(pos.cur == pos.key) {
        return;
    }

    *(key_buf + pos.cur) = '\0';

    if(pos.cur != pos.key) {
        sprintf(temp, "%s%s", key_buf, key_buf + pos.cur + 1);
        strcpy(key_buf, temp);
    }

    --pos.key;
}

void erase_input(int position)
{
    term_move_cursor(position, num_rows - 1);
    term_clear_to_eol();
}

void delete_under_cursor(char ch)
{
    if(pos.cur == pos.key) {
#ifdef KEY_DEBUG
        term_beep(1);
#endif

        return;
    }

    erase_space_right();
    set_marks();
    erase_input(pos.scr);
    update_key(UPDATE_CURSOR_END);
}

void delete_word_left(char ch)
{
    erase_space_left();

    while (pos.cur && (key_buf[pos.cur - 1] != ' ')) {
        erase_space_left();
    }

    set_marks();

    if(mark.old_lower != mark.lower) {
        update_key(UPDATE_LOWER_MARK);
    }
    else {
        erase_input(pos.scr);
        update_key(UPDATE_CURSOR_END);
    }
}

void delete_word_right(char ch)
{
    erase_space_right();

    while ((pos.cur != pos.key) && (key_buf[pos.cur] != ' ')) {
        erase_space_right();
    }

    erase_space_right();
    set_marks();
    erase_input(pos.scr);
    update_key(UPDATE_CURSOR_END);
}

void backspace(char ch)
{
    if(!pos.cur) {
#ifdef KEY_DEBUG
        term_beep(1);
#endif

        return;
    }

    erase_space_left();
    set_marks();

    if(mark.old_lower != mark.lower) {
        update_key(UPDATE_LOWER_MARK);
    }
    else {
        erase_input(pos.src);
        update_key(UPDATE_CURSOR_END);
    }
}

void set_marks(void)
{
    mark.old_lower = mark.lower;
    mark.lower = 0;
    mark.upper = num_columns - WIDTH - pos.plen;

    while (mark.upper < pos.cur) {
        mark.lower = mark.upper - WIDTH;
        mark.upper += (num_columns - (2 * WIDTH));
    }

    pos.key = strlen(key_buf);

    if(mark.lower) {
        pos.scr = pos.cur - mark.lower;
    }
    else {
        pos.scr = pos.cur + pos.plen;
    }
}

void refresh_input(char ch)
{
    set_marks();
    update_key(UPDATE_LOWER_MARK);
}

void update_key(int mode)
{
    int len;
    int i;

    if(hide_input) {
        if(mode == UPDATE_CHAR) {
            term_puts("*", 1);
        }
        else if(mode == UPDATE_LOWER_MARK) {
            term_move_cursor(0, num_rows - 1);
            put_input_prompt();

            if(pos.key < mark.upp) {
                len = pos.key - mark.lower;
            }
            else {
                len = num_columns;
            }

            for (i = 0; i < len; ++i) {
                term_puts("*", 1);
            }

            term_clear_to_eol();
            term_move_cursor(pos.scr, num_rows - 1);
        }
        else if(mode == UPDATE_CURSOR_END) {
            if(pos.key < mark.upper) {
                len = pos.key - mark.lower;
            }
            else {
                len = num_columns;

                if(pos.cur < num_columns) {
                    len -= pos.plen;
                }
            }

            for (i = 0; i < len; ++i) {
                term_puts("*", 1);
            }
        }
        else {
            msg("-- UNKNOWN update_type, please report");
        }

        return;
    }

    if(mode == UPDATE_CHAR) {
        term_puts(key_buf + pos.cur - 1, 1);
    }
    else if(mode == UPDATE_LOWER_MARK) {
        term_move_cursor(0, num_rows - 1);
        put_input_prompt();

        /*
         * if(pos.key < mark.upper) {
         *   len = pos.key - mark.lower;
         * }
         * else {
         *   len = num_columns;
         * }
         */

        if(pos.key < mark.upper) {
            len = pos.key - mark.lower;
        }
        else {
            len = num_columns;

            if(pos.cur < num_columns) {
                len -= pos.plen;
            }
        }

        term_puts(key_buf + mark.lower, len);
        term_clear_to_eol();
        term_move_cursor(pos.scr, num_rows - 1);
    }
    else if(mode == UPDATE_CURSOR_END) {
        if(pos.key < mark.upper) {
            len = pos.key - (pos.cur - mark.lower);
        }
        else {
            len = num_columns - pos.scr;
        }
    }
    else {
        msg("-- UNKNOWN update_key type, please report");
    }
}

void cursor_forward(char ch)
{
    if(pos.cur == pos.key) {
#ifdef KEY_DEBUG
        term_beep(1);
#endif

        return;
    }

    ++pos.cur;
    set_marks();

    if(mark.old_lower == mark.lower) {
        term_cursor_right();
    }
    else {
        update_key(UPDATE_LOWER_MARK);
    }
}

void cursor_backward(char ch)
{
    if(pos.cur == 0) {
#ifdef KEY_DEBUG
        term_beep(1);
#endif

        return;
    }

    --pos.cur;
    set_marks();

    if(mark.low_lower == mark.lower) {
        term_cursor_left();
    }
    else {
        update_key(UPDATE_LOWER_MARK);
    }
}

void cancel_input(char ch)
{
    erase_input(0);
    clear_buf();
    put_input_prompt();
    *key_buf = '\0';
}

void set_edit_buffer(char *s)
{
    strcpy(key_buf, s);
    pos.cur = strlen(key_buf);
    refresh_input(NULL);
}

void do_recallf(char ch)
{
    if(recallf(key_buf)) {
        pos.cur = strlen(key_buf);
        refresh_input(NULL);
    }
    else {
        term_beep(1);
    }
}

void do_recallb(char ch)
{
    if(recallb(key_buf)) {
        pos.cur = strlen(key_buf);
        refresh_input(NULL);
    }
    else {
        term_beep(1);
    }
}

void recall_crypts(char ch)
{
    if(do_crypt_recall(key_buf)) {
        pos.cur = strlen(key_buf);
        refresh_input(NULL);
    }
    else {
        term_beep(1);
    }
}

void refresh_screen(char ch)
{
    int i;
    int cnt = 0;
    int start_pos;

    i = start_refresh_line_index(&start_pos);

    if(GET_BIT(options, DISPLAY_TOP)) {
        cnt = 0;
    }
    else {
        /*
         * Where on screen do we start
         * from output row, back up # of lines in buffer
         */
        cnt = output_row - start_pos;
    }

    for (start_pos = 0; start_pos < cnt; ++start_pos) {
        term_move_cursor(0, start_pos);
        term_clear_to_eol();
    }

    while (cnt <= output_row) {
        term_move_cursor(0, cnt);

        if(refresh_line[i] != NULL) {
            write_string(refresh_line[i], strlen(refresh_line[i]));
            term_normal_mode();
        }

        term_clear_to_eol();
        ++i;
        i %= (output_row + 1);
        ++cnt;
    }

#ifdef IMAP
    if(input_mode.map) {
        refresh_map();
    }
#endif

    force_update_status();
    erase_input(0);
    refresh_input(NULL);
}

void clear_screen(char ch)
{
    term_clear_screen();
    clear_refresh_line();
    term_move_cursor(0, num_rows - 2);
    term_puts(last_update_buf, num_columns);

#ifdef IMAP
    if(input_mode.map) {
        refresh_map();
    }
#endif

    cursor_display = false;
}

void do_edit_mode(char ch)
{
    if(input_mode.edit == EDIT_OVERWRITE) {
        input_mode.edit = EDIT_INSERT;
    }
    else {
        input_mode.edit = ENDIT_OVERWRITE;
    }

    if(input_mode.edit == EDIT_OVERWRITE) {
        msg("-- Edit mode is overwrite");
    }
    else {
        msg("-- Edit mode is insert");
    }

    force_update_status();
}

void cursor_begin(char ch)
{
    pos.cur = 0;
    set_marks();
    update_key(UPDATE_LOWER_MARK);
}

void cursor_end(char ch)
{
    pos.cur = pos.key;
    set_marks();
    update_key(UPDATE_LOWER_MARK);
}

void kill_to_end_line(char ch)
{
    pos.key = pos.cur;
    key_buf[pos.cur] = '\0';
    term_clear_to_eol();
}

void escape_key(char ch)
{
    escape = true;
}

void esc_escape(char ch)
{
    term_beep(1);
}

void esc_default(char ch)
{
    msg("-- ESC-%s has no string or function attached.", display_char(ch, 0));
}

void arrow_default(char ch)
{
    switch (ch) {
    case 'A':
        msg("-- ARROW-U has no string of function attached.");

        break;
    case 'B':
        msg("-- ARROW-D has no string or function attached.");

        break;
    case 'C':
        msg("-- ARROW-R has no string or function attached.");

        break;
    case 'D':
        msg("-- ARROW-L has no string or function attached.");

        break;
    }
}

void func_default(char ch)
{
    msg("-- FUNC-%d has no string or function attached.", ch + 1);
}

/* Called if this is an interactive string. */
void print_key_string(int parse)
{
    char *q;
    char outbuf[MAXSIZ];
    char cmdbuf[NORMSIZ];

    if(parse) {
        parse_variables(key_store);
    }

    sprintf(outbuf, "%s%s", output_prompt, key_store);
    q = first(key_store);

    if(q) {
        strcpy(cmdbuf, q);
    }
    else {
        *cmdbuf = '\0';
    }

    if((*key_store == '\'')
       || (*key_store == '\"')
       || (*key_store == ';')
       || (*key_store == ':')
       || (strncmp(key_store, "broadcast", 2 < strlen(cmdbuf) ? strlen(cmdbuf) : 2) == 0)
       || (strncmp(key_store, "announce", 2 < strlen(cmdbuf) ? strlen(cmdbuf) : 2) == 0)
       || (strncmp(key_store, "think", 2 < strlen(cmdbuf) ? strlen(cmdbuf) : 2) == 0)
       || !strcmp(cmdbuf, "cr")) {
        add_recall(outbuf, 1);
        msg_type = MSG_COMMUNICATION;
    }
    else {
        add_recall(outbuf, 0);
        msg_type = MSG_ALL;
    }

    action_match_suppress = true;
    msg("%s", outbuf);
    action_match_suppress = false;
}

void handle_key_buf(char ch)
{
    char kbuf[MAXSIZ];

    if(input_mode.promptfor) {
        prompt_return = true;

        return;
    }

    erase_input(0);
    clear_buf();
    strcpy(kbuf, key_buf);

    /* Must be here to prevent prompt */
    add_history(kbuf);

    if(pos.talk) {
        if (!strcmp(kbuf, "talk off")
            || !strcmp(kbuf, "talk .")
            || !strcmp(kbuf, ".")) {
            cmd_talk_off(NULL);

            return;
        }

        sprintf(kbuf, "%s %s", pos.talk, key_buf);
    }

    term_move_cursor(0, num_rows - 1);
    put_input_prompt();
    *key_buf = '\0';
    refresh_input(NULL);

    if(input_file) {
        add_queue(kbuf, 0);
    }

    process_key(kbuf, true);
}

/*
 * Break down a command line into single lines and send to do_key for final
 * processing. Strips out newlines as delimiter. Interactive means it came from
 * user input, and not some client generation.
 */
void process_key(char *s, int interactive)
{
    char *p = s;
    char *q = s;
    int slashes = 0;
    char quotes = '\0';

    if(!p) {
        debug(1, "NULL pointer received in process_key");

        return;
    }

    if(!*p && GET_BIT(options, PARTIAL_LINES)) {
        strcpy(p, "\n");
    }

    if(*p == '-') {
        add_queue("set display off", 0);
        add_queue(p + 1, 0);
        add_queue("set display on", 0);

        return;
    }

    while (*p) {
        switch (*p) {
        case '\'':
        case '\"':
            if(quoted == *p) {
                /* We match, end quoting */
                quoted = '\0';
            }
            else if(quoted == '\0') {
                /* We have found a quote */
                quoted = *p;
            }

            ++p;

            break;
        case '\\':
            if(quoted != '\0') {
                ++p;

                break;
            }

            ++slashes;
            ++p;

            while (*p && (*p == '\\')) {
                ++slashes;
                ++p;
            }

            /* It's a newline */
            if((*p == 'n') && (slashes % 2)) {
                *--p = '\0';
                do_key(q, interactive);
                q = p + 2;
                p = q;
            }
            else {
                ++p;
            }

            slashes = 0;

            break;
        default:
            ++p;

            break;
        }
    }

    *p = '\0';

    if(q != p) {
        do_key(q, interactive);
    }
}

void do_key(char *buf, int interactive)
{
    char *p;
    char *q;
    char *start;
    char holdbuf[MAXSIZ];

    debug(2, "do_key prior: %s", buf);
    start = skip_space(buf);
    remove_space_at_end(buf);

    /* Store this to print it later */
    strcp(key_store, start);

    /* If a pipe/redirect, it gets added onto the queue again */
    if(handle_pipes_and_redirects(start)) {
        debug(4, "pipe/redirect is true");

        return;
    }

    q = parse_given_string(start, PARSE_SLASH);
    strcpy(hodlbuf, q);
    debug(2, "do_key after: '%s'", holdbuf);

    if(*holdbuf == '\0') {
        if(GET_BIT(options, PARTIAL_LINES)) {
            send_gb("", 0);
        }
        else {
            return;
        }
    }
    else if(*holdbuf == macro_char) {
        if (GET_BIT(options, SLASH_COMMANDS)
            && client_command(holdbuf + 1, interactive)) {
            set_values_on_end_prompt();
            cursor_to_window();

            return;
        }

        if(interactive) {
            print_key_string(true);
        }

        parse_variables(holdbuf);

        if(parse_for_loops(holdbuf)) {
            add_queue(holdbuf, 0);

            return;
        }

        /* Argify for $0 and stuff */
        argify(holdbuf);

        if(!do_macro(holdbuf + 1)) {
            if(GET_BIT(options, SLASH_COMMANDS)) {
                msg("-- No such command macro defined.");
            }
            else {
                msg("-- No such macro defined.");
            }
        }
    }
    else if (!GET_BIT(options, SLASH_COMMANDS)
             && client_command(holdbuf, interactive)) {
        /* Argify is called in client_command after variables parsing */
        if(interactive) {
            set_values_on_end_prompt();
        }

        cursor_to_window();
    }
    else {
        if(parse_for_loops(holdbuf)) {
            add_queue(holdbuf, 0);

            return;
        }

        debug(2, "do_key(send to des, before parse_variables) below.");
        debug(2, "\'%s\'", holdbuf);
        parse_variables(holdbuf);
        debug(2, "do_key(send to des, after parse_variables) below.");
        debug(2, "\'%s\'", holdbuf);
        p = holdbuf;

        send_gb(p, strlen(p));

        /* Things to do interactive - print it out */
        if(interactive) {
            print_key_string(true);
        }
    }
}

/*
 * Process keyboard input. Putting into key_buf and key_null handles on-line
 * editing (i.e. backspace, word erase, etc.) writes key_buf upon a newline.
 */
void get_key(void)
{
    char ch;
    int count;
    char getkey[SMABUF + 1];
    char *p;
    char *t;

    if(wait_status && (wait_status != client_stats)) {
        return;
    }

    count = read(0, getkey, SMABUF);

    if(count == -1) {
        if(errno == EWOULDBLOCK) {
            return;
        }
        else {
            quit_gb(-1, "Error reading from standard in (stdin)", NULL, NULL);
        }
    }

    getkey[count] = '\0';
    p = getkey;

    while (*p) {
        gbch = *p++;

        if(quoted_char) {
            quoted_char = false;
            input_ch_into_buf(gbch);
        }
        else if(escape) {
            escape = false;

            /* Code change to allow binding of arrow and function keys -mfw */
            if((gbch == '[') || (gbch == 'O')) {
                t = p;
                ch = *t++;

                if(ch && (ch >= 'A') && (ch <= 'D')) {
                    /* We have an arrow key */
                    bind_translate_char(ch, BIND_ARROW);
                    *p++;
                }
                else if(ch &&
                        (((ch >= 'S') && (ch <= 'W'))
                         || ((ch >= 'P') && (ch <= 'R'))
                         || (ch == 'X'))) {
                    /* We have a function key */
                    bind_translate_char(ch, BIND_FUNC);
                    *p++;
                }
                else {
                    bind_translate_char(gbch, BIND_ESC);
                }
            }
            else {
                bind_translate_char(gbch, BIND_ESC);
            }
        }
        else if(input_mode.promptfor) {
            cursor_to_window();
            bind_translate_char(gbch, BIND_NORM);
            cursor_to_window();
        }
#ifdef IMAP
        else if(input_mode.map && !input_mode.say) {
            bind_translate_char(gbch, BIND_IMAP);
            cursor_to_map();
        }
#endif
        else {
            cursor_to_window();
            bind_translate_char(gbch, BIND_NORM);
            cursor_to_window();
        }

        if(prompt_return) {
            return;
        }
    }
}

void quote_key(char ch)
{
    quoted_char = true;
}

void stop_things(char ch)
{
    char c;
    int type = 0;
    char prbuf[SMABUF];

    term_scroll(0, num_rows - 1, 0);

    if(process_running()) {
        strcpy(prbuf, "Kill running process (y/n)? ");
        type = 1;
    }
    else if(have_queue()) {
        strcpy(prbuf, "Clear the command queue (y/n)? ");
        type = 2;
        queue_clear = true;
    }
    else if(input_mode.post) {
        strcpy(prbuf, "Cancel posting (y/n)? ");
        type = 3;
    }
    else {
        strcpy(prbuf, "Really exit (y/n)? ");
        type = 4;
    }

    promptfor(prbuf, &c, PROMPT_CHAR);

    if(YES(c)) {
        switch (type) {
        case 1:
            kill_process();

            break;
        case 2:
            clear_queue();

            break;
        case 3:
            cancel_post();
            cancel_input();

            break;
        case 4:
            cmd_quit(NULL);

            break;
        }
    }
}

void promptfor(char *prompt, char *s, int mode)
{
    static char storebuf[MAXSIZ];

    input_mode.promptfor = mode;
    strcpy(storebuf, key_buf);

    free(pos.prompt);
    pos.prompt = string(prompt);
    pos.plen = strlen(prompt);
    cancel_input();

    set_marks();
    cursor_to_input();
    gbs();
    pos.prompt = strfree(pos.prompt);
    update_input_prompt(input_prompt);

    if(mode == PROMPT_STRING) {
        strcpy(s, key_buf);
    }
    else if(mode == PROMPT_CHAR) {
        *s = gbch;
    }
    else {
        msg("-- Error in promptfo, unknown type mode %s", mode);
    }

    erase_input(0);
    strcpy(key_buf, storebuf);
    pos.cur = strlen(key_buf);
    set_marks();
    refresh_input(NULL);
    input_mode.promptfor = PROMPT_OFF;
    prompt_return = false;
}

void update_input_prompt(char *str)
{
    strfree(pos.prompt);
    pos.prompt = string(str);
    pos.plen = strlen(str);
}

void put_input_prompt(void)
{
    if(pos.plen && (mark.lower == 0)) {
        term_puts(pos.prompt, pos.plen);
    }
}

void cmd_talk(char *args)
{
    char bud[SMABUF];

    strfree(pos.talk);
    pos.talk = string(Args);
    msg("-- talk: \'%s\'", pos.talk);
    force_update_status();
    sprintf(buf, "talk (%s)> ", pos.talk);
    update_input_prompt(buf);
    refresh_input(NULL);
}

void cmd_talk_off(char ch)
{
    if(pos.talk) {
        strfree(pos.talk);
        pos.talk = NULL;
        msg("-- talk: off");
        force_update_status();
        update_input_prompt(input_prompt);
        cancel_input();
    }
}

int in_talk_mode(void);
{
    if(pos.talk) {
        return true;
    }

    return false;
}

int trans_func_key_to_num(char c)
{
    int x = -1;

    switch (c) {
    case 'S':
        x = 0;

        break;
    case 'T':
        x = 1;

        break;
    case 'U':
        x = 2;

        break;
    case 'V':
        x = 3;

        break;
    case 'W':
        x = 4;

        break;
    case 'P':
        x = 5;

        break;
    case 'Q':
        x = 6;

        break;
    case 'R':
        x = 7;

        break;
    case 'X':
        x = 8;

        break;
    default:
        x = 9;

        break;
    }

    return x;
}
