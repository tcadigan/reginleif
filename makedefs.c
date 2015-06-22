/* Construct definitions of object constants */
#include "makedefs.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEF_FILE "def.objects.h"
#define LINSZ 1000
#define STRSZ 40

int fd;
char string[STRSZ];

int main(int argc, char *argv[])
{
    int index = 0;
    int propct = 0;
    char *sp;

    fd = open(DEF_FILE, 0);
    if(fd < 0) {
        perror(DEF_FILE);
        
        return 1;
    }

    skipuntil("objects[] = {");

    while(getentry()) {
        if(*string == 0) {
            ++index;

            continue;
        }

        for(sp = string; *sp != 0; ++sp) {
            if((*sp == ' ') || (*sp == '\t')) {
                *sp = '_';
            }
        }

        if(strncmp(string, "RIN_", 4) == 0) {
            capitalize(string + 4);
            
            printf("#define %s u.uprops[%d].p_flgs\n", string + 4, propct);
            ++propct;
        }
        
        for(sp = string; *sp != 0; ++sp) {
            capitalize(sp);
        }
        
        /* Avoid stupid trouble with stupid C preprocessors */
        if(strncmp(string, "WORTHLESS_PIECE_OF_", 19) == 0) {
            printf("/* #define %s %d */\n", string, index);
        }
        else {
            printf("#define %s %d\n", string, index);

        }

        ++index;
    }

    printf("\n#define CORPSE DEAD_HUMAN\n");
    printf("#define LAST_GEM (JADE + 1)\n");
    printf("#define LAST_RING %d\n", propct);
    printf("#define NROFOBJECTS %d\n", index - 1);

    return 0;
}

char line[LINSZ];
char *lp = line;
char *lp0 = line;
char *lpe = line;
int eof;

void readline()
{
    int n = read(fd, lp0, (line + LINSZ) - lp0);
    if(n < 0) {
        printf("Input error.\n");

        exit(1);
    }

    if(n == 0) {
        ++eof;
    }

    lpe = lp0 + n;
}

char nextchar()
{
    if(lp == lpe) {
        readline();

        lp = lp0;
    }

    if(lp == lpe) {
        return 0;
    }
    else {
        ++lp;

        return *(lp - 1);
    }
}

int skipuntil(char *s)
{
    char *sp0;
    char *sp1;

    while(1) {
        while(*s != nextchar()) {
            if(eof != 0) {
                printf("Cannot skipuntil %s\n", s);
                
                exit(1);
            }
        }

        if(strlen(s) > ((lpe - lp) + 1)) {
            char *lp1;
            char *lp2;

            lp2 = lp;
            lp = lp0;
            lp1 = lp;

            while(lp2 != lpe) {
                *lp1 = *lp2;
                ++lp1;
                ++lp2;
            }

            lp2 = lp0; /* Save value */
            lp0 = lp1;
            
            readline();

            lp0 = lp2;

            if(strlen(s) > ((lpe - lp) + 1)) {
                printf("Error in skipuntil");

                exit(1);
            }
        }

        sp0 = s + 1;
        sp1 = lp;

        while((*sp0 != 0) && (*sp0 == *sp1)) {
            ++sp0;
            ++sp1;
        }

        if(*sp0 == 0) {
            lp = sp1;

            break;
        }
    }

    return 1;
}

int getentry()
{
    int inbraces = 0;
    int inparens = 0;
    int stringseen = 0;
    int commaseen = 0;
    int prefix = 0;
    char ch;
#define NSZ 10
    char identif[NSZ];
    char *ip;

    string[4] = 0;
    string[0] = string[4];

    /*
     * Read until {...} or XXX(...) followed by,
     * skip comment and #define lines
     * deliver 0 on failure
     */

    int counter = 0;

    ch = nextchar();
    while(1) {

        /* START DEBUG */
        ++counter;
        if(counter > 100) {
            exit(1);
        }
        /* END DEBUG */

        if(letter(ch) != 0) {
            ip = identif;
            
            if(ip < ((identif + NSZ) - 1)) {
                *ip = ch;
                ++ip;
            }

            ch = nextchar();

            while((letter(ch) != 0) || (digit(ch) != 0)) {
                if(ip < ((identif + NSZ) - 1)) {
                    *ip = ch;
                    ++ip;
                }

                ch = nextchar();
            }

            *ip = 0;

            while((ch == ' ') || (ch == '\t')) {
                ch = nextchar();
            }

            if((ch == '(') && (inparens == 0) && (stringseen == 0)) {
                if((strcmp(identif, "WAND") == 0)
                   || (strcmp(identif, "RING") == 0)
                   || (strcmp(identif, "POTION") == 0) 
                   || (strcmp(identif, "SCROLL") == 0)) {
                    strncpy(string, identif, 3);
                    string[3] = '_';
                    prefix = 4;
                }
            }
        }

        switch(ch) {
        case '/':
            /* Watch for comment */
            ch = nextchar();
            if(ch == '*') {
                skipuntil("*/");
            }
            
            continue;
        case '{':
            ++inbraces;

            ch = nextchar();

            continue;
        case '(':
            ++inparens;

            ch = nextchar();

            continue;
        case '}':
            --inbraces;

            if(inbraces < 0) {
                return 0;
            }

            ch = nextchar();

            continue;
        case ')':
            --inparens;

            if(inparens < 0) {
                printf("Too many ')'?");
                
                exit(1);
            }

            ch = nextchar();
            
            continue;
        case '\n':
            /* Watch for #define at begin of line */
            ch = nextchar();
            if(ch == '#') {
                char pch;

                /* Skip until '\n' not preceded by '\\' */
                pch = ch;
                ch = nextchar();

                while((ch != '\n') || (pch == '\\')) {
                    pch = ch;
                    ch = nextchar();
                }

                ch = nextchar();
                
                continue;
            }

            continue;
        case ',':
            if((inparens == 0) && (inbraces == 0)) {
                if((prefix != 0) && (string[prefix] == 0)) {
                    string[0] = 0;
                }

                if(stringseen != 0) {
                    return 1;
                }

                printf("Unexpected '\n'");

                exit(1);
            }

            ++commaseen;

            ch = nextchar();

            continue;
        case '\'':
            ch = nextchar();

            if(ch == '\\') {
                ch = nextchar();
            }

            if(nextchar() != '\'') {
                printf("String character notation?\n");

                exit(1);
            }

            ch = nextchar();

            continue;
        case '"':
            {
                char *sp = string + prefix;
                char pch;

                int store = 0;

                if((inbraces == 1) || (inparens == 1)) {
                    if((stringseen == 0) && (commaseen == 0)) {
                        store = 1;
                    }

                    ++stringseen;
                }

                pch = ch;
                ch = nextchar();

                if((store != 0) && (sp < (string + STRSZ))) {
                    *sp = ch;
                    ++sp;
                }

                while((ch != '"') || (pch == '\\')) {
                    pch = ch;
                    ch = nextchar();

                    if((store != 0) && (sp < (string + STRSZ))) {
                        *sp = ch;
                        ++sp;
                    }
                }

                if(store) {
                    --sp;
                    *sp = 0;
                }

                ch = nextchar();

                continue;
            }
        }

        ch = nextchar();
    }
}
                   
void capitalize(char *sp)
{
    if(('a' <= *sp) && (*sp <= 'z')) {
        *sp += ('A' - 'a');
    }
}

int letter(char ch)
{
    return ((('a' <= ch) && (ch <= 'z')) || (('A' <= ch) && (ch <= 'Z')));
}

int digit(char ch)
{
    return (('0' <= ch) && (ch <= '9'));
}
