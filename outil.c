/*
 * copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * outils.c
 *
 * Random utility functions called from all over
 */

#include "outil.h"

#include <stdlib.h>
#include <string.h>

#include "ogen1.h"
#include "oglob.h"
#include "omon.h"
#include "oscr.h"

/* x and y on level? */
int inbounds(int x, int y)
{
    if((x >= 0) && (y >= 0) && (x < WIDTH) && (y < LENGTH)) {
        return 1;
    }
    else {
        return 0;
    }
}

int random_range(int k)
{
    if(k == 0) {
        return 0;
    }
    else {
        return (rand() % k);
    }
}

/* Modify absolute y coordinate reative to which part of level we are on */
int screenmod(int y)
{
    return (y - ScreenOffset);
}

int offscreen(int y)
{
    if((y < 0)
       || (y  < ScreenOffset)
       || (y > (ScreenOffset + ScreenLength - 1))
       || (y > LENGTH)) {
        return 1;
    }
    else {
        return 0;
    }
}

/* Always hit on a natural 0; never hit on a natural 19 */
int hitp(int hit, int ac)
{
    int roll = random_range(20);

    if(roll == 0) {
        return TRUE;
    }
    else if(roll == 19) {
        return FALSE;
    }
    else {
        if(roll < (hit - ac)) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
}

/* This function doesn't seem to exist by itself... */
int sign(int n)
{
    if(n == 0) {
        return 0;
    }
    else {
        if(n < 0) {
            return -1;
        }
        else {
            return 1;
        }
    }
}

/* Why load the math library... */
int max(int a, int b)
{
    if(a > b) {
        return a;
    }
    else {
        return b;
    }
}

/* Why load the math library... */
int min(int a, int b)
{
    if(a < b) {
        return a;
    }
    else {
        return b;
    }
}

/* Number of moves from x1,y1 to x2,y2 */
int distance(int x1, int y1, int x2, int y2)
{
    return max(abs(x2 - x1), abs(y2 - y1));
}

/* Can you shoot or move monsters through a spot? */
int unblocked(int x, int y)
{
    if(!inbounds(x, y)
       || (Level->site[x][y].creature != NULL)
       || (Level->site[x][y].locchar == WALL)
       || (Level->site[x][y].locchar == PORTCULLIS)
       || (Level->site[x][y].locchar == STATUE)
       || (Level->site[x][y].locchar == HEDGE)
       || (Level->site[x][y].locchar == CLOSED_DOOR)
       || loc_statusp(x, y, SECRET)
       || ((x == Player.x) && (y == Player.y))) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

/* Do monsters want to move through a spot */
int m_unblocked(struct monster *m, int x, int y)
{
    if(!inbounds(x, y) || ((x == Player.x) && (y == Player.y))) {
        return FALSE;
    }
    else if((Level->site[x][y].creature != NULL)
            || (Level->site[x][y].locchar == SPACE)) {
        return FALSE;
    }
    else if(m_statusp(m, ONLYSWIM)) {
        if(Level->site[x][y].locchar == WATER) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else if((Level->site[x][y].locchar == FLOOR)
            || (Level->site[x][y].locchar == OPEN_DOOR)) {
        return TRUE;
    }
    else if((Level->site[x][y].locchar == PORTCULLIS)
            || (Level->site[x][y].locchar == WALL)
            || (Level->site[x][y].locchar == STATUE)
            || loc_statusp(x, y, SECRET)) {
        return m_statusp(m, INTANGIBLE);
    }
    else if(Level->site[x][y].locchar == WATER) {
        if(m_statusp(m, SWIMMING)
           || m_statusp(m, ONLYSWIM)
           || m_statusp(m, FLYING)) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else if(Level->site[x][y].locchar == CLOSED_DOOR) {
        if(m->movef == M_MOVE_SMART) {
            mprint("You hear a door creak open.");
            Level->site[x][y].locchar = OPEN_DOOR;

            return TRUE;
        }
        else if(random_range(m->dmg) > random_range(100)) {
            mprint("You hear a door shattering.");
            Level->site[x][y].locchar = RUBBLE;

            return TRUE;
        }
        else {
            return m_statusp(m, INTANGIBLE);
        }
    }
    else if(Level->site[x][y].locchar == LAVA) {
        if((m_immunityp(m, FLAME) && m_statusp(m, SWIMMING))
           || m_statusp(m, FLYING)) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else if(Level->site[x][y].locchar == FIRE) {
        return m_immunityp(m, FLAME);
    }
    else if((Level->site[x][y].locchar == TRAP)
            || (Level->site[x][y].locchar == HEDGE)
            || (Level->site[x][y].locchar == ABYSS)) {
        if((m->movef == M_MOVE_CONFUSED) || m_statusp(m, FLYING)) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        return TRUE;
    }
}

/* Can yo usee through a spot? */
int view_unblocked(int x, int y)
{
    if(!inbounds(x, y)) {
        return FALSE;
    }
    else if((Level->site[x][y].locchar == WALL)
            || (Level->site[x][y].locchar == STATUE)
            || (Level->site[x][y].locchar == HEDGE)
            || (Level->site[x][y].locchar == FIRE)
            || (Level->site[x][y].locchar == CLOSED_DOOR)
            || loc_statusp(x, y, SECRET)) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

/* 8 moves in Dirs */
void initdirs()
{
    Dirs[0][0] = 1;
    Dirs[0][1] = 1;
    Dirs[0][2] = -1;
    Dirs[0][3] = -1;
    Dirs[0][4] = 1;
    Dirs[0][5] = -1;
    Dirs[0][6] = 0;
    Dirs[0][7] = 0;
    Dirs[0][8] = 0;
    Dirs[1][0] = 1;
    Dirs[1][1] = -1;
    Dirs[1][2] = 1;
    Dirs[1][3] = -1;
    Dirs[1][4] = 0;
    Dirs[1][5] = 0;
    Dirs[1][6] = 1;
    Dirs[1][7] = -1;
    Dirs[1][8] = 0;
}

/*
 * Moves pvx along lineofsight from x1 to x2.
 * x1 and x2 are pointers because as a side effect they are changed to the 
 * final location of the pyx
 */
void do_los(char pyx, int *x1, int *y1, int x2, int y2)
{
    int ox;
    int oy;
    int sx;
    int sy;
    int v;

    ox = *x1;
    oy = *y1;
    sx = sign(x2 - *x1);
    sy = sign(y2 - *y1);

    if(abs(x2 - *x1) > abs(y2 - *y1)) {
        *x1 += sx;
    }
    else if(abs(x2 - *x1) < abs(y2 - *y1)) {
        *y1 += sy;
    }
    else {
        *x1 += sx;
        *y1 += sy;
    }

    Level->site[*x1][*y1].showchar = pyx;

    /* Delay enough to make pyx visible */

    for(v = 1; v < 10; ++v) {
        plotchar(pyx, *x1, *y1);
    }

    plotspot(ox, oy, FALSE);

    while(unblocked(*x1, *y1) && ((*x1 != x2) && (*y1 != y2))) {
        ox = *x1;
        oy = *y1;
        sx = sign(x2 - *x1);
        sy = sign(y2 - *y1);

        if(abs(x2 - *x1) > abs(y2 - *y1)) {
            *x1 += sx;
        }
        else if(abs(x2 - *x1) < abs(y2 - *y1)) {
            *y1 += sy;
        }
        else {
            *x1 += sx;
            *y1 += sy;
        }

        Level->site[*x1][*y1].showchar = pyx;

        /* Delay enough to make pyx visible */
        for(v = 1; v < 10; ++v) {
            plotchar(pyx, *x1, *y1);
        }

        plotspot(ox, oy, FALSE);
    }

    plotspot(*x1, *y1, FALSE);
    levelrefresh();
}

/*
 * This is the same as do_los, except we stop before hitting 
 * nonliving obstructions
 */
void do_object_los(char pyx, int *x1, int *y1, int x2, int y2)
{
    int ox;
    int oy;
    int sx;
    int sy;
    int v;

    ox = *x1;
    oy = *y1;
    sx = sign(x2 - *x1);
    sy = sign(y2 - *y1);

    if(abs(x2 - *x1) > abs(y2 - *y1)) {
        *x1 += sx;
    }
    else if(abs(x2 - *x1) < abs(y2 - *y1)) {
        *y1 += sy;
    }
    else {
        *x1 += sx;
        *y1 += sy;
    }

    if(unblocked(*x1, *y1)) {
        for(v = 1; v < 10; ++v) {
            plotchar(pyx, *x1, *y1);
        }

        Level->site[*x1][*y1].showchar = pyx;
    }

    plotspot(ox, oy, TRUE);

    while(unblocked(*x1, *y1) && ((*x1 != x2) && (*y1 != y2))) {
        ox = *x1;
        oy = *y1;
        sx = sign(x2 - *x1);
        sy = sign(y2 - *y1);

        if(abs(x2 - *x1) > abs(y2 - *y1)) {
            *x1 += sx;
        }
        else if(abs(x2 - *x1) < abs(y2 - *y1)) {
            *y1 += sy;
        }
        else {
            *x1 += sx;
            *y1 += sy;
        }

        if(unblocked(*x1, *y1)) {
            for(v = 1; v < 10; ++v) {
                plotchar(pyx, *x1, *y1);
            }

            Level->site[*x1][*y1].showchar = pyx;
        }

        plotspot(ox, oy, TRUE);
    }

    if(!unblocked(*x1, *y1) && (Level->site[*x1][*y1].creature == NULL)) {
        *x1 = ox;
        *y1 = oy;
    }

    plotspot(*x1, *y1, TRUE);
    levelrefresh();
}

/* los_p check to see whether there is an unblocked los from x1,y1 to x2,y2 */
int los_p(int x1, int y1, int x2, int y2)
{
    int sx;
    int sy;

    sx = sign(x2 - x1);
    sy = sign(y2 - y1);

    if(abs(x2 - x1) > abs(y2 - y1)) {
        x1 += sx;
    }
    else if(abs(x2 - x1) - abs(y2 - y1)) {
        y1 += sy;
    }
    else {
        x1 += sx;
        y1 += sy;
    }

    while(unblocked(x1, y1) && ((x1 != x2) && (y1 != y2))) {
        sx = sign(x2 - x1);
        sy = sign(y2 - y1);

        if(abs(x2 - x1) > abs(y2 - y1)) {
            x1 += sx;
        }
        else if(abs(x2 - x1) < abs(y2 - y1)) {
            y1 += sy;
        }
        else {
            x1 += sx;
            y1 += sy;
        }
    }

    if((x1 == x2) && (y1 == y2)) {
        return 1;
    }
    else {
        return 0;
    }
}

/* view_los_p sees through monsters */
int view_los_p(int x1, int y1, int x2, int y2)
{
    int sx;
    int sy;

    sx = sign(x2 - x1);
    sy = sign(y2 - y1);

    if(abs(x2 - x1) > abs(y2 - y1)) {
        x1 += sx;
    }
    else if(abs(x2 - x1) < abs(y2 - y1)) {
        y1 += sy;
    }
    else {
        x1 += sx;
        y1 += sy;
    }

    while(view_unblocked(x1, y1) && ((x1 != x2) || (y1 != y2))) {
        sx = sign(x2 - x1);
        sy = sign(y2 - y1);

        if(abs(x2 - x1) > abs(y2 - y1)) {
            x1 += sx;
        }
        else if(abs(x2 - x1) < abs(y2 - y1)) {
            y1 += sy;
        }
        else {
            x1 += sx;
            y1 += sy;
        }
    }

    if((x1 == x2) && (y1 == y2)) {
        return 1;
    }
    else {
        return 0;
    }
}

int gamestatusp(long flag)
{
    return (GameStatus & flag);
}

void setgamestatus(long flag)
{
    GameStatus |= flag;
}

void resetgamestatus(long flag)
{
    GameStatus &= ~flag;
}

/* Returns the command direction from the index into Dirs */
char inversedir(int dirindex)
{
    switch(dirindex) {
    case 0:
        
        return 'n';
    case 1:

        return 'u';
    case 2:

        return 'b';
    case 3:

        return 'y';
    case 4:

        return 'l';
    case 5:

        return 'h';
    case 6:

        return 'j';
    default:
        
        return 'k';
    }
}

int calc_points()
{
    int i;
    int points = 0;

    if(gamestatusp(SPOKE_TO_DRUID)) {
        points += 50;
    }

    if(gamestatusp(COMPLETED_CAVES)) {
        points += 100;
    }

    if(gamestatusp(COMPLETED_SEWERS)) {
        points += 200;
    }

    if(gamestatusp(COMPLETED_CASTLE)) {
        points += 300;
    }

    if(gamestatusp(COMPLETED_ASTRAL)) {
        points += 400;
    }

    if(gamestatusp(COMPLETED_VOLCANO)) {
        points += 500;
    }

    if(gamestatusp(KILLED_DRAGONLORD)) {
        points += 100;
    }

    if(gamestatusp(KILLED_EATER)) {
        points += 100;
    }

    if(gamestatusp(KILLED_LAWBRINGER)) {
        points += 100;
    }

    points += (Player.xp / 50);
    points += (Player.cash / 500);

    for(i = 0; i < MAXITEMS; ++i) {
        if(Player.possessions[i] != NULL) {
            points += (Player.possessions[i]->level * (Player.possessions[i]->known + 1));
        }
    }

    for(i = 0; i < MAXPACK; ++i) {
        if(Player.pack[i] != NULL) {
            points += (Player.pack[i]->level * (Player.pack[i]->known + 1));
        }
    }

    for(i = 0; i < NUMRANKS; ++i) {
        if(Player.rank[i] == 5) {
            points += 500;
        }
        else {
            points += (20 * Player.rank[i]);
        }
    }

    if(Player.hp < 1) {
        points = points / 2;
    }
    else if(Player.rank[ADEPT]) {
        points *= 10;
    }

    return points;
}

/* Returns the 24 hour clock hour */
int hour()
{
    return (((Time + 720) / 60) % 24);
}

/* Returns 0, 10, 20, 30, 40, or 50 */
int showminute()
{
    return (((Time % 60) / 10) * 10);
}

/* Returns the 12 hour clock hour */
int showhour() 
{
    int showtime;

    if((hour() == 0) || (hour() == 12)) {
        showtime = 12;
    }
    else {
        showtime = hour() % 12;
    }

    return showtime;
}

/* nighttime is defined from 9 PM to 6 AM */
int nighttime()
{
    if((hour() > 20) || (hour() < 7)) {
        return 1;
    }
    else {
        return 0;
    }
}

char *getarticle(char *str)
{
    if((str[0] == 'a')
       || (str[0] == 'A')
       || (str[0] == 'e')
       || (str[0] == 'E')
       || (str[0] == 'i')
       || (str[0] == 'I')
       || (str[0] == 'o')
       || (str[0] == 'O')
       || (str[0] == 'u')
       || (str[0] == 'U')
       || (((str[0] == 'h') || (str[0] == 'H'))
           && ((str[1] == 'i') || (str[1] == 'e')))) {
        return "an ";
    }
    else {
        return "a ";
    }
}

int day()
{
    return ((Date % 30) + 1);
}

char *ordinal(int number)
{
    if((number == 11)
       || (number == 12)
       || (number == 13)) {
        return "th";
    }
    else {
        switch(number % 10) {
        case 1:

            return "st";
        case 2:

            return "nd";
        case 3:

            return "rd";
        default:

            return "th";
        }
    }
}

char *month()
{
    switch((Date % 360) / 30) {
    case 0:

        return "Freeze";
    case 1:

        return "Ice";
    case 2:

        return "Mud";
    case 3:

        return "Storm";
    case 4:

        return "Breeze";
    case 5:

        return "Light";
    case 6:

        return "Flame";
    case 7:

        return "Broil";
    case 8:

        return "Cool";
    case 9:

        return "Haunt";
    case 10:

        return "Chill";
    case 11:

        return "Dark";
    default:
        
        return "Twixt";
    }
}

/*
 * Finds floor space on level with buildaux not equal to baux, sets
 * x,y, there. There must *be* floor space somewhere on level...
 */
void findspace(int *x, int *y, int baux)
{
    int i;
    int j;
    int k;
    int l;
    int done = FALSE;

    k = random_range(WIDTH);
    i = k;
    l = random_range(LENGTH);
    j = l;
    ++i;

    if(i >= WIDTH) {
        i = 0;
        ++j;

        if(j > LENGTH) {
            j = 0;
        }

        if((i == k) && (j == l)) {
            done = 1;
        }
        else {
            done = 0;
        }
    }

    if(done
       || ((Level->site[i][j].locchar == FLOOR)
           && (Level->site[i][j].creature == NULL)
           && (Level->site[i][j].buildaux != baux))) {
        done = 1;
    }
    else {
        done = 0;
    }

    while(!done) {
        ++i;

        if(i >= WIDTH) {
            i = 0;
            ++j;

            if(j > LENGTH) {
                j = 0;
            }

            if((i == k) && (j == l)) {
                done = 1;
            }
            else {
                done = 0;
            }
        }

        if(done
           || ((Level->site[i][j].locchar == FLOOR)
               && (Level->site[i][j].creature == NULL)
               && (Level->site[i][j].buildaux != baux))) {
            done = 1;
        }
        else {
            done = 0;
        }
    }

    *x = i;
    *y = j;
}

/* Is prefix a prefix of s? */
int strprefix(char *prefix, char *s)
{
    int i = 0;
    int matched = TRUE;

    if(strlen(prefix) > strlen(s)) {
        return FALSE;
    }
    else {
        while(matched && (i < strlen(prefix))) {
            if(prefix[i] == s[i]) {
                matched = 1;
            }
            else {
                matched = 0;
            }

            ++i;
        }

        return matched;
    }
}

int confirmation()
{
    switch(random_range(4)) {
    case 0:
        mprint("Are you sure? [yn] ");

        break;
    case 1:
        mprint("Certain about that? [yn] ");

        break;
    case 2:
        mprint("Do you really mean it? [yn] ");

        break;
    case 3:
        mprint("Confirm that, would you? [yn] ");

        break;
    }

    if(ynq() == 'y') {
        return 1;
    }
    else {
        return 0;
    }
}

/* Is character c a member of string s */
int strmem(char c, char *s)
{
    int i;
    int found = FALSE;

    for(i = 0; (i < strlen(s)) && !found; ++i) {
        if(s[i] == c) {
            found = 1;
        }
        else {
            found = 0;
        }
    }

    return found;
}

/* Returns true if its ok to get rid of a level */
int ok_to_free(plv level)
{
    if(level == NULL) {
        return FALSE;
    }
    else {
        if((level->environment != E_CITY)
           && (level->environment != E_VILLAGE)
           && (level->environment != Current_Dungeon)) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

void calc_weight()
{
    int i;
    int weight = 0;

    for(i = 1; i < MAXITEMS; ++i) {
        if(Player.possessions[i] != NULL) {
            weight += (Player.possessions[i]->weight * Player.possessions[i]->number);
        }
    }

    if((Player.possessions[O_WEAPON_HAND] != NULL)
       && (Player.possessions[O_READY_HAND] == Player.possessions[O_WEAPON_HAND])) {
        weight -= (Player.possessions[O_READY_HAND]->weight * Player.possessions[O_READY_HAND]->number);
    }

    for(i = 1; i < MAXPACK; ++i) {
        if(Player.pack[i] != NULL) {
            weight += (Player.pack[i]->weight * Player.pack[i]->number);
        }
    }

    Player.itemweight = weight;
    dataprint();
}

/* Alloc just enough string space for str, strcpy, and return pointer */
char *salloc(char *str)
{
    char *s = calloc((unsigned)(strlen(str) + 1), sizeof(char));
    strcpy(s, str);

    return s;
}
