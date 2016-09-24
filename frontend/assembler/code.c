/*
 * Translates Hack assembly language mnemonics into binary code.
 */
#include "code.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns the binary code of the dest mnemonic. */
char *dest(char *str)
{
    if(str == NULL) {
        return NULL;
    }

    char *result = (char *)malloc(4);

    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for dest");

        return NULL;
    }

    if(strncmp(str, "null", strlen("null")) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
    }
    else if(strncmp(str, "AMD", strlen("AMD")) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
    }
    else if(strncmp(str, "AD", strlen("AD")) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '0';
    }
    else if(strncmp(str, "AM", strlen("AM")) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '1';
    }
    else if(strncmp(str, "MD", strlen("MD")) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
    }
    else if(strncmp(str, "A", strlen("A")) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '0';
    }
    else if(strncmp(str, "D", strlen("D")) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '0';
    }
    else if(strncmp(str, "M", strlen("M")) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
    }

    result[3] = '\0';

    return result;
}

/* Returns the binary code of the comp mnemonic. */
char *comp(char *str)
{
    if(str == NULL) {
        return NULL;
    }

    char *result = (char *)malloc(8);

    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for comp");

        return NULL;
    }

    if(strncmp(str, "0", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '0';
        result[3] = '1';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "1",  strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '1';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "-1", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '1';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "D", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '1';
        result[4] = '1';
        result[5] = '0';
        result[6] = '0';
    }
    else if(strncmp(str, "A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '0';
    }
    else if(strncmp(str, "M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '0';
    }
    else if(strncmp(str, "!D", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '1';
        result[4] = '1';
        result[5] = '0';
        result[6] = '1';
    }
    else if(strncmp(str, "!A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '1';
    }
    else if(strncmp(str, "!M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '1';
    }
    else if(strncmp(str, "-D", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
        result[3] = '1';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "-A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "-M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "D+1", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
        result[3] = '1';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "A+1", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "M+1", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "D-1", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '1';
        result[4] = '1';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "A-1", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "M-1", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "D+A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "D+M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '0';
    }
    else if(strncmp(str, "D-A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "D-M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '1';
        result[3] = '0';
        result[4] = '0';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "A-D", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "M-D", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '1';
        result[5] = '1';
        result[6] = '1';
    }
    else if(strncmp(str, "D&A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '0';
    }
    else if(strncmp(str, "D&M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '0';
        result[3] = '0';
        result[4] = '0';
        result[5] = '0';
        result[6] = '0';
    }
    else if(strncmp(str, "D|A", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
        result[3] = '0';
        result[4] = '1';
        result[5] = '0';
        result[6] = '1';
    }
    else if(strncmp(str, "D|M", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '1';
        result[3] = '0';
        result[4] = '1';
        result[5] = '0';
        result[6] = '1';
    }
    
    result[7] = '\0';

    return result;
}

/* Returns the binary code of the jump mnemonic */
char *jump(char *str)
{
    if(str == NULL) {
        return NULL;
    }

    char *result = (char *)malloc(4);

    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for jump");

        return NULL;
    }
    
    if(strncmp(str, "null", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '0';
    }
    else if(strncmp(str, "JGT", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '0';
        result[2] = '1';
    }
    else if(strncmp(str, "JEQ", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '0';
    }
    else if(strncmp(str, "JGE", strlen(str)) == 0) {
        result[0] = '0';
        result[1] = '1';
        result[2] = '1';
    }
    else if(strncmp(str, "JLT", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '0';
    }
    else if(strncmp(str, "JNE", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '0';
        result[2] = '1';
    }
    else if(strncmp(str, "JLE", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '0';
    }
    else if(strncmp(str, "JMP", strlen(str)) == 0) {
        result[0] = '1';
        result[1] = '1';
        result[2] = '1';
    }

    result[3] = '\0';

    return result;
}
