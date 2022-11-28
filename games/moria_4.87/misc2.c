#include "misc2.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"
#include "constants.h"
#include "externs.h"
#include "io.h"
#include "misc1.h"
#include "types.h"


/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

#if defined(ultrix) || defined(sun) || defined(USG)
int getuid();

#else
uid_t getuid();

#endif

/* Places a particular trap at location y,x    -RAK- */
void place_trap(int y, int x, int typ, int subval)
{
    int cur_pos;
    treasure_type cur_trap;

    if(typ == 1) {
	cur_trap = trap_lista[subval];
    }
    else {
	cur_trap = trap_listb[subval];
    }

    popt(&cur_pos);
    cave[y][x].tptr = cur_pos;
    t_list[cur_pos] = cur_trap;
}

/* Places rubble at location y,x    -RAK- */
void place_rubble(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    cave_ptr->fopen = FALSE;
    t_list[cur_pos] = rubble;
}

void place_open_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[0];
    cave_ptr->fval = corr_floor3.ftval;
    cave_ptr->fopen = TRUE;
}

void place_broken_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[0];
    cave_ptr->fval = corr_floor3.ftval;
    cave_ptr->fopen = TRUE;
    t_list[cur_pos].p1 = 1;
}

void place_closed_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[1];
    cave_ptr->fval = corr_floor3.ftval;
    cave_ptr->fopen = FALSE;
}

void place_locked_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[1];
    cave_ptr->fval = corr_floor3.ftval;
    cave_ptr->fopen = FALSE;
    t_list[cur_pos].p1 = randint(10) + 10;
}

void place_stuck_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[1];
    cave_ptr->fval = corr_floor3.ftval;
    cave_ptr->fopen = FALSE;
    t_list[cur_pos].p1 = -randint(10) - 10;
}

void place_secret_door(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    popt(&cur_pos);
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = door_list[2];
    cave_ptr->fval = corr_floor4.ftval;
    cave_ptr->fopen = FALSE;
}

void place_door(int y, int x)
{
    switch(randint(3)) {
    case 1:
	switch(randint(4)) {
	case 1:
	    place_broken_door(y, x);

	    break;
	default:
	    place_open_door(y, x);

	    break;
	}

	break;
    case 2:
	switch(randint(12)) {
	case 1:
	case 2:
	    place_locked_door(y, x);

	    break;
	case 3:
	    place_stuck_door(y, x);

	    break;
	default:
	    place_closed_door(y, x);

	    break;
	}

	break;
    case 3:
	place_secret_door(y, x);

	break;
    }
}

/* Place an up staircase at given y,x    -RAK- */
void place_up_stairs(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    cave_ptr = &cave[y][x];

    if(cave_ptr->tptr != 0) {
	pusht((int)cave_ptr->tptr);
	cave_ptr->tptr = 0;
	cave_ptr->fopen = TRUE;
    }

    popt(&cur_pos);
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = up_stair;
}

/* Place a down staircase at given y,x    -RAK- */
void place_down_stairs(int y, int x)
{
    int cur_pos;
    cave_type *cave_ptr;

    cave_ptr = &cave[y][x];

    if(cave_ptr->tptr != 0) {
	pusht((int)cave_ptr->tptr);
	cave_ptr->tptr = 0;
	cave_ptr->fopen = TRUE;
    }

    popt(&cur_pos);
    cave_ptr->tptr = cur_pos;
    t_list[cur_pos] = down_stair;
}

/* Places a staircase 1 == up, 2 == down    -RAK- */
void place_stairs(int typ, int num, int walls)
{
    cave_type *cave_ptr;
    int i;
    int j;
    int y1;
    int x1;
    int y2;
    int x2;
    int flag;

    for(i = 0; i < num; ++i) {
	flag = FALSE;

	j = 0;

	y1 = randint(cur_height - 12) - 1;
	x1 = randint(cur_width - 12) - 1;
	y2 = y1 + 12;
	x2 = x1 + 12;

	cave_ptr = &cave[y1][x1];

	if((cave_ptr->fval == 1)
	   || (cave_ptr->fval == 2)
	   || (cave_ptr->fval == 4)) {
	    if(cave_ptr->tptr == 0) {
		if(next_to4(y1, x1, 10, 11, 12) >= walls) {
		    flag = TRUE;

		    switch(typ) {
		    case 1:
			place_up_stairs(y1, x1);

			break;
		    case 2:
			place_down_stairs(y1, x1);

			break;
		    }
		}
	    }
	}

	++x1;

	while((x1 != x2) && !flag) {
	    cave_ptr = &cave[y1][x1];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)) {
		if(cave_ptr->tptr == 0) {
		    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			flag = TRUE;

			switch(typ) {
			case 1:
			    place_up_stairs(y1, x1);

			    break;
			case 2:
			    place_down_stairs(y1, x1);

			    break;
			}
		    }
		}
	    }

	    ++x1;
	}

	x1 = x2 - 12;
	++y1;

	while((y1 != y2) && !flag) {
	    cave_ptr = &cave[y1][x1];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)) {
		if(cave_ptr->tptr == 0) {
		    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			flag = TRUE;

			switch(typ) {
			case 1:
			    place_up_stairs(y1, x1);

			    break;
			case 2:
			    place_down_stairs(y1, x1);

			    break;
			}
		    }
		}
	    }

	    ++x1;

	    while((x1 != x2) && !flag) {
		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;
	    }

	    x1 = x2 - 12;
	    ++y1;
	}

	++j;

	while((j <= 30) && !flag) {
	    y1 = randint(cur_height - 12) - 1;
	    x1 = randint(cur_width - 12) - 1;
	    y2 = y1 + 12;
	    x2 = x1 + 12;

	    cave_ptr = &cave[y1][x1];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)) {
		if(cave_ptr->tptr == 0) {
		    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			flag = TRUE;

			switch(typ) {
			case 1:
			    place_up_stairs(y1, x1);

			    break;
			case 2:
			    place_down_stairs(y1, x1);

			    break;
			}
		    }
		}
	    }

	    ++x1;

	    while((x1 != x2) && !flag) {
		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;
	    }

	    x1 = x2 - 12;
	    ++y1;

	    while((y1 != y2) && !flag) {
		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;

		while((x1 != x2) && !flag) {
		    cave_ptr = &cave[y1][x1];

		    if((cave_ptr->fval == 1)
		       || (cave_ptr->fval == 2)
		       || (cave_ptr->fval == 4)) {
			if(cave_ptr->tptr == 0) {
			    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
				flag = TRUE;

				switch(typ) {
				case 1:
				    place_up_stairs(y1, x1);

				    break;
				case 2:
				    place_down_stairs(y1, x1);

				    break;
				}
			    }
			}
		    }

		    ++x1;
		}

		x1 = x2 - 12;
		++y1;
	    }

	    ++j;
	}

	--walls;

	while(!flag) {
	    j = 0;

	    y1 = randint(cur_height - 12) - 1;
	    x1 = randint(cur_width - 12) - 1;
	    y2 = y1 + 12;
	    x2 = x1 + 12;

	    cave_ptr = &cave[y1][x1];

	    if((cave_ptr->fval == 1)
	       || (cave_ptr->fval == 2)
	       || (cave_ptr->fval == 4)) {
		if(cave_ptr->tptr == 0) {
		    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			flag = TRUE;

			switch(typ) {
			case 1:
			    place_up_stairs(y1, x1);

			    break;
			case 2:
			    place_down_stairs(y1, x1);

			    break;
			}
		    }
		}
	    }

	    ++x1;

	    while((x1 != x2) && !flag) {
		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;
	    }

	    x1 = x2 - 12;
	    ++y1;

	    while((y1 != y2) && !flag) {
		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;

		while((x1 != x2) && !flag) {
		    cave_ptr = &cave[y1][x1];

		    if((cave_ptr->fval == 1)
		       || (cave_ptr->fval == 2)
		       || (cave_ptr->fval == 4)) {
			if(cave_ptr->tptr == 0) {
			    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
				flag = TRUE;

				switch(typ) {
				case 1:
				    place_up_stairs(y1, x1);

				    break;
				case 2:
				    place_down_stairs(y1, x1);

				    break;
				}
			    }
			}
		    }

		    ++x1;
		}

		x1 = x2 - 12;
		++y1;
	    }

	    ++j;

	    while((j <= 30) && !flag) {
		y1 = randint(cur_height - 12) - 1;
		x1 = randint(cur_width - 12) - 1;
		y2 = y1 + 12;
		x2 = x1 + 12;

		cave_ptr = &cave[y1][x1];

		if((cave_ptr->fval == 1)
		   || (cave_ptr->fval == 2)
		   || (cave_ptr->fval == 4)) {
		    if(cave_ptr->tptr == 0) {
			if(next_to4(y1, x1, 10, 11, 12) >= walls) {
			    flag = TRUE;

			    switch(typ) {
			    case 1:
				place_up_stairs(y1, x1);

				break;
			    case 2:
				place_down_stairs(y1, x1);

				break;
			    }
			}
		    }
		}

		++x1;

		while((x1 != x2) && !flag) {
		    cave_ptr = &cave[y1][x1];

		    if((cave_ptr->fval == 1)
		       || (cave_ptr->fval == 2)
		       || (cave_ptr->fval == 4)) {
			if(cave_ptr->tptr == 0) {
			    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
				flag = TRUE;

				switch(typ) {
				case 1:
				    place_up_stairs(y1, x1);

				    break;
				case 2:
				    place_down_stairs(y1, x1);

				    break;
				}
			    }
			}
		    }

		    ++x1;
		}

		x1 = x2 - 12;
		++y1;

		while((y1 != y2) && !flag) {
		    cave_ptr = &cave[y1][x1];

		    if((cave_ptr->fval == 1)
		       || (cave_ptr->fval == 2)
		       || (cave_ptr->fval == 4)) {
			if(cave_ptr->tptr == 0) {
			    if(next_to4(y1, x1, 10, 11, 12) >= walls) {
				flag = TRUE;

				switch(typ) {
				case 1:
				    place_up_stairs(y1, x1);

				    break;
				case 2:
				    place_down_stairs(y1, x1);

				    break;
				}
			    }
			}
		    }

		    ++x1;

		    while((x1 != x2) && !flag) {
			cave_ptr = &cave[y1][x1];

			if((cave_ptr->fval == 1)
			   || (cave_ptr->fval == 2)
			   || (cave_ptr->fval == 4)) {
			    if(cave_ptr->tptr == 0) {
				if(next_to4(y1, x1, 10, 11, 12) >= walls) {
				    flag = TRUE;

				    switch(typ) {
				    case 1:
					place_up_stairs(y1, x1);

					break;
				    case 2:
					place_down_stairs(y1, x1);

					break;
				    }
				}
			    }
			}

			++x1;
		    }

		    x1 = x2 - 12;
		    ++y1;
		}

		++ j;
	    }

	    --walls;
	}
    }
}

