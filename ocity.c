/*
 * Omega copyright (C) by Laurence Raphael Brothers, 1987-1989.
 *
 * ocity.c
 *
 * Some functions to make the city level
 */
#include "ocity.h"

#include "oglob.h"

/* Loads the city level */
void load_city()
{
    int i;
    int j;
    pml ml;
    char site;

    FILE *fd;

    for(i = 0; i < NUMCITYSITES; ++i) {
        CitySiteList[i][0] = FALSE;
    }

    strcpy(Str3, OMEGALIB);
    strcat(Str3, "ocity.dat");
    fd = fopen(Str3, "r");

    TempLevel = Level;

    if(ok_to_free(TempLevel)) {
        free((char *)TempLevel);
        TempLevel = NULL;
    }

    Level = (plv)malloc(sizeof(levtype));
    clear_level(Level);
    Level->depth = -;
    Level->environment = E_CITY;

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            lset(i, j, SEEN);
            site = getc(fd);

            switch(site) {
            case 'g':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_GARDEN;

                break;
            case 'z':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_MAZE;

                break;
            case 'y':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_CEMETARY;

                break;
            case 'x':
                assign_city_function(i, j);

                break;
            case 't':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TEMPLE;
                CitySiteList[L_TEMPLE - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_TEMPLE - CITYSITEBASE][1] = i;
                CitySiteList[L_TEMPLE - CITYSITEBASE][2] = j;

                break;
            case 'T':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_PORTCULLIS_TRAP;
                Level->site[i][j].aux = NOCITYMOVE;

                break;
            case 'R':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_RAISE_PORTCULLIS;
                Level->site[i][j].aux = NOCITYMOVE;

                break;
            case '7':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_PORTCULLIS;
                Level->site[i][j].aux = NOCITYMOVE;

                break;
            case 'C':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_COLLEGE;
                CitySiteList[L_COLLEGE - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_COLLEGE - CITYSITEBASE][1] = i;
                CitySiteList[L_COLLEGE - CITYSITEBASE][2] = j;

                break;
            case 's':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_SORCERORS;
                CitySiteList[L_SORCERORS - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_SORCERORS - CITYSITEBASE][1] = i;
                CitySiteList[L_SORCERORS - CITYSITEBASE][2] = j;

                break;
            case 'M':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_MERC_GUILD;
                CitySiteList[L_MERC_GUILD - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_MERC_GUILD - CITYSITEBASE][1] = i;
                CitySiteList[L_MERC_GUILD - CITYSITEBASE][2] = j;

                break;
            case 'c':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_CASTLE;
                CitySiteList[L_CASTLE - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_CASTLE - CITYSITEBASE][1] = i;
                CitySiteList[L_CASTLE - CITYSITEBASE][2] = j;

                break;
            case '?':
                mazesite(i, j);

                break;
            case 'P':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_ORDER;
                CitySiteList[L_ORDER - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_ORDER - CITYSITEBASE][1] = i;
                CitySiteList[L_ORDER - CITYSITEBASE][2] = j;

                break;
            case 'H':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_CHARITY;
                CitySiteList[L_CHARITY - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_CHARITY - CITYSITEBASE][1] = i;
                CitySiteList[L_CHARITY - CITYSITEBASE][2] = j;

                break;
            case 'j':
                Level->site[i][j].locchar = FLOOR;
                make_justiciar(i, j);

                break;
            case 'J':
                Level->site[i][j].locchar = CLOSED_DOOR;
                Level->site[i][j].p_locf = L_JAIL;

                break;
            case 'A':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_ARENA;
                CitySiteList[L_ARENA - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_ARENA - CITYSITEBASE][1] = i;
                CitySiteList[L_ARENA - CITYSITEBASE][2] = j;

                break;
            case 'B':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_BANK;
                CitySiteList[L_BANK - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_BANK - CITYSITEBASE][1] = i;
                CitySiteList[L_BANK - CITYSITEBASE][2] = j;
                lset(i, j + 1, STOPS);
                lset(i + 1, j, STOPS);
                lset(i - 1, j, STOPS);
                lset(i, j - 1, STOPS);

                break;
            case 'X':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_COUNTRYSITE;
                CitySiteList[L_COUNTRYSIDE - CITYSITEBASE][0] = TRUE;
                CitySiteList[L_COUNTRYSIDE - CITYSITEBASE][1] = i;
                CitySiteList[L_COUNTRYSIDE - CITYSITEBASE][2] = j;

                break;
            case 'V':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_VAULT;
                Level->site[i][j].aux = NOCITYMOVE;
                lset(i, j, SECRET);

                break;
            case '\\':
                Level->site[i][j].aux = NOCITYMOVE;
                Level->site[i][j].showchar = WALL;
                Level->site[i][j].locchar = CLOSED_DOOR;
                Level->site[i][j].p_locf = L_VAULT;
                lset(i, j, SECRET);

                break;
            case 'S':
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].aux = NOCITYMOVE;
                lset(i, j, SECRET);

                break;
            case 'G':
                Level->site[i][j].locchar = FLOOR;
                make_site_monster(i, j, ML0 + 3);

                break;
            case 'u':
                Level->site[i][j].locchar = FLOOR;
                make_minor_undead(i, j);

                break;
            case 'U':
                Level->site[i][j].locchar = FLOOR;
                make_major_undead(i, j);

                break;
            case '%':
                Level->site[i][j].showchar = WALL;
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = L_TRAP_SIREN;
                make_site_treasure(i, j, 5);
                Level->site[i][j].aux = NOCITYMOVE;
                lset(i, j, SECRET);

                break;
            case '$':
                Level->site[i][j].locchar = FLOOR;
                make_site_treasure(i, j, 5);

                break;
            case '2':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = ODIN;

                break;
            case '3':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = SET;

                break;
            case '4':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = ATHENA;

                break;
            case '5':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = HECATE;

                break;
            case '6':
                Level->site[i][j].locchar = ALTAR;
                Level->site[i][j].p_locf = L_ALTAR;
                Level->site[i][j].aux = DESTINY;

                break;
            case 'O':
                Level->site[i][j].locchar = OPEN_DOOR;
                Level->site[i][j].p_locf = L_ORACLE;
                CitySiteList[L_ORACLE - CITYSITEBASE][1] = i;
                CitySiteList[L_ORACLE - CITYSITEBASE][2] = j;

                break;
            case '^':
                Level->site[i][j].showchar = WALL;
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].p_locf = TRAP_BASE + random_range(NUMTRAPS);
                lset(i, j, SECRET);

                break;
            case '"':
                Level->site[i][j].locchar = HEDGE;

                break;
            case '~':
                Level->site[i][j].locchar = WATER;
                Level->site[i][j].p_locf = L_WATER;

                break;
            case '=':
                Level->site[i][j].locchar = WATER;
                Level->site[i][j].p_locf = L_MAGIC_POOL;

                break;
            case '>':
                Level->site[i][j].locchar = DOWN;
                Level->site[i][j].p_locf = L_SEWER;

                break;
            case '*':
                Level->site[i][j].locchar = WALL;
                Level->site[i][j].aux = 10;

                break;
            case '#':
                Level->site[i][j].locchar = WALL;
                LEvel->site[i][j].aux = 500;

                break;
            case '.':
                Level->site[i][j].locchar = FLOOR;

                break;
            case ',':
                Level->site[i][j].showchar = WALL;
                Level->site[i][j].locchar = FLOOR;
                Level->site[i][j].aux = NOCITYMOVE;
                lset(i, j, SECRET);

                break;
            default:
                Level->site[i][j].locchar = site;

                break;
            }

            if(loc_statusp(i, j, SEEN)) {
                if(loc_statusp(i, j, SECRET)) {
                    Level->site[i][j].showchar = '#';
                }
                else {
                    Level->site[i][j].showchar = Level->site[i][j].locchar;
                }
            }
        }
        
        fscanf(fd, "\n");
    }

    City = Level;

    /*
     * Make all city monsters asleep, and shorten their wakeup range
     * to 2 to prevent players from being molested by vicious monsters
     * on the streets
     */
    for(ml = Level->mlist; ml != NULL; ml = ml->next) {
        m_status_reset(ml->m, AWAKE);
        ml->m->wakeup = 2;
    }

    fclose(fd);
}

