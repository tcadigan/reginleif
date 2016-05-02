/*
 * Omega copyright (C) by Laurence Raphael Brothers, 1987-1989
 *
 * ocountry.c
 *
 * load_country(), and all load_ functions for places which are
 * accessible from the country and don't have their own files.
 */

#include "oglob.h"

/* Loads the countryside level from the data file */
void load_country()
{
    int i;
    int j;
    char site;

    FILE *fd;

    strcpy(Str3, OMEGALIB);
    strcat(Str3, "ocountry.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            site = getc(fd);
            Country[i][j].base_terrain_type = site;
            Country[i][j].aux = 0;

            if((site == PASS)
               || (site == CASTLE)
               || (site == STARPEAK)
               || (site == CAVES)
               || (site == VOLCANO)) {
                Country[i][j].current_terrain_type = MOUNTAINS;
            }
            else if(site == DRAGONLAIR) {
                Country[i][j].current_terrain_type = DESERT;
            }
            else if(site == MAGIC_ISLE) {
                Country[i][j].current_terrain_type = CHAOS_SEA;
            }
            else if((site >= 'a') && (site <= 'f')) {
                Country[i][j].base_terrain_type = 'o';
                Country[i][j].current_terrain_type = Country[i][j].base_terrain_type;
                Country[i][j].aux = 1 + site - 'a';
            }
            else if((size >= '1') && (site <= '6')) {
                Country[i][j].base_terrain_type = TEMPLE;
                Country[i][j].current_terrain_type = Country[i][j].base_terrain_type;
                Country[i][j].aux = size - '0';
            }
            else {
                Country[i][j].current_terrain_type = site;
            }

            Country[i][j].explored = FALSE;
        }

        fscanf(fd, "\n");
    }

    fclose(fd);
}

/* Loads the dragon's lair into level */
void load_dlair(int empty)
{
    int i;
    int j;
    char site;
    FILE *fd;

    if(empty) {
        mprint("The Lair is now devoid of inhabitants and treasure.");
    }

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
        free((char *)TempLevel);
        TempLevel = NULL;
    }

    Level = (plv)malloc(sizeof(levtype));
    clear_level(level);
    Level->environment = E_DLAIR;
    strcpy(Str3, OMEGALIB);
    strcat(Str3, "odlair.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            Level->site[i][j].lstatus = 0;
            Level->site[i][j].roomnumber = RS_CAVERN;
            Level->site[i][j].p_locf = L_NO_OP;
            site = getc(fd);

            switch(site) {
            case 'D':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Dragon lord */
                    make_site_monster(i, j, ML10 + 3);
                }

                Level->site[i][j].creature->specialf = M_SP_LAIR;

                break;
            case 'd':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Elite dragon */
                    make_site_monster(i, j, ML8 + 3);
                }

                Level->site[i][j].creature->specialf = M_SP_LAIR;
                Level->site[i][j].creature->hit *= 2;
                Level->site[i][j].creature->dmg *= 2;

                break;
            case 'W':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    make_site_monster(i, j, ML9 + 2);
                }

                break;
            case 'M':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    make_site_monster(i, j, -1);
                }

                break;
            case 'S':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].shwochar = WALL;

                if(!empty) {
                    lset(i, j, SECRET);
                }

                Level->site[i][j].roomnumber = RS_SECRETPASSAGE;

                break;
            case '$':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].roomnumber = RS_DRAGONLORD;

                if(!empty) {
                    make_site_treasure(i, j, 10);
                }

                break;
            case 's':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TRAP_SIREN;

                break;
            case '7':
                if(!empty) {
                    Level->site[i][j].locchar = PORTCULLIS;
                }
                else {
                    Level->site[i][j].locchar = FLOOR;
                }

                Level->site[i][j].p_locf = L_PORTCULLIS;

                break;
            case 'R':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_RAISE_PORTCULLIS;

                break;
            case 'p':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_PORTCULLIS;

                break;
            case 'T':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    Level->site[i][j].p_locf = L_PORTCULLIS_TRAP;
                }

                break;
            case 'X':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TACTICAL_EXIT;

                break;
            case '#':
                Level->site[i][j].locchar = WALL;
                Level->site[i][j].aux = 150;

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            }
        }

        fscanf(fd, "\n");
    }

    fclose(fd);
}

