/*
 * str.c: Handles the string manipulation, as well as matching.
 *        Format output by wrapping around columns
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "str.h"

#include "action.h"
#include "bind.h"
#include "gb.h"
#include "key.h"
#include "option.h"
#include "proc.h"
#include "socket.h"
#include "types.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Using strerror() now -mfw */
/* extern int sys_nerr; */
/* extern char *sys_errlist[]; */
extern int kill_socket_output;
extern int pipe_output;

int kill_client_output = false;
int refresh_next = 0;
int debug_level = 0;
char *client_prompt;
char *input_prompt;
char *output_prompt;
char column_maker_buf[MAXSIZ]; /* For building the columns */
int column_maker_pos; /* Indexing location columns */
int column_maker_width;

/* Results for pattern matcher */
char pattern1[MAXSIZ];
char pattern2[MAXSIZ];
char pattern3[MAXSIZ];
char pattern4[MAXSIZ];
char pattern5[MAXSIZ];
char pattern6[MAXSIZ];
char pattern7[MAXSIZ];
char pattern8[MAXSIZ];
char pattern9[MAXSIZ];
char pattern10[MAXSIZ];
char pattern11[MAXSIZ];
char pattern12[MAXSIZ];
char pattern13[MAXSIZ];
char pattern14[MAXSIZ];
char pattern15[MAXSIZ];
char pattern16[MAXSIZ];
char pattern17[MAXSIZ];
char pattern18[MAXSIZ];
char pattern19[MAXSIZ];
char pattern20[MAXSIZ];

char *pattern[] = {
    pattern1, pattern2, pattern3, pattern4,
    pattern5, pattern6, pattern7, pattern8,
    pattern9, pattern10, pattern11, pattern12,
    pattern13, pattern14, pattern15, pattern16,
    pattern17, pattern18, pattern19, pattern20
};

char *skip_space(char *s);
void add_refresh_line(char *s, int cnt);
void display_msg(char *s);
int more(void);
void place_string_on_output_window(char *str, int len);
void write_string(char *s, int cnt);
void debug(int level, char *fmt, ...);
void msg(char *fmt, ...);
void msg_error(char *fmt, ...);
int has_esc_codes(char *str);

/* Returns a pointer to a static containing the first word of a string */
char *first(char *str)
{
    static char buf[MAXSIZ];
    char *s = buf;

    if (!str) {
        return NULL;
    }

    str = skip_space(str);

    if (*str == '\"') {
        /* Copy from " to " */
        ++str;

        while (*str && (*str != '\"')) {
            *s++ = *str++;
        }
    } else {
        /* Copy from here to first whitspace */
        while (*str && !isspace(*str)) {
            *s++ = *str++;
        }
    }

    *s = '\0';
    s = buf;

    return s;
}

/* Returns the rest of a sting. I.e. skips over the first word. */
char *rest(char *str)
{
    if (!str) {
        return NULL;
    }

    str = skip_space(str);

    if (!*str) {
        return NULL;
    }

    if (*str == '\"') {
        /* Copy from " to " */
        ++str;

        while (*str && (*str != '\"')) {
            ++str;
        }

        ++str;
    } else {
        /* Copy from here to first whitespace */
        while (*str && !isspace(*str)) {
            ++str;
        }
    }

    str = skip_space(str);

    if (!*str) {
        return NULL;
    }

    return str;
}

/*
 * Splits the string into first and rest buffers.
 *
 * NOTE: You must send valid sized buffers
 */
void split(char *s, char *fbuf, char *rbuf)
{
    char *p;

    p = first(s);

    if (p) {
        strcpy(fbuf, p);
    } else {
        *fbuf = '\0';
    }

    p = rest(s);

    if (p) {
        strcpy(rbuf, p);
    } else {
        *rbuf = '\0';
    }
}

/* This function skips the leading white space of a string. */
char *skip_space(char *s)
{
    char *str = s;

    if (!str) {
        return NULL;
    }

    while (*str && isspace(*str)) {
        ++str;
    }


    return str;
}

