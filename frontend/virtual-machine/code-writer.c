/* Translates VM commands into Hack assembly code. */
#include "code-writer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *output_fd;
int ctr;
int return_ctr;
char *filename;
char *in_function_name = NULL;

/* Opens the output file/stream and gets ready to write into it */
void construct_code_writer(char *file)
{
    char *suffix = strrchr(file, '.');

    if(suffix != NULL) {
        *suffix = '\0';
    }
    
    filename = (char *)malloc(strlen(file) + 5);
    
    if(filename == NULL) {
        fprintf(stderr, "Unable to allocate output file");
    }

    strncpy(filename, file, strlen(file));
    strncpy(filename + strlen(file), ".asm", strlen(".asm"));
    filename[strlen(file) + 4] = '\0';
    
    output_fd = fopen(filename, "w");
    
    if(!output_fd) {
        fprintf(stderr, "Unable to open file \'%s\'", file);
    }

    set_filename(file);
}

/* Closes the output file. */
void close_code_writer()
{
    if(output_fd) {
        fclose(output_fd);
    }
}

/* Informs the code writer that the translation of a new VM is started */
void set_filename(char *fileName)
{
    if(filename) {
        free(filename);
    }

    char *suffix = strrchr(fileName, '.');

    if(suffix != NULL) {
        filename = (char *)malloc(suffix - fileName + 1);

        if(filename == NULL) {
            fprintf(stderr, "Unable to allocate filename\n");

            return;
        }

        strncpy(filename, fileName, suffix - fileName);
        filename[suffix - fileName] = '\0';
    }
    else {
        filename = (char *)malloc(strlen(fileName) + 1);
        
        if(filename == NULL) {
            fprintf(stderr, "Unable to allocate filename");
        }
        
        strncpy(filename, fileName, strlen(fileName));
        filename[strlen(fileName)] = '\0';
    }
    
    ctr = 0;
    return_ctr = 0;
}

/*
 * Writes the assembly code that is the translation of the given arithmetic
 * command.
 */
void write_arithmetic(char *string)
{
    if((strncmp(string, "add", strlen("add")) == 0)
       || (strncmp(string, "sub", strlen("sub")) == 0)
       || (strncmp(string, "and", strlen("and")) == 0)
       || (strncmp(string, "or", strlen("or")) == 0)) {
        
        fprintf(output_fd,
                "@SP\n"
                "AM=M-1\n"
                "D=M\n"
                "@SP\n"
                "AM=M-1\n");

        if(strncmp(string, "add", strlen("add")) == 0) {
            fprintf(output_fd, "MD=M+D\n");
        }
        else if(strncmp(string, "sub", strlen("sub")) == 0) {
            fprintf(output_fd, "MD=M-D\n");
        }
        else if(strncmp(string, "and", strlen("and")) == 0) {
            fprintf(output_fd, "MD=M&D\n");
        }
        else {
            fprintf(output_fd, "MD=M|D\n");
        }

        fprintf(output_fd,
                "@SP\n"
                "M=M+1\n");
    }
    else if((strncmp(string, "neg", strlen("neg")) == 0)
            || (strncmp(string, "not", strlen("not")) == 0)) {
        
        fprintf(output_fd,
                "@SP\n"
                "AM=M-1\n"
                "D=M\n");

        if(strncmp(string, "neg", strlen("neg")) == 0) {
            fprintf(output_fd, "M=-D\n");
        }
        else {
            fprintf(output_fd, "M=!D\n");
        }
        
        fprintf(output_fd,
                "@SP\n"
                "M=M+1\n");
    }
    else if((strncmp(string, "eq", strlen("eq")) == 0)
            || (strncmp(string, "gt", strlen("gt")) == 0)
            || (strncmp(string, "lt", strlen("lt")) == 0)) {
        fprintf(output_fd,
                "@SP\n"
                "AM=M-1\n"
                "D=M\n"
                "@SP\n"
                "AM=M-1\n"
                "D=M-D\n"
                "@LOGIC_FALSE.%s.%d\n", filename, ctr);

        if(strncmp(string, "eq", strlen("eq")) == 0) {
            fprintf(output_fd, "D;JNE\n");
        }
        else if(strncmp(string, "gt", strlen("gt")) == 0) {
            fprintf(output_fd, "D;JLE\n");
        }
        else {
            fprintf(output_fd, "D;JGE\n");
        }
        
        fprintf(output_fd,
                "D=-1\n"
                "@SP\n"
                "A=M\n"
                "M=D\n"
                "@RESUME.%s.%d\n"
                "0;JMP\n"
                "(LOGIC_FALSE.%s.%d)\n"
                "D=0\n"
                "@SP\n"
                "A=M\n"
                "M=D\n"
                "(RESUME.%s.%d)\n"
                "@SP\n"
                "M=M+1\n",
                filename,
                ctr,
                filename,
                ctr,
                filename,
                ctr);
        ++ctr;
    }
}
/*
 * Writes the assembly code that is the translation of the given command, where
 * command is either C_PUSH or C_POP.
 */
