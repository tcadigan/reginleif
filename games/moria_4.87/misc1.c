#include "misc1.h"

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "constants.h"
#include "externs.h"
#include "io.h"
#include "types.h"

/* Gets a new random seed for the random number generator */
void init_seeds()
{
    long clock;
    int euid;

    /* In case game is setuid root */
    euid = geteuid();

    if(euid == 0) {
	euid = (int)time((long *)0);
    }

    clock = time((long *)0);
    clock = clock * getpid() * euid;

#ifdef USG
    /* Only uses randes_seed */
#else
    initstate((unsigned int)clock, randes_state, STATE_SIZE);
#endif

    randes_seed = (unsigned int)clock;

    clock = time((long *)0);
    clock = clock * getpid() * euid;

#ifdef USG
    /* Only uses town_seed */
#else
    initstate((unsigned int)clock, town_state, STATE_SIZE);
#endif

    town_seed = (unsigned int)clock;
    clock = time((long *)0);

#if 0
    clock = clock * getpid() * euid * getuid();
#endif

#ifdef USG
    /* Can't do this, so fake it */
    srand48(clock);

    /* Make it a little more random */
    for(clock = randint(100); clock >= 0; --clock) {
	lrand48();
    }
#else
    initstate((unsigned int)clock, norm_state, STATE_SIZE);

    /* Make it a little more random */
    for(clock = randint(100); clock >= 0; --clock) {
	random();
    }
#endif
}

#ifdef USG
/* Special array for restoring the SYS V number generator */
unsigned short oldseed[3];
#endif

/* Change to different random number generator state */
void set_seed(char *state, int seed)
{
#ifdef USG
    unsigned short *pointer;

    /* Make phony call to get points to old value of seed */
    pointer = seed48(oldseed);

    /* Copy old seed into oldseed */
    oldseed[0] = pointer[0];
    oldseed[1] = pointer[1];
    oldseed[2] = pointer[2];

    /* Want reproducible state here, so call srand38 */
    srand48((long)seed);
#else
    setstate(state);

    /* Want reproducible state here, so call srandom */
    srandom(seed);
#endif
}

/* Restore the normal random number generator state */
void reset_seed()
{
#ifdef USG
    seed48(oldseed);

#if 0
    /* Can't do this, so just call srand() with the current time */
    srand48((unsigned int)(time((long *)0)));
#endif

#else
    setstate(norm_state);
#endif
}

/* Returns the day number; 0 == Sunday ... 6 == Saturday    -RAK */
int day_num()
{
    long clock;
    struct tm *time_struct;

    clock = time((long *)0);
    time_struct = localtime(&clock);

    return time_struct->tm_wday;
}

/* Returns the hour number; 0 == midnight ... 23 == 11 PM    -RAK- */
int hour_num()
{
    long clock;
    struct tm *time_struct;

    clock = time((long *)0);
    time_struct = localtime(&clock);

    return time_struct->tm_hour;
}

/* Check the day-time strings to see if open    -RAK- */
int check_time()
{
    switch((int)days[day_num()][hour_num() + 4]) {
    case '.': /* Closed */
	
	return FALSE;
    case 'x': /* Normal hours */

	return TRUE;
    default: /* Other, assumed closed */

	break;
    }
    
    return FALSE;
}

/* Generates a random integer x where 1 <== X <== MAXVAL    -RAK- */
int randint(int maxval)
{
    long randval;

#ifdef USG
    randval = lrand48();

#else
    randval = random();

#endif

    return ((randval % maxval) + 1);
}

/* For i := 1 to y do sum := sum + randint(x) */
int rand_rep(int num, int die)
{
    int sum = 0;
    int i;

    for(i = 0; i < num; ++i) {
	sum += randint(die);
    }

    return sum;
}

/* Generates a random integer number of NORMAL distribution    -RAK- */
int randnor(int mean, int stand)
{
    return ((int)((sqrt(-2.0 * log(randint((int)9999999) / 10000000.0)) * cos(6.283 * (randint((int)9999999) / 10000000.0)) * stand) + mean));
}

/* Returns position of first set bit and clears that bit    -RAK- */
int bit_pos(unsigned int *test)
{
    int i;
    int mask = 0x1;

    for(i = 0; i < (sizeof(int) * 8); ++i) {
	if(*test & mask) {
	    *test &= ~mask;

	    return i;
	}

	mask <<= 1;
    }

    /* No 1 bits found */
    return -1;
}

/* Check a coordinate for in bounds status    -RAK- */
int in_bounds(int y, int x)
{
    if((y > 0)
       && (y < (cur_height - 1))
       && (x > 0)
       && (x < (cur_width - 1))) {
	return TRUE;
    }

    return FALSE;
}

/*
 * Distance between two points    -RAK-
 * There is a bessel funciton named y1 in the math library, ignore warning
 */
int distance(int y1, int x1, int y2, int x2)
{
    int dy;
    int dx;

    dy = y1 - y2;

    if(dy < 0) {
	dy = -dy;
    }

    dx = x1 - x2;

    if(dx < 0) {
	dx = -dx;
    }

    if(dy > dx) {
	return ((2 * (dy + dx) - dx) / 2);
    }
    else {
	return ((2 * (dy + dx) - dy) / 2);
    }
}

/* Check points north, south, east, and west for a type    -RAK- */
int next_to4(int y, int x, int elem_a, int elem_b, int elem_c)
{
    int i;

    i = 0;

    if(y > 0) {
	if((cave[y - 1][x].fval == elem_a)
	   || (cave[y - 1][x].fval == elem_b)
	   || (cave[y - 1][x].fval == elem_c)) {
	    ++i;
	}
    }

    if(y < (cur_height - 1)) {
	if((cave[y + 1][x].fval = elem_a)
	   || (cave[y + 1][x].fval == elem_b)
	   || (cave[y + 1][x].fval == elem_c)) {
	    ++i;
	}
    }

    if(x > 0) {
	if((cave[y][x - 1].fval == elem_a)
	   || (cave[y][x - 1].fval == elem_b)
	   || (cave[y][x - 1].fval == elem_c)) {
	    ++i;
	}
    }

    if(x < (cur_width - 1)) {
	if((cave[y][x + 1].fval == elem_a)
	   || (cave[y][x + 1].fval == elem_b)
	   || (cave[y][x + 1].fval == elem_c)) {
	    ++i;
	}
    }

    return i;
}

/* Checks all adjacent spots for elements    -RAK- */
int next_to8(int y, int x, int elem_a, int elem_b, int elem_c)
{
    int k;
    int j;
    int i;

    i = 0;

    for(j = (y - 1); j <= (y + 1); ++j) {
	for(k = (x - 1); k <= (x + 1); ++k) {
	    if(in_bounds(j, k)) {
		if((cave[j][k].fval == elem_a)
		   || (cave[j][k].fval == elem_b)
		   || (cave[j][k].fval == elem_c)) {
		    ++i;
		}
	    }
	}
    }

    return i;
}

/* Link all free space in treasure list together */
void tlink()
{
    int i;

    for(i = 0; i < MAX_TALLOC; ++i) {
	t_list[i] = blank_treasure;
	t_list[i].p1 = i - 1;
    }

    tcptr = MAX_TALLOC - 1;
}