/*
 * fstring: Prepares a string, generally for writing to a file, by doubling up
 *          the backslashes. Generated so strings could be read in correctly
 *          from the GBRC
 */
char *fstring(char *str)
{
    static char buf[MAXSIZ];
    char *p;
    char *q;

    for (p = str, q = buf; *p; *q++ = *p++) {
        if (*p == '\\') {
            *q = '\\';
        }
    }

    *q = '\0';

    return buf;
}

/*
 * patterm_match: Given a string1, a string2 and a pattern containing one of
 *                more embedded stars (*) (which mean any numb of characters)
 *                return true if the match succeeds, and set patter[i] to the
 *                characters matched by the i-th *.
 */
int pattern_match(char *string1, char *string2, char **pattern)
{
    char *star = 0;
    char *starend = NULL;
    char *resp = NULL;
    int nres = 0;

    while (1) {
        if (*string2 == '*') {
            star = ++string2; /* Pattern after * */
            starend = string1; /* Data after * match */
            resp = pattern[nres++]; /* Result string */
            *resp = '\0'; /* Initially NULL */
        } else if (*string1 == *string2) { /* Characters match */
            if (*string2 == '\0') { /* Pattern match */
                return 1;
            }

            ++string2; /* Try next position */
            ++string1;
        } else {
            if (*string1 == '\0') { /* Pattern fails - */
                return 0; /* No more string1 */
            }

            if (star == 0) { /* Pattern fails - no * */
                return 0; /* To adjust */
            }

            string2 = star; /* Restart pattern after * */
            *resp++ = *starend; /* Copy char to result */
            *resp = '\0'; /* Null terminate */
            string1 = ++starend; /* Rescan copied char */
        }
    } /* while */
}

/*
 * Converts the given string to upper case, returning a pointer to the
 * static buffer. It leaves the original string unchanged.
 */
char *strtou(char *str)
{
    static char upper[MAXSIZ];
    char *p = str;
    char *q = upper;

    while (*p) {
        if (islower(*p)) {
            *q++ = toupper(*p);
        } else {
            *q++ = *p;
        }

        ++p;
    }

    *q = '\0';

    return upper;
}

/* Makes a string via malloc and does some error checking */
char *string(char *str)
{
    char *s;

    s = (char *)malloc(strlen(str) + 1);

    if (s) {
        strcpy(s, str);

        return s;
    } else {
        quit_gb(-2, "-- Could not malloc memory in makestring.", NULL, NULL);
    }

    /* Redundant but shuts the compiler up */
    return NULL;
}

char *maxstring(char *str)
{
    char *s;
    int len;

    len = (strlen(str) % MAXSIZ) + 1;
    s = (char *)malloc(len + 1);

    if (s) {
        strcpy(s, str);

        return s;
    } else {
        quit_gb(-2, "-- Could not mallow memory in maxstring.", NULL, NULL);
    }

    /* Redundant but shuts the compiler up */
    return NULL;
}