void write_push_pop(enum VM_TYPE command, char *segment, int index)
{
    if((command != C_PUSH) && (command != C_POP)) {
        return;
    }

    if(command == C_PUSH) {
        if(strncmp(segment, "constant", strlen("constant")) == 0) {
            fprintf(output_fd,
                    "@%d\n"
                    "D=A\n",
                    index);
        }
        else if(strncmp(segment, "static", strlen("static")) == 0) {
            fprintf(output_fd,
                    "@%s.%d\n"
                    "D=M\n",
                    filename,
                    index);
        }
        else if((strncmp(segment, "local", strlen("local")) == 0)
                || (strncmp(segment, "argument", strlen("argument")) == 0)
                || (strncmp(segment, "this", strlen("this")) == 0)
                || (strncmp(segment, "that", strlen("that")) == 0)) {

            fprintf(output_fd,
                    "@%d\n"
                    "D=A\n",
                    index);

            if(strncmp(segment, "local", strlen("local")) == 0) {
                fprintf(output_fd, "@LCL\n");
            }
            else if(strncmp(segment, "argument", strlen("argument")) == 0) {
                fprintf(output_fd, "@ARG\n");
            }
            else if(strncmp(segment, "this", strlen("this")) == 0) {
                fprintf(output_fd, "@THIS\n");
            }
            else {
                fprintf(output_fd, "@THAT\n");
            }
            
            fprintf(output_fd,
                    "A=M\n"
                    "A=D+A\n"
                    "D=M\n");
        }
        else if((strncmp(segment, "temp", strlen("temp")) == 0)
                || (strncmp(segment, "pointer", strlen("pointer")) == 0)) {
            
            fprintf(output_fd,
                    "@%d\n"
                    "D=A\n",
                    index);

            if(strncmp(segment, "temp", strlen("temp")) == 0) {
                fprintf(output_fd, "@R5\n");
            }
            else {
                fprintf(output_fd, "@R3\n");
            }
            
            fprintf(output_fd,
                    "A=D+A\n"
                    "D=M\n");
        }

        fprintf(output_fd,
                "@SP\n"
                "A=M\n"
                "M=D\n"
                "D=A+1\n"
                "@SP\n"
                "M=D\n");
    }
    else if(command == C_POP) {
        fprintf(output_fd,
                "@SP\n"
                "MD=M-1\n");
        
        if(strncmp(segment, "static", strlen("static")) == 0) {
            fprintf(output_fd,
                    "A=D\n"
                    "D=M\n"
                    "@%s.%d\n"
                    "M=D\n",
                    filename,
                    index);
        }
        else if((strncmp(segment, "local", strlen("local")) == 0)
                || (strncmp(segment, "argument", strlen("argument")) == 0)
                || (strncmp(segment, "this", strlen("this")) == 0)
                || (strncmp(segment, "that", strlen("that")) == 0)) {
            
            fprintf(output_fd,
                    "@%d\n"
                    "D=A\n",
                    index);

            if(strncmp(segment, "local", strlen("local")) == 0) {
                fprintf(output_fd, "@LCL\n");
            }
            else if(strncmp(segment, "argument", strlen("argument")) == 0) {
                fprintf(output_fd, "@ARG\n");
            }
            else if(strncmp(segment, "this", strlen("this")) == 0) {
                fprintf(output_fd, "@THIS\n");
            }
            else {
                fprintf(output_fd, "@THAT\n");
            }
            
            fprintf(output_fd,
                    "A=M\n"
                    "D=D+A\n"
                    "@R13\n"
                    "M=D\n"
                    "@SP\n"
                    "A=M\n"
                    "D=M\n"
                    "@R13\n"
                    "A=M\n"
                    "M=D\n"
                    "@R13\n"
                    "M=0\n");
        }
        else if((strncmp(segment, "temp", strlen("temp")) == 0)
                || (strncmp(segment, "pointer", strlen("pointer")) == 0)) {
            
            fprintf(output_fd,
                    "@%d\n"
                    "D=A\n",
                    index);

            if(strncmp(segment, "temp", strlen("temp")) == 0) {
                fprintf(output_fd, "@R5\n");
            }
            else {
                fprintf(output_fd, "@R3\n");
            }
                
            fprintf(output_fd,
                    "D=D+A\n"
                    "@R13\n"
                    "M=D\n"
                    "@SP\n"
                    "A=M\n"
                    "D=M\n"
                    "@R13\n"
                    "A=M\n"
                    "M=D\n"
                    "@R13\n"
                    "M=0\n");
        }
    }
}

