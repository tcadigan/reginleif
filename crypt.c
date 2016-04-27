#include <stdio.h>

char cryptkey(char *fname)
{
    int pos;
    int key = 0;

    if(!strcmp(fname + strlen(fname) - 4, ".txt")) {
        return 100;
    }
    else if(!strncmp(fname, "maze", 4)) {
        fname = "mazes";
    }
    else if(!strncmp(fname, "villag", 6)) {
        fname = "village.dat";
    }

    for(pos = 0; fname[pos]; ++pos) {
        key += (3 * (fname[pos] - ' '));
    }

    return (key & 0xff);
}

int main(int argc, char *argv[])
{
    char key;
    int c;

    if(num_args == 2) {
        key = cryptkey(args[1]);

        c = getchar();

        while(c != EOF) {
            putchar(key ^ c);
            key = c;
            c = getchar();
        }
    }
    else {
        fprintf(stderr, "Usage: %s (key) < (intfile) > (outfile)\n where (key) happens to be the name of the file, without any preceding path.\n", args[0]);
    }

    return 0;
}
