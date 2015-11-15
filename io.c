#include "io.h"

#include <curses.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>

#else

#include <string.h>
#include <sgtty.h>
#include <sys/wait.h>
#endif

/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

/* Static output string for the pad function */
vtype pad_output;

char *getenv();

/* Last messages */
vtype old_msg[SAVED_MSGS];

/* Number of last msg generated in old_msg */
int last_message = 0;

/* Number of last msg printed (^M) */
int last_displayed_msg = 0;

/* Flag which lets repeat_msg call msg_print */
int repeating_old_msg = 0;

/* Value of msg flag at start of turn */
extern int save_msg_flag;

#ifdef USG
void exit();
unsigned int sleep();
#endif

#ifdef ultrix
void exit();
void sleep();
#endif

#ifdef USG
/* No local special characters */
#else
struct ltchars save_special_chars;
#endif

/* Initialize curses routines */
void init_curses()
{
#ifdef USG
    /* No local special characters */
#else
    struct ltchars buf;
#endif

#ifdef USG
    if(initscr() == NULL) {
        printf("Error allocating screen in curses package\n");
        
        exit_game();
    }
#else
    if(initscr() == ERR) {
        printf("Error allocating screen in curses package\n");
        
        exit_game();
    }
#endif
    
    clear();

#ifdef USG
    saveterm();
#endif

#if defined(ultrix)
    crmode();
#else
    cbreak();
#endif

    noecho();

#ifndef BUGGY_CURSES
    nonl();
#endif

    /* Save old settings of the local special characters */
#ifdef USG
    /* No local special characters */
#else
    ioctl(0, TIOCGLTC, (char *)&save_special_chars);

    /* 
     * Disable all of the local special characters except the suspend char,
     * have to disable ^Y for tunneling
     */

    /* control-Z */
    buf.t_suspc = (char)26;
    buf.t_dsuspc = (char)-1;
    buf.t_rprntc = (char)-1;
    buf.t_flushc = (char)-1;
    buf.t_werasc = (char)-1;
    buf.t_lnextc = (char)-1;
    ioctl(0, TIOCSLTC, (char *)&buf);
#endif
}

/* Dump IO to buffer    -RAK- */
void put_buffer(char *out_str, int row, int col)
{
    vtype tmp_str;

    if(mvaddstr(row, col, out_str) == ERR) {
        sprintf(tmp_str, "error row = %d col = %d\n", row, col);
        prt(tmp_str, 0, 0);

        /* Wait so user can see error */
        sleep(2);
    }
}

/* Dump the IO buffer to terminal    -RAK- */
void put_qio()
{
    refresh();
}

void shell_out()
{
    int val;
    char *str;

#ifdef USG
    /* No local special characters */
#else
    struct ltchars buf;
#endif

    /* Clear screen and print 'exit' message */
    clear_screen(0, 0);
    prt("[Entering shell, type 'exit' to resume your game]\n", 0, 0);
    put_qio();

#ifndef BUGGY_CURSES
    nl();
#endif

#if defined(ultrix)
    nocrmode();
#else
    nocbreak();
#endif

    echo();
    ignore_signals();
    val = fork();

    if(val == 0) {
        default_signals();

#ifdef USG
        /* No local special characters */
        resetterm();
#else
        ioctl(0, TIOCSLTC, (char *)&save_special_chars);
#endif

        /* Close scoreboard descriptor */
        close(highscore_fd);
        str = getenv("SHELL");

        if(str) {
            execl(str, str, (char *)0);
        }
        else {
            execl("/bin/sh", "sh", (char *)0);
        }

        msg_print("Cannot execute shell");

        exit(1);
    }

    if(val == -1) {
        msg_print("Fork failed. Try again.");

        return;
    }

#ifdef USG
    wait((int *)0);
#else
    wait((union wait *)0);
#endif

    restore_signals();

    /* Restore the cave to the screen */
    really_clear_screen();
    draw_cave();

#if defined(ultrix)
    crmode();
#else
    cbreak();
#endif

    noecho();

#ifndef BUGGY_CURSES
    nonl();
#endif

    /* 
     * Disable all of the local special characters except the suspend char,
     * have to disable ^Y for tunneling
     */

#ifdef USG
    /* No local special characters */
#else
    /* control-Z */
    buf.t_suspc = (char)26;
    buf.t_dsuspc = (char)-1;
    buf.t_rprntc = (char)-1;
    buf.t_flushc = (char)-1;
    buf.t_werasc = (char)-1;
    buf.t_lnextc = (char)-1;
    ioctl(0, TIOCSLTC, (char *)&buf);
#endif
}

