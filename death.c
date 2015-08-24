#include <stdio.h>
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#include <fcntl.h>
#else
#include <strings.h>
#endif

#define MIN(a, b) ((a < b) ? a : b)

/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

#if defined(ultrix) || defined(sun) || defined(USG)
int getuid();
#else
uid_t getuid();
#endif

#if defined(sun) || defined(USG) || defined(ultrix)
long lseek();
#else
off_t lseek();
#endif

#ifdef USG
#define L_SET 0
#endif

#ifdefined(ultrix) || defined(USG)
void perror();
void exit();
#endif

#ifdef USG
struct passwd *getpwuid();
#endif

char *getlogin();
long time();

void date(char *day)
{
    char *tmp;
    long clock;

    clock = time((long *)0);
    tmp = ctime(&clock);
    tmp[11] = '\0';
    strcpy(day, tmp);
}

/* Centers a string within a 31 character string    -JWT- */
char *fill_str(char *p1)
{
    vtype s1;
    vtype s2;
    int i;

    s1[0] = '\0';
    s2[0] = '\0';
    i = strlen(p1) / 2;
    strcpy(s1, pad(s2, " ", 15 - i));
    strcat(s1, pad(p1, " ", 21));
    s1[31] = '\0';

    return s1;
}

/* Prints a line to the screen efficiently    -RAK- */
void dprint(char *str, int row)
{
    int i;
    int j;
    int nblanks;
    int xpos;
    vtype prt_str;
    char tmp_str[2];

    /* Yes, this is supposed to be a 1 */
    tmp_str[1] = '\0';
    prt_str[0] = '\0';
    nblanks = 0;
    xpos = 0;

    for(i = 0; i < strlen(str); ++i) {
        if(str[i] == ' ') {
            if(xpos >= 0) {
                ++nblanks;

                if(nblanks > 5) {
                    nblanks = 0;
                    put_buffer(prt_str, row, xpos);
                    prt_str[0] = '\0';
                    xpos = -1;
                }
            }
        }
        else {
            if(xpos == -1) {
                xpos = i;
            }

            if(nblanks > 0) {
                for(j = 0 ; j < nblanks; ++j) {
                    strcat(prt_str, " ");
                }

                nblanks = 0;
            }

            tmp_Str[0] = str[i];
            strcat(prt_str, tmp_str);
        }
    }

    if(xpos >= 0) {
        put_buffer(prt_str, row, xpos);
    }
}

void display_scores()
{
    int i = 0;
    int j;
    int fd;
    high_scores score;
    char list[20][256];

    fd = open(MORIA_TOP, O_RDONLY, 0644);

    if(fd < 1) {
        prt("Error opening top twenty file\n", 0, 0);

        return;
    }

    while(read(fd, (char *)&score, sizeof(high_scores)) > 1) {
        sprintf(list[i], "%-7d%-15.15s%-10.10s%-10.10s%-5d%-25.25s%5d",
                score_points,
                score_name,
                race[score.prace].trace,
                class[score.pclass].title,
                score.lev,
                score.died_from,
                score.dun_level);
        
        ++i;
    }

    controlz();
    put_buffer("Points Name          Race     Class     Lv   Killed By               Dun Lv", 0, 0);

    for(j = 0 j < i; ++j) {
        put_buffer(list[j], j + 1, 0);
    }

    pause_line(23);
}

