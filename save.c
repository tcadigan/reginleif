// Save and restore routines
//
// @(#)save.c 3.9 (Berkeley) 6/16/81
#include "save.h"

#include "io.h"
#include "main.h"
#include "options.h"
#include "rogue.h"

#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include <term.h>

typedef struct stat STAT;

extern char version[];
extern char encstr[];

STAT sbuf;

// save_game:
//     Something...
int save_game()
{
    FILE *savef;
    int c;
    char buf[80];

    // Get the file name
    mpos = 0;
    
    if(file_name[0] != '\0') {
	msg("Save file (%s)? ", file_name);

        c = getchar();

        while((c != 'n') && (c != 'N') && (c != 'y') && (c != 'Y')) {
            c = getchar();
        }

	mpos = 0;
        
	if((c == 'y') || (c == 'Y')) {
	    msg("File name: %s", file_name);

            savef = fopen(file_name, "w");

            if(savef == NULL) {
                // Fake perror()
                msg("%s", strerror(errno));
            }

            while(savef == NULL) {
                msg("File name: ");
                mpos = 0;
                buf[0] = '\0';

                if(get_str(buf, cw) == QUIT) {
                    msg("");

                    return FALSE;
                }

                strcpy(file_name, buf);
                savef = fopen(file_name, "w");

                if(savef == NULL) {
                    // Fake perror()
                    msg("%s", strerror(errno));
                }
            }

            // Write out encrypted file (after a stat),
            // the fwrite() is to force allocation of the buffer
            // before the write
            save_file(savef);
            return TRUE;
	}
    }

    msg("File name: ");
    mpos = 0;
    buf[0] = '\0';

    if(get_str(buf, cw) == QUIT) {
        msg("");
        return FALSE;
    }

    strcpy(file_name, buf);
    savef = fopen(file_name, "w");
    
    if(savef == NULL) {
        // Fake perror()
        msg("%s", strerror(errno));
    }

    while(savef == NULL) {
        msg("File name: ");
        mpos = 0;
        buf[0] = '\0';

        if(get_str(buf, cw) == QUIT) {
            msg("");
            return FALSE;
        }

        strcpy(file_name, buf);
        savef = fopen(file_name, "w");

        if(savef == NULL) {
            // Fake perror()
            msg("%s", strerror(errno));
        }
    }

    // Write out encrypted file (after a stat),
    // The fwrite() is to force allocation of the buffer
    // before the write
    save_file(savef);

    return TRUE;
}

// auto_save:
//     Automatically save a file. This is used if a HUP
//     signal is recieved
void auto_save(int parameter)
{
    FILE *savef;
    int i;

    for(i = 0; i < NSIG; ++i) {
	signal(i, SIG_IGN);
    }
    
    
    if(file_name[0] != '\0') {
        savef = fopen(file_name, "w");
        if(savef != NULL) {
            save_file(savef);
        }
    }
    
    exit(1);
}

// save_file:
//     Write the saved game on the file
int save_file(FILE *savef)
{
    wmove(cw, LINES-1, 0);
    wrefresh(cw);
    fstat(fileno(savef), &sbuf);
    fwrite("junk", 1, 5, savef);
    fseek(savef, 0L, 0);
    encwrite(version, (char *)sbrk(0) - version, savef);
    fclose(savef);

    return 0;
}

// restore:
//     Something...
int restore(char *file, char **envp)
{
    int inf;
    extern char **environ;
    char buf[80];
    STAT sbuf2;

    if(strcmp(file, "-r") == 0) {
	file = file_name;
    }
    inf = open(file, 0);
    if(inf < 0) {
	perror(file);

	return FALSE;
    }

    fflush(stdout);
    encread(buf, strlen(version) + 1, inf);
    if(strcmp(buf, version) != 0) {
	printf("Sorry, saved game is out of date.\n");

	return FALSE;
    }

    fstat(inf, &sbuf2);
    fflush(stdout);
    brk(version + sbuf2.st_size);
    lseek(inf, 0L, 0);
    encread(version, (unsigned int) sbuf2.st_size, inf);

    // We do not close the file so that we will have a hold of the
    // inode for as long as possible
    if(!wizard) {
	if((sbuf2.st_ino != sbuf.st_ino) || (sbuf2.st_dev != sbuf.st_dev)) {
	    printf("Sorry, saved game is not in the same file.\n");
            
	    return FALSE;
	}
	else if((sbuf2.st_ctime - sbuf.st_ctime) > 300) {
	    printf("Sorry, file has been touched.\n");
            
	    return FALSE;
	}
    }
    mpos = 0;
    mvwprintw(cw, 0, 0, "%s: %s", file, ctime(&sbuf2.st_mtime));

    // Defeat multiple restarting from the same place
    if(!wizard) {
	if(sbuf2.st_nlink != 1) {
	    printf("Cannot restore from a linked file\n");
	    return FALSE;
	}
	else if(unlink(file) < 0) {
	    printf("Cannot unlink file\n");
	    return FALSE;
	}
    }

    environ = envp;
    strcpy(file_name, file);
    setup();
    clearok(curscr, TRUE);
    touchwin(cw);
    srand(getpid());
    playit();

    /*NOT REACHED*/
    return 0;
}

// encwrite:
//     Perform an encrypted write
int encwrite(char *start, unsigned int size, FILE *outf)
{
    char *ep;

    ep = encstr;

    while(size--) {
	putc(*start++ ^ *ep++, outf);
	if(*ep == '\0') {
	    ep = encstr;
        }
    }

    return 0;
}

// encread:
//     Perform an encrypted read
int encread(char *start, unsigned int size, int inf)
{
    char *ep;
    int read_size;

    read_size = read(inf, start, size);
    if((read_size == -1) || (read_size == 0)) {
	return read_size;
    }

    ep = encstr;

    while(size--) {
	*start++ ^= *ep++;
	if(*ep == '\0') {
	    ep = encstr;
        }
    }
    
    return read_size;
}
