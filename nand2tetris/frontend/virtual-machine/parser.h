#ifndef PARSER_H
#define PARSER_H

enum VM_TYPE {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

void constructor_parser(char *file);
int parser_has_more_commands();
void advance_parser();
enum VM_TYPE get_command_type();
char *get_arg1();
int get_arg2();

#endif /* PARSER_H */
