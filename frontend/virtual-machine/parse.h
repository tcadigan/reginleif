#ifndef PARSE_H
#define PARSE_H

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

void parseConstructor(char *file);
int hasMoreCommands();
void advance();
VM_TYPE commandType();
char *arg1();
int arg2();

#endif /* PARSE_H */