/* Places a treasure (Gold or Gems) at given row, column    -RAK- */
void place_gold(int y, int x)
{
    int cur_pos;
    int i;
    treasure_type *t_ptr;

    popt(&cur_pos);
    i = ((randint(dun_level + 2) + 2) / 2.0) - 1;

    if(randint(OBJ_GREAT) == 1) {
	i += randint(dun_level + 1);
    }

    if(i >= MAX_GOLD) {
	i = MAX_GOLD - 1;
    }

    cave[y][x].tptr = cur_pos;
    t_list[cur_pos] = gold_list[i];
    t_ptr = &t_list[cur_pos];
    t_ptr->cost += randint(8 * t_ptr->cost);
}

/* Returns the array number of a random object    -RAK- */
int get_obj_num(int level)
{
    int i;

    if(level > MAX_OBJ_LEVEL) {
	level = MAX_OBJ_LEVEL;
    }

    if(randint(OBJ_GREAT) == 1) {
	level = MAX_OBJ_LEVEL;
    }

    if(level == 0) {
	i = randint(t_level[0]) - 1;
    }
    else {
	i = randint(t_level[level]) - 1;
    }

    return i;
}

/* Place an object at given row,column coordinate    -RAK- */
void place_object(int y, int x)
{
    int cur_pos;
    int tmp;
    treasure_type tr_tmp;

    popt(&cur_pos);
    cave[y][x].tptr = cur_pos;

    /* Split this line to avoid a reported compiler bug */
    tmp = get_obj_num(dun_level);
    tr_tmp = object_list[tmp];
    t_list[cur_pos] = tr_tmp;
    magic_treasure(cur_pos, dun_level);
}

/* Allocates an object for tunnels and rooms    -RAK- */
void alloc_object(int (*alloc_set)(), int typ, int num)
{
    int i;
    int j;
    int k;

    for(k = 0; k < num; ++k) {
	i = randint(cur_height) - 1;
	j = randint(cur_width) - 1;

	while((!(*alloc_set)(cave[i][j].fval))
	      || (cave[i][j].tptr != 0)) {
	    i = randint(cur_height) - 1;
	    j = randint(cur_width) - 1;
	}

	switch(typ) {
	case 1:
	    place_trap(i, j, 1, randint(MAX_TRAPA) - 1);

	    break;
	case 2:
	    place_trap(i, j, 2, randint(MAX_TRAPB) - 1);

	    break;
	case 3:
	    place_rubble(i, j);

	    break;
	case 4:
	    place_gold(i, j);

	    break;
	case 5:
	    place_object(i, j);

	    break;
	}
    }
}

/* Creates objects nearby the coordinates given    -RAK- */
void random_object(int y, int x, int num)
{
    int i;
    int j;
    int k;
    cave_type *cave_ptr;

    i = 0;

    j = y - 3 + randint(5);
    k = x - 4 + randint(7);
    cave_ptr = &cave[j][k];

    if((cave_ptr->fval <= 7) && (cave_ptr->fval >= 1)) {
	if(cave_ptr->tptr == 0) {
	    if(randint(100) < 75) {
		place_object(j, k);
	    }
	    else {
		place_gold(j, k);
	    }

	    i = 9;
	}
    }

    ++i;

    while(i <= 10) {
	j = y - 3 + randint(5);
	k = x - 4 + randint(7);
	cave_ptr = &cave[j][k];

	if((cave_ptr->fval <= 7) && (cave_ptr->fval >= 1)) {
	    if(cave_ptr->tptr == 0) {
		if(randint(100) < 75) {
		    place_object(j, k);
		}
		else {
		    place_gold(j, k);
		}

		i = 9;
	    }
	}

	++i;
    }

    --num;

    while(num != 0) {
	i = 0;

	j = y - 3 + randint(5);
	k = x - 4 + randint(7);
	cave_ptr = &cave[j][k];

	if((cave_ptr->fval <= 7) && (cave_ptr->fval >= 1)) {
	    if(cave_ptr->tptr == 0) {
		if(randint(100) < 75) {
		    place_object(j, k);
		}
		else {
		    place_gold(j, k);
		}

		i = 9;
	    }
	}
	
	++i;

	while(i <= 10) {
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = &cave[j][k];

	    if((cave_ptr->fval <= 7) && (cave_ptr->fval >= 1)) {
		if(cave_ptr->tptr == 0) {
		    if(randint(100) < 75) {
			place_object(j, k);
		    }
		    else {
			place_gold(j, k);
		    }

		    i = 9;
		}
	    }

	    ++i;
	}

	--num;
    }
}

