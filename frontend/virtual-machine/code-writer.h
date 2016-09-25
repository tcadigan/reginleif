#ifndef CODEWRITER_H
#define CODEWRITER_H

#include "parser.h"

void construct_code_writer(char *file);
void set_filename(char *fileName);
void write_arithmetic(char *string);
void write_push_pop(enum VM_TYPE command, char *segment, int index);
void close_code_writer();
    
#endif /* CODEWRITER_H */