void assigne_city_functions(int x, int y)
{
    static int setup = 0;
    static int next = 0;

    /* Number of x's in city map */
    static int permutation[64];

    int i;
    int j;
    int k;
    int l;

    Level->site[x][y].aux = TRUE;

    lset(x, y + 1, STOPS);
    lset(x + 1, y, STOPS);
    lset(x - 1, y, STOPS);
    lset(x, y - 1, STOPS);

    if(setup == 0) {
        setup = 1;

        for(i = 0; i < 64; ++i) {
            permutation[i] = i;
        }

        for(i = 0; i < 500; ++i) {
            j = random_range(64);
            k = random_range(64);
            l = permutation[j];
            permutation[j] = permutation[k];
            permutation[k] = l;
        }
    }

    /* In case someone changes the number of x's */
    if(next > 63) {
        Level->site[x][y].locchar = CLOSED_DOOR;
        Level->site[x][y].p_locf = L_HOUSE;

        if(random_range(5)) {
            Level->site[x][y].aux = LOCKED;
        }
    }
    else {
        switch(permutation[next]) {
        case 0:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_ARMORER;
            CitySiteList[L_ARMORER - CITYSITEBASE][1] = x;
            CitySiteList[L_ARMORER - CITYSITEBASE][2] = y;

            break;
        case 1:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_CLUB;
            CitySiteList[L_CLUB - CITYSITEBASE][1] = x;
            CitySiteList[L_CLUB - CITYSITEBASE][2] = y;

            break;
        case 2:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_GYM;
            CitySiteList[L_GYM - CITYSITEBASE][1] = x;
            CitySiteList[L_GYM - CITYSITEBASE][2] = y;

            break;
        case 3:
            Level->site[x][y].locchar = CLOSED_DOOR;
            Level->site[x][y].p_locf = L_THIEVES_GUILD;
            CitySiteList[L_THIEVES_GUILD - CITYSITEBASE][1] = x;
            CitySiteList[L_THIEVES_GUILD - CITYSITEBASE][2] = y;

            break;
        case 4:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_HEALER;
            CitySiteList[L_HEALER - CITYSITEBASE][1] = x;
            CitySiteList[L_HEALER - CITYSITEBASE][2] = y;

            break;
        case 5:
            Level->site[x][y] = OPEN_DOOR;
            Level->site[x][y].p_locf = L_CASINO;
            CitySiteList[L_CASINO - CITYSITEBASE][1] = x;
            CitySiteList[L_CASINO - CITYSITEBASE][2] = y;

            break;
        case 7:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_DINER;
            CitySiteBase[L_DINER - CITYSITEBASE][1] = x;
            CitySiteBase[L_DINER - CITYSITEBASE][2] = y;

            break;
        case 8:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_CRAP;
            CitySiteList[L_CRAP - CITYSITEBASE][1] = x;
            CitySiteList[L_CRAP - CITYSITEBASE][2] = y;

            break;
        case 6:
        case 9:
        case 20:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_COMMANDANT;
            CitySiteList[L_COMMANDANT - CITYSITEBASE][1] = x;
            CitySiteList[L_COMMANDANT - CITYSITEBASE][2] = y;

            break;
        case 21:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_TAVERN;
            CitySiteList[L_TAVERN - CITYSITEBASE][1] = x;
            CitySiteList[L_TAVERN - CITYSITEBASE][2] = y;

            break;
        case 10:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_ALCHEMIST;
            CitySiteList[L_ALCHEMIST - CITYSITEBASE][1] = x;
            CitySiteList[L_ALCHEMIST - CITYSITEBASE][2] = y;

            break;
        case 11:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_DPW;
            CitySiteList[L_DPW - CITYSITEBASE][1] = x;
            CitySiteList[L_DPW - CITYSITEBASE][2] = y;

            break;
        case 12:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_LIBRARY;
            CitySiteList[L_LIBRARY - CITYSITEBASE][1] = x;
            CitySiteList[L_LIBRARY - CITYSITEBASE][2] = y;

            break;
        case 13:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_PAWN_SHOP;
            CitySiteList[L_PAWN_SHOP - CITYSITEBASE][1] = x;
            CitySiteList[L_PAWN_SHOP - CITYSITEBASE][2] = y;

            break;
        case 14:
            Level->site[x][y].locchar = OPEN_DOOR;
            Level->site[x][y].p_locf = L_CONDO;
            CitySiteList[L_CONDO - CITYSITEBASE][1] = x;
            CitySiteList[L_CONDO - CITYSITEBASE][2] = y;

            break;
        case 15:
            Level->site[x][y].locchar = CLOSED_DOOR;
            Level->site[x][y].p_locf = L_BROTHEL;

            break;
        default:
            Level->site[x][y].locchar = CLOSED_DOOR;

            switch(random_range(6)) {
            case 0:
                Level->site[x][y].p_locf = L_HOVEL;

                break;
            case 1:
            case 2:
            case 3:
            case 4:
                Level->site[x][y].p_locf = L_HOUSE;

                break;
            case 5:
                Level->site[x][y].p_locf = L_MANSION;

                break;
            }

            if(random_range(5)) {
                Level->site[x][y].aux = LOCKED;
            }

            break;
        }
    }

    ++next;
}