/* Link all free space in monster list together */
void mlink()
{
    int i;

    for(i = 0; i < MAX_MALLOC; ++i) {
	m_list[i] = blank_monster;
	m_list[i].nptr = i - 1;
    }

    m_list[1].nptr = 0;
    muptr = 0;
    mfptr = MAX_MALLOC - 1;
}

/* Initializes M_LEVEL array for use with PLACE_MONSTER    -RAK- */
void init_m_level()
{
    int i;
    int j;
    int k;

    i = 0;
    j = 0;
    k = MAX_CREATURES - WIN_MON_TOT;

    while(j < MAX_MONS_LEVEL) {
	m_level[j] = 0;

	while((i < k) && (c_list[i].level == j)) {
	    ++m_level[j];
	    ++i;
	}

	++j;
    }

    for(i = 2; i < MAX_MONS_LEVEL; ++i) {
	m_level[i] += m_level[i - 1];
    }
}

/* Initializes T_LEVEL array for use with PLACE_OBJECT    -RAK- */
void init_t_level()
{
    int i;
    int j;

    i = 0;
    j = 0;

    while((j <= MAX_OBJ_LEVEL) && (i < MAX_OBJECTS)) {
	while((i < MAX_OBJECTS) && (object_list[i].level == j)) {
	    ++t_level[j];
	    ++i;
	}

	++j;
    }

    for(i = 1; i < MAX_OBJ_LEVEL; ++i) {
	t_level[i] += t_level[i - 1];
    }
}

/* Adjust prices of objects    -RAK- */
void price_adjust()
{
    int i;

    for(i = 0; i < MAX_OBJECTS; ++i) {
	object_list[i].cost = (object_list[i].cost * COST_ADJ) + 0.99;
    }

    for(i = 0; i < INVEN_INIT_MAX; ++i) {
	inventory_init[i].cost = (inventory_init[i].cost * COST_ADJ) + 0.99;
    }
}

/*
 * Converts input string into a dice roll    -RAK-
 *     Normal string will look like "2d6", "3d8", ... etc.
 */
int damroll(char *dice)
{
    int num;
    int sides;

    num = 0;
    sides = 0;
    sscanf(dice, "%d d %d", &num, &sides);

    return rand_rep(num, sides);
}

/*
 * Returns true if no obstructions between two given points    -RAK-
 * There is a bessel function named y1 in the math library, ignore warning
 */
int los(int y1, int x1, int y2, int x2)
{
    int ty;
    int tx;
    int stepy;
    int stepx;
    int aty;
    int atx;
    int p1;
    int p2;
    double slp;
    double tmp;
    int flag;

    ty = y1 - y2;
    tx = x1 - x2;
    flag = TRUE;

    if((ty != 0) || (tx != 0)) {
	if(ty < 0) {
	    stepy = -1;
	    aty = -ty;
	}
	else {
	    stepy = 1;
	    aty = ty;
	}

	if(tx < 0) {
	    stepx = -1;
	    atx = -tx;
	}
	else {
	    stepx = 1;
	    atx = tx;
	}

	if(ty == 0) {
	    x2 += stepx;
	    flag = cave[y2][x2].fopen;

	    while((x1 != x2) && flag) {
		x2 += stepx;
		flag = cave[y2][x2].fopen;
	    }
	}
	else if(tx == 0) {
	    y2 += stepy;
	    flag = cave[y2][x2].fopen;

	    while((y1 != y2) && flag) {
		y2 += stepy;
		flag = cave[y2][x2].fopen;
	    }
	}
	else if(aty > atx) {
	    slp = ((double)atx / (double)aty) * stepx;
	    tmp = x2;

	    y2 += stepy;
	    tmp += slp;

	    /* Round to nearest integer */
	    p1 = (int)floor(tmp - 0.1 + 0.5);
	    p2 = (int)floor(tmp + 0.1 + 0.5);

	    if(!cave[y2][p1].fopen && !cave[y2][p2].fopen) {
		flag = FALSE;
	    }

	    while((y1 != y2) && flag) {
		y2 += stepy;
		tmp += slp;

		/* Round to nearest integer */
		p1 = (int)floor(tmp - 0.1 + 0.5);
		p2 = (int)floor(tmp + 0.1 + 0.5);

		if(!cave[y2][p1].fopen && !cave[y2][p2].fopen) {
		    flag = FALSE;
		}
	    }
	}
	else {
	    slp = ((double)aty / (double)atx) * stepy;
	    tmp = y2;

	    x2 += stepx;
	    tmp += slp;

	    /* Round to the nearest integer */
	    p1 = (int)floor(tmp - 0.1 + 0.5);
	    p2 = (int)floor(tmp + 0.1 + 0.5);

	    if(!cave[p1][x2].fopen && !cave[p2][x2].fopen) {
		flag = FALSE;
	    }

	    while((x1 != x2) && flag) {
		x2 = stepx;
		tmp += slp;

		/* Round to nearest integer */
		p1 = (int)floor(tmp - 0.1 + 0.5);
		p2 = (int)floor(tmp + 0.1 + 0.5);

		if(!cave[p1][x2].fopen && !cave[p2][x2].fopen) {
		    flag = FALSE;
		}
	    }
	}
    }

    return flag;
}

/* Returns symbols for given row, column    -RAK- */
void loc_symbol(int y, int x, char *sym)
{
    cave_type *cave_ptr;
    monster_type *mon_ptr;

    cave_ptr = &cave[y][x];

    if((cave_ptr->cptr == 1) && !find_flag) {
	*sym = '@';
    }
    else if(py.flags.blind > 0) {
	*sym = ' ';
    }
    else {
	if(cave_ptr->cptr > 1) {
	    mon_ptr = &m_list[cave_ptr->cptr];

	    if(mon_ptr->ml
	       && (((c_list[mon_ptr->mptr].cmove & 0x00010000) == 0)
		   || py.flags.see_inv)) {
		*sym = c_list[mon_ptr->mptr].cchar;
	    }
	    else if(cave_ptr->tptr != 0) {
		*sym = t_list[cave_ptr->tptr].tchar;
	    }
	    else if(cave_ptr->fval < 10) {
		*sym = '.';
	    }
	    else {
		*sym = '#';
	    }
	}
	else if(cave_ptr->tptr != 0) {
	    *sym = t_list[cave_ptr->tptr].tchar;
	}
	else if(cave_ptr->fval < 10) {
	    *sym = '.';
	}
	else {
	    *sym = '#';
	}
    }
}

