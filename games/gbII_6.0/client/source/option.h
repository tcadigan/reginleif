/*
 * option.h: Header for the bit operations of options
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Coypright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef OPTION_H_
#define OPTION_H_

#include "gb.h"
#include "term.h"
#include "types.h"

#include <stdio.h>

#ifdef IMAP
void set_cursor_sector(char *args, int val);
#endif

void set_client_prompt(char *args, int nop_int, char *nop_str);
void set_connect_delay(char *args, int nop_int, char *nop_str);
void set_debug(char *args, int nop_int, char *nop_str);
void set_encrypt(char *args, int nop_int, char *nop_str);
void set_entry_quote(char *args, int nop_int, char *nop_str);
void set_exit_quote(char *args, int nop_int, char *nop_str);
void set_full_screen(char *args, int nop_int, char *nop_str);
void set_help(char *args, int nop_int, char *nop_str);
void set_history(char *args, int nop_int, char *nop_str);
void set_input_prompt(char *args, int nop_int, char *nop_str);
void set_insert_edit_mode(char *args, int nop_int, char *nop_str);
void set_macro_char(char *args, int nop_int, char *nop_str);
void set_map_opts(char *args, int nop_int, char *nop_str);
void set_more(char *args, int nop_int, char *nop_str);
void set_more_delay(char *args, int nop_int, char *nop_str);
void set_more_rows(char *args, int nop_int, char *nop_str);
void set_notify(char *args, int nop_int, char *nop_str);
void set_notify_beeps(char *args, int nop_int, char *nop_str);
void set_output_prompt(char *args, int nop_int, char *nop_str);
void set_overwrite_edit_mode(char *args, int nop_int, char *nop_str);
void set_primary_password(char *args, int nop_int, char *nop_str);
void set_recall(char *args, int nop_int, char *nop_str);
void set_rwho(char *args, int nop_int, char *nop_str);
void set_scroll();
void set_secondary_password(char *args, int nop_int, char *nop_str);
void set_show_clock(char *args, int val, char *name);
void set_show_mail(char *args, int val, char *name);
void set_status_bar(char *args, int nop_int, char *nop_str);
void set_status_bar_char(char *args, int nop_int, char *nop_str);
void set_robo(char *args, int nop_int, char *nop_str);

/* -mfw */
void set_ansi(char *args, int val, char *name);

#ifdef CLIENT_DEVEL
void set_devel(char *args, int nop_int, char *nop_str);
#endif

void doubletoggle(char *args, int type, char *name);
void toggle(char *args, int type, char *name);
void display_set(void);
void cmd_set(char *s);
void save_settings(FILE *fd);

/* For binary search */
CommandSet *binary_set_search(char *cmd);

extern char options[8];

#endif // OPTION_H_
