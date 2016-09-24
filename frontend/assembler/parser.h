#ifndef PARSE_H
#define PARSE_H

enum {
    A_COMMAND,
    L_COMMAND,
    C_COMMAND
};

void initialize(char *input_file);
int has_more_commands();
void advance();
int command_type();
char *get_symbol(void);
char *get_dest(void);
char *get_comp(void);
char *get_jump(void);

#endif
