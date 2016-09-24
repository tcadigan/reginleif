#ifndef PARSE_H
#define PARSE_H

enum {
    A_COMMAND,
    L_COMMAND,
    C_COMMAND
};

void initialize_parser(char *input_file);
void destroy_parser();
int parser_has_more_commands();
void advance_parser();
int get_command_type();
char *get_symbol(void);
char *get_dest(void);
char *get_comp(void);
char *get_jump(void);

#endif