/* Converts stat num into string    -RAK- */
void cnv_stat(byteint stat, char *out_val)
{
    vtype tmp_str;
    int part1;
    int part2;

    if(stat > 18) {
	part1 = 18;
	part2 = stat - 18;

	if(part2 == 100) {
	    strcpy(tmp_str, "18/100");
	}
	else if(part2 < 10) {
	    sprintf(tmp_str, " %2d/0%d", part1, part2);
	}
	else {
	    sprintf(tmp_str, " %2d/%2d", part1, part2);
	}
    }
    else {
	sprintf(tmp_str, "%6d", stat);
    }

    if(strlen(tmp_str) < 6) {
	strcpy(tmp_str, pad(tmp_str, " ", 6));
    }

    strcpy(out_val, tmp_str);
}

/* Print character stat in given row,column    -RAK- */
void prt_stat(vtype stat_name, byteint stat, int row, int column)
{
    stat_type out_val1;
    vtype out_val2;

    cnv_stat(stat, out_val1);
    strcpy(out_val2, stat_name);
    strcat(out_val2, out_val1);
    put_buffer(out_val2, row, column);
}

/*
 * Print character info in given row,column    -RAK-
 *     The longest title is 13 characters, so only pad to 13
 */
void prt_field(vtype info, int row, int column)
{
    put_buffer(pad(info, " ", 13), row, column);
}

/* Print number with header at given row,column    -RAK- */
void prt_num(vtype header, int num, int row, int column)
{
    vtype out_val;

    sprintf(out_val, "%s%6d", header, num);
    put_buffer(out_val, row, column);
}

/* Adjustment for wisdom    -RAK- */
int wis_adj()
{
    if(py.stats.cwis > 117) {
	return 7;
    }
    else if(py.stats.cwis > 107) {
	return 6;
    }
    else if(py.stats.cwis > 87) {
	return 5;
    }
    else if(py.stats.cwis > 67) {
	return 4;
    }
    else if(py.stats.cwis > 17) {
	return 3;
    }
    else if(py.stats.cwis > 14) {
	return 2;
    }
    else if(py.stats.cwis > 7) {
	return 1;
    }
    else {
	return 0;
    }
}

/* Adjustment for intelligence    -JWT- */
int int_adj()
{
    if(py.stats.cint > 117) {
	return 7;
    }
    else if(py.stats.cint > 107) {
	return 6;
    }
    else if(py.stats.cint > 87) {
	return 5;
    }
    else if(py.stats.cint > 67) {
	return 4;
    }
    else if(py.stats.cint > 17) {
	return 3;
    }
    else if(py.stats.cint > 14) {
	return 2;
    }
    else if(py.stats.cint > 7) {
	return 1;
    }
    else {
	return 0;
    }
}

/*
 * Adjustment for charisma    -RAK-
 *     Percent decrease or increase in price of goods
 */
double chr_adj()
{
    if(py.stats.cchr > 117) {
	return -0.10;
    }
    else if(py.stats.cchr > 107) {
	return -0.08;
    }
    else if(py.stats.cchr > 87) {
	return -0.06;
    }
    else if(py.stats.cchr > 67) {
	return -0.04;
    }
    else if(py.stats.cchr > 18) {
	return -0.02;
    }
    else {
	switch(py.stats.cchr) {
	case 18:

	    return 0.00;
	case 17:

	    return 0.01;
	case 16:

	    return 0.02;
	case 15:

	    return 0.03;
	case 14:

	    return 0.04;
	case 13:

	    return 0.06;
	case 12:

	    return 0.08;
	case 11:

	    return 0.10;
	case 10:

	    return 0.12;
	case 9:

	    return 0.14;
	case 8:

	    return 0.16;
	case 7:

	    return 0.18;
	case 6:

	    return 0.20;
	case 5:

	    return 0.22;
	case 4:

	    return 0.24;
	case 3:

	    return 0.25;
	default:
	    /* Error trap */

	    return 0.00;
	}
    }
}

/* Returns a character's adjustment to hit points    -JWT- */
int con_adj()
{
    if(py.stats.ccon == 3) {
	return -4;
    }
    else if(py.stats.ccon == 4) {
	return -3;
    }
    else if(py.stats.ccon == 5) {
	return -2;
    }
    else if(py.stats.ccon == 6) {
	return -1;
    }
    else if(py.stats.ccon < 17) {
	return 0;
    }
    else if(py.stats.ccon == 17) {
	return 1;
    }
    else if(py.stats.ccon < 94) {
	return 2;
    }
    else if(py.stats.ccon < 117) {
	return 3;
    }
    else {
	return 4;
    }
}

/* Calculates hit points for each level that is gained.    -RAK- */
int get_hitdie()
{
    int hitpoints;

    hitpoints = randint((int)py.misc.hitdie) + con_adj();

    /* Always give 'em at least one point */
    if(hitpoints < 1) {
	hitpoints = 1;
    }

    return hitpoints;
}

/* Prints titel of character    -RAK- */
void prt_title()
{
    prt_field(py.misc.title, 4, stat_column);
}

/* Prints strength    -RAK- */
void prt_strength()
{
    prt_strength("\0", py.stats.cstr, 6, stat_column + 6);
}

/* Prints intelligence    -RAK- */
void prt_intelligence()
{
    prt_stat("\0", py.stats.cint, 7, stat_column + 6);
}

/* Prints wisdom    -RAK- */
void prt_wisdom()
{
    prt_stat("\0", py.stats.cwis, 8, stat_column + 6);
}

/* Prints dexterity    -RAK- */
void prt_dexterity()
{
    prt_stat("\0", py.stats.cdex, 9, stat_column + 6);
}

/* Prints constitution    -RAK- */
void prt_constitution()
{
    prt_stat("\0", py.stats.ccon, 10, stat_column + 6);
}

/* Prints charisma    -RAK- */
void prt_charisma()
{
    prt_stat("\0", py.stats.cchr, 11, stat_column + 6);
}

/* Prints level    -RAK- */
void prt_level()
{
    prt_num("\0", (int)py.misc.lev, 13, stat_column + 6);
}

/* Prints player's current mana points (a real number...)    -RAK- */
void prt_cmana()
{
    prt_num("\0", (int)py.misc.cmana, 15, stat_column + 6);
}

/* Prints max hit points    -RAK- */
void prt_mhp()
{
    prt_num("\0", py.misc.mhp, 16, stat_column + 6);
}

/* Prints player's current hit points (a real number...)    -RAK- */
void prt_chp()
{
    prt_num("\0", (int)py.misc.chp, 17, stat_column + 6);
}

/* Prints current AC    -RAK- */
void prt_pac()
{
    prt_num("\0", py.misc.dis_ac, 19, stat_column + 6);
}

/* Prints current gold    -RAK- */
void prt_gold()
{
    prt_num("\0", py.misc.au, 20, stat_column + 6);
}

/* Prints depth in stat area    -RAK- */
void prt_depth()
{
    vtype depths;
    int depth;

    depth = dun_level * 50;

    if(depth == 0) {
	strcpy(depths, "Town level");
    }
    else {
	sprintf(depths, "Depth: %d (feet)", depth);
    }

    prt(depths, 23, 60);
}

/* Prints status of hunger    -RAK- */
void prt_hunger()
{
    if(py.flags.status & 0x000002) {
	put_buffer("Weak    ", 23, 0);
    }
    else if(py.flags.status & 0x000001) {
	put_buffer("Hungry  ", 23, 0);
    }
    else {
	put_buffer("        ", 23, 0);
    }
}

/* Prints blind status    -RAK- */
void prt_blind()
{
    if(py.flags.status & 0x000004) {
	put_buffer("Blind  ", 23, 8);
    }
    else {
	put_buffer("       ", 23, 8);
    }
}

/* Prints confusion status    -RAK- */
void prt_confused()
{
    if(py.flags.status & 0x000008) {
	put_buffer("Confused  ", 23, 15);
    }
    else {
	put_buffer("          ", 23, 15);
    }
}

