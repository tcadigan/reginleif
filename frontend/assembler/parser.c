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
void initialize_parser(char *input_file)
{
    if(fd) {
        fclose(fd);
    }
    
    fd = fopen(input_file, "r");

    if(!fd) {
        fprintf(stderr, "Unable to open file \'%s\'", input_file);
    }

    if(command == NULL) {
        command = (char *)malloc(257);
        
        if(command == NULL) {
            fprintf(stderr, "Unable to create command buffer");
        }
    }
}

void destroy_parser()
{
    if(fd) {
        fclose(fd);
    }

    if(command) {
        free(command);
    }
}

/*
 * Are there more commands in the input?
 */
int parser_has_more_commands()
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
void advance_parser()
{
    if(parser_has_more_commands() && command) {
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
int get_command_type()
{
    switch(command[0]) {
    case '@':

        return A_COMMAND;
    case '(':

        return L_COMMAND;
    default:

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

    switch(get_command_type()) {
    case A_COMMAND:
        result = (char *)malloc(strlen(command));
        
        if(result == NULL) {
            fprintf(stderr, "Unable to create symbol");

            return NULL;
        }
        
        memcpy(result, command + 1, strlen(command) - 1);
        result[strlen(command) - 1] = '\0';

        break;
    case L_COMMAND:
        result = (char *)malloc((strlen(command) - 1));

        if(result == NULL) {
            fprintf(stderr, "Unable to create symbol");

            return NULL;
        }

        memcpy(result, command + 1, strlen(command) - 2);
        result[strlen(command) - 2] = '\0';

        break;
    case C_COMMAND:
        fprintf(stderr, "Invalid command type for get_symbol()");

        break;
    default:
        fprintf(stderr, "Unknown command type for get_symbol()");
    }

    return result;
}

/*
 * Returns the dest mnemonic in current C-command (8 possibilities). Should be
 * called only when commandType() is C_COMMAND.
 */
char *get_dest(void)
{
    if(get_command_type() != C_COMMAND) {
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
        strncpy(result, command, i);
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
    if(get_command_type() != C_COMMAND) {
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

    if((j >= 4) && (command[j - 4] == ';')) {
        j = strlen(command) - 4;
    }

    char *result = (char *)malloc(j - i + 1);
    
    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for comp");
        
        return NULL;
    }

    if(i == 0) {
        strncpy(result, command, j - i);
    }
    else {
        strncpy(result, command + i + 1, j - i);
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
    if(get_command_type() != C_COMMAND) {
        fprintf(stderr, "Invalid command type for jump()");
        
        return NULL;
    }

    char *result = NULL;
    
    if((strlen(command) >= 4) && (command[strlen(command) - 4] == ';')) {
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