/* Loads the star peak into level */
void load_speak(int empty)
{
    int i;
    int j;
    int safe = (Player.alignment > 0);
    char site;
    FILE *fd;

    if(empty) {
        mprint("The peak is now devoid of inhabitants and treasure.");
    }

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
        free((char *)TempLevel);
        TempLevel = NULL;
    }

    Level = (plv)malloc(sizeof(levtype));
    clear_level(Level);
    Level->environment = E_STARPEAK;
    strcpy(Str3, OMEGALIB);
    strcat(Str3, "ospeak.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            Level->site[i][j].lstatus = 0;
            Level->site[i][j].roomnumber = RS_STARPEAK;
            Level->site[i][j].p_locf = L_NO_OP;
            site = getc(fd);

            switch(site) {
            case 'S':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].showchar = WALL;
                lset(i, j, SECRET);
                Level->site[i][j].roomnumber = RS_SECRETPASSAGE;

                break;
            case 'L':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Lawbringer */
                    make_site_monster(i, j, ML10 + 2);
                }

                if(safe) {
                    m_status_reset(Level->site[i][j].creature, HOSTILE);
                }

                break;
            case 's':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Servent of law */
                    make_site_monster(i, j, ML4 + 12);
                }

                if(safe) {
                    m_status_reset(Level->site[i][j].creature, HOSTILE);
                }

                break;
            case 'M':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    make_site_monster(i, j, -1);
                }

                if(safe) {
                    m_status_reset(Level->site[i][j].creature, HOSTILE);
                }

                break;
            case '$':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    make_site_treasure(i, j, 10);
                }

                break;
            case '7':
                if(!empty) {
                    Level->site[i][j].locchar = PORTCULLIS;
                }
                else {
                    Level->site[i][j].locchar = FLOOR;
                }

                Level->site[i][j].p_locf = L_PORTCULLIS;

                break;
            case 'R':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_RAISE_PORTCULLIS;

                break;
            case '-':
                Level->site[i][j].locchar = CLOSED_DOOR;

                break;
            case '|':
                Level->site[i][j].locchar = OPEN_DOOR;

                break;
            case 'p':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_PORTCULLIS;

                break;
            case 'T':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    Level->site[i][j].p_locf = L_PORTCULLIS_TRAP;
                }

                break;
            case 'X':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TACTICAL_EXIT;

                break;
            case '#':
                Level->site[i][j].locchar = WALL;
                Level->site[i][j].aux = 150;

                break;
            case '4':
                Level->site[i][j].locchar = RUBBLE;
                Level->site[i][j].p_locf = L_RUBBLE;

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            }
        }

        fscanf(fd, "\n");
    }

    fclose(fd);
}

/* Loads the magic isle into level */
void load_isle(int empty)
{
    int i;
    int j;
    char site;
    FILE *fd;

    if(empty) {
        mprint("The isle is now devoid of inhabitants and treasure.");
    }

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
        free((char *)TempLevel);
        TempLevel = NULL;
    }

    Level = (plv)malloc(sizeof(levtype));
    clear_level(Level);
    Level->environment = E_MAGIC_ISLE;
    strcpy(Str3, OMEGALIB);
    strcat(Str3, "omisle.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            Level->site[i][j].lstatus = 0;
            Level->site[i][j].roomnumber = RS_MAGIC_ISLE;
            Level->site[i][j].p_locf = L_NO_OP;
            site = getc(fd);

            switch(site) {
            case 'E':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Eater of magic */
                    make_site_monster(i, j, ML10 + 1);
                }

                break;
            case 'm':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Militan priest */
                    make_site_monster(i, j, ML8 + 9);
                }

                break;
            case 'n':
                Level->site[i][j].locchar = FLOOR;

                if(!empty) {
                    /* Nazgul */
                    make_site_monster(i, j, ML7 + 1);
                }

                break;
            case 'X':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TACTICAL_EXIT;

                break;
            case '#':
                Level->site[i][j].locchar = WALL;
                Level->site[i][j].aux = 150;

                break;
            case '4':
                Level->site[i][j].locchar = RUBBLE;
                Level->site[i][j].p_locf = L_RUBBLE;

                break;
            case '~':
                Level->site[i][j].locchar = WATER;
                Level->site[i][j].p_locf = L_CHAOS;

                break;
            case '=':
                Level->site[i][j].locchar = WATER;
                Level->site[i][j].p_locf = L_MAGIC_POOL;

                break;
            case '-':
                Level->site[i][j].locchar = CLOSED_DOOR;

                break;
            case '|':
                Level->site[i][j].locchar = OPEN_DOOR;

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            }
        }

        fscanf(fd, "\n");
    }

    fclose(fd);
}

