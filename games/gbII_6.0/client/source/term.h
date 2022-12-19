/*
 * term.h: Header for the terminfo stuff
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1991
 *
 * See the COPYRIGHT file.
 */
#ifndef TERM_H_
#define TERM_H_

#include <sys/termios.h>

void get_termcap(void);
void term_test(void);
void term_termcap(void);
int term_null(int, int, int);
int term_CE_clear_to_eol(void);
int term_SPACE_clear_to_eol(int x, int y);
int term_CS_scroll(int l1, int l2, int n);
int term_ALDL_scroll(int l1, int l2, int n);
int term_param_ALDL_scroll(int l1, int l2, int n);
int term_IC_insert(char *c);
int term_IMEI_insert(char *c);
int term_DC_delete(void);
int term_ND_cursor_right(void);
int term_LE_cursor_left(void);
int term_BS_cursor_left(void);
int term_putchar(char c);
void term_puts(char *str, int len);
void term_normal_mode(void);
void term_toggle_standout(void);
void term_standout_on(void);
void term_standout_off(char *nop_str, int nop_int, char *nop_str2);
int term_standout_status(void);
void term_toggle_underline(void);
void term_underline_on(void);
void term_underline_off(void);
void term_toggle_boldface(void);
void term_boldface_on(void);
void term_boldface_off(void);
void term_put_termstring(char *c);
void term_move_cursor(int x, int y);
void term_clear_screen(void);
void term_clear(int l1, int l2);
void term_beep(int n);
void get_screen_size(void);
void term_mode_on(void);
void term_mode_off(void);

extern int last_output_row;
extern int output_row;
extern int num_columns;
extern int num_rows;
extern int (*term_clear_to_eol)();
extern int (*term_scroll)(int, int, int);
extern int (*term_cursor_right)();
extern int (*term_cursor_left)();

#define NAMESIZE 18
#define SCALE 100.0
#define S_X (num_columns - NAMESIZE)
#define S_Y (output_row)
#define Midx ((0.5 * (S_X - NAMESIZE)) / SCALE)
#define Midy ((0.5 * S_Y) / SCALE)

#endif // TERM_H_