/* Prints fear status    -RAK- */
void prt_afraid()
{
    if(py.flags.status & 0x000010) {
	put_buffer("Afraid  ", 23, 25);
    }
    else {
	put_buffer("        ", 23, 25);
    }
}

/* Prints poisoned status    -RAK- */
void prt_poisoned()
{
    if(py.flags.status & 0x000020) {
	put_buffer("Poisoned  ", 23, 33);
    }
    else {
	put_buffer("          ", 23, 33);
    }
}

/* Prints searching status    -RAK- */
void prt_search()
{
    if(py.flags.status & 0x000100) {
	put_buffer("Searching  ", 23, 43);
    }
    else {
	put_buffer("           ", 23, 43);
    }
}

/* Prints resting status    -RAK- */
void prt_rest()
{
    if(py.flags.status & 0x000200) {
	put_buffer("Resting    ", 23, 43);
    }
    else {
	put_buffer("           ", 23, 43);
    }
}

/* Prints winner status on display    -RAK- */
void prt_winner()
{
    put_buffer("*Winner*", 22, 0);
}

/* Increases a stat by one randomized level    -RAK- */
byteint in_statp(byteint stat)
{
    if(stat < 18) {
	++stat;
    }
    else if(stat < 88) {
	stat += randint(25);
    }
    else if(stat < 108) {
	stat += randint(10);
    }
    else {
	++stat;
    }

    if(stat > 118) {
	stat = 118;
    }

    return stat;
}

/* Decreases a stat by one randomized level    -RAK- */
byteint de_statp(byteint stat)
{
    if(stat < 19) {
	--stat;
    }
    else if(stat < 109) {
	stat += (-randint(10) - 5);

	if(stat < 18) {
	    stat = 18;
	}
    }
    else {
	stat -= randint(3);
    }

    if(stat < 3) {
	stat = 3;
    }

    return stat;
}

/* Increases a stat by one true level    -RAK- */
byteint in_statt(byteint stat)
{
    if(stat < 18) {
	++stat;
    }
    else {
	stat += 10;

	if(stat > 118) {
	    stat = 118;
	}
    }

    return stat;
}

/* Decreases a stat by true level    -RAK- */
byteint de_statt(byteint stat)
{
    if(stat > 27) {
	stat -= 10;
    }
    else if(stat > 18) {
	stat = 18;
    }
    else {
	--stat;

	if(stat < 3) {
	    stat = 3;
	}
    }

    return stat;
}

/* Returns a character's adjustment to hit.    -JWT- */
int tohit_adj()
{
    int total;
    struct stats *s_ptr;

    s_ptr = &py.stats;

    if(s_ptr->cdex < 4) {
	total = -1;
    }
    else if(s_ptr->cdex < 6) {
	total = -2;
    }
    else if(s_ptr->cdex < 8) {
	total = -1;
    }
    else if(s_ptr->cdex < 16) {
	total = 0;
    }
    else if(s_ptr->cdex < 17) {
	total = 1;
    }
    else if(s_ptr->cdex < 18) {
	total = 2;
    }
    else if(s_ptr->cdex < 69) {
	total = 3;
    }
    else if(s_ptr->cdex < 118) {
	total = 4;
    }
    else {
	total = 5;
    }

    if(s_ptr->cstr < 4) {
	total -= 3;
    }
    else if(s_ptr->cstr < 5) {
	total -= 2;
    }
    else if(s_ptr->cstr < 7) {
	total -= 1;
    }
    else if(s_ptr->cstr < 18) {
	total -= 0;
    }
    else if(s_ptr->cstr < 94) {
	total += 1;
    }
    else if(s_ptr->cstr < 109) {
	total += 2;
    }
    else if(s_ptr->cstr < 117) {
	total += 3;
    }
    else {
	total += 4;
    }

    return total;
}

/* Returns a character's adjustment to armor class    -JWT- */
int toac_adj()
{
    struct stats *s_ptr;

    s_ptr = &py.stats;

    if(s_ptr->cdex < 4) {
	return -4;
    }
    else if(s_ptr->cdex == 4) {
	return -3;
    }
    else if(s_ptr->cdex == 5) {
	return -2;
    }
    else if(s_ptr->cdex == 6) {
	return -1;
    }
    else if(s_ptr->cdex < 15) {
	return 0;
    }
    else if(s_ptr->cdex < 18) {
	return 1;
    }
    else if(s_ptr->cdex < 59) {
	return 2;
    }
    else if(s_ptr->cdex < 94) {
	return 3;
    }
    else if(s_ptr->cdex < 117) {
	return 4;
    }
    else {
	return 5;
    }
}

/* Returns a character's adjustment to disarm    -RAK- */
int todis_adj()
{
    struct stats *s_ptr;

    s_ptr = &py.stats;

    if(s_ptr->cdex == 3) {
	return -8;
    }
    else if(s_ptr->cdex == 4) {
	return -6;
    }
    else if(s_ptr->cdex == 5) {
	return -4;
    }
    else if(s_ptr->cdex == 6) {
	return -2;
    }
    else if(s_ptr->cdex == 7) {
	return -1;
    }
    else if(s_ptr->cdex < 13) {
	return 0;
    }
    else if(s_ptr->cdex < 16) {
	return 1;
    }
    else if(s_ptr->cdex < 18) {
	return 2;
    }
    else if(s_ptr->cdex < 59) {
	return 4;
    }
    else if(s_ptr->cdex < 94) {
	return 5;
    }
    else if(s_ptr->cdex < 117) {
	return 6;
    }
    else {
	return 8;
    }
}

/* Returns a character's adjustment to damage    -JWT- */
int todam_adj()
{
    struct stats *s_ptr;

    s_ptr = &py.stats;

    if(s_ptr->cstr < 4) {
	return -2;
    }
    else if(s_ptr->cstr < 5) {
	return -1;
    }
    else if(s_ptr->cstr < 16) {
	return 0;
    }
    else if(s_ptr->cstr < 17) {
	return 1;
    }
    else if(s_ptr->cstr < 18) {
	return 2;
    }
    else if(s_ptr->cstr < 94) {
	return 3;
    }
    else if(s_ptr->cstr < 109) {
	return 4;
    }
    else if(s_ptr->cstr < 117) {
	return 5;
    }
    else {
	return 6;
    }
}

/* Prints character-screen information    -RAK- */
void prt_stat_block()
{
    int status;
    struct stats *s_ptr;
    struct misc *m_ptr;

    s_ptr = &py.stats;
    m_ptr = &py.misc;

    prt_field(m_ptr->race, 2, stat_column);
    prt_field(m_ptr->tclass, 3, stat_column);
    prt_field(m_ptr->title, 4, stat_column);
    
    prt_stat("STR : ", s_ptr->cstr, 6, stat_column);
    prt_stat("INT : ", s_ptr->cstr, 7, stat_column);
    prt_stat("WIS : ", s_ptr->cwis, 8, stat_column);
    prt_stat("DEX : ", s_ptr->cdex, 9, stat_column);
    prt_stat("CON : ", s_ptr->ccon, 10, stat_column);
    prt_stat("CHR : ", s_ptr->cchr, 11, stat_column);
    
    prt_num("LEV : ", (int)m_ptr->lev, 13, stat_column);
    prt_num("EXP : ", m_ptr->exp, 14, stat_column);
    prt_num("MANA: ", (int)m_ptr->cmana, 15, stat_column);
    prt_num("MHP : ", m_ptr->mhp, 16, stat_column);
    prt_num("CHP : ", (int)m_ptr->chp, 17, stat_column);
    prt_num("AC  : ", m_ptr->dis_ac, 19, stat_column);
    prt_num("GOLD: ", m_ptr->au, 20, stat_column);

    if(total_winner) {
	prt_winner();
    }

    status = py.flags.status;
    
    if(status & 0x000003) {
	prt_hunger();
    }

    if(status & 0x000004) {
	prt_blind();
    }

    if(status & 0x000008) {
	prt_confused();
    }

    if(status & 0x000010) {
	prt_afraid();
    }

    if(status & 0x000020) {
	prt_poisoned();
    }

    if(status & 0x000100) {
	prt_search();
    }

    if(status & 0x000200) {
	prt_rest();
    }
}