/* Wrap a string to num_columns based on spaces */
int wrap(char *line)
{
    char s[MAXSIZ];
    char temp[MAXSIZ];
    char out[MAXSIZ];
    char *place;
    char ch;
    char *p;
    int i;
    int first_line = true;
    int num_lines = 0;
    int len;
    int flag = 0;

    if (detached) {
        return 0;
    }

    strcpy(s, line);

    if (*s == '\n') {
        cursor_output_window();
        scroll_output_window();
        add_refresh_line("", 1);

        return 0;
    }

    if (options[BRACKETS / 32] & ((BRACKETS < 32) ?
                                  (1 << BRACKETS)
                                  : (1 << (BRACKETS % 32)))) {
        for (p = s; *p; ++p) {
            switch (*p) {
            case '[':
            case '{':
                *p = '(';

                break;
            case ']':
            case '}':
                *p = ')';

                break;
            default:

                break;
            }
        }
    }

    if ((strlen(s) <= num_columns) || has_esc_codes(s)) {
        if (more()) {
            return 0;
        }

        place_string_on_output_window(s, strlen(s));

        return 1;
    }

    strcpy(temp, s);

    ++num_lines;
    p = temp + num_columns - 1;
    ch = *p;
    *p = '\0';
    place = p - 1;

    while (!isspace(*place) && (place != temp)) {
        --place;
    }

    if (place == temp) {
        place = NULL;
    }

    /* Can't do it, give up */
    if (place == NULL) {
        p = s;
        i = strlen(p);

        while (i > (num_columns - 1)) {
            if (more()) {
                return 0;
            }

            if (!first_line) {
                *out = '+';
                strncpy(out + 1, p, num_columns - 1);
            } else {
                first_line = false;
                strncpy(out, p, num_columns);
            }

            place_string_on_output_window(out, num_columns);
            p = p + num_columns - 1;
            i = strlen(p);
        }

        strcpy(s, p);
        flag = 1;
    } else {
        if (more()) {
            return 0;
        }

        *p = ch;
        *place = '\0';
        p = place + 1;
        len = strlen(temp);

        if (!first_line) {
            *out = '+';
            strncpy(out + 1, temp, len);
            ++len;
        } else {
            first_line = false;
            strncpy(out, temp, len);
        }

        out[len] = '\0';
        place_string_on_output_window(out, len);
    }

    p = skip_space(p);
    strcpy(temp, p); /* Rest of string */
    strcpy(s, temp);

    while (!flag && (strlen(s) > num_columns)) {
        ++num_lines;
        p = temp + num_columns - 1;
        ch = *p;
        *p = '\0';
        place = p -1;

        while (!isspace(*place) && (place != temp)) {
            --place;
        }

        if (place == temp) {
            place = NULL;
        }

        /* Can't do it, give up */
        if (place == NULL) {
            p = s;
            i = strlen(p);

            while (i > (num_columns - 1)) {
                if (more()) {
                    return 0;
                }

                if (!first_line) {
                    *out = '+';
                    strncpy(out + 1, p, num_columns - 1);
                } else {
                    first_line = false;
                    strncpy(out, p, num_columns);
                }

                place_string_on_output_window(out, num_columns);
                p = p + num_columns - 1;
                i = strlen(p);
            }

            strcpy(s, p);

            flag = 1;
        } else {
            if (more()) {
                return 0;
            }

            *p = ch;
            *place = '\0';
            p = place + 1;
            len = strlen(temp);

            if (!first_line) {
                *out = '+';
                strncpy(out + 1, temp, len);
                ++len;
            } else {
                first_line = false;
                strncpy(out, temp, len);
            }

            out[len] = '\0';
            place_string_on_output_window(out, len);
        }

        p = skip_space(p);
        strcpy(temp, p); /* Rest of string */
        strcpy(s, temp);
    }

    len = strlen(s);

    if (len != 0) {
        if (more()) {
            return 0;
        }

        if (!first_line) {
            *out = '+';
            strncpy(out + 1, s, len);
            ++len;
        } else {
            first_line = false;
            strncpy(out, s, len);
        }

        out[len] = '\0';
        place_string_on_output_window(out, len);
        ++num_lines;
    }

    fflush(stdout);

    return num_lines;
}

int more(void)
{
    long present_time;
    static char more_buf[] = "-- more --";
    char c;

    if (!more_val.on
        || !more_val.delay
        || more_val.non_stop
        || kill_client_output
        || kill_socket_output
        || (client_stats < L_BOOTED)) {
        return 0;
    }

    /* If we are doing a forward then skip if need be */
    if (more_val.forward) {
        if (more_val.forward < (more_val.num_rows - 1)) {
            ++more_val.forward;

            return 1;
        } else {
            more_val.forward = false;
            more_val.num_lines_scrolled = 0;
        }
    }

    /* Too many lines in specified time? If not continue */
    present_time = time(0);

    if (present_time > (more_val.last_line_time + more_val.delay)) {
        more_val.num_lines_scrolled = 1;
        more_val.last_line_time = present_time;

        return 0;
    }

    /* Not too many lines yet...so print out the line */
    ++more_val.num_lines_scrolled;

    if (more_val.num_lines_scrolled < more_val.num_rows) {
        return 0;
    }

    /* Oops..too many lines...prompt the more */
    paused = true;
    promptfor(more_buf, &c, PROMPT_CHAR);
    paused = false;

    if (c == more_val.k_quit) {
        /* Prevent problems if prompt triggers */
        ++kill_client_output;
        ++kill_socket_output;

        return 1;
    } else if (c == more_val.k_forward) {
        more_val.forward = true;

        return 1;
    } else if ((c == more_val.k_clear)
               || (c == more_val.k_cancel)
               || (c == more_val.k_nonstop)
               || (c == more_val.k_oneline)) {
        bind_translate_char(c, BIND_MORE);
    } else {
        more_val.num_lines_scrolled = 0;
    }

    refresh_input('\0');
    more_val.last_line_time = time(0);

    return 0;
}