/* Makes a hiscore npc for mansions */
void make_justiciar(int i, int j)
{
    pml ml = (pml)malloc(sizeof(mltype));
    ml->m = (pmt)malloc(sizeof(montype));
    *(ml->m) = Monsters[NPC];
    make_hiscore_npc(ml->m, 15);
    ml->m->x = i;
    ml->m->y = j;
    Level->site[i][j].creature = ml->m;
    ml->m->click = (Tick + 1) % 60;
    ml->next = Level->mlist;
    Level->mlist = ml;
    m_status_reset(ml->m, AWAKE);
}

/* Loads the city level */
void resurrect_guards()
{
    int i;
    int j;
    pml ml;
    char site;
    FILE *fd;

    strcpy(Str3, OMEGALIB);
    strcat(Str3, "ocity.dat");
    fd = fopen(Str3, "r");

    for(j = 0; j < LENGTH; ++j) {
        for(i = 0; i < WIDTH; ++i) {
            site = getc(fd);

            if(site == 'G') {
                make_site_monster(i, j, ML0 + 3);
                Level->site[i][j].creature->monstring = salloc("undead guardsman");
                Level->site[i][j].creature->meleef = M_MELEE_SPIRIT;
                Level->site[i][j].creature->movef = M_MOVE_SPIRIT;
                Level->site[i][j].creature->strikef = M_STRIKE_MISSILE;
                Level->site[i][j].creature->immunity = EVERYTHING - pow2(NORMAL_DAMAGE);
                Level->site[i][j].creature->hp *= 2;
                Level->site[i][j].creature->hit *= 2;
                Level->site[i][j].creature->dmg *= 2;
                Level->site[i][j].creature->ac *= 2;
                m_status_set(Level->site[i][j].creature, HOSTILE);
                m_status_set(Level->site[i][j].creature, AWAKE);
            }
        }

        fscanf(fd, "\n");
    }

    City = Level;

    fclose(fd);
}