/* Draws entire screen    -RAK- */
void draw_cave()
{
    clear_screen(0, 0);
    prt_stat_block();
    prt_map();
    prt_depth();
}

/* Prints the following information on the screen    -JWT- */
void put_character()
{
    char tmp_str[80];
    struct misc *m_ptr;

    m_ptr = &py.misc;
    clear_screen(0, 0);
    prt(strcat(strcpy(tmp_str, "Name      : "), m_ptr->name), 2, 2);
    prt(strcat(strcpy(tmp_str, "Race      : "), m_ptr->race), 3, 2);
    prt(strcat(strcpy(tmp_str, "Sex       : "), m_ptr->sex), 4, 2);
    prt(strcat(strcpy(tmp_str, "Class     : "), m_ptr->tclass), 5, 2);
}

/* Prints the following information on the screen.    -JWT- */
void put_stats()
{
    struct stats *s_ptr;
    struct misc *m_ptr;

    m_ptr = &py.misc;
    s_ptr = &py.stats;

    prt_stat("STR : ", s_ptr->cstr, 2, 64);
    prt_stat("INT : ", s_ptr->cint, 3, 64);
    prt_stat("WIS : ", s_ptr->cwis, 4, 64);
    prt_stat("DEX : ", s_ptr->cdex, 5, 64);
    prt_stat("CON : ", s_ptr->ccon, 6, 64);
    prt_stat("CHR : ", s_ptr->cchr, 7, 64);

    prt_num("+ To Hit   : ", m_ptr->dis_th, 9, 3);
    prt_num("+ To Damage: ", m_ptr->dis_td, 10, 3);
    prt_num("+ To AC    : ", m_ptr->dis_tac, 11, 3);
    prt_num("  Total AC : ", m_ptr->dis_ac, 12, 3);
}

/* Returns a rating of x depending on y    -JWT- */
char *likert(int x, int y)
{
    switch(x / y) {
    case -3:
    case -2:
    case -1:

	return "Very Bad";
    case 0:
    case 1:

	return "Bad";
    case 2:

	return "Poor";
    case 3:
    case 4:

	return "Fair";
    case 5:

	return "Good";
    case 6:

	return "Very Good";
    case 7:
    case 8:

	return "Excellent";
    default:

	return "Superb";
    }
}

/* Prints age, height, weight, and SC    -JWT- */
void put_misc1()
{
    struct misc *m_ptr;

    m_ptr = &py.misc;

    prt_num("Age          : ", (int)m_ptr->age, 2, 39);
    prt_num("Height       : ", (int)m_ptr->ht, 3, 39);
    prt_num("Weight       : ", (int)m_ptr->wt, 4, 39);
    prt_num("Social Class : ", (int)m_ptr->sc, 5, 39);
}

/* Prints the following information on the screen    -JWT- */
void put_misc2()
{
    struct misc *m_ptr;

    m_ptr = &py.misc;

    prt_num("Level      :", (int)m_ptr->lev, 9, 30);
    prt_num("Experience :", m_ptr->exp, 10, 30);
    prt_num("Gold       :", m_ptr->au, 11, 30);
    prt_num("Max Hit Points : ", m_ptr->mhp, 9, 53);
    prt_num("Cur Hit Points : ", (int)m_ptr->chp, 10, 53);
    prt_num("Max Mana       : ", m_ptr->mana, 11, 53);
    prt_num("Cur Mana       : ", (int)m_ptr->cmana, 12, 53);
}

/* Prints ratings on certain abilities    -RAK- */
void put_misc3()
{
    int xbth;
    int xbthb;
    int xfos;
    int xsrh;
    int xstl;
    int xdis;
    int xsave;
    int xdev;
    vtype xinfra;
    struct misc *p_ptr;
    char tmp_str[80];

    clear_screen(13, 0);
    p_ptr = &py.misc;
    xbth = p_ptr->bth + (p_ptr->lev * BTH_LEV_ADJ) + (p_ptr->ptohit * BTH_PLUS_ADJ);
    xbthb = p_ptr->bthb + (p_ptr->lev * BTH_LEV_ADJ) + (p_ptr->ptohit * BTH_PLUS_ADJ);

    /* This results in a range from 0 to 29 */
    xfos = 40 - p_ptr->fos;

    if(xfos < 0) {
	xfos = 0;
    }

    xsrh = p_ptr->srh + int_adj();

    /* This results in a range from 0 to 9 */
    xstl = p_ptr->stl + 1;
    xdis = p_ptr->disarm + p_ptr->lev + (2 * todis_adj()) + int_adj();
    xsave = p_ptr->save + p_ptr->lev + wis_adj();
    xdev = p_ptr->save + p_ptr->lev + int_adj();

    sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    prt("(Miscellaneous Abilities)", 15, 23);
    put_buffer(strcat(strcpy(tmp_str, "Fighting    : "), likert(xbth, 12)), 16, 1);
    put_buffer(strcat(strcpy(tmp_str, "Bows/Throw  : "), likert(xbthb, 12)), 17, 1);
    put_buffer(strcat(strcpy(tmp_str, "Saving Throw: "), likert(xsave, 6)), 18, 1);
    put_buffer(strcat(strcpy(tmp_str, "Stealth     : "), likert(xstl, 1)), 16, 26);
    put_buffer(strcat(strcpy(tmp_str, "Disarming   : "), likert(xdis, 8)), 17, 26);
    put_buffer(strcat(strcpy(tmp_str, "Magic Device: "), likert(xdev, 6)), 18, 26);
    put_buffer(strcat(strcpy(tmp_str, "Perception  : "), likert(xfos, 3)), 16, 51);
    put_buffer(strcat(strcpy(tmp_str, "Searching   : "), likert(xsrh, 6)), 17, 51);
    put_buffer(strcat(strcpy(tmp_str, "Infra-Vision: "), xinfra), 18, 51);
}

/* Used to display the character on the screen    -RAK- */
void display_char()
{
    put_character();
    put_misc1();
    put_stats();
    put_misc2();
    put_misc3();
}

/* Gets a name for the character    -JWT- */
void get_name()
{
    prt("Enter your player's name [press <RETURN> when finished]", 21, 2);
    get_string(py.misc.name, 2, 14, 24);
    clear_screen(20, 0);
}

/* Changes the name of the character    -JWT- */
void change_name()
{
    char c;
    int flag;

    flag = FALSE;
    display_char();

    prt("<c>hange character name.     <ESCAPE> to continue.", 21, 2);
    inkey(&c);

    switch(c) {
    case 99:
	get_name();

	break;
    case 0:
    case 27:
	flag = TRUE;

	break;
    default:

	break;
    }

    while(!flag) {
	prt("<c>hange character name.     <ESCAPE> to continue.", 21, 2);
	inkey(&c);

	switch(c) {
	case 99:
	    get_name();

	    break;
	case 0:
	case 27:
	    flag = TRUE;

	    break;
	default:

	    break;
	}
    }
}

/* Builds passwords    -RAK- */
void bpswd()
{
    strcpy(password1, PASSWD1);
    strcpy(password2, PASSWD2);
}