void msg(char *fmt, ...)
{
    va_list vargs;
    char buf[MAXSIZ];
    char *p;

    if (!(options[DISPLAYING / 32] & ((DISPLAYING < 32) ?
                                      (1 << DISPLAYING)
                                      : (1 << (DISPLAYING % 32))))
        || paused) {
        return;
    }

    va_start(vargs, fmt);
    vsprintf(buf, fmt, vargs);
    va_end(vargs);

    if (!action_match_suppress) {
        p = buf;

        /* So '-- A' and 'A' don't match twice */
        if (!strncmp(buf, "-- ", 3)) {
            p += 3;
        }

        if (handle_action_matches(p)) {
            return;
        }
    }

    if (kill_client_output) {
        if (logfile.redirect && logfile.on) {
            fprintf(logfile.fd, "%s\n", buf);
        }

        return;
    }

    display_msg(buf);
}

void display_msg(char *s)
{
    char store[MAXSIZ];

    if (!(options[DISPLAYING / 32] & ((DISPLAYING < 32) ?
                                      (1 << DISPLAYING)
                                      : (1 << (DISPLAYING % 32))))
        || paused) {
        return;
    }

    if (pipe_running && !pipe_output) {
        strcpy(store, s);
    }

    if (logfile.on && (logfile.level >= msg_type)) {
        fprintf(logfile.fd, "%s\n", s);
    }

    wrap(s);
    term_normal_mode();

    if (pipe_running && !pipe_output) {
        send_process(store);
    }
}

void display_bold_communication(char *s)
{
    char *p;
    char ch;
    char temp[MAXSIZ];

    if (strchr(s, '>')) {
        p = strchr(s, '>');
        ch = *p;
        --p;
        *p = '\0';
        sprintf(temp, "%c%s%c %c%s", BOLD_CHAR, s, BOLD_CHAR, ch, p + 2);
        display_msg(temp);
    } else if (strchr(s, ':')) {
        p = strchr(s, ':');
        ch = *p;
        --p;
        *p = '\0';
        sprintf(temp, "%c%s%c %c%s", BOLD_CHAR, s, BOLD_CHAR, ch, p + 2);
        display_msg(temp);
    } else if (strchr(s, '=')) {
        p = strchr(s, '=');
        ch = *p;
        --p;
        *p = '\0';
        sprintf(temp, "%c%s%c %c%s", BOLD_CHAR, s, BOLD_CHAR, ch, p + 2);
        display_msg(temp);
    } else if (strchr(s, '!')) {
        p = strchr(s, '!');
        ch = *p;
        --p;
        *p = '\0';
        sprintf(temp, "%c%s%c %c%s", BOLD_CHAR, s, BOLD_CHAR, ch, p + 2);
        display_msg(temp);
    } else {
        display_msg(s);
    }
}

void msg_error(char *fmt, ...)
{
    va_list vargs;
    char buf[MAXSIZ];

    if (!(options[DISPLAYING / 32] & ((DISPLAYING < 32) ?
                                      (1 << DISPLAYING)
                                      : (1 << (DISPLAYING % 32))))
        || paused) {
        return;
    }

    va_start(vargs, fmt);
    vsprintf(buf, fmt, vargs);
    va_end(vargs);

    /* -mfw
     * if(errno < sys_nerr) {
     *     msg("%s %s", buf, sys_errlist[errno]);
     * }
     * else {
     *      msg("%s - Unknown error. Report Error #%d.", buf, errno);
     * }
     */

    msg("%s %s", buf, strerror(errno));
}

