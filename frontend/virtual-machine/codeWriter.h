#ifndef CODEWRITER_H
#define CODEWRITER_H

#include "parse.h"

void codeWriterConstructor(char *file);
void setFileName(char *fileName);
void writeArithmetic(char *string);
void writePushPop(VM_TYPE command, char *segment, int index);
void Close();
    
#endif /* CODEWRITER_H */