/* Tests a spot for light or field mark status    -RAK- */
int test_light(int y, int x)
{
    cave_type *cave_ptr;

    cave_ptr = &cave[y][x];

    if(cave_ptr->pl || cave_ptr->fm || cave_ptr->tl) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

/* Prints the map of the dungeon    -RAK- */
void prt_map()
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int ypos;
    int isp;

    /* This eliminates link warning: xpos may be used before set */
    int xpos = 0;
    vtype floor_str;
    char tmp_char[2];
    int flag;
    cave_type *cave_ptr;

    /* Used for erasing dity lines */
    k = 0;

    /* Erasure starts in this column */
    l = 13;

    /* Top to bottom */
    for(i = panel_row_min; i <= panel_row_max; ++i) {
	/* Increment dity line counter */
	++k;

	/* If line is dirty... */
	if(used_line[k]) {
	    /* Erase it */
	    erase_line(k, l);

	    /* Now it's clean */
	    used_line[k] = FALSE;
	}

	/* floor_str is string to be printer */
	floor_str[0] = '\0';

	/* Save row */
	ypos = i;

	/* False until floor_str != "" */
	flag = FALSE;

	/* Number of blanks encountered */
	isp = 0;

	/* Left to right */
	for(j = panel_col_min; j <= panel_col_max; ++j) {
	    /* Get character for location */
	    cave_ptr = &cave[i][j];

	    if(cave_ptr->pl || cave_ptr->fm || cave_ptr->tl) {
		loc_symbol(i, j, tmp_char);
	    }
	    else if((cave_ptr->cptr == 1) && !find_flag) {
		tmp_char[0] = '@';
	    }
	    else if(cave_ptr->cptr > 1) {
		if(m_list[cave_ptr->cptr].ml) {
		    loc_symbol(i, j, tmp_char);
		}
		else {
		    tmp_char[0] = ' ';
		}
	    }
	    else {
		tmp_char[0] = ' ';
	    }

	    if(py.flags.image > 0) {
		if(randint(12) == 1) {
		    tmp_char[0] = randint(95) + 31;
		}
	    }

	    /* If blank... */
	    if(tmp_char[0] == ' ') {
		/* If floor_str != "" */
		if(flag) {
		    /* Increment blank ctr */
		    ++isp;

		    /* Too many blanks, print floor_str and reset */
		    if(isp > 3) {
			print(floor_str, ypos, xpos);
			flag = FALSE;
			isp = 0;
		    }
		}
	    }
	    else {
		/* floor_str != "" */
		if(flag) {
		    /* Add on the blanks */
		    if(isp > 0) {
			for(m = 0; m < isp; ++m) {
			    strcat(floor_str, " ");
			}

			isp = 0;
		    }

		    /* Add on the character */
		    tmp_char[1] = '\0';
		    strcat(floor_str, tmp_char);
		}
		else {
		    /* floor_str == "" */

		    /* Save column for printing */
		    xpos = j;

		    /* Set flag to true */
		    flag = TRUE;

		    /* floor_str != "" */
		    floor_str[0] = tmp_char[0];
		    floor_str[1] = '\0';
		}
	    }
	}

	/* Print remainder, if any */
	if(flag) {
	    print(floor_str, ypos, xpos);
	}
    }
}

/* Compact monsters    -RAK- */
void compact_monsters()
{
    int i;
    int j;
    int k;
    int cur_dis;
    int delete_1;
    int delete_any;
    monster_type *mon_ptr;

    cur_dis = 66;
    delete_any = FALSE;

    i = muptr;
    j = 0;

    while(i > 0) {
	delete_1 = FALSE;
	k = m_list[i].nptr;
	mon_ptr = &m_list[i];
	
	if(cur_dis > mon_ptr->cdis) {
	    if(randint(3) == 1) {
		if(j == 0) {
		    muptr = k;
		}
		else {
		    m_list[j].nptr = k;
		}

		cave[mon_ptr->fy][mon_ptr->fx].cptr = 0;
		m_list[i] = blank_monster;
		m_list[i].nptr = mfptr;
		mfptr = i;
		delete_1 = TRUE;
		delete_any = TRUE;
	    }
	}

	if(!delete_1) {
	    j = i;
	}

	i = k;
    }

    if(!delete_any) {
	cur_dis -= 6;
    }

    while(!delete_any) {
	i = muptr;
	j = 0;

	while(i > 0) {
	    delete_1 = FALSE;
	    k = m_list[i].nptr;
	    mon_ptr = &m_list[i];

	    if(cur_dis > mon_ptr->cdis) {
		if(randint(3) == 1) {
		    if(j == 0) {
			muptr = k;
		    }
		    else {
			m_list[j].nptr = k;
		    }

		    cave[mon_ptr->fy][mon_ptr->fx].cptr = 0;
		    m_list[i] = blank_monster;
		    m_list[i].nptr = mfptr;
		    mfptr = i;
		    delete_1 = TRUE;
		    delete_any = TRUE;
		}
	    }

	    if(!delete_1) {
		j = i;
	    }

	    i = k;
	}

	if(!delete_any) {
	    cur_dis -= 6;
	}
    }

    if(cur_dis < 66) {
	prt_map();
    }
}

/* Returns a pointer to next free space    -RAK- */
void popm(int *x)
{
    if(mfptr <= 1) {
	compact_monsters();
    }

    *x = mfptr;
    mfptr = m_list[*x].nptr;
}

/* Pushes a record back onto free space list    -RAK- */
void pushm(int x)
{
    m_list[x] = blank_monster;
    m_list[x].nptr = mfptr;
    mfptr = x;
}

/* Gives max hit points    -RAK- */
int max_hp(char *hp_str)
{
    int num;
    int die;

    sscanf(hp_str, "%d d %d", &num, &die);

    return (num * die);
}

/* Places a monster a given location    -RAK- */
void place_monster(int y, int x, int z, int slp)
{
    int cur_pos;
    monster_type *mon_ptr;

    popm(&cur_pos);
    mon_ptr = &m_list[cur_pos];
    mon_ptr->fy = y;
    mon_ptr->fx = x;
    mon_ptr->mptr = x;
    mon_ptr->nptr = muptr;
    muptr = cur_pos;

    if(c_list[x].cdefense & 0x4000) {
	mon_ptr->hp = max_hp(c_list[z].hd);
    }
    else {
	mon_ptr->hp = damroll(c_list[z].hd);
    }

    mon_ptr->cspeed = c_list[z].speed + py.flags.speed;
    mon_ptr->stunned = 0;
    mon_ptr->cdis = distance(char_row, char_col, y, x);
    cave[y][x].cptr = cur_pos;

    if(slp) {
	if(c_list[z].sleep == 0) {
	    mon_ptr->csleep = 0;
	}
	else {
	    mon_ptr->csleep = (c_list[x].sleep / 5.0) + randint(c_list[z].sleep);
	}
    }
    else {
	mon_ptr->csleep = 0;
    }
}

/* Places a monster at given location    -RAK- */
void place_win_monster()
{
    int cur_pos;
    int y;
    int x;
    monster_type *mon_ptr;

    if(!total_winner) {
	popm(&cur_pos);
	mon_ptr = &m_list[cur_pos];

	y = randint(cur_height - 2);
	x = randint(cur_width - 2);

	while(((cave[y][x].fval != 1)
	       && (cave[y][x].fval != 2)
	       && (cave[y][x].fval != 4))
	      || (cave[y][x].cptr != 0)
	      || (cave[y][x].tptr != 0)
	      || (distance(y, x, char_row, char_col) <= MAX_SIGHT)) {
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}

	mon_ptr->fy = y;
	mon_ptr->fx = x;
	mon_ptr->mptr = randint(WIN_MON_TOT) - 1 + m_level[MAX_MONS_LEVEL - 1] + m_level[0];
	mon_ptr->nptr = muptr;
	muptr = cur_pos;

	if(c_list[mon_ptr->mptr].cdefense & 0x400) {
	    mon_ptr->hp = max_hp(c_list[mon_ptr->mptr].hd);
	}
	else {
	    mon_ptr->hp = damroll(c_list[mon_ptr->mptr].hd);
	}

	mon_ptr->cspeed = c_list[mon_ptr->mptr].speed + py.flags.speed;
	mon_ptr->stunned = 0;
	mon_ptr->cdis = distance(char_row, char_col, y, x);
	cave[y][x].cptr = cur_pos;
	mon_ptr->csleep = 0;
    }
}