/*
 * Debugging function
 * If the current debug level is equal to or greater than the level of the
 * given debug statement, then display it else do nothing.
 *
 * Current debug levels are:
 *  1 - Minor statements indicating where and what the client is doing
 *  2 - Display introduction and exit values
 *  3 - Detailed internal information of functions
 *  4 - Very specific internal information
 */
void debug(int level, char *fmt, ...)
{
    va_list vargs;
    char buf[MAXSIZ];

    if (debug_level && (debug_level >= level)) {
        va_start(vargs, fmt);
        vsprintf(buf, fmt, vargs);
        wrap(buf);
        term_normal_mode();
        va_end(vargs);
    }
}

void do_column_maker(char *s)
{
    int len;

    len = strlen(s);

    if ((len + column_maker_pos + 1) > num_columns) {
        column_maker_buf[num_columns] = '\0';
        msg(column_maker_buf);
        set_column_maker(column_maker_width);
    }

    strcpy(column_maker_buf + column_maker_pos, s);
    *(column_maker_buf + column_maker_pos + len) = ' ';
    column_maker_pos += (column_maker_width * ((len / column_maker_width) + 1));
}

void set_column_maker(int width)
{
    int i;

    column_maker_pos = 0;
    column_maker_width = width;

    for (i = 0; i < num_columns; ++i) {
        *(column_maker_buf + i) = ' ';
    }
}

void flush_column_maker(void)
{
    if (column_maker_pos) {
        msg(column_maker_buf);
    }
}

#define OCT1989 623303940
#define SECOND  1L
#define MINUTES (60 * SECOND)
#define HOURS   (60 * MINUTES)
#define DAYS    (24 * HOURS)

char *time_dur(long int dur)
{
    long cnt = dur;
    long yr = 0;
    char *units = "seconds";
    char *s;
    char ybuf[SMABUF];
    char dbuf[SMABUF];
    static char tdbuf[NORMSIZ];

    if (dur < OCT1989) {
        strcpy(tdbuf, "a long time");
        s = tdbuf;

        return s;
    }

    dur = time(0) - dur;

    if (dur > (365 * DAYS * 20952)) {
        yr = dur / ((365 * DAYS) + 20952); /* Years */
        dur -= (yr * ((365 * DAYS) + 20952)); /* Portion of a year */
    }

    if (dur < 101) {
        cnt = dur;
        units = "second";
    } else if (dur < 6001) {
        cnt = dur / MINUTES;
        units = "minute";
    } else if (dur < 120000) {
        cnt = dur / HOURS;
        units = "hour";
    } else if (dur < 1200000) {
        cnt = dur / DAYS;
        units = "day";
    } else if (dur < 13000000) {
        cnt = dur / (7 * DAYS);
        units = "week";
    } else {
        cnt = dur / ((365 / 12) * DAYS);
        units = "month";
    }

    if (yr == 0) {
        strcpy(ybuf, "");
    } else {
        sprintf(ybuf, "%ld year%s", yr, (yr == 1) ? "" : "s");
    }

    if (cnt == 0) {
        strcpy(dbuf, "");
    } else {
        sprintf(dbuf, "%ld %s%s", cnt, units, (cnt == 1) ? "" : "s");
    }

    if ((yr == 0) && (cnt == 0)) {
        sprintf(tdbuf, "an instant");
    } else if ((yr > 0) && (cnt > 0)) {
        sprintf(tdbuf, "%s and %s", ybuf, dbuf);
    } else if (yr > 0) {
        strcpy(tdbuf, ybuf);
    } else {
        strcpy(tdbuf, dbuf);
    }

    s = tdbuf;

    return s;
}

void remove_space_at_end(char *s)
{
    char *p = s;

    if (strlen(s) == 1) {
        return;
    }

    p += (strlen(s) - 1); /* Get past the NULL term */

    while (*p == ' ') {
        --p;
    }

    *++p = '\0'; /* Increment past last non space and NULL term */
}

