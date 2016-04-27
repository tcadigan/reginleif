/*
 * Omega copyright (C) by Laurence Raphael Brothers, 1987-1989
 * 
 * oabyss.c
 * 
 * Some functions to make the abyss level and run the final challenge
 */

#include "oglob.h"

#ifdef MSDOS
#include <alloc.h>
#include <dos.h>
#include <sys/timeb.h>
#include <sys/types.h>
#endif

/* Loads the abyss level into Level */
void load_abyss()
{
    int i;
    int j;
    char site;
    FILE *fd;

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
#ifndef MSDOS
        free((char *)TempLevel);
#endif
        TempLevel = NULL;
    }

#ifndef MSDOS
    Level = (plv)malloc(sizeof(levtype));
#else
    msdos_changelevel(TempLevel, 0, -1);
    Level = &TheLevel;
#endif

    clear_level(Level);

    strcpy(Str3, OMEGALIB);
    strcat(Str3, "oabyss.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0, i < WIDTH; ++i) {
            site = getc(fd);

            switch(site) {
            case '0':
                Level->site[i][j].locchar = ' ';
                Level->site[i][j].p_locf = L_VOID;

                break;
            case 'V':
                Level->site[i][j].locchar = ' ';
                Level->site[i][j].p_locf = L_VOID_STATION;

                break;
            case '1':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_VOICE1;

                break;
            case '2':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_VOICE2;

                break;
            case '3':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_VOICE3;

                break;
            case '~':
                Level->site[i][j].locchar = WATER;
                Level->site[i][j].p_locf = L_WATER_STATION;

                break;
            case ';':
                Level->site[i][j].locchar = FIRE;
                Level->site[i][j].p_locf = L_FIRE_STATION;

                break;
            case '"':
                Level->site[i][j].locchar = HEDGE;
                Level->site[i][j].p_locf = L_EARTH_STATION;

                break;
            case '6':
                Level->site[i][j].locchar = WHIRLWIND;
                Level->site[i][j].p_locf = L_AIR_STATION;

                break;
            case '#':
                Level->site[i][j].locchar = WALL;

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            }
        }

        fscanf(fd, "\n");
    }
}

#ifdef MSDOS
/* This stuff is in this file because the file was really small. */

void msdos_init()
{
    int i;

    /* Allocate the inner level of pointers for TheLevel */
    for(i = 0; i < MAXWIDTH; ++i) {
        TheLevel.site[i] = (plc)malloc(MAXLENGTH * sizeof(loctype));
    }

    /* Remove old level files */
    kill_all_levels();
}

void kill_all_levels()
{
    kill_levels("om*.lev");
}

void kill_levels(char *str)
{
    /* int i;
     * struct find_t buf;
     
     * /\* Remove old level files laying around *\/
     * sprintf(Str1, "%s%s", OMEGALIB, str);
     
     * for(i = _dos_findfirst(Str1, _A_NORMAL, &buf); i != 0; i = _dos_findnext(&buf)) {
     *  sprintf(Str2, "%s%s", OMEGALIB, buf.name);
     *  remove(Str2);
     * }
     */
}

#define MEM_CHECK_AMOUNT 0xf00

void check_memory()
{
    char *mems[50];
    long amount = 0;
    int num_mems = 0;
    unsigned try;

    /* sprintf(Str1, "_heapchk returned %d.", _heapchk()) */
    mprint(Str1);

    try = MEM_CHECK_AMOUNT;

    while(try > 1000) {
        while(try > 0) {
            mems[num_mems] = malloc(try);

            if(mems[num_mems] != NULL) {
                break;
            }

            try -= 0x400;
        }

        amount += try;
        ++num_mems;
    }

    --num_mems;

    while(num_mems >= 0) {
        if(mems[num_mems] != NULL) {
            free(mems[num_mems]);
        }
        
        --num_mems;
    }

    sprintf(Str1, "Free mem approx %dK", (int)(amount / 0x400));
    mprint(Str1);
}

#ifndef MSDOS
void sleep(int n)
{
    struct timeb otime;
    struct timeb ntime;

    ftime(&otime);
    ftime(&ntime);

    while((((short)(ntime.tim - otime.time)) * 1000 + (short)ntime.millitm - (short)otime.millitm) < (n * 1000)) {
        ftime(&ntime);
    }
}
#endif

static FILE *open_levfile(int env, int depth, int rw)
{
    sprintf(Str1, "%s0m%03d%03d.lev", OMEGALIB, env, depth);

    if(rw) {
        return fopen(Str1, "wb");
    }
    else {
        return fopen(Str1, "rb");
    }
}

static void free_objlist(pol pobjlist)
{
    pol tmp;

    while(pobjlist) {
        tmp = pobjlist;
        free(tmp->thing);
        poblist = pobjlist->next;
        free(tmp);
    }
}

static void free_mons_and_objs(pml mlist)
{
    pml tmp;

    while(mlist) {
        tmp = mlist;
        free_objlist(tmp->m->possessions);
        free(tmp->m);
        mlist = mlist->next;
        free(tmp);
    }
}

/* Free up monsters and items */
void free_levelstuff(plv oldlevel)
{
    int i;
    int j;

    free_monst_and_objs(oldlevel->mlist);

    for(i = 0; i < MAXWIDTH; ++i) {
        for(j = 0; j < MAXLENGTH; ++j) {
            free_objlist(oldlevel->site[i][j].things);
        }
    }
}

/* 
 * Saves oldlevel (unless NULL), and reads in the new level, unless
 * depth < 0.
 */
plv msdos_changelevel(plv oldlevel, int newenv, int newdepth)
{
    FILE *fp;

    if(oldlevel != NULL) {
        if((oldlevel->environment == newenv) && (oldlevel->depth == newdepth)) {
            return oldlevel;
        }

        fp = open_levfile(oldlevel->environment, oldlevel->depth, 1);

        if(fp != NULL) {
            save_level(fp, oldlevel);
            fclose(fp);
        }
        else {
            mprint("Cannot save level!!!");
        }

        /* Free up monsters and items */
        free_levelstuff(oldlevel);
    }

    if(newdepth >= 0) {
        fp = open_levfile(newenv, newdepth, 0);

        if(fp == NULL) {
            return NULL;
        }

        restore_level(fp);
        fclose(fp);

        return level;
    }

    return NULL;
}

#endif