/* Loads a temple into level */
void load_temple(int deity)
{
    int i;
    int j;
    char size;
    pml ml;
    FILE *fd;

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
        free((char *)TempLevel);
        TempLevel = NULL;
    }

    Level = (plv)malloc(sizeof(levtype));
    clear_level(Level);
    Level->environment = E_TEMPLE;
    strcpy(Str2, OMEGALIB);
    strcat(Str3, "otemple.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            switch(deity) {
            case ODIN:
                Level->site[i][j].roomnumber = RS_ODIN;

                break;
            case SET:
                Level->site[i][j].roomnumber = RS_SET;

                break;
            case HECATE:
                Level->site[i][j].roomnumber = RS_HECATE;

                break;
            case ATHENA:
                Level->site[i][j].roomnumber = RS_ATHENA;

                break;
            case DRUID:
                Level->site[i][j].roomnumber = RS_DRUID;

                break;
            case DESTINY:
                Level->site[i][j].roomnumber = RS_DESTINY;

                break;
            }

            site = getc(fd);

            switch(site) {
            case '8':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = deity;

                break;
            case 'H':
                Level->site[i][j].locchar = FLOOR;

                if(strcmp(Player.name, Priest[Player.patron]) != 0) {
                    make_high_priest(i, j, deity);
                }

                break;
            case 'S':
                Level->site[i][j].locchar = FLOOR;

                if(strcmp(Player.name, Priest[Player.patron]) != 0) {
                    lset(i, j, SECRET);
                }

                break;
            case 'W':
                Level->site[i][j].locchar = FLOOR;

                if((deity != Player.patron)
                   && ((deity == ODIN)
                       || (deity == SET)
                       || (deity == HECATE)
                       || (deity == ATHENA)
                       || (deity == DESTINY))) {
                    Level->site[i][j].p_locf = L_TEMPLE_WARNING;
                }

                break;
            case 'm':
                Level->site[i][j].locchar = FLOOR;

                /* Militant priest */
                make_site_monster(i, j, ML8 + 9);

                break;
            case 'd':
                Level->site[i][j].locchar = FLOOR;

                /* Doberman death hound */
                make_site_monster(i, j, ML4 + 10);

                break;
            case 'X':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_lcof = L_TACTICAL_EXIT;

                break;
            case '#':
                if(deity != DRUID) {
                    Level->site[i][j].locchar = WALL;
                    Level->site[i][j].aux = 150;
                }
                else {
                    Level->site[i][j].locchar = HEDGE;
                    Level->site[i][j].p_locf = L_HEDGE;
                }

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            case 'x':
                Level->site[i][j].locchar = FLOOR;
                random_temple_site(i, j, deity);

                break;
            case '?':
                if(deity != DESTINY) {
                    Level->site[i][j].locchar = FLOOR;
                }
                else {
                    Level->site[i][j].locchar = ABYSS;
                    Level->site[i][j].p_locf = L_ADEPT;
                }

                break;
            case '-':
                Level->site[i][j].locchar = CLOSED_DOOR;

                break;
            case '|':
                Level->site[i][j].locchar = OPEN_DOOR;

                break;
            }
        }

        fscanf(fd, "\n");
    }

    /* Main temple is peaceful for player of same sect, druids always peaceful */
    if((Player.patron == deity) || (deity == DRUID)) {
        for(ml = Level->mlist; ml != NULL; ml = ml->next) {
            m_status_reset(ml->m, HOSTILE);
        }
    }

    fclose(fd);
}

void random_temple_site(int i, int j, int deity)
{
    switch(random_range(12)) {
    case 0:
        /* Mendicant priest */
        make_site_monster(i, j, ML0 + 1);

        break;
    case 1:
        Level->site[i][j].locchar = WATER;
        Level->site[i][j].p_locf = L_MAGIC_POOL;
    case 2:
        /* Inner circle demon */
        make_site_monster(i, j, ML7 + 14);

        break;
    case 3:
        /* Angel of appropriate sect */
        make_site_monster(i, j, ML6 + 11);
        Level->site[i][j].creature->aux1 = deity;

        break;
    case 4:
        /* Archangel of appropriate sect */
        make_site_monster(i, j, ML8 + 11);
        Level->site[i][j].creature->aux1 = deity;

        break;
    case 5:
        /* Archangel of appropriate sect */
        make_site_monster(i, j, ML9 + 6);
        Level->site[i][j].creature->aux1 = deity;

        break;
    }
}

void make_high_priest(int i, int j, int deity)
{
    pml ml = (pml)malloc(sizeof(mltype));
    pmt m = (pmt)malloc(sizeof(montype));
    make_hiscore_npc(m, deity);
    m->x = i;
    m->y = j;
    Level->site[i][j].creature = m;
    ml->m = m;
    ml->next = Level->mlist;
    Level->mlist = ml;
}