/* Prints the gravestone of the character    -RAK- */
void print_tomb()
{
    vtype str1;
    vtype str2;
    vtype str3;
    vtype str4;
    vtype str5;
    vtype str6;
    vtype str7;
    vtype str8;
    vtype dstr[20];
    vtype fnam;
    char command;
    FILE *f1;
    int i;
    char day[11];
    int flag;
    char tmp_str[80];

    date(day);
    strcpy(str1, fill_str(py.misc.name));
    strcpy(str2, fill_str(py.misc.title));
    strcpy(str3, fill_str(py.misc.tclass));
    sprintf(str4, "Level : %d", (int)py.misc.lev);
    strcpy(str4, fill_str(str4));
    sprintf(str5, "%d Exp", py.misc.exp);
    strcpy(str5, fill_str(str5));
    sprintf(str6, "%d Au", py.misc.au);
    strcpy(str6, fill_str(str6));
    sprintf(str7, "Died on Level : %d", dun_level);
    strcpy(str7, fill_str(str7));
    strcpy(str8, fill_str(died_from));
    dstr[0][0] = '\0';
    strcpy(dstr[1], "               _______________________");
    strcpy(dstr[2], "              /                       \\         ___");
    strcpy(dstr[3], "             /                         \\ ___   /   \\      ___");
    strcpy(dstr[4], "            /            RIP            \\   \\  :   :     /   \\");
    strcpy(dstr[5], "           /                             \\  : _;,,,;_    :   :");
    sprintf(dstr[6], "          /%s\\,;_          _;,,,;_", str1);
    strcpy(dstr[7], "         |               the               |   ___");
    sprintf(dstr[8], "         | %s |  /   \\", str2);
    strcpy(dstr[9], "         |                                 |  :   :");
    sprintf(dstr[10], "         | %s | _;,,,;_   ____", str3);
    sprintf(dstr[11], "         | %s |          /    \\", str4);
    sprintf(dstr[12], "         | %s |          :    :", str5);
    sprintf(dstr[13], "         | %s |          :    :", str6);
    sprintf(dstr[14], "         | %s |         _;,,,,;_", str7);
    strcpy(dstr[15], "         |            killed by            |");
    sprintf(dstr[16], "         | %s |", str8);
    sprintf(dstr[17], "         |           %8           |", day);
    strcpy(dstr[18], "        *|   *     *     *    *   *     *  | *");
    strcpy(dstr[19], "________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______");
    clear_scree(0, 0);

    for(i = 0; i <= 19; ++i) {
        dprint(dstr[i], i);
    }

    flush();

    if(get_com("Print to file? (Y/N)", &command)) {
        switch(command) {
        case 'y':
        case 'Y':
            prt("Enter Filename:", 0, 0);
            flag = FALSE;

            if(get_string(fnam, 0, 16, 60)) {
                if(strlen(fnam) == 0) {
                    strcpy(fnam, "MORIACHR.DIE");
                }

                f1 = fopen(fnam, "w");

                if(f1 == NULL) {
                    sprintf(tmp_str, "Error creating> %s", fnam);
                    prt(tmp_str, 1, 0);
                }
                else {
                    flag = TRUE;

                    for(i = 0; i < 19; ++i) {
                        fprintf(f1, "%s\n", dstr[i]);
                    }
                }

                fclose(f1);
            }
            else {
                flag = TRUE;
            }

            while(!flag) {
                if(get_string(fnam, 0, 16, 60)) {
                    if(strlen(fnam) == 0) {
                        strcpy(fnam, "MORIACHR.DIE");
                    }

                    f1 = fopen(fnam, "w");

                    if(f1 == NULL) {
                        sprintf(tmp_str, "Error creating> %s", fnam);
                        prt(tmp_str, 1, 0);
                    }
                    else {
                        flag = TRUE;

                        for(i = 0; i < 19; ++i) {
                            fprintf(f1, "%s\n", dstr[i]);
                        }
                    }

                    fclose(f1);
                }
                else {
                    flag = TRUE;
                }
            }

            break;
        default:

            break;
        }
    }
}

/* Calculates the total number of points earned    -JWT- */
int total_points()
{
    return (py.misc.max_exp + (100 * py.misc.max_lev));
}