void mazesite(int i, int j)
{
    static FILE *fd = NULL;
    static int k = 0;
    char site;

    if(fd == NULL) {
        strcpy(Str2, OMEGALIB);
        strcpy(Str4, "omaze .dat");
        Str4[5] = '1' + random_range(4);
        strcat(Str2, Str4);
        fd = fopen(Str2, "r");
    }

    site = getc(fd);
    ++k;

    if(k == 282) {
        fclose(fd);
    }

    switch(site) {
    case '"':
        Level->site[i][j].locchar = HEDGE;

        if(random_range(10)) {
            Level->site[i][j].p_locf = L_HEDGE;
        }
        else {
            Level->site[i][j].p_locf = L_TRIFID;
        }

        break;
    case '-':
        Level->site[i][j].locchar = CLOSED_DOOR;
    case '.':
        Level->site[i][j].locchar = FLOOR;

        break;
    case '>':
        Level->site[i][j].locchar = DOWN;
        Level->site[i][j].p_locf = L_SEWER;

        break;
    case 'z':
        Level->site[i][j].locchar = FLOOR;
        Level->site[i][j].p_locf = L_MAZE;

        break;
    case 'O':
        Level->site[i][j].locchar = OPEN_DOOR;
        Level->site[i][j].p_locf = L_ORACLE;
        CitySiteList[L_ORACLE - CITYSITEBASE][1] = i;
        CitySiteList[L_ORACLE - CITYSITEBASE][2] = j;

        break;
    case '?':
        randommazesite(i, j);

        break;
    }

    lreset(i, j, SEEN);
}