/* Allocates a random monster    -RAK- */
void alloc_monster(int (*alloc_set)(), int num, int dis, int slp)
{
    int y;
    int x;
    int i;
    int j;
    int k;

    for(i = 0; i < num; ++i) {
	y = randint(cur_height - 2);
	x = randint(cur_width - 2);

	while((!(*alloc_set)(cave[y][x].fval))
	       || (cave[y][x].cptr != 0)
	       || !cave[y][x].fopen
	      || (distance(y, x, char_row, char_col) <= dis)) {
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}

	if(dun_level == 0) {
	    j = randint(m_level[0]) - 1;
	}
	else if(dun_level >= MAX_MONS_LEVEL) {
	    j = randint(m_level[MAX_MONS_LEVEL - 1]) - 1 + m_level[0];
	}
	else if(randint(MON_NASTY) == 1) {
	    /* abs may be a macro, don't call it with randnor as a parameter */
	    k = randnor(0, 4);
	    j = dun_level + abs(k) + 1;

	    if(j >= MAX_MONS_LEVEL) {
		j = MAX_MONS_LEVEL;
	    }

	    k = m_level[j] - m_level[j - 1];
	    j = randint(k) - 1 + m_level[j - 1];
	}
	else {
	    j = randint(m_level[dun_level]) - 1 + m_level[0];
	}
    }
}

/* Places creature adjacent to given location    -RAK- */
int summon_monster(int *y, int *x, int slp)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    cave_type *cave_ptr;
    int summon;

    i = 0;
    m = dun_level + MON_SUMMON_ADJ;
    summon = FALSE;

    if(m > MAX_MONS_LEVEL) {
	l = MAX_MONS_LEVEL;
    }
    else {
	l = m;
    }

    if(dun_level == 0) {
	l = randint(m_level[0]) - 1;
    }
    else {
	l = randint(m_level[l]) - 1 + m_level[0];
    }

    j = *y - 2 + randint(3);
    k = *x - 2 + randint(3);

    if(in_bounds(j, k)) {
	cave_ptr = &cave[j][k];

	if((cave_ptr->fval == 1)
	   || (cave_ptr->fval == 2)
	   || (cave_ptr->fval == 4)
	   || (cave_ptr->fval == 5)) {
	    if(cave_ptr->cptr == 0) {
		if(cave_ptr->fopen) {
		    place_monster(j, k, l, slp);
		    summon = TRUE;
		    i = 9;
		    *y = j;
		    *x = k;
		}
	    }
	}
    }

    ++i;

    while(i <= 9) {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);

	if(in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)
	       || (cave_ptr->fval == 5)) {
		if(cave_ptr->cptr == 0) {
		    if(cave_ptr->fopen) {
			place_monster(j, k, l, slp);
			summon = TRUE;
			i = 9;
			*y = j;
			*x = k;
		    }
		}
	    }
	}

	++i;
    }

    return summon;
}

/* Places undead adjacent to given location    -RAK- */
int summon_undead(int *y, int *x)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int ctr;
    int summon;
    cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL - 1] + m_level[0];

    m = randint(l) - 1;
    ctr = 0;

    if(c_list[m].cdefense & 0x0008) {
	ctr = 20;
	l = 0;
    }
    else {
	++m;

	if(m > l) {
	    ctr = 20;
	}
	else {
	    ++ctr;
	}
    }

    while(ctr <= 19) {
	if(c_list[m].cdefense & 0x0008) {
	    ctr = 20;
	    l = 0;
	}
	else {
	    ++m;

	    if(m > l) {
		ctr = 20;
	    }
	    else {
		++ctr;
	    }
	}
    }

    while(l != 0) {
	m = randint(l) - 1;
	ctr = 0;

	if(c_list[m].cdefense & 0x0008) {
	    ctr = 20;
	    l = 0;
	}
	else {
	    ++m;

	    if(m > l) {
		ctr = 20;
	    }
	    else {
		++ctr;
	    }
	}

	while(ctr <= 19) {
	    if(c_list[m].cdefense & 0x0008) {
		ctr = 20;
		l = 0;
	    }
	    else {
		++m;

		if(m > l) {
		    ctr = 20;
		}
		else {
		    ++ctr;
		}
	    }
	}
    }

    j = *y - 2 + randint(3);
    k = *x - 2 + randint(3);

    if(in_bounds(j, k)) {
	cave_ptr = &cave[j][k];

	if((cave_ptr->fval == 1)
	   || (cave_ptr->fval == 2)
	   || (cave_ptr->fval == 4)
	   || (cave_ptr->fval == 5)) {
	    if((cave_ptr->cptr == 0) && cave_ptr->fopen) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
    }

    ++i;

    while(i <= 9) {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);

	if(in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)
	       || (cave_ptr->fval == 5)) {
		if((cave_ptr->cptr == 0) && (cave_ptr->fopen)) {
		    place_monster(j, k, m, FALSE);
		    summon = TRUE;
		    i = 9;
		    *y = j;
		    *x = k;
		}
	    }
	}

	++i;
    }

    return summon;
}