/*
 * strfree checks the given ptr to see if it is NULL, if not..it frees
 * it. strfree always returns NULL
 */
char *strfree(char *ptr)
{
    if (ptr) {
        free(ptr);
    }

    return NULL;
}

void place_string_on_output_window(char *str, int len)
{
    if (options[FULLSCREEN / 32] & ((FULLSCREEN < 32) ?
                                    (1 << FULLSCREEN)
                                    : (1 << (FULLSCREEN % 32)))) {
        scroll_output_window();
    }

    cursor_output_window();
    write_string(str, len);
    ++last_output_row;

    if (!(options[FULLSCREEN / 32] & ((FULLSCREEN < 32) ?
                                      (1 << FULLSCREEN)
                                      : (1 << (FULLSCREEN % 32))))) {
        scroll_output_window();
    }

    add_refresh_line(str, len);
}

void write_string(char *s, int cnt)
{
    int i;
    char *p;

    for (i = 0, p = s; i <= cnt; ++i) {
        switch (*p) {
        case BELL_CHAR:
            if (options[DO_BELLS / 32] & ((DO_BELLS < 32) ?
                                          (1 << DO_BELLS)
                                          : (1 << (DO_BELLS % 32)))) {
                term_beep(1);
            } else {
                if (!term_standout_status()) {
                    term_standout_on();
                    term_putchar(*p + 'A' - 1);
                    term_standout_off();
                } else {
                    term_putchar(*p + 'A' - 1);
                }
            }

            ++p;

            break;
        case BOLD_CHAR:
            term_toggle_boldface();
            ++p;

            break;
        case INVERSE_CHAR:
            term_toggle_standout();
            ++p;

            break;
        case UNDERLINE_CHAR:
            term_toggle_underline();
            ++p;

            break;
        default:
            term_putchar(*p);
            ++p;

            break;
        }
    }
}

void init_refresh_lines(void)
{
    int i;

    refresh_next = 0;
    refresh_line = (char **)malloc((output_row + 1) * sizeof(char));

    for (i = 0; i < (output_row + 1); ++i) {
        refresh_line[i] = NULL;
    }
}

void free_refresh_lines(void)
{
    int i;

    for (i = 0; i < (output_row + 1); ++i) {
        free(refresh_line[1]);
    }

    free(refresh_line);
}

void add_refresh_line(char *s, int cnt)
{
    char cbuf[MAXSIZ]; /* I increased the buffer size for ANSI codes. -mfw */

    strncpy(cbuf, s, cnt);
    cbuf[cnt] = '\0';

    if (refresh_next && (refresh_line[refresh_next] != NULL)) {
        free(refresh_line[refresh_next]);
    }

    refresh_line[refresh_next++] = string(cbuf);
    refresh_next %= (output_row + 1);
}

int start_refresh_line_index(int *start_pos)
{
    int i = refresh_next;
    int oldrn = refresh_next;

    while (!refresh_line[i]) {
        ++i;

        if (i > output_row) {
            i = 0;
        }

        if (i == oldrn) {
            break;
        }
    }

    /* Don't have a full table yet */
    if (i == refresh_next) {
        *start_pos = output_row;
    } else {
        *start_pos = refresh_next - 1;
    }

    return i;
}

/* Clear the lines, but keep table intact */
void clear_refresh_line(void)
{
    int i;

    for (i = 0; i < (output_row + 1); ++i) {
        if (refresh_line[i] != NULL) {
            refresh_line[i] = strfree(refresh_line[i]);
        }
    }

    refresh_next = 0;
}

/*
 * Delete the refresh table. Lines need to be freed via
 * clear_refresh_line(). Reset via init_refresh_lines()
 */
void clear_refresh_line_mode(void)
{
    if (refresh_line != NULL) {
        free(refresh_line);
    }

    refresh_next = 0;
}

int has_esc_codes(char *str)
{
    char *p = str;

    while (*p) {
        if (*p == ESC_CHAR) {
            return 1;
        }

        ++p;
    }

    return 0;
}
