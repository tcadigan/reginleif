/*
 * Encapsulates access to the input code. Reads an assembly language command,
 * parses it, and provides convenient access to the command's components (fields
 * and symbols). In addition, removes all white space and comments.
 */
#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fd;
char *command;

/*
 * Opens the input file/stream and gets ready to parse it.
 */
void initialize(char *input_file)
{
    fd = fopen(input_file, "r");

    if(!fd) {
        fprintf(stderr, "Unable to open file \'%s\'", input_file);
    }

    command = (char *)malloc(257);

    if(command == NULL) {
        fprintf(stderr, "Unable to create command buffer");
    }
}

/*
 * Are there more commands in the input?
 */
int has_more_commands()
{
    if(fd == NULL) {
        return 0;
    }
    
    char c = fgetc(fd);
    
    while(1) {
        if(c == '/') {
            c = fgetc(fd);
            
            if(c == '/') {
                while((c != '\n') && !feof(fd)) {
                    c = fgetc(fd);
                }
            }
            else {
                ungetc(c, fd);
                ungetc('/', fd);

                break;
            }
        }
        else if(!isspace(c)) {
            ungetc(c, fd);

            break;
        }
        else if(feof(fd)) {
            break;
        }

        c = fgetc(fd);
    }

    return !feof(fd);
}

/*
 * Reads the next command from the input and makes it the current
 * command. Should be called only if hasMoreCommands() is true. Initially there
 * is no current command.
 */
void advance()
{
    if(has_more_commands() && command) {
        char c = fgetc(fd);
        int i = 0;
        int full = 0;
        int maybe_comment = 0;
        while((c != '\n') && !feof(fd)) {
            if((c == '\t') || (c == '\v') || (c == '\f') || (c == ' ')) {
                c = fgetc(fd);

                continue;
            }
            
            if(i > 256) {
                full = 1;
            }

            if(!full) {
                if(c == '/') {
                    ++maybe_comment;
                }

                if(maybe_comment == 2) {
                    ungetc('/', fd);
                    ungetc('/', fd);
                    command[i - 1] = '\0';

                    break;
                }
                else if(maybe_comment < 2) {
                    command[i] = c;
                    ++i;
                    c = fgetc(fd);
                }
            }
        }

        command[i] = '\0';

        if(command[i - 1] == '\r') {
            command[i - 1] = '\0';
        }

        while(isspace(command[strlen(command) - 1])) {
            command[strlen(command) - 1] = '\0';
        }
                
        if(!feof(fd)) {
            c = ungetc(c, fd);
        }
    }
}

/*
 * Returns the type of the current command:
 *     A_COMMAND for @Xxx where Xxx is either a symbol or a decimal number
 *     C_COMMAND for dest=comp;jump
 *     L_COMMAND (actually, pseudo-command) for (Xxx) where Xxx is a symbol
 */
int command_type()
{
    if(command[0] == '@') {
        return A_COMMAND;
    }
    else if(command[0] == '(') {
        return L_COMMAND;
    }
    else {
        return C_COMMAND;
    }
}

/*
 * Returns the symbol or decimal Xxx of the current command @Xxx or
 * (Xxx). Should be called only when commandType() is A_COMMAND or L_COMMAND.
 */
char *get_symbol(void)
{
    char *result = NULL;
    
    if(command_type() == A_COMMAND) {
        result = (char *)malloc(strlen(command));
        
        if(result == NULL) {
            fprintf(stderr, "Unable to create symbol");

            return NULL;
        }
        
        memcpy(result, command + 1, strlen(command) - 1);
        result[strlen(command) - 1] = '\0';
    }
    else if(command_type() == L_COMMAND) {
        result = (char *)malloc((strlen(command) - 1));

        if(result == NULL) {
            fprintf(stderr, "Unable to create symbol");

            return NULL;
        }

        memcpy(result, command + 1, strlen(command) - 2);
        result[strlen(command) - 2] = '\0';
    }
    else if(command_type() == C_COMMAND) {
        fprintf(stderr, "Invalid command type for symbol()");
    }

    return result;
}

/*
 * Returns the dest mnemonic in current C-command (8 possibilities). Should be
 * called only when commandType() is C_COMMAND.
 */
char *get_dest(void)
{
    if(command_type() != C_COMMAND) {
        fprintf(stderr, "Invalid command type for dest()");
        
        return NULL;
    }

    int i = 4;
    
    if(command[1] == '=') {
        i = 1;
    }
    else if(command[2] == '=') {
        i = 2;
    }
    else if(command[3] == '=') {
        i = 3;
    }

    char *result = (char *)malloc(i + 1);
    
    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for dest");
        
        return NULL;
    }

    if(i < 3) {
        int k = 0;

        while(k < i) {
            result[k] = command[k];
            ++k;
        }
    }
    else {
        strncpy(result, "null", strlen("null"));
    }

    result[i] = '\0';

    return result;
}

/*
 * Returns the comp mnemonic in the current C-command (28 possibilities). Should
 * be called only when commandType() is C_COMMAND.
 */
char *get_comp(void)
{
    if(command_type() != C_COMMAND) {
        fprintf(stderr, "Invalid command type for comp()");

        return NULL;
    }

    int i = 0;
    
    if(command[1] == '=') {
        i = 1;
    }
    else if(command[2] == '=') {
        i = 2;
    }
    else if(command[3] == '=') {
        i = 3;
    }

    int j = strlen(command);

    if(command[strlen(command) - 4] == ';') {
        j = strlen(command) - 4;
    }

    char *result = (char *)malloc(j - i);
    
    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for comp");
        
        return NULL;
    }

    int k = 0;

    while(k < (j - i)) {
        if(i == 0) {
            result[k] = command[k];
        }
        else {
            result[k] = command[i + k + 1];
        }

        ++k;
    }

    result[j - i] = '\0';

    return result;
}

/*
 * Returns the jump mnemonic in current C-command (8 possibilities). Should be
 * called only when commandType() is C_COMMAND.
 */
char *get_jump(void)
{
    if(command_type() != C_COMMAND) {
        fprintf(stderr, "Invalid command type for jump()");
        
        return NULL;
    }

    char *result = NULL;
    
    if(command[strlen(command) - 4] == ';') {
        result = (char *)malloc(strlen("XXX") + 1);
        
        if(result == NULL) {
            fprintf(stderr, "Unable to allocate space for jump");
            
            return NULL;
        }

        strncpy(result, command + strlen(command) - 3, strlen("XXX"));
        result[3] = '\0';
    }
    else {
        result = (char *)malloc(strlen("null") + 1);
        
        if(result == NULL) {
            fprintf(stderr, "Unable to allocate space for jump");
            
            return NULL;
        }

        strncpy(result, "null", strlen("null"));
        result[4] = '\0';        
    }

    return result;
}
