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
        strncpy(result, "000", strlen("000"));
    }
    else if(strncmp(str, "AMD", strlen("AMD")) == 0) {
        strncpy(result, "111", strlen("111"));
    }
    else if(strncmp(str, "AD", strlen("AD")) == 0) {
        strncpy(result, "110", strlen("110"));
    }
    else if(strncmp(str, "AM", strlen("AM")) == 0) {
        strncpy(result, "101", strlen("101"));
    }
    else if(strncmp(str, "MD", strlen("MD")) == 0) {
        strncpy(result, "011", strlen("011"));
    }
    else if(strncmp(str, "A", strlen("A")) == 0) {
        strncpy(result, "100", strlen("100"));
    }
    else if(strncmp(str, "D", strlen("D")) == 0) {
        strncpy(result, "010", strlen("010"));
    }
    else if(strncmp(str, "M", strlen("M")) == 0) {
        strncpy(result, "001", strlen("001"));
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
        strncpy(result, "0101010", strlen("0101010"));
    }
    else if(strncmp(str, "1",  strlen(str)) == 0) {
        strncpy(result, "0111111", strlen("0111111"));
    }
    else if(strncmp(str, "-1", strlen(str)) == 0) {
        strncpy(result, "0111010", strlen("0111010"));
    }
    else if(strncmp(str, "D", strlen(str)) == 0) {
        strncpy(result, "0001100", strlen("0001100"));
    }
    else if(strncmp(str, "A", strlen(str)) == 0) {
        strncpy(result, "0110000", strlen("0110000"));
    }
    else if(strncmp(str, "M", strlen(str)) == 0) {
        strncpy(result, "1110000", strlen("1110000"));
    }
    else if(strncmp(str, "!D", strlen(str)) == 0) {
        strncpy(result, "0001101", strlen("0001101"));
    }
    else if(strncmp(str, "!A", strlen(str)) == 0) {
        strncpy(result, "0110001", strlen("0110001"));
    }
    else if(strncmp(str, "!M", strlen(str)) == 0) {
        strncpy(result, "1110001", strlen("1110001"));
    }
    else if(strncmp(str, "-D", strlen(str)) == 0) {
        strncpy(result, "0001111", strlen("0001111"));
    }
    else if(strncmp(str, "-A", strlen(str)) == 0) {
        strncpy(result, "0110011", strlen("0110011"));
    }
    else if(strncmp(str, "-M", strlen(str)) == 0) {
        strncpy(result, "1110011", strlen("1110011"));
    }
    else if(strncmp(str, "D+1", strlen(str)) == 0) {
        strncpy(result, "0011111", strlen("0011111"));
    }
    else if(strncmp(str, "A+1", strlen(str)) == 0) {
        strncpy(result, "0110111", strlen("0110111"));
    }
    else if(strncmp(str, "M+1", strlen(str)) == 0) {
        strncpy(result, "1110111", strlen("1110111"));
    }
    else if(strncmp(str, "D-1", strlen(str)) == 0) {
        strncpy(result, "0001110", strlen("0001110"));
    }
    else if(strncmp(str, "A-1", strlen(str)) == 0) {
        strncpy(result, "0110010", strlen("0110010"));
    }
    else if(strncmp(str, "M-1", strlen(str)) == 0) {
        strncpy(result, "1110010", strlen("1110010"));
    }
    else if(strncmp(str, "D+A", strlen(str)) == 0) {
        strncpy(result, "0000010", strlen("0000010"));
    }
    else if(strncmp(str, "D+M", strlen(str)) == 0) {
        strncpy(result, "1000010", strlen("1000010"));
    }
    else if(strncmp(str, "D-A", strlen(str)) == 0) {
        strncpy(result, "0010011", strlen("0010011"));
    }
    else if(strncmp(str, "D-M", strlen(str)) == 0) {
        strncpy(result, "1010011", strlen("1010011"));
    }
    else if(strncmp(str, "A-D", strlen(str)) == 0) {
        strncpy(result, "0000111", strlen("0000111"));
    }
    else if(strncmp(str, "M-D", strlen(str)) == 0) {
        strncpy(result, "1000111", strlen("1000111"));
    }
    else if(strncmp(str, "D&A", strlen(str)) == 0) {
        strncpy(result, "0000000", strlen("0000000"));
    }
    else if(strncmp(str, "D&M", strlen(str)) == 0) {
        strncpy(result, "1000000", strlen("1000000"));
    }
    else if(strncmp(str, "D|A", strlen(str)) == 0) {
        strncpy(result, "0010101", strlen("0010101"));
    }
    else if(strncmp(str, "D|M", strlen(str)) == 0) {
        strncpy(result, "1010101", strlen("1010101"));
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
        strncpy(result, "000", strlen("000"));
    }
    else if(strncmp(str, "JGT", strlen(str)) == 0) {
        strncpy(result, "001", strlen("001"));
    }
    else if(strncmp(str, "JEQ", strlen(str)) == 0) {
        strncpy(result, "010", strlen("010"));
    }
    else if(strncmp(str, "JGE", strlen(str)) == 0) {
        strncpy(result, "011", strlen("011"));
    }
    else if(strncmp(str, "JLT", strlen(str)) == 0) {
        strncpy(result, "100", strlen("100"));
    }
    else if(strncmp(str, "JNE", strlen(str)) == 0) {
        strncpy(result, "101", strlen("101"));
    }
    else if(strncmp(str, "JLE", strlen(str)) == 0) {
        strncpy(result, "110", strlen("110"));
    }
    else if(strncmp(str, "JMP", strlen(str)) == 0) {
        strncpy(result, "111", strlen("111"));
    }

    result[3] = '\0';

    return result;
}