void exit_game()
{
    /* Restore the saved values of the local special chars */
    /* Dump any remaining buffer */
    put_qui();

    /* Exit curses */
    endwin();
    echo();

#ifndef BUGGY_CURSES
    nl();
#endif

#if defined(ultrix)
    nocrmode();
#else
    nocbreak();
#endif

#ifdef USG
    /* No local special characters */
    resetterm();
#else
    ioctl(0, TIOCSLTC, (char *)&save_special_chars);
#endif

    /* Exit from game */
    exit(0);
}

/* Gets single character from keyboard and returns */
void inkey(char *ch)
{
    /* Dump IO buffer */
    put_qio();
    *ch = getch();
    msg_flag = FALSE;
}

/* Flush the buffer    -RAK- */
void flush()
{
#ifdef USG
    /* Flush the input queue */
    ioctl(0, TCFLSH, 0);
#else
    int arg;

    arg = FREAD;

    /* Flush all input */
    ioctl(0, TIOCFLUSH, (char *)&arg);
#endif
}

#if 0
/* This is no longer used anywhere */

/* Flush buffer before input    -RAK- */
void inkey_flush(char *x)
{
    if(!wizard1) {
        flush();
    }

    inkey(x);
}
#endif

/* Clears given line of text    -RAK- */
void erase_line(int row, int col)
{
    move(row, col);
    clrtoeol();

    if(row == MSG_LINE) {
        msg_flag = FALSE;
    }
}

/* Clears screen at given row, column */
void clear_screen(int row, int col)
{
    int i;

    for(i = row; i < 23; ++i) {
        used_line[i] = FALSE;
    }

    move(row, col);
    clrtobot();
    msg_flag = FALSE;
}

/* 
 * Clears enitre screen, even if there are characters that curses does
 * not know about
 */
void really_clear_screen()
{
    int i;

    for(i = 1; i < 23; ++i) {
        used_line[i] = FALSE;
    }

    clear();
    msg_flag = FALSE;
}

/* Outputs a line to a given interpolated y, x position    -RAK- */
void print(char *str_buf, int row, int col)
{
    /* Real co-ords convert to screen positions */
    row -= panel_row_prt;
    col -= panel_col_prt;
    used_line[row] = TRUE;
    put_buffer(str_buff, row, col);
}

/* Outputs a line to a given y, x position    -RAK- */
void prt(char *str_buf, int row, int col)
{
    move(row, col);
    clrtoeol();
    put_buffer(str_buff, row, col);
}

/* Move cursor to a given y, x position */
void move_cursor(int row, int col)
{
    move(row, col);
}

/* Outputs a message to top line of screen */
void msg_print(char *str_buf)
{
    int old_len;
    char in_char;
    int do_flush = 0;

    /* Stop the character if s/he is in a run */
    if(find_flag) {
        find_flag = FALSE;
        move_light(char_row, char_col, char_row, char_col);
    }

    if(msg_flag) {
        old_len = strlen(old_msg[last_message]) + 1;
        put_buffer(" -more-", MSG_LINE, old_len);

        /* Let signal handler know that we are waiting for a space */
        wait_for_more = 1;
        inkey(&in_char);

        while((in_char != ' ') && (in_char != '\033')) {
            inkey(&in_char);
        }

        wait_for_more = 0;
        do_flush = 1;
    }

    move(MSG_LINE, 0);
    clrtoeol();

    if(do_flush) {
        put_qio();
    }

    put_buffer(str_buf, MSG_LINE, 0);

    if(!repeating_old_msg) {
        /* Increment last message pointer */
        ++last_message;

        if(last_message == SAVE_MSGS) {
            last_message = 0;
        }

        last_displayed_msg = last_message;
        strcpy(old_msg[last_message], str_buff);
    }

    msg_flag = TRUE;
}

/* Repeat an old message */
void repeat_msg()
{
    repeating_old_msg = 1;

    /* If message still visible, decrement counter to display previous one */
    if(save_msg_flag) {
        if(last_displayed_msg == 0) {
            last_displayed_msg = SAVED_MSGS;
        }

        --last_displayed_msg;
        msg_flag = FALSE;
        msg_print(old_msg[last_displayed_msg]);
    }
    else {
        /* Display current message */
        msg_print(old_msg[last_displayed_msg]);
    }

    repeating_old_msg = 0;
}