/* If too many objects on floor level, delete some of them    -RAK- */
void compact_objects()
{
    int i;
    int j;
    int ctr;
    int cur_dis;
    int flag;
    cave_type *cave_ptr;
    treasure_type *t_ptr;

    ctr = 0;
    cur_dis = 66;

    for(i = 0; i < cur_height; ++i) {
	for(j = 0; j < cur_width; ++j) {
	    cave_ptr = &cave[i][j];

	    if(cave_ptr->tptr != 0) {
		if(distance(i, j, char_row, char_col) > cur_dis) {
		    flag = FALSE;
		    t_ptr = &t_list[cave_ptr->tptr];

		    switch(t_ptr->tval) {
		    case 102:
			if((t_ptr->subval == 1)
			   || (t_ptr->subval == 6)
			   || (t_ptr->subval == 9)) {
			    flag = TRUE;
			}
			else if(randint(4) == 1) {
			    flag = TRUE;
			}

			break;
		    case 103:
			flag = TRUE;

			break;
		    case 104:
		    case 105:
			/* Doors */
			if(randint(4) == 1) {
			    flag = TRUE;
			}

			break;
		    case 107:
		    case 108:
			/* Stairs, don't delete them */

			break;
		    case 110:
			/* Shop doors, don't delete them */

			break;
		    default:
			if(randint(8) == 1) {
			    flag = TRUE;
			}
		    }

		    if(flag) {
			cave_ptr->fopen = TRUE;
			t_list[cave_ptr->tptr] = blank_treasure;
			t_list[cave_ptr->tptr].p1 = tcptr;
			tcptr = cave_ptr->tptr;
			cave_ptr->tptr = 0;
			++ctr;
		    }
		}
	    }
	    
	    if(ctr == 0) {
		cur_dis -= 6;
	    }
	}
    }

    while(ctr <= 0) {
	for(i = 0; i < cur_height; ++i) {
	    for(j = 0; j < cur_width; ++j) {
		cave_ptr = &cave[i][j];

		if(cave_ptr->tptr != 0) {
		    if(distance(i, j, char_row, char_col) > cur_dis) {
			flag = FALSE;
			t_ptr = &t_list[cave_ptr->tptr];

			switch(t_ptr->tval) {
			case 102:
			    if((t_ptr->subval == 1)
			       || (t_ptr->subval == 6)
			       || (t_ptr->subval == 9)) {
				flag = TRUE;
			    }
			    else if(randint(4) == 1) {
				flag = TRUE;
			    }

			    break;
			case 103:
			    flag = TRUE;

			    break;
			case 104:
			case 105:
			    /* Doors */
			    if(randint(4) == 1) {
				flag = TRUE;
			    }

			    break;
			case 107:
			case 108:
			    /* Stairs don't delete them */

			    break;
			case 110:
			    /* Shop doors, don't delete them */

			    break;
			default:
			    if(randint(8) == 1) {
				flag = TRUE;
			    }
			}

			if(flag) {
			    cave_ptr->fopen = TRUE;
			    t_list[cave_ptr->tptr] = blank_treasure;
			    t_list[cave_ptr->tptr].p1 = tcptr;
			    tcptr = cave_ptr->tptr;
			    cave_ptr->tptr = 0;
			    ++ctr;
			}
		    }
		}

		if(ctr == 0) {
		    cur_dis -= 6;
		}
	    }
	}
    }

    if(cur_dis < 66) {
	prt_map();
    }
}

/* Gives pointer to next free space    -RAK- */
void popt(int *x)
{
    if(tcptr < 1) {
	compact_objects();
    }

    *x = tcptr;
    tcptr = t_list[*x].p1;
}

/* Pushes a record back onto free space list    -RAK- */
void pusht(int x)
{
    t_list[x] = blank_treasure;
    t_list[x].p1 = tcptr;
    tcptr = x;
}

/* Order the treasure list by level    -RAK- */
void sort_objects()
{
    int i;
    int j;
    int k;
    int gap;
    treasure_type tmp;

    gap = MAX_OBJECTS / 2;

    while(gap > 0) {
	for(i = gap; i < MAX_OBJECTS; ++i) {
	    j = i - gap;

	    while(j >= 0) {
		k = j + gap;

		if(object_list[j].level > object_list[k].level) {
		    tmp = object_list[j];
		    object_list[j] = object_list[k];
		    object_list[k] = tmp;
		}
		else {
		    j = -1;
		}

		j -= gap;
	    }
	}

	gap = gap / 2;
    }
}

