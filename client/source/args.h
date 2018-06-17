#ifndef ARGS_H_
#define ARGS_H_

#include <stdbool.h>
#include <stdio.h>

char *parse_given_string(char *s, int mode);
void parse_variables(char *s);
void init_assign(int new_conn);
void add_assign(char *name, char *value);
void cmd_listassign(char *args);
char *get_assign(char *name);
void cmd_assign(char *args);
int valid_assign_name(char *name);
int test_assign(char *name);
void save_assigns(FILE *fd);
char *parse_macro_args(char *fmt, char *list);
char *parse_sec_args(char *fmt, char *list);
void argify(char *list);
char *get_args(int lo, int hi);
bool parse_for_loops(char *s);

#endif // ARGS_H_