/* Prompts (optional) and retuns ordinal value of input character */
/* Function returns false if <ESCAPE> is input */
int get_com(char *prompt, char *command)
{
    int com_val;
    int res;

    if(strlen(prompt) > 1) {
        prt(prompt, 0, 0);
    }

    inkey(command);
    com_val = (*command);

    switch(com_val) {
    case 0:
    case 27:
        res = FALSE;

        break;
    default:
        res = TRUE;

        break;
    }

    erase_line(MSG_LINE, 0);
    msg_flag = FALSE;

    return res;
}

/* 
 * Gets a string terminated by <RETURN>
 * Function returns false if <ESCAPE>, control-Y, control-C, or
 * control-Z is input
 */
int get_string(char *in_str, int row, int column, int slen)
{
    int start_col;
    int end_col;
    int i;
    char x;
    char tmp[2];
    int flag;
    int abort;

    abort = FALSE;
    flag = FALSE;
    in_str[0] = '\0';
    tmp[1] = '\0';
    put_buffer(pad(in_str, " ", slen), row, column);
    put_buffer("\0", row, column);
    stat_col = column;
    end_col = column + slen - 1;

    inkey(&x);

    switch(x) {
    case 27:
        abort = TRUE;

        break;
    case 10:
    case 13:
        flag = TRUE;

        break;
    case 127:
    case 8:
        if(column > start_col) {
            --column;
            put_buffer(" \b", row, column);
            in_str[strlen(in_str) - 1] = '\0';
        }

        break;
    default:
        tmp[0] = x;
        put_buffer(tmp, row, column);
        strcat(in_str, tmp);
        ++column;

        if(column > end_col) {
            flag = TRUE;
        }

        break;
    }

    while(!flag && !abort) {
        inkey(&x);

        switch(x) {
        case 27:
            abort = TRUE;

            break;
        case 10:
        case 13:
            flag = TRUE;

            break;
        case 127:
        case 8:
            if(column > stat_col) {
                --column;
                put_buffer(" \b", row, column);
                in_str[strlen(in_str) - 1] = '\0';
            }

            break;
        default:
            tmp[0] = x;
            put_buffer(tmp, row, column);
            strcat(in_str, tmp);
            ++column;

            if(column > end_col) {
                flag = TRUE;
            }

            break;
        }
    }

    if(abort) {
        return FALSE;
    }
    else {
        /* Remove trailing blanks */
        i = strlen(in_str);

        if(i > 0) {
            while((in_str[i] == ' ') && (i > 0)) {
                --i;
            }

            in_str[i + 1] = '\0';
        }
    }

    return TRUE;
}

/* Return integer value of hex string    -RAK- */
int get_hex_value(int row, int col, int slen)
{
    vtype tmp_str;
    int hex_value;

    hex_value = 0;

    if(get_string(tmp_str, row, col, slen)) {
        if(strlen(tmp_str) <= 8) {
            sscanf(tmp_str, "%x", &hex_value);
        }
    }

    return hex_value;
}

/* Pauses for user response before returning    -RAK- */
void pause_line(int prt_line)
{
    char dummy;

    prt("[Press any key to continue]", prt_line, 23);
    inkey(&dummy);
    erase_line(23, 0);
}

/*
 * Pauses for user response before returning    -RAK-
 * NOTE: Delay is for player trying to roll up "perfect"
 *       characters. Make them wait a bit...
 */
void pause_exit(int prt_line, int delay)
{
    char dummy;

    prt("[Press any key to continue, or Q to exit]", prt_line, 10);
    inkey(&dummy);

    switch(dummy) {
    case 'Q':
        erase_line(prt_line, 0);

        if(delay > 0) {
            sleep((unsigned)delay);
        }

        exit_game();

        break;
    default:

        break;
    }

    erase_line(prt_line, 0);
}

/* Pad a string with fill characters to specified length */
char *pad(char *string, char *fill, int filllength)
{
    int length;
    int i;

    strcpy(pad_output, string);
    length = strlen(pad_output);

    for(i = length; i < filllength; ++i) {
        pad_output[i] = *fill;
    }

    pad_output[i] = '\0';

    return pad_output;
}

int confirm()
{
    char command;

    if(get_com("Are you sure?", &command)) {
        switch(command) {
        case 'y':
        case 'Y':
            
            return TRUE;
        default:

            return FALSE;
        }
    }

    return FALSE;
}
        
