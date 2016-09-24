#ifndef PARSE_H
#define PARSE_H

enum {
    A_COMMAND,
    L_COMMAND,
    C_COMMAND
};

void initialize(char *input_file);
int hasMoreCommands();
void advance();
int commandType();
char *symbol(void);
char *dest(void);
char *comp(void);
char *jump(void);

#endif