/* Destroy an item in the inventory    -RAK- */
void inven_destroy(int item_val)
{
    int j;
    treasure_type *i_ptr;

    inventory[INVEN_MAX] = inventory[item_val];
    i_ptr = &inventory[item_val];

    if((i_ptr->number > 1) && (i_ptr->subval < 512)) {
	--i_ptr->number;
	inven_weight -= i_ptr->weight;
	inventory[INVEN_MAX].number = 1;
    }
    else {
	inven_weight -= (i_ptr->weight * i_ptr->number);

	for(j = item_val; j < (inven_ctr - 1); ++j) {
	    inventory[j] = inventory[j + 1];
	}

	inventory[inven_ctr - 1] = blank_treasure;
	--inven_ctr;
    }
}

/* Drops an item from inventory to given location    -RAK- */
void inven_drop(int item_val, int y, int x)
{
    int i;
    cave_type *cave_ptr;

    cave_ptr = &cave[y][x];

    if(cave_ptr->tptr != 0) {
	pusht((int)cave_ptr->tptr);
    }

    inven_destroy(item_val);
    popt(&i);
    t_list[i] = inventory[INVEN_MAX];
    cave_ptr->tptr = i;
}

/* Destroys a type of item on given percent chance    -RAK- */
int inven_damage(int (*typ)(), int perc)
{
    int i;
    int j;

    j = 0;

    for(i = 0; i < inven_ctr; ++i) {
	if((*typ)(inventory[i].tval)) {
	    if(randint(100) < perc) {
		inven_destroy(i);
		++j;
	    }
	}
    }

    return j;
}

/* Computes current weight limit    -RAK- */
int weight_limit()
{
    int weight_cap;

    weight_cap = (py.stats.cstr * PLAYER_WEIGHT_CAP) + py.misc.wt;

    if(weight_cap > 3000) {
	weight_cap = 3000;
    }

    return weight_cap;
}

/* Check inventory for too much weight    -RAK- */
int inven_check_weight()
{
    int item_wgt;
    int max_weight;
    int check_weight;

    check_weight = FALSE;
    max_weight = weight_limit();
    item_wgt = inventory[INVEN_MAX].number * inventory[INVEN_MAX].weight;

    /* Now, check to see if player can carry object */
    if((inven_weight + item_wgt) <= max_weight) {
	/* Can carry weight */
	check_weight = TRUE;
    }

    return check_weight;
}

/* Check to see if he will be carrying too many objects    -RAK- */
int inven_check_num()
{
    int i;
    int check_num;

    check_num = FALSE;

    if(inven_ctr < 22) {
	check_num = TRUE;
    }
    else if(inventory[INVEN_MAX].subval > 255) {
	for(i = 0; i < inven_ctr; ++i) {
	    if(inventory[i].tval == inventory[INVEN_MAX].tval) {
		if(inventory[i].subval == inventory[INVEN_MAX].subval) {
		    check_num = TRUE;
		}
	    }
	}
    }

    return check_num;
}

/* Insert INVEN_MAX at given location */
void insert_inv(int pos, int wgt)
{
    int i;

    for(i = (inven_ctr - 1); i >= pos; --i) {
	inventory[i + 1] = inventory[i];
    }

    inventory[pos] = inventory[INVEN_MAX];
    ++inven_ctr;
    inven_weight += wgt;
}

/*
 * Add the item in INVEN_MAX to player's inventory. Return the item position for
 * a description if needed...    -RAK-
 */
void inven_carry(int *item_val)
{
    int item_num;
    int wgt;
    int typ;
    int subt;
    int flag;
    treasure_type *i_ptr;

    /* Now, check to see if player can carry object */
    *item_val = 0;
    flag = FALSE;
    i_ptr = &inventory[INVEN_MAX];
    item_num = i_ptr->number;
    typ = i_ptr->tval;
    subt = i_ptr->subval;
    wgt = i_ptr->number * i_ptr->weight;

    i_ptr = &inventory[*item_val];

    if(typ == i_ptr->tval) {
	/* Adds to other item */
	if(subt == i_ptr->subval) {
	    if(subt > 255) {
		i_ptr->number += item_num;
		inven_weight += wgt;
		flag = TRUE;
	    }
	}
    }
    else if(typ > i_ptr->tval) {
	/* Insert into list */
	insert_inv(*item_val, wgt);
	flag = TRUE;
    }

    ++(*item_val);

    while((*item_val < inven_ctr) && !flag) {
	i_ptr = &inventory[*item_val];

	if(typ == i_ptr->tval) {
	    /* Adds to other item */
	    if(subt == i_ptr->subval) {
		if(subt > 255) {
		    i_ptr->number += item_num;
		    inven_weight += wgt;
		    flag = TRUE;
		}
	    }
	}
	else if(typ > i_ptr->tval) {
	    /* Insert into list */
	    insert_inv(*item_val, wgt);
	    flag = TRUE;
	}

	++(*item_val);
    }

    /* Becomes the last item in list */
    if(!flag) {
	insert_inv(inven_ctr, wgt);
	*item_val = inven_ctr - 1;
    }
    else {
	--(*item_val);
    }
}

/* Returns spell chance of failure for spell    -RAK- */
void spell_chance(spl_rec *spell)
{
    spell_type *s_ptr;

    s_ptr = &magic_spell[py.misc.pclass][spell->splnum];
    spell->splchn = s_ptr->sfail - (3 * (py.misc.lev - s_ptr->slevel));

    if(class[py.misc.pclass].mspell) {
	spell->splchn -= (3 * (int_adj() - 1));
    }
    else {
	spell->splchn -= (3 * (wis_adj() - 1));
    }

    if(s_ptr->smana > py.misc.cmana) {
	spell->splchn += (5 * (s_ptr->smana - (int)py.misc.cmana));
    }

    if(spell->splchn > 95) {
	spell->splchn = 95;
    }
    else if(spell->splchn < 5) {
	spell->splchn = 5;
    }
}

/* Print list of spells    -RAK- */
void print_new_spells(spl_type spell, int num, int *redraw)
{
    int i;
    vtype out_val;
    spell_type *s_ptr;

    *redraw = TRUE;
    clear_screen(1, 0);
    prt("   Name                          Level  Mana  %Failure", 1, 0);

    for(i = 0; i < num; ++i) {
	s_ptr = &magic_spell[py.misc.pclass][spell[i].splnum];
	spell_chance(&spell[i]);

	sprintf(out_val,
		"%c) %s%d    %d      %d",
		97 + i,
		pad(s_ptr->sname, " ", 30),
		s_ptr->slevel,
		s_ptr->smana,
		spell[i].splchn);

	prt(out_val, 2 + i, 0);
    }
}

/* Returns spell pointer    -RAK- */
int get_spell(spl_type spell, int num, int *sn, int *sc, vtype prompt, int *redraw)
{
    int flag;
    char choice;
    vtype out_val1;

    *sn = -1;
    flag = TRUE;
    sprintf(out_val1, "(Spells a-%c, *==List, <ESCAPE>=exit) %s", num + 96, prompt);
    prt(out_val1, 0, 0);

    while(((*sn < 0) || (*sn >= num)) && flag) {
	inkey(&choice);
	*sn = choice;

	switch(*sn) {
	case 0:
	case 27:
	    flag = FALSE;
	    reset_flag = TRUE;

	    break;
	case 42:
	    print_new_spells(spell, num, redraw);

	    break;
	default:
	    *sn -= 97;

	    break;
	}
    }

    erase_line(MSG_LINE, 0);
    msg_flag = FALSE;

    if(flag) {
	spell_chance(&spell[*sn]);
	*sc = spell[*sn].splchn;
	*sn = spell[*sn].splnum;
    }

    return flag;
}