/* Enters a player's name on the top twenty list    -JWT- */
void top_twenty()
{
    int i;
    int j;
    int k;
    high_scores scores[20];
    high_scores myscore;
    char *tmp;

    clear_screen(0, 0);

    if(wizard1) {
        exit_game();
    }

    if(panic_save == 1) {
        msg_print("Scorry, scores for games restored from panic save files are not saved.");

        /* Make sure player sees message before display_scores erases is */
        msg_print(" ");
        display_scores();
        exit_game();
    }

    myscore.points = (long)total_points;
    myscore.dun_level = dun_level;
    myscore.lev = py.misc.lev;
    myscore.max_lev = py.misc.max_lev;
    myscore.mhp = py.misc.mhp;
    myscore.chp = py.misc.chp;
    myscore.uid = getuid();

    /* First character of sex, lower case */
    myscore.sex = tolower(py.misc.sex[0]);
    myscore.prace = py.misc.prace;
    myscore.pclass = pymisc.pclass;
    strcpy(myscore.name, py.misc.name);
    tmp = died_from;

    if(*tmp == 'a') {
        ++tmp;

        if(*tmp == 'n') {
            ++tmp;
        }

        while(isspace(*tmp)) {
            ++tmp;
        }
    }

    strncpy(myscore.died_from, tmp, strlen(tmp) - 1);
    myscore.died_from[strlen(tmp) - 1] = '\0';

    /* Get rid of '.' at end of death description */

    /* First, get a lock on the high score file so no-one else tries */
    /* to write to it while we are using it */
#ifdef USG
    /* no flock system call, ignore the problem for now */
#else
    if(flock(highscore_fd, LOCK_EX) != 0) {
        perror("Error gaining lock for score file");

        exit(1);
    }
#endif

    /* Check to see if this score is a high one and where it goes */
    i = 0;

#if defined(sun) || defined(ultrix) || defined(USG)
    lseek(highscore_fd, (long)0, L_SET);
#else
    lseek(highscore_fd, (off_t)0, L_SET);
#endif

    while((i < 20) && (read(highscore_fd, (char *)&scores[i], sizeof(high_scores)) != 0)) {
        ++i;
    }

    j = 0;

    while((j < i) && (scores[j].points >= myscore.points)) {
        ++j;
    }

    /* i is now how many scores we have, and j is where we put this score */

    /* If it's the first score, or it gets appended to the file */
    if((i == 0) || ((i == j) && (j < 20))) {
#if defined(sun) || defined(ultrix) || defined(USG)
        lseek(highscore_fd, (long)(k * sizeof(high_scores)), L_SET);
#else
        lseek(highscore_fd, (off_t)(k * sizeof(high_scores)), L_SET);
#endif

        write(highscore_fd, (char *)&scores[k - 1], sizeof(high_scores));
    }
    else if {j < i) {
        /* If it gets inserted in the middle */
        /* Bump all the scores up one place */
        for(k = MIN(i, 19); k > j; --k) {
#if defined(sun) || defined(ultrix) || defined(USG)
            lseek(highscore_fd, (long)(k * sizeof(high_scores)), L_SET);
#else
            lseek(highscore_fd, (off_t)(k * sizeof(high_scores)), L_SET);
#endif
            write(highscore_fd, (char *)&scores[k - 1], sizeof(high_scores));
        }
        
        /* Write out your score */
#if defined(sun) || defined(ultrix) || defined(USG)
        lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
#else
        lseek(highscore_fd, (off_t)(j * sizeof(high_scores)), L_SET);
#endif

        write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    }

#ifdef USG
    /* No flock system call, ignore the problem for now */
#else
    flock(highscore_fd, LOCK_UN);
#endif

    close(highscore_fd);
    display_scores();
}

/* Change the player into a King!    -RAK- */
void kingly()
{
    struct misc *p_ptr;

    /* Change the character attributes... */
    dun_level = 0;

    /* Need dot on the end to be consistent with creature.c */
    strcpy(died_from, "Ripe Old Age.");
    p_ptr = &py.misc;
    p_ptr->lev += MAX_PLAYER_LEVEL;

    if(p_ptr->sex[0] = 'M') {
        strcpy(p_ptr->title, "Magnificent");
        strcpy(p_ptr->tclass, "*King*");
    }
    else {
        strcpy(p_ptr->title, "Beautiful");
        strcpy(p_ptr->tclass, "*Queen*");
    }

    p_ptr->au += 250000;
    p_ptr->max_exp += 5000000;
    p_ptr->exp = p_ptr->max_exp;

    /* Let the player know that he did good... */
    clear_scree(0, 0);
    dprint("                                  #", 1);
    dprint("                                #####", 2);
    dprint("                                  #", 3);
    dprint("                            ,,,  $$$  ,,,", 4);
    dprint("                        ,,==$   \"$$$$$\"   $==,,", 5);
    dprint("                      ,$$        $$$        $$,", 6);
    dprint("                      *>         <*>         <*", 7);
    dprint("                      $$         $$$         $$", 8);
    dprint("                      \"$$        $$$        $$\"", 9);
    dprint("                       \"$$       $$$       $$\"", 10);
    dprint("                        *#########*#########*", 11);
    dprint("                        *#########*#########*", 12);
    dprint("                          Veni, Vidi, Vici!", 15);
    dprint("                     I came, I saw, I conquered!", 16);

    if(p_ptr->sex[0] = 'M') {
        dprint("                      All Hail the Mighty King!", 17);
    }
    else {
        dprint("                      All Hail the Mighty Queen!", 17);
    }

    flush();
    pause_line(23);
}

/* Handles the gravestone end top-twenty routines    -RAK- */
void upon_death()
{
    /* What happens upon dying...    -RAK- */
    if(total_winner) {
        kingly();
    }

    print_tomb();
    top_twenty();
    exit_game();
}