/* Boolean: Is object enchanted?    -RAK- */
int magik(int chance)
{
    if(randint(100) <= chance) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

/* Enchance a bonus based on degree desired    -RAK- */
int m_bonus(int base, int max_std, int level)
{
    int x;
    int stand_dev;
    int tmp;

    stand_dev = (OBJ_STD_ADJ * level) + OBJ_STD_MIN;

    if(stand_dev > max_std) {
	stand_dev = max_std;
    }

    /* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);
    x = (abs(tmp) / 10.0) + base;

    if(x < base) {
	return base;
    }
    else {
	return x;
    }
}

/*
 * Chance of treasure having magic abilities    -RAK-
 *     Chance increases with each dungeon level
 */
void magic_treasure(int x, int level)
{
    treasure_type *t_ptr;
    int chance;
    int special;
    int cursed;
    int i;

    chance = OBJ_BASE_MAGIC + level;

    if(chance > OBJ_BASE_MAX) {
	chance = OBJ_BASE_MAX;
    }

    special = chance / OBJ_DIV_SPECIAL;
    cursed = chance / OBJ_DIV_CURSED;
    t_ptr = &t_list[x];

    /* I vhemently disagree with this!! */
    /* t_ptr->level = level; */

    /* Depending on treasure type, it can hace certain magical properties */
    switch(t_ptr->tval) {
    case 34:
    case 35:
    case 36:
	/* Armor and shields */

	if(magik(chance)) {
	    t_ptr->toac = m_bonus(1, 30, level);

	    if(magik(special)) {
		switch(randint(9)) {
		case 1:
		    t_ptr->flags |= 0x02380000;
		    strcat(t_ptr->name, " (R)");
		    t_ptr->toac += 5;
		    t_ptr->cost += 2500;

		    break;
		case 2: /* Resist acid */
		    t_ptr->flags |= 0x00100000;
		    strcat(t_ptr->name, " (RA)");
		    t_ptr->cost += 1000;

		    break;
		case 3:
		case 4:
		    /* Resist fire */

		    t_ptr->flags |= 0x00080000;
		    strcat(t_ptr->name, " (RF)");
		    t_ptr->cost += 600;

		    break;
		case 5:
		case 6:
		    /* Resist cold */

		    t_ptr->flags |= 0x00200000;
		    strcat(t_ptr->name, " (RC)");
		    t_ptr->cost += 600;

		    break;
		case 7:
		case 8:
		case 9:
		    /* Resist lightning */
		    
		    t_ptr->flags |= 0x02000000;
		    strcat(t_ptr->name, " (RL)");
		    t_ptr->cost += 500;

		    break;
		}
	    }
	}
	else if(magik(cursed)) {
	    t_ptr->toac = -m_bonus(1, 40, level);
	    t_ptr->cost = 0;
	    t_ptr->flags |= 0x80000000;
	}
	
	break;
    case 21:
    case 22:
    case 23:
	/* Weapons */

	if(magik(chance)) {
	    t_ptr->tohit = m_bonus(0, 40, level);
	    t_ptr->todam = m_bonus(0, 40, level);

	    if(magik(special)) {
		switch(randint(16)) {
		case 1: /* Holy avenger */
		    t_ptr->flags += 0x0141801;
		    t_ptr->tohit += 5;
		    t_ptr->todam += 5;
		    t_ptr->toac = randint(4);

		    /* The value in p1 is used for strength increase */
		    t_ptr->p1 = randint(4);
		    strcat(t_ptr->name, " [%P4] (HA) (%P1 to STR)");
		    t_ptr->cost += (t_ptr->p1 * 500);
		    t_ptr->cost += 10000;

		    break;
		case 2: /* Defender */
		    t_ptr->flags = 0x7B80900;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    t_ptr->toac = 5 + randint(5);
		    strcat(t_ptr->name, " [%P4] (DF)");

		    /* Note that the value in p1 is unused */
		    t_ptr->p1 = randint(3);
		    t_ptr->cost += (t_ptr->p1 * 500);
		    t_ptr->cost += 75000;

		    break;
		case 3:
		case 4:
		    /* Slay monster */
		    
		    t_ptr->flags |= 0x01004000;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    strcat(t_ptr->name, " (SM)");
		    t_ptr->cost += 5000;

		    break;
		case 5:
		case 6:
		    /* Slay dragon */

		    t_ptr->flags |= 0x00002000;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    strcat(t_ptr->name, " (SD)");
		    t_ptr->cost += 4000;

		    break;
		case 7:
		case 8:
		    /* Slay evil */

		    t_ptr->flags |= 0x00008000;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    strcat(t_ptr->name, " (SE)");
		    t_ptr->cost += 4000;

		    break;
		case 9:
		case 10:
		    /* Slay undead */

		    t_ptr->flags |= 0x00010000;
		    t_ptr->tohit += 2;
		    t_ptr->todam += 2;
		    strcat(t_ptr->name, " (SU)");
		    t_ptr->cost += 3000;

		    break;
		case 11:
		case 12:
		case 13:
		    /* Flame tongue */

		    t_ptr->flags |= 0x00040000;
		    ++t_ptr->tohit;
		    t_ptr->todam += 3;
		    strcat(t_ptr->name, " (FT)");
		    t_ptr->cost += 2000;

		    break;
		case 14:
		case 15:
		case 16:
		    /* Frost brand */

		    t_ptr->flags |= 0x00020000;
		    ++t_ptr->tohit;
		    ++t_ptr->todam;
		    strcat(t_ptr->name, " (FB)");
		    t_ptr->cost += 1200;

		    break;
		}
	    }
	}
	else if(magik(cursed)) {
	    t_ptr->tohit = -m_bonus(1, 55, level);
	    t_ptr->todam = -m_bonus(1, 55, level);
	    t_ptr->flags |= 0x80000000;
	    t_ptr->cost = 0;
	}

	break;
    case 20: /* Bows, corssbows and slings */
	if(magik(chance)) {
	    t_ptr->tohit = m_bonus(1, 30, level);
	}
	else if(magik(cursed)) {
	    t_ptr->tohit = -m_bonus(1, 50, level);
	    t_ptr->flags |= 0x80000000;
	    t_ptr->cost = 0;
	}

	break;
    case 25: /* Digging tools */
	if(magik(chance)) {
	    switch(randint(3)) {
	    case 1:
	    case 2:
		t_ptr->p1 = m_bonus(2, 25, level);
		t_ptr->cost += (t_ptr->p1 * 100);

		break;
	    case 3:
		t_ptr->p1 = -m_bonus(1, 30, level);
		t_ptr->cost = 0;
		t_ptr->flags |= 0x80000000;
	    }
	}

	break;
    case 31: /* Gloves and gauntlets */
	if(magik(chance)) {
	    t_ptr->toac = m_bonus(1, 20, level);

	    if(magik(special)) {
		switch(randint(2)) {
		case 1:
		    t_ptr->flags |= 0x00800000;
		    strcat(t_ptr->name, " of Free Action");
		    t_ptr->cost += 1000;

		    break;
		case 2:
		    t_ptr->tohit = 1 + randint(3);
		    t_ptr->todam = 1 + randint(3);
		    strcat(t_ptr->name, " of Slaying (%P2,%P3)");
		    t_ptr->cost += ((t_ptr->tohit + t_ptr->todam) * 250);

		    break;
		}
	    }
	}
	else if(magik(cursed)) {
	    if(magik(special)) {
		switch(randint(2)) {
		case 1:
		    t_ptr->flags |= 0x80000002;
		    strcat(t_ptr->name, " of Clumsiness");
		    t_ptr->p1 = 1;

		    break;
		case 2:
		    t_ptr->flags |= 0x80000001;
		    strcat(t_ptr->name, " of Weakness");
		    t_ptr->p1 = 1;

		    break;
		}
	    }

	    t_ptr->toac = -m_bonus(1, 40, level);
	    t_ptr->p1 = -m_bonus(1, 10, level);
	    t_ptr->flags |= 0x80000000;
	    t_ptr->cost = 0;
	}

	break;
    case 30: /* Boots */
	if(magik(chance)) {
	    t_ptr->toac = m_bonus(1, 20, level);

	    if(magik(special)) {
		switch(randint(12)) {
		case 1:
		    t_ptr->flags |= 0x00001000;
		    strcat(t_ptr->name, " of Speed");
		    t_ptr->p1 = 1;
		    t_ptr->cost += 5000;

		    break;
		case 2:
		case 3:
		case 4:
		case 5:
		    t_ptr->flags |= 0x00000100;
		    t_ptr->p1 = randint(3);
		    strcat(t_ptr->name, " of Stealth (%P1)");
		    t_ptr->cost += 500;

		    break;
		default:
		    t_ptr->flags |= 0x04000000;
		    strcat(t_ptr->name, " of Slow descent");
		    t_ptr->cost += 250;

		    break;
		}
	    }
	}
	else if(magik(cursed)) {
	    switch(randint(3)) {
	    case 1:
		t_ptr->flags |= 0x80001000;
		strcat(t_ptr->name, " of Slowness");
		t_ptr->p1 = -1;

		break;
	    case 2:
		t_ptr->flags |= 0x80000200;
		strcat(t_ptr->name, " of Noise");

		break;
	    case 3:
		t_ptr->flags |= 0x80000000;
		strcat(t_ptr->name, " of Great Mass");
		t_ptr->weight = t_ptr->weight * 5;

		break;
	    }

	    t_ptr->cost = 0;
	    t_ptr->ac = -m_bonus(2, 45, level);
	}

	break;
    case 33: /* Helms */
	if(magik(chance)) {
	    t_ptr->toac = m_bonus(1, 20, level);

	    if(magik(special)) {
		switch(t_ptr->subval) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		    switch(randint(3)) {
		    case 1:
			t_ptr->p1 = randint(2);
			t_ptr->flags |= 0x00000008;
			strcat(t_ptr->name, " of Intelligence (%P1)");
			t_ptr->cost += (t_ptr->p1 * 500);

			break;
		    case 2:
			t_ptr->p1 = randint(2);
			t_ptr->flags |= 0x00000010;
			strcat(t_ptr->name, " of Wisdom (%P1)");
			t_ptr->cost += (t_ptr->p1 * 500);

			break;
		    case 3:
			t_ptr->p1 = 1 + randint(4);
			t_ptr->flags |= 0x40000000;
			strcat(t_ptr->name, " of Infra-Visition (%P1)");
			t_ptr->cost += (t_ptr->p1 * 250);

			break;
		    }

		    break;
		case 6:
		case 7:
		case 8:
		    switch(randint(6)) {
		    case 1:
			t_ptr->p1 = randint(3);
			t_ptr->flags |= 0x00800007;
			strcat(t_ptr->name, " of Might (%P1)");
			t_ptr->cost += (1000 + (t_ptr->p1 * 500));

			break;
		    case 2:
			t_ptr->p1 = randint(3);
			t_ptr->flags |= 0x00000030;
			strcat(t_ptr->name, " of Lordliness (%P1)");
			t_ptr->cost += (1000 + (t_ptr->p1 * 500));
			
			break;
		    case 3:
			t_ptr->p1 = randint(3);
			t_ptr->flags |= 0x01380008;
			strcat(t_ptr->name, " of the Magi (%P1)");
			t_ptr->cost += (3000 + (t_ptr->p1 * 500));

			break;
		    case 4:
			t_ptr->p1 = randint(3);
			t_ptr->flags |= 0x00000020;
			strcat(t_ptr->name, " of Beauty (%P1)");
			t_ptr->cost += 750;

			break;
		    case 5:
			t_ptr->p1 = 1 + randint(4);
			t_ptr->flags |= 0x01000040;
			strcat(t_ptr->name, " of Seeing (%P1)");
			t_ptr->cost += (1000 + (t_ptr->p1 * 100));

			break;
		    case 6:
			t_ptr->flags |= 0x00000800;
			strcat(t_ptr->name, " of Regeneration");
			t_ptr->cost += 1500;

			break;
		    }

		    break;
		}
	    }
	    else if(magik(cursed)) {
		t_ptr->toac = -m_bonus(1, 45, level);
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = 0;

		if(magik(special)) {
		    switch(randint(7)) {
		    case 1:
			t_ptr->p1 = -1;
			t_ptr->flags |= 0x00000008;
			strcat(t_ptr->name, " of Stupidity");

			break;
		    case 2:
			t_ptr->p1 = -1;
			t_ptr->flags |= 0x00000010;
			strcat(t_ptr->name, " of Dullness");

			break;
		    case 3:
			t_ptr->flags |= 0x08000000;
			strcat(t_ptr->name, " of Blindness");

			break;
		    case 4:
			t_ptr->flags |= 0x10000000;
			strcat(t_ptr->name, " of Timidness");

			break;
		    case 5:
			t_ptr->p1 = -1;
			t_ptr->flags |= 0x00000001;
			strcat(t_ptr->name, " of Weakness");

			break;
		    case 6:
			t_ptr->flags |= 0x00000400;
			strcat(t_ptr->name, " of Teleportation");

			break;
		    case 7:
			t_ptr->p1 = -1;
			t_ptr->flags |= 0x00000020;
			strcat(t_ptr->name, " of Ugliness");

			break;
		    }
		}
		
		t_ptr->p1 = t_ptr->p1 * randint(5);
	    }
	}

	break;
    case 45: /* Rings */
	switch(t_ptr->subval) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	    if(magik(cursed)) {
		t_ptr->p1 = -m_bonus(1, 20, level);
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }
	    else {
		t_ptr->p1 = m_bonus(1, 10, level);
		t_ptr->cost += (t_ptr->p1 * 100);
	    }

	    break;
	case 7:
	    if(magik(cursed)) {
		t_ptr->p1 = -randint(3);
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }
	    else {
		t_ptr->p1 = 1;
	    }

	    break;
	case 8:
	    t_ptr->p1 = 5 * m_bonus(1, 20, level);
	    t_ptr->cost += (t_ptr->p1 * 100);

	    break;
	case 22: /* Increase damage */
	    t_ptr->todam = m_bonus(1, 20, level);
	    t_ptr->cost += (t_ptr->p1 * 100);

	    if(magik(cursed)) {
		t_ptr->todam = -t_ptr->todam;
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }

	    break;
	case 23: /* Increate to-hit */
	    t_ptr->tohit = m_bonus(1, 20, level);
	    t_ptr->cost += (t_ptr->tohit * 100);

	    if(magik(cursed)) {
		t_ptr->tohit = -t_ptr->tohit;
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }

	    break;
	case 24: /* Protection */
	    t_ptr->toac = m_bonus(1, 20, level);
	    t_ptr->cost += (t_ptr->toac * 100);

	    if(magik(cursed)) {
		t_ptr->toac = -t_ptr->toac;
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }

	    break;
	case 33: /* Slaying */
	    t_ptr->todam = m_bonus(1, 25, level);
	    t_ptr->tohit = m_bonus(1, 25, level);
	    t_ptr->cost += ((t_ptr->tohit + t_ptr->todam) * 100);

	    if(magik(cursed)) {
		t_ptr->tohit = -t_ptr->tohit;
		t_ptr->todam = -t_ptr->todam;
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }

	    break;
	default:

	    break;
	}

	break;
    case 40: /* Amulets */
	switch(t_ptr->subval) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	    if(magik(cursed)) {
		t_ptr->p1 = -m_bonus(1, 20, level);
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = -t_ptr->cost;
	    }
	    else {
		t_ptr->p1 = m_bonus(1, 10, level);
		t_ptr->cost += (t_ptr->p1 * 100);
	    }

	    break;
	case 7:
	    t_ptr->p1 = 5 * m_bonus(1, 25, level);

	    if(magik(cursed)) {
		t_ptr->p1 = -t_ptr->p1;
		t_ptr->cost = -t_ptr->cost;
		t_ptr->flags |= 0x80000000;
	    }
	    else {
		t_ptr->cost += (20 * t_ptr->p1);
	    }

	    break;
	default:

	    break;
	}

	break;
    case 15: /* Lamps and torches */
	/* 
	 * Subval should be even for store, odd for dungeon. Dungeon found ones
	 * will be partially charged.
	 */

	if((t_ptr->subval % 2) == 1) {
	    t_ptr->p1 = randint(t_ptr->p1);
	}

	break;
    case 65: /* Wands */
	switch(t_ptr->subval) {
	case 1:
	    t_ptr->p1 = randint(10) + 6;

	    break;
	case 2:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 3:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 4:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 5:
	    t_ptr->p1 = randint(4) + 3;

	    break;
	case 6:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 7:
	    t_ptr->p1 = randint(20) + 12;

	    break;
	case 8:
	    t_ptr->p1 = randint(20) + 12;

	    break;
	case 9:
	    t_ptr->p1 = randint(10) + 6;

	    break;
	case 10:
	    t_ptr->p1 = randint(12) + 6;

	    break;
	case 11:
	    t_ptr->p1 = randint(10) + 12;

	    break;
	case 12:
	    t_ptr->p1 = randint(3) + 3;

	    break;
	case 13:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 14:
	    t_ptr->p1 = randint(10) + 6;

	    break;
	case 15:
	    t_ptr->p1 = randint(5) + 3;

	    break;
	case 16:
	    t_ptr->p1 = randint(5) + 3;

	    break;
	case 17:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 18:
	    t_ptr->p1 = randint(5) + 4;

	    break;
	case 19:
	    t_ptr->p1 = randint(8) + 4;

	    break;
	case 20:
	    t_ptr->p1 = randint(6) + 2;

	    break;
	case 21:
	    t_ptr->p1 = randint(4) + 2;

	    break;
	case 22:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 23:
	    t_ptr->p1 = randint(5) + 2;

	    break;
	case 24:
	    t_ptr->p1 = randint(12) + 12;

	    break;
	default:

	    break;
	}

	break;
    case 55: /* Staffs */
	switch(t_ptr->subval) {
	case 1:
	    t_ptr->p1 = randint(20) + 12;

	    break;
	case 2:
	    t_ptr->p1 = randint(8) + 6;

	    break;
	case 3:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 4:
	    t_ptr->p1 = randint(20) + 12;

	    break;
	case 5:
	    t_ptr->p1 = randint(15) + 6;

	    break;
	case 6:
	    t_ptr->p1 = randint(4) + 5;

	    break;
	case 7:
	    t_ptr->p1 = randint(5) + 3;

	    break;
	case 8:
	    t_ptr->p1 = randint(3) + 1;

	    break;
	case 9:
	    t_ptr->p1 = randint(3) + 1;

	    break;
	case 10:
	    t_ptr->p1 = randint(3) + 1;

	    break;
	case 11:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 12:
	    t_ptr->p1 = randint(10) + 12;

	    break;
	case 13:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 14:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 15:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 16:
	    t_ptr->p1 = randint(10) + 12;

	    break;
	case 17:
	    t_ptr->p1 = randint(3) + 4;

	    break;
	case 18:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 19:
	    t_ptr->p1 = randint(5) + 6;

	    break;
	case 20:
	    t_ptr->p1 = randint(3) + 4;

	    break;
	case 21:
	    t_ptr->p1 = randint(10) + 12;

	    break;
	case 22:
	    t_ptr->p1 = randint(3) + 4;

	    break;
	case 23:
	    t_ptr->p1 = randint(3) + 4;

	    break;
	case 24:
	    t_ptr->p1 = randint(3) + 1;

	    break;
	case 25:
	    t_ptr->p1 = randint(10) + 6;

	    break;
	default:

	    break;
	}

	break;
    case 32: /* Cloaks */
	if(magik(chance)) {
	    if(magik(special)) {
		switch(randint(2)) {
		case 1:
		    strcat(t_ptr->name, " of Protection");
		    t_ptr->toac = m_bonus(2, 40, level);
		    t_ptr->cost += 250;

		    break;
		case 2:
		    t_ptr->toac = m_bonus(1, 20, level);
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= 0x00000100;
		    strcat(t_ptr->name, " of Stealth (%P1)");
		    t_ptr->cost += 500;

		    break;
		}
	    }
	    else {
		t_ptr->toac = m_bonus(1, 20, level);
	    }
	}
	else if(magik(cursed)) {
	    switch(randint(3)) {
	    case 1:
		t_ptr->flags |= 0x80000200;
		strcat(t_ptr->name, " of Irritation");
		t_ptr->ac = 0;
		t_ptr->toac = -m_bonus(1, 10, level);
		t_ptr->tohit = -m_bonus(1, 10, level);
		t_ptr->todam = -m_bonus(1, 10, level);
		t_ptr->cost = 0;

		break;
	    case 2:
		t_ptr->flags |= 0x80000000;
		strcat(t_ptr->name, " of Vulnerability");
		t_ptr->ac = 0;
		t_ptr->toac = -m_bonus(10, 100, level + 50);
		t_ptr->cost = 0;

		break;
	    case 3:
		t_ptr->flags |= 0x80000000;
		strcat(t_ptr->name, " of Enveloping");
		t_ptr->toac = -m_bonus(1, 10, level);
		t_ptr->tohit = -m_bonus(2, 40, level + 10);
		t_ptr->todam = -m_bonus(2, 40, level + 10);
		t_ptr->cost = 0;

		break;
	    }
	}

	break;
    case 2: /* Chests */
	switch(randint(level + 4)) {
	case 1:
	    t_ptr->flags = 0;
	    strcat(t_ptr->name, "^ (Empty)");

	    break;
	case 2:
	    t_ptr->flags |= 0x00000001;
	    strcat(t_ptr->name, "^ (Locked");

	    break;
	case 3:
	case 4:
	    t_ptr->flags |= 0x00000011;
	    strcat(t_ptr->name, "^ (Poison Needle)");

	    break;
	case 5:
	case 6:
	    t_ptr->flags |= 0x00000021;
	    strcat(t_ptr->name, "^ (Poison Needle)");

	    break;
	case 7:
	case 8:
	case 9:
	    t_ptr->flags |= 0x00000041;
	    strcat(t_ptr->name, "^ (Gas Trap)");

	    break;
	case 10:
	case 11:
	    t_ptr->flags |= 0x00000081;
	    strcat(t_ptr->name, "^ (Exlposion Device)");

	    break;
	case 12:
	case 13:
	case 14:
	    t_ptr->flags |= 0x00000101;
	    strcat(t_ptr->name, "^ (Summoning Runes)");

	    break;
	case 15:
	case 16:
	case 17:
	    t_ptr->flags |= 0x00000071;
	    strcat(t_ptr->name, "^ (Multiple Traps)");

	    break;
	default:
	    t_ptr->flags |= 0x00000181;
	    strcat(t_ptr->name, "^ (Multiple Traps)");

	    break;
	}

	break;
    case 10:
    case 11:
    case 12:
    case 13:
	/* Arrows, bolts, ammo and spikes */

	if((t_ptr->tval == 11) || (t_ptr->tval == 12)) {
	    if(magik(chance)) {
		t_ptr->tohit = m_bonus(1, 35, level);
		t_ptr->todam = m_bonus(1, 35, level);

		if(magik(special)) {
		    switch(t_ptr->tval) {
		    case 11:
		    case 12:
			switch(randint(10)) {
			case 1:
			case 2:
			case 3:
			    strcat(t_ptr->name, " of Slaying");
			    t_ptr->tohit += 5;
			    t_ptr->todam += 5;
			    t_ptr->cost += 20;

			    break;
			case 4:
			case 5:
			    t_ptr->flags |= 0x00040000;
			    t_ptr->tohit += 2;
			    t_ptr->todam += 4;
			    strcat(t_ptr->name, " of Fire");
			    t_ptr->cost += 25;

			    break;
			case 6:
			case 7:
			    t_ptr->flags |= 0x00008000;
			    t_ptr->tohit += 3;
			    t_ptr->todam += 3;
			    strcat(t_ptr->name, " of Slay Evil");
			    t_ptr->cost += 25;

			    break;
			case 8:
			case 9:
			    t_ptr->flags |= 0x01004000;
			    t_ptr->tohit += 2;
			    t_ptr->todam += 2;
			    strcat(t_ptr->name, " of Slay Monster");
			    t_ptr->cost += 30;

			    break;
			case 10:
			    t_ptr->flags |= 0x00002000;
			    t_ptr->tohit += 10;
			    t_ptr->todam += 10;
			    strcat(t_ptr->name, " of Dragon Slaying");
			    t_ptr->cost += 35;

			    break;
			}

		    default:

			break;
		    }
		}
	    }
	    else if(magik(cursed)) {
		t_ptr->tohit = -m_bonus(5, 55, level);
		t_ptr->todam = -m_bonus(5, 55, level);
		t_ptr->flags |= 0x80000000;
		t_ptr->cost = 0;
	    }
	}

	t_ptr->number = 0;

	for(i = 0; i < 7; ++i) {
	    t_ptr->number += randint(6);
	}

	++missile_ctr;

	if(missile_ctr > 65000) {
	    missile_ctr = 1;
	}

	t_ptr->subval = missile_ctr + 512;

	break;
    default:

	break;
    }
}
