/*
 * str.h: Header for string manipulation
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1991
 *
 * See the COPYRIGHT file.
 */
#ifndef STR_H_
#define STR_H_

/* Results for pattern matcher */
extern char pattern1[];
extern char pattern2[];
extern char pattern3[];
extern char pattern4[];
extern char pattern5[];
extern char pattern6[];
extern char pattern7[];
extern char pattern8[];
extern char pattern9[];
extern char pattern10[];
extern char pattern11[];
extern char pattern12[];
extern char pattern13[];
extern char pattern14[];
extern char pattern15[];
extern char pattern16[];
extern char pattern17[];
extern char pattern18[];
extern char pattern19[];
extern char pattern20[];
extern char *pattern[];
extern char *client_prompt;
extern char *input_prompt;
extern char *output_prompt;

char *first(char *str);
char *rest(char *str);
void split(char *s, char *fbuf, char *rbuf);
char *skip_space(char *s);
char *fstring(char *str);
int pattern_match(char *string1, char *string2, char **pattern);
char *strtou(char *str);
char *string(char *str);
char *maxstring(char *str);
int wrap(char *line);
int more(void);
void msg(char *fmt, ...);
void display_msg(char *s);
void display_bold_communication(char *s);
void msg_error(char *fmt, ...);
void debug(int level, char *fmt, ...);
void do_column_maker(char *s);
void set_column_maker(int width);
void flush_column_maker(void);
char *time_dur(long int dur);
void remove_space_at_end(char *s);
char *strfree(char *ptr);
void place_string_on_output_window(char *str, int len);
void write_string(char *s, int cnt);
void init_refresh_lines(void);
void free_refresh_lines(void);
void add_refresh_line(char *s, int cnt);
int start_refresh_line_index(int *start_pos);
void clear_refresh_line(void);
void clear_refresh_line_mode(void);
int has_esc_codes(char *str);

#endif // STR_H_