/* Learn some magic spells (Mage)    -RAK- */
int learn_spell(int *redraw)
{
    unsigned int j;
    int i;
    int k;
    int new_spells;
    int sn;
    int sc;
    unsigned int spell_flag;
    spl_type spell;
    int learn;
    spell_type *s_ptr;

    learn = FALSE;

    switch(int_adj()) {
    case 0:
	new_spells = 0;

	break;
    case 1:
	new_spells = 1;

	break;
    case 2:
	new_spells = 1;

	break;
    case 3:
	new_spells = 1;

	break;
    case 4:
	new_spells = randint(2);

	break;
    case 5:
	new_spells = randint(2);

	break;
    case 6:
	new_spells = randint(3);

	break;
    case 7:
	new_spells = randint(2) + 1;

	break;
    default:
	new_spells = 0;

	break;
    }

    i = 0;
    spell_flag = 0;

    if(inventory[i].tval == 90) {
	spell_flag |= inventory[i].flags;
    }

    ++i;

    while(i < inven_ctr) {
	if(inventory[i].tval == 90) {
	    spell_flag |= inventory[i].flags;
	}

	++i;
    }

    while((new_spells > 0) && (spell_flag != 0)) {
	i = 0;
	j = spell_flag;

	k = bit_pos(&j);
	s_ptr = &magic_spell[py.misc.pclass][k];

	if(s_ptr->slevel <= py.misc.lev) {
	    if(!s_ptr->learned) {
		spell[i].splnum = k;
		++i;
	    }
	}

	while(j != 0) {
	    k = bit_pos(&j);
	    s_ptr = &magic_spell[py.misc.pclass][k];

	    if(s_ptr->slevel <= py.misc.lev) {
		if(!s_ptr->learned) {
		    spell[i].splnum = k;
		    ++i;
		}
	    }
	}

	if(i > 0) {
	    print_new_spells(spell, i, redraw);

	    if(get_spell(spell, i, &sn, &sc, "Learn which spell?", redraw)) {
		magic_spell[py.misc.pclass][sn].learned = TRUE;
		learn = TRUE;
		--new_spells;

		if(py.misc.mana == 0) {
		    py.misc.mana = 1;
		    py.misc.cmana = 1.0;
		}
	    }

	    /* Else do nothing if get_spell fails */
	}
	else {
	    new_spells = 0;
	}
    }

    return learn;
}

/* Learn some prayers    -RAK- */
int learn_prayer()
{
    int i;
    int j = 0;
    int k;
    int l;
    int new_spell;
    int test_array[32];
    unsigned int spell_flag;
    int learn;
    spell_type *s_ptr;

    i = 0;
    spell_flag = 0;

    if(inventory[i].tval == 91) {
	spell_flag |= inventory[i].flags;
    }

    ++i;

    while(i < inven_ctr) {
	if(inventory[i].tval == 91) {
	    spell_flag |= inventory[i].flags;
	}

	++i;
    }

    i = 0;

    while(spell_flag != 0) {
	j = bit_pos(&spell_flag);
	s_ptr = &magic_spell[py.misc.pclass][j];

	if(s_ptr->slevel <= py.misc.lev) {
	    if(!s_ptr->learned) {
		test_array[i] = j;
		++i;
	    }
	}
    }

    switch(wis_adj()) {
    case 0:
	j = 0;

	break;
    case 1:
	j = 1;

	break;
    case 2:
	j = 1;

	break;
    case 3:
	j = 1;

	break;
    case 4:
	j = randint(2);

	break;
    case 5:
	j = randint(2);

	break;
    case 6:
	j = randint(3);

	break;
    case 7:
	j = randint(2) + 1;

	break;
    }

    new_spell = 0;

    while((i > 0) && (j > 0)) {
	k = randint(i) - 1;
	magic_spell[py.misc.pclass][test_array[k]].learned = TRUE;
	++new_spell;

	for(l = k; l <= (i - 1); ++l) {
	    test_array[l] = test_array[l + 1];
	}

	/* One less spell to learn */
	--i;

	/* Learned one */
	--j;
    }

    if(new_spell > 0) {
	if(new_spell > 1) {
	    msg_print("You learned new prayers!");
	}
	else {
	    msg_print("You learned a new prayer!");
	}

	/* Make sure player see the message when game starts */
	if(py.misc.exp == 0) {
	    msg_print(" ");
	}

	if(py.misc.mana == 0) {
	    py.misc.mana = 1;
	    py.misc.cmana = 1.0;
	}

	learn = TRUE;
    }
    else {
	learn = FALSE;
    }

    return learn;
}

/* Gain some mana if you know at least one spell    -RAK- */
void gain_mana(int amount)
{
    int i;
    int new_mana;
    int knows_spell;

    knows_spell = FALSE;

    for(i = 0; i < 31; ++i) {
	if(magic_spell[py.misc.pclass][i].learned) {
	    knows_spell = TRUE;
	}
    }

    if(knows_spell) {
	if(py.misc.lev & 0x1) {
	    switch(amount) {
	    case 0:
		new_mana = 0;

		break;
	    case 1:
		new_mana = 1;

		break;
	    case 2:
		new_mana = 1;

		break;
	    case 3:
		new_mana = 1;

		break;
	    case 4:
		new_mana = 2;

		break;
	    case 5:
		new_mana = 2;

		break;
	    case 6:
		new_mana = 3;

		break;
	    case 7:
		new_mana = 4;

		break;
	    default:
		new_mana = 0;

		break;
	    }
	}
	else {
	    switch(amount) {
	    case 0:
		new_mana = 0;

		break;
	    case 1:
		new_mana = 1;

		break;
	    case 2:
		new_mana = 1;

		break;
	    case 3:
		new_mana = 2;

		break;
	    case 4:
		new_mana = 2;

		break;
	    case 5:
		new_mana = 3;

		break;
	    case 6:
		new_mana = 3;

		break;
	    case 7:
		new_mana = 4;

		break;
	    default:
		new_mana = 0;

		break;
	    }
	}

	py.misc.mana += new_mana;
	py.misc.cmana += new_mana;
    }
}

/* Increases hit points and level    -RAK- */
void gain_level()
{
    int nhp;
    int dif_exp;
    int need_exp;
    int redraw;
    vtype out_val;
    struct misc *p_ptr;
    class_type *c_ptr;

    p_ptr = &py.misc;
    nhp = get_hitdie();
    p_ptr->mhp += nhp;
    p_ptr->chp += (double)nhp;
    ++p_ptr->lev;
    need_exp = player_exp[p_ptr->lev - 1] * p_ptr->expfact;

    if(py.misc.exp > need_exp) {
	/* Lose some of the 'extra' exp when gain a level */
	dif_exp = py.misc.exp - need_exp;
	py.misc.exp = need_exp + (dif_exp / 2);
    }

    strcpy(p_ptr->title, player_title[p_ptr->pclass][p_ptr->lev - 1]);
    sprintf(out_val, "Welcome to level %d.", (int)p_ptr->lev);
    msg_print(out_val);

    /* Make sure palyer sees message, before learn_spell erases it */
    msg_print(" ");
    msg_flag = FALSE;
    prt_mhp();
    prt_chp();
    prt_level();
    prt_title();
    c_ptr = &class[p_ptr->pclass];

    if(c_ptr->mspell) {
	redraw = FALSE;
	learn_spell(&redraw);

	if(redraw) {
	    draw_cave();
	}

	gain_mana(int_adj());
	prt_cmana();
    }
    else if(c_ptr->pspell) {
	learn_prayer();
	gain_mana(wis_adj());
	prt_cmana();
    }
}

/* Prints experience    -RAK- */
void prt_experience()
{
    struct misc *p_ptr;

    p_ptr = &py.misc;

    if(p_ptr->exp > player_max_exp) {
	p_ptr->exp = player_max_exp;
    }

    if(p_ptr->lev < MAX_PLAYER_LEVEL) {
	while((player_exp[p_ptr->lev - 1] * p_ptr->expfact) <= p_ptr->exp) {
	    gain_level();
	}

	if(p_ptr->exp > p_ptr->max_exp) {
	    p_ptr->max_exp = p_ptr->exp;
	}
    }

    prt_num("", py.misc.exp, 14, stat_column + 6);
}

