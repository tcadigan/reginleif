/*
 * This utility program is not part of the sources to Omega. It is written by:
 * Nathan Glasser nathan@brokaw.lcs.mit.edu  (internet)
 *                nathan@mit-eddie.uucp      (usenet)
 * As such, it is copyright by Nathan Glasser, 1987-1989.
 * Please don't attempt to sell this program or take credit for it yourself,
 * and please don't remove this notice.
 *
 * This program reads in a C source file and replaces all static
 * strings with variable names as defined in the file, which are all
 * declared to be far arrays. You don't need to run this for the unix
 * version.
 *
 * This program does not know about comments, or \'s in front of "'s.
 * Thus it can be broken.
 */
#include <stdio.h>
#include <malloc.h>

#define TMPFILE "fixtmp.c"

int num_strings;
char **string_list;

#define REALLOC_INCR 500

int main(int argc, char *argv[])
{
    FILE *sourcefp;
    FILE *destfp;

    if(argc != 2) {
        printf("Usage: %s <cfile>\n", argv[0]);

        exit(1);
    }

    sourcefp = fopen(argv[1], "r");

    if(sourcefp != NULL) {
        destfp = fopen(TMPFILE, "w");
    }

    if((sourcefp == NULL) || (destfp == NULL)) {
        perror("Can't open a file (pass1)");

        exit(1);
    }

    printf("Scanning %s...", argv[1]);
    fflush(stdout);
    do_scan(sourcefp, destfp);
    printf("Done\n");

    fclose(sourcefp);
    fclose(destfp);

    sourcefp = fopen(TMPFILE, "r");

    if(sourcefp != NULL) {
        destfp = fopen(argv[1], "w");
    }

    if((sourcefp == NULL) || (destfp == NULL)) {
        perror("Can't open a file (pass2)");

        exit(1);
    }

    printf("Writing new %s...", argv[1]);
    fflush(stdout);
    do_output(sourcefp, destfp);
    printf("Done\n");
    remove(TMPFILE);

    exit(0);
}

char include[] = "#include";

#define include_size (sizeof(include) - 1)

void do_scan(FILE *sourcefp, FILE *destfp)
{
    int max_strings;
    int ch;
    int last_ch = EOF;
    char temp_string[128];
    char *temp;

    /* Variables for figuring out about #include's */
    int pos = 0;
    int include_flag = 0;
    int include_tmp = 1;

    max_strings = REALLOC_INCR;
    string_list = (char **)malloc(max_strings * sizeof(char *));

    ch = getc(sourcefp);

    while(ch != EOF) {
        switch(ch) {
        case '\n':
            putc(ch, destfp);
            include_flag = 0;
            pos = include_flag;
            include_tmp = 1;

            break;
        case '"':
            if(!include_flag && (last_ch != '\'')) {
                /* Start of a string */
                for(temp = temp_string; *temp != '"'; ++temp) {
                    *temp = getc(sourcefp);
                }

                *temp = '\0';
                string_list[num_strings] = (char *)malloc(temp - temp_string + 1);
                strcpy(string_list[num_strings], temp_string);
                fprintf(destfp, "_str_%d", num_strings);
                ++num_strings;
                
                if(num_strings == max_strings) {
                    max_strings += REALLOC_INCR;
                    string_list = (char **)realloc(string_list, max_strings * sizeof(char *));
                }

                include_tmp = 0;
            }
            
            break;
        default:
            if(include_tmp) {
                include_tmp = (ch == include[pos++]);

                if(include_tmp && (pos == include_size)) {
                    include_flag = 1;
                    include_tmp = 0;
                }
            }

            putc(ch, destfp);

            break;
        }

        last_ch = ch;
        ch = getc(sourcefp);
    }
}

void do_output(FILE *sourcefp, FILE *destfp)
{
    char buf[1024];
    int i;

    fprintf(destfp, "/* These static strings have been moved here to */\n");
    fprintf(destfp, "/* Declare them as far and avoid having too much */\n");
    fprintf(destfp, "/* initialized memory in the CONST segment. */\n\n");

    for(i = 0; i < num_strings; ++i) {
        fprintf(destfp, "static char far _str_%d[] = \"%s\";\n", i, string_list[i]);
    }

    putc('\n', destfp);

    i = fread(buf, 1, sizeof(buf), sourcefp);

    while(i) {
        fwrite(buf, i, 1, destfp);
        i = fread(buf, 1, sizeof(buf), sourcefp);
    }
}
