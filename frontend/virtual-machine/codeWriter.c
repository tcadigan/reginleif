/* Translates VM commands into Hack assembly code. */
#include "codeWriter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *output_fd;
int ctr;
char *filename;

/* Opens the output file/stream and gets ready to write into it */
void codeWriterConstructor(char *file)
{
    filename = (char *)malloc(strlen(file) + 2);
    
    if(filename == NULL) {
        fprintf(stderr, "Unable to allocate output file");
    }

    memcpy(filename, file, strlen(file) - 2);
    filename[strlen(file) - 2] = 'a';
    filename[strlen(file) - 1] = 's';
    filename[strlen(file)] = 'm';
    filename[strlen(file) + 1] = '\0';

    printf("TC_DEBUG: opening output file -> \'%s\'\n", filename);
    
    output_fd = fopen(filename, "w");
    
    if(!output_fd) {
        fprintf(stderr, "Unable to open file \'%s\'", file);
    }

    setFileName(file);
}

/* Informs the code writer that the translation of a new VM is started */
void setFileName(char *fileName)
{
    if(filename) {
        free(filename);
    }
    
    filename = (char *)malloc(strlen(fileName) - 2);

    if(filename == NULL) {
        fprintf(stderr, "Unable to allocate filename");
    }

    memcpy(filename, fileName, strlen(fileName) - 3);
    filename[strlen(fileName) - 3] = '\0';
    
    ctr = 0;
}

/*
 * Writes the assembly code that is the translation of the given arithmetic
 * command.
 */
void writeArithmetic(char *string)
{
    if(strncmp(string, "add", strlen("add")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "MD=M+D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
    else if(strncmp(string, "sub", strlen("sub")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "MD=M-D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
    else if(strncmp(string, "neg", strlen("neg")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "M=-D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
    else if(strncmp(string, "eq", strlen("eq")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M-D\n");
        fprintf(output_fd, "@NOTEQUAL.%s.%d\n", filename, ctr);
        fprintf(output_fd, "D;JNE\n");
        fprintf(output_fd, "D=-1\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "@RESUME.%s.%d\n", filename, ctr);
        fprintf(output_fd, "0;JMP\n");
        fprintf(output_fd, "(NOTEQUAL.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "D=0\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "(RESUME.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
        ++ctr;
    }
    else if(strncmp(string, "gt", strlen("gt")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M-D\n");
        fprintf(output_fd, "@SMALLER.%s.%d\n", filename, ctr);
        fprintf(output_fd, "D;JLE\n");
        fprintf(output_fd, "D=-1\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "@RESUME.%s.%d\n", filename, ctr);
        fprintf(output_fd, "0;JMP\n");
        fprintf(output_fd, "(SMALLER.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "D=0\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "(RESUME.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
        ++ctr;
    }
    else if(strncmp(string, "lt", strlen("lt")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M-D\n");
        fprintf(output_fd, "@BIGGER.%s.%d\n", filename, ctr);
        fprintf(output_fd, "D;JGE\n");
        fprintf(output_fd, "D=-1\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "@RESUME.%s.%d\n", filename, ctr);
        fprintf(output_fd, "0;JMP\n");
        fprintf(output_fd, "(BIGGER.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "D=0\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "A=M\n");
        fprintf(output_fd, "M=D\n");
        fprintf(output_fd, "(RESUME.%s.%d)\n", filename, ctr);
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
        ++ctr;
    }
    else if(strncmp(string, "and", strlen("and")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "MD=M&D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
    else if(strncmp(string, "or", strlen("or")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "MD=M|D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
    else if(strncmp(string, "not", strlen("not")) == 0) {
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "AM=M-1\n");
        fprintf(output_fd, "D=M\n");
        fprintf(output_fd, "M=!D\n");
        fprintf(output_fd, "@SP\n");
        fprintf(output_fd, "M=M+1\n");
    }
}
/*
 * Writes the assembly code that is the translation of the given command, where
 * command is either C_PUSH or C_POP.
 */
void writePushPop(VM_TYPE command, char *segment, int index)
{
    if((command != C_PUSH) && (command != C_POP)) {
        return;
    }

    if(command == C_PUSH) {
        if(strncmp(segment, "constant", strlen("constant")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "local", strlen("local")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@LCL\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "argument", strlen("argument")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@ARG\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "this", strlen("this")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@THIS\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "that", strlen("that")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@THAT\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "temp", strlen("temp")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@R5\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "pointer", strlen("pointer")) == 0) {
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@R3\n");
            fprintf(output_fd, "A=D+A\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
        else if(strncmp(segment, "static", strlen("static")) == 0) {
            fprintf(output_fd, "@%s.%d\n", filename, index);
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "D=A+1\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "M=D\n");
        }
    }
    else if(command == C_POP) {
        if(strncmp(segment, "constant", strlen("contant")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
        }
        else if(strncmp(segment, "local", strlen("local")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@LCL\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "argument", strlen("argument")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@ARG\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "this", strlen("this")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@THIS\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "that", strlen("that")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@THAT\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "temp", strlen("temp")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@R5\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "pointer", strlen("pointer")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "@%d\n", index);
            fprintf(output_fd, "D=A\n");
            fprintf(output_fd, "@R3\n");
            fprintf(output_fd, "D=D+A\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "A=M\n");
            fprintf(output_fd, "M=D\n");
            fprintf(output_fd, "@R13\n");
            fprintf(output_fd, "M=0\n");
        }
        else if(strncmp(segment, "static", strlen("static")) == 0) {
            fprintf(output_fd, "@SP\n");
            fprintf(output_fd, "MD=M-1\n");
            fprintf(output_fd, "A=D\n");
            fprintf(output_fd, "D=M\n");
            fprintf(output_fd, "@%s.%d\n", filename, index);
            fprintf(output_fd, "M=D\n");
        }
    }
}

/* Closes the output file. */
void Close()
{
    fclose(output_fd);
}