/*
 * Writes assembly code that effects the VM initialization, also called
 * bootstrap code. This code must be placed at the beginning of the output file
 */
void write_init()
{
    fprintf(output_fd,
            "@256\n"
            "D=A\n"
            "@SP\n"
            "M=D\n");

    write_call("Sys.init", 0);
}

/*
 * Writes assembly code that effects the label command.
 */
void write_label(char *label)
{
    if(in_function_name != NULL) {
        fprintf(output_fd, "(%s$%s)\n", in_function_name, label);
    }
    else {
        fprintf(output_fd, "(%s)\n", label);
    }
}

/*
 * Writes assembly code that effects the goto command.
 */
void write_goto(char *label)
{
    if(in_function_name != NULL) {
        fprintf(output_fd,
                "@%s$%s\n"
                "0;JMP\n",
                in_function_name,
                label);
    }
    else {
        fprintf(output_fd,
                "@%s\n"
                "0;JMP\n",
                label);
    }   
}

/*
 * Writes assembly code that effects the if-goto command.
 */
void write_if(char *label)
{
    if(in_function_name != NULL) {
        fprintf(output_fd,
                "@SP\n"
                "MD=M-1\n"
                "A=D\n"
                "D=M\n"
                "@%s$%s\n"
                "D;JNE\n",
                in_function_name,
                label);
    }
    else {
        fprintf(output_fd,
                "@SP\n"
                "MD=M-1\n"
                "A=D\n"
                "D=M\n"
                "@%s\n"
                "D;JNE\n",
                label);
    }
}

/*
 * Writes assembly code that effects the call command.
 */
void write_call(char *function_name, int num_args)
{
    fprintf(output_fd,
            "@RIP.%s.%d\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "D=A+1\n"
            "@SP\n"
            "M=D\n"
            "@LCL\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n"
            "@ARG\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n"
            "@THIS\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n"
            "@THAT\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n"
            "@SP\n"
            "D=M\n"
            "@%d\n"
            "D=D-A\n"
            "@5\n"
            "D=D-A\n"
            "@ARG\n"
            "M=D\n"
            "@SP\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            "@%s\n"
            "0;JMP\n"
            "(RIP.%s.%d)\n",
            filename,
            return_ctr,
            num_args,
            function_name,
            filename,
            return_ctr);

    ++return_ctr;
}

/*
 * Writes assembly code that effects the return command.
 */
void write_return()
{
    fprintf(output_fd,
            "@LCL\n"
            "D=M\n"
            "@R13\n"
            "M=D\n"
            "@5\n"
            "D=A\n"
            "@R13\n"
            "A=M-D\n"
            "D=M\n"
            "@R14\n"
            "M=D\n"
            "@SP\n"
            "MD=M-1\n"
            "D=0\n"
            "@ARG\n"
            "A=M\n"
            "D=A\n"
            "@R15\n"
            "M=D\n"
            "@SP\n"
            "A=M\n"
            "D=M\n"
            "@R15\n"
            "A=M\n"
            "M=D\n"
            "@R15\n"
            "M=0\n"
            "@ARG\n"
            "D=M+1\n"
            "@SP\n"
            "M=D\n"
            "@R13\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@THAT\n"
            "M=D\n"
            "@R13\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@THIS\n"
            "M=D\n"
            "@R13\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@ARG\n"
            "M=D\n"
            "@R13\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            "@R13\n"
            "M=0\n"
            "@R14\n"
            "D=M\n"
            "M=0\n"
            "A=D\n"
            "0;JMP\n");
}

/*
 * Writes assembly code that effects the function command.
 */
void write_function(char *function_name, int num_locals)
{
    if(in_function_name) {
        free(in_function_name);
    }

    in_function_name = (char *)malloc(strlen(function_name) + 1);

    if(in_function_name == NULL) {
        fprintf(stderr, "Unable to allocation function name\n");
    }
    
    strncpy(in_function_name, function_name, strlen(function_name));
    in_function_name[strlen(function_name)] = '\0';

    fprintf(output_fd,
            "(%s)\n"
            "@%d\n"
            "D=A\n"
            "@R13\n"
            "M=D\n"
            "@%s.while.check\n"
            "0;JMP\n"
            "(%s.while)\n"
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "D=A+1\n"
            "@SP\n"
            "M=D\n"
            "@R13\n"
            "M=M-1\n"
            "(%s.while.check)\n"
            "@R13\n"
            "D=M\n"
            "@%s.while\n"
            "D;JNE\n"
            "@R13\n"
            "M=0\n",
            function_name,
            num_locals,
            function_name,
            function_name,
            function_name,
            function_name);
}