void randommazesite(int i, int j)
{
    switch(random_range(7)) {
    case 0:
    case 1:
        Level->site[i][j].locchar = FLOOR;
        Level->site[i][j].p_locf = TRAP_BASE + random_range(NUMTRAPS);

        break;
    case 2:
    case 3:
        Level->site[i][j].locchar = FLOOR;
        make_site_monster(i, j, -1);

        break;
    case 4:
    case 5:
        Level->site[i][j].locchar = FLOOR;
        mate_site_treasure(y, j, 5);

        break;
    default:
        Level->site[i][j].locchar = FLOOR;
    }
}

/* Undead are not hostile unless disturbed... */
void make_minor_undead(int i, int j)
{
    int mid;

    if(random_range(2)) {
        /* Ghost */
        mid = ML2 + 6;
    }
    else {
        /* Haunt */
        mid = ML4 + 5;
    }

    make_site_monster(i, j, mid);
    m_status_reset(Level->site[i][j].creature, AWAKE);
    m_status_reset(Level->site[i][j].creature, HOSTILE);
}

/* Undead are not hostile unless disturbed... */
void make_major_undead(int i, int j)
{
    int mid;

    if(random_range(2)) {
        /* Lich */
        mid = ML6 + 5;
    }
    else {
        /* Vampire lord */
        mid = ML9 + 5;
    }

    make_site_monster(i, j, mid);
    m_status_reset(Level->site[i][j].creature, AWAKE);
    m_status_reset(Level->site[i][j].creature, HOSTILE);
}

static char jail[5][11] = {
    '#', '#', '*', '#', '#', '*', '#', '#', '*', '#', '#',
    '#', '#', '*', '#', '#', '*', '#', '*', '#', '#', '#',
    '#', '#', 'T', '#', 'T', '#', 'T', '#', 'T', '#', '#',
    '#', '#', '7', '#', '7', '#', '7', '#', '7', '#', '#',
    '#', '#', 'R', '#', 'R', '#', 'R', '#', 'R', '#', '#'
};

/* Fixes up the jail in case it has been munged by player action */
void repair_jail()
{
    int i;
    int j;

    for(i = 0; i < 11; ++i) {
        for(j = 0; j < 5; ++j) {
            switch(jail[j][i]) {
            case '#':
                City->site[i + 35][j + 52].locchar = WALL;
                City->site[i + 35][j + 52].p_locf = L_NO_OP;
                City->site[i + 35][j + 52].aux = 666;

                break;
            case '*':
                City->site[i + 35][j + 52].locchar = WALL;
                City->site[i + 35][j + 52].p_locf = L_NO_OP;
                City->site[i + 35][j + 52].aux = 10;

                break;
            case 'T':
                City->site[i + 35][j + 52].locchar = FLOOR;
                City->site[i + 35][j + 52].p_locf = L_PORTULLIS_TRAP;
                City->site[i + 35][j + 52].aux = NOCITYMOVE;

                break;
            case '7':
                City->site[i + 35][j + 52].locchar = FLOOR;
                City->site[i + 35][j + 52].p_locf = L_PORTCULLIS;
                City->site[i + 35][j + 52].aux = NOCITYMOVE;

                break;
            case 'R':
                City->site[i + 35][j + 52].locchar = FLOOR;
                City->site[i + 35][j + 52].p_locf = L_RAISE_PORTCULLIS;
                City->site[i + 35][j + 52].aux = NOCITYMOVE;

                break;
            }
        }
    }
}