/* Inserts a string into a string */
void insert_str(char *object_str, char *mtc_str, char *insert)
{
    int obj_len;
    char *bound;
    char *pc;
    int i;
    int mtc_len;
    char *temp_obj;
    char *temp_mtc;
    char out_val[80];

    mtc_len = strlen(mtc_str);
    obj_len = strlen(object_str);
    bound = object_str + obj_len - mtc_len;

    for(pc = object_str; pc <= bound; ++pc) {
	temp_obj = pc;
	temp_mtc = mtc_str;

	for(i = 0; i < mtc_len; ++i) {
	    if(*temp_obj++ != *temp_mtc++) {
		break;
	    }
	}

	if(i == mtc_len) {
	    break;
	}
    }

    if(pc <= bound) {
	strncpy(out_val, object_str, pc - object_str);
	out_val[pc - object_str] = '\0';
	strcat(out_val, insert);
	strcat(out_val, (char *)(pc + mtc_len));
	strcpy(object_str, out_val);
    }
}

/* Inserts a number into a string */
void insert_num(char *object_str, char *mtc_str, int number, int show_sign)
{
    int pos;
    int mlen;
    vtype str1;
    vtype str2;
    char *string;
    char *tmp_str;
    int flag;

    flag = 1;
    tmp_str = object_str;

    string = index(tmp_str, mtc_str[0]);

    if(string == 0) {
	flag = 0;
    }
    else {
	flag = strncmp(string, mtc_str, strlen(mtc_str));

	if(flag) {
	    tmp_str = string + 1;
	}
    }

    while(flag) {
	string = index(tmp_str, mtc_str[0]);

	if(string == 0) {
	    flag = 0;
	}
	else {
	    flag = strncmp(string, mtc_str, strlen(mtc_str));

	    if(flag) {
		tmp_str = string + 1;
	    }
	}
    }

    if(string) {
	pos = strlen(object_str) - strlen(string);
    }
    else {
	pos = -1;
    }

    if(pos >= 0) {
	mlen = strlen(mtc_str);
	strncpy(str1, object_str, pos);
	str1[pos] = '\0';
	strcpy(str2, &object_str[pos + mlen]);

	if((number > 0) && show_sign) {
	    sprintf(object_str, "%s+%d%s", str1, number, str2);
	}
	else {
	    sprintf(object_str, "%s%d%s", str1, number, str2);
	}
    }
}

/* Checks to see if used is a wizard    -RAK- */
int check_paswd()
{
    int i;
    char x;
    char tpw[12];
    int check;

    check = FALSE;

    if(getuid() != UID) {
	return FALSE;
    }

    i = 0;
    tpw[0] = '\0';
    prt("Password : ", 0, 0);

    inkey(&x);

    switch(x) {
    case 10:
    case 13:

	break;
    default:
	tpw[i] = x;
	++i;

	break;
    }

    while((i != 12) && (x != 13) && (x != 10)) {
	inkey(&x);

	switch(x) {
	case 10:
	case 13:

	    break;
	default:
	    tpw[i] = x;
	    ++i;

	    break;
	}
    }

    tpw[i] = '\0';

    if(!strcmp(tpw, password1)) {
	wizard1 = TRUE;
	check = TRUE;
    }
    else if(!strcmp(tpw, password2)) {
	wizard1 = TRUE;
	wizard2 = TRUE;
	check = TRUE;
    }

    msg_flag = FALSE;
    erase_line(MSG_LINE, 0);

    return check;
}

/* Weapon weight vs. strength and dexterity    -RAK- */
int attack_blows(int weight, int *wtohit)
{
    int adj_weight;
    int blows;
    struct stats *p_ptr;

    blows = 1;
    *wtohit = 0;
    p_ptr = &py.stats;

    if((p_ptr->cstr * 15) < weight) {
	*wtohit = (p_ptr->cstr * 15) - weight;
    }
    else {
	if(p_ptr->cdex < 10) {
	    blows = 1;
	}
	else if(p_ptr->cdex < 19) {
	    blows = 2;
	}
	else if(p_ptr->cdex < 68) {
	    blows = 3;
	}
	else if(p_ptr->cdex < 108) {
	    blows = 4;
	}
	else if(p_ptr->cdex < 118) {
	    blows = 5;
	}
	else {
	    blows = 6;
	}
	
	adj_weight = (p_ptr->cstr * 10) / weight;

	if(adj_weight < 2) {
	    blows = 1;
	}
	else if(adj_weight < 3) {
	    blows = (blows / 3.0) + 1;
	}
	else if(adj_weight < 4) {
	    blows = (blows / 2.5) + 1;
	}
	else if(adj_weight < 5) {
	    blows = (blows / 2.25) + 1;
	}
	else if(adj_weight < 7) {
	    blows = (blows / 2.00) + 1;
	}
	else if(adj_weight < 9) {
	    blows = (blows / 1.75) + 1;
	}
	else {
	    blows = (blows / 1.50) + 1;
	}
    }

    return blows;
}

/* Critical hits, nasty way to die...    -RAK- */
int critical_blow(int weight, int plus, int dam)
{
    int critical;

    critical = dam;

    /*
     * Weight of weapon, plusses to hit, and character level all contribute to
     * the chance of a critical
     */
    if(randint(5000) < (int)(weight + (5 * plus) + (3 * py.misc.lev))) {
	weight += randint(650);

	if(weight < 400) {
	    critical = (2 * dam) + 5;
	    msg_print("It was a good hit! (x2 damage)");
	}
	else if(weight < 700) {
	    critical = (3 * dam) + 10;
	    msg_print("It was an excellent hit! (x3 damage)");
	}
	else if(weight < 900) {
	    critical = (4 * dam) + 15;
	    msg_print("It was a superb hit! (x4 damage)");
	}
	else {
	    critical = (5 * dam) + 20;
	    msg_print("It was a *GREAT* hit! (x5 damage)");
	}
    }

    return critical;
}

/* Given a direction "dir", returns new row, column location    -RAK- */
int moria_move(int dir, int *y, int *x)
{
    int new_row = 0;
    int new_col = 0;
    int bool;

    switch(dir) {
    case 1:
	new_row = *y + 1;
	new_col = *x - 1;

	break;
    case 2:
	new_row = *y + 1;
	new_col = *x;

	break;
    case 3:
	new_row = *y + 1;
	new_col = *x + 1;

	break;
    case 4:
	new_row = *y;
	new_col = *x - 1;

	break;
    case 5:
	new_row = *y;
	new_col = *x;

	break;
    case 6:
	new_row = *y;
	new_col = *x + 1;

	break;
    case 7:
	new_row = *y - 1;
	new_col = *x - 1;

	break;
    case 8:
	new_row = *y - 1;
	new_col = *x;

	break;
    case 9:
	new_row = *y - 1;
	new_col = *x + 1;

	break;
    }

    bool = FALSE;

    if((new_row >= 0) && (new_row < cur_height)) {
	if((new_col >= 0) && (new_col < cur_width)) {
	    *y = new_row;
	    *x = new_col;
	    bool = TRUE;
	}
    }

    return bool;
}

/* Saving throws for player character...    -RAK- */
int player_saves(int adjust)
{
    if(randint(100) <= (py.misc.save + py.misc.lev + adjust)) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

/* Init players with some belongings    -RAK- */
void char_inven_init()
{
    int i;
    int j;
    int dummy;

    /* This is needed for bash to work right, it can't hurt anyway */
    for(i = 0; i < INVEN_ARRAY_SIZE; ++i) {
	inventory[i] = blank_treasure;
    }

    for(i = 0; i < 5; ++i) {
	j = player_init[py.misc.pclass][i];
	inventory[INVEN_MAX] = inventory_init[j];
	inven_carry(&dummy);
    }
}
