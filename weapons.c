/*
 * Functions for dealing with problems brought about by weapons
 *
 * @(#)weapons.c	3.17 (Berkeley) 6/15/81
 */

#include "weapons.h"

#include "chase.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "move.h"
#include "pack.h"
#include "things.h"

#include <ctype.h>

#define NONE 100

char *w_names[MAXWEAPONS] = {
    "mace",
    "long sword",
    "short bow",
    "arrow",
    "dagger",
    "rock",
    "two handed sword",
    "sling",
    "dart",
    "crossbow",
    "crossbow bolt",
    "spear",
};

static struct init_weps {
    char *iw_dam;
    char *iw_hrl;
    char iw_launch;
    int iw_flags;
} init_dam[MAXWEAPONS] = {
    { "2d4", "1d3", NONE, 0 },		/* Mace */
    { "1d10", "1d2", NONE,0 },		/* Long sword */
    { "1d1", "1d1", NONE,	0 },		/* Bow */
    { "1d1", "1d6", BOW,	ISMANY|ISMISL },	/* Arrow */
    { "1d6", "1d4", NONE,	ISMISL },		/* Dagger */
    { "1d2", "1d4", SLING,ISMANY|ISMISL },	/* Rock */
    { "3d6", "1d2", NONE,	0 },		/* 2h sword */
    { "0d0", "0d0", NONE, 0 },		/* Sling */
    { "1d1", "1d3", NONE,	ISMANY|ISMISL },	/* Dart */
    { "1d1", "1d1", NONE, 0 },		/* Crossbow */
    { "1d2", "1d10", CROSSBOW, ISMANY|ISMISL },/* Crossbow bolt */
    { "1d8", "1d6", NONE, ISMISL },		/* Spear */
};

/*
 * missile:
 *	Fire a missile in a given direction
 */

int missile(int ydelta, int xdelta)
{
    register struct object *obj;
    register struct linked_list *item, *nitem;

    /*
     * Get which thing we are hurling
     */
    if ((item = get_item("throw", WEAPON)) == NULL)
	return 0;
    obj = (struct object *)item->l_data;
    if (!dropcheck(obj) || is_current(obj))
	return 0;
    /*
     * Get rid of the thing.  If it is a non-multiple item object, or
     * if it is the last thing, just drop it.  Otherwise, create a new
     * item with a count of one.
     */
    if(obj->o_count < 2) {
	_detach(&player.t_pack, item);
	--inpack;
    }
    else {
	--obj->o_count;

        if(obj->o_group == 0) {
	    --inpack;
        }
        
	nitem = (struct linked_list *) new_item(sizeof *obj);
	obj = (struct object *)nitem->l_data;
	*obj = *((struct object *)item->l_data);
	obj->o_count = 1;
	item = nitem;
    }

    do_motion(obj, ydelta, xdelta);
    /*
     * AHA! Here it has hit something.  If it is a wall or a door,
     * or if it misses (combat) the mosnter, put it on the floor
     */
    if (!isupper(mvwinch(mw, obj->o_pos.y, obj->o_pos.x))
	|| !hit_monster(obj->o_pos.y, obj->o_pos.x, obj))
	    fall(item, TRUE);
    mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);

    return 0;
}

/*
 * do the actual motion on the screen done by an object traveling
 * across the room
 */
int do_motion(struct object *obj, int ydelta, int xdelta)
{
    /*
     * Come fly with us ...
     */
    obj->o_pos = player.t_pos;
    for (;;)
    {
	register int ch;

        // Erase the old one
	if(!((obj->o_pos.x == player.t_pos.x) && (obj->o_pos.y == player.t_pos.y))
           && cansee(obj->o_pos.y, obj->o_pos.x)
           && mvwinch(cw, obj->o_pos.y, obj->o_pos.x) != ' ') {
            mvwaddch(cw,
                     obj->o_pos.y,
                     obj->o_pos.x,
                     show(obj->o_pos.y, obj->o_pos.x));
        }
	/*
	 * Get the new position
	 */
	obj->o_pos.y += ydelta;
	obj->o_pos.x += xdelta;

        if(mvwinch(mw, obj->o_pos.y, obj->o_pos.y) == ' ') {
            ch = mvwinch(stdscr, obj->o_pos.y, obj->o_pos.x);
        }
        else {
            ch = winch(mw);
        }

	if(step_ok(ch) && (ch != DOOR)) {
	    /*
	     * It hasn't hit anything yet, so display it
	     * If it alright.
	     */
	    if (cansee(obj->o_pos.y, obj->o_pos.x) &&
		mvwinch(cw, obj->o_pos.y, obj->o_pos.x) != ' ')
	    {
		mvwaddch(cw, obj->o_pos.y, obj->o_pos.x, obj->o_type);
                wrefresh(cw);
	    }
	    continue;
	}
	break;
    }

    return 0;
}

/*
 * fall:
 *	Drop an item someplace around here.
 */

int fall(struct linked_list *item, bool pr)
{
    register struct object *obj;
    register struct room *rp;
    static coord fpos;

    obj = (struct object *)item->l_data;
    if (fallpos(&obj->o_pos, &fpos, TRUE))
    {
	mvaddch(fpos.y, fpos.x, obj->o_type);
	obj->o_pos = fpos;
	if ((rp = roomin(&player.t_pos)) != NULL && !(rp->r_flags & ISDARK))
	{
	    light(&player.t_pos);
	    mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
	}
	_attach(&lvl_obj, item);
	return 0;
    }
    if (pr)
	msg("Your %s vanishes as it hits the ground.", w_names[obj->o_which]);
    discard(item);

    return 0;
}

/*
 * init_weapon:
 *	Set up the initial goodies for a weapon
 */

int init_weapon(struct object *weap, char type)
{
    register struct init_weps *iwp;

    iwp = &init_dam[(int)type];
    weap->o_damage = iwp->iw_dam;
    weap->o_hurldmg = iwp->iw_hrl;
    weap->o_launch = iwp->iw_launch;
    weap->o_flags = iwp->iw_flags;
    if (weap->o_flags & ISMANY)
    {
	weap->o_count = rnd(8) + 8;
        ++group;
	weap->o_group = group;
    }
    else
	weap->o_count = 1;

    return 0;
}

/*
 * Does the missile hit the monster
 */

int hit_monster(int y, int x, struct object *obj)
{
    static coord mp;

    mp.y = y;
    mp.x = x;

    char temp;
    if(mvwinch(mw, y, x) == ' ') {
        temp = mvwinch(stdscr, y, x);
    }
    else {
        temp = winch(mw);
    }
    
    return fight(&mp, temp, obj, TRUE);
}

/*
 * num:
 *	Figure out the plus number for armor/weapons
 */

char *num(int n1, int n2)
{
    static char numbuf[80];

    if (n1 == 0 && n2 == 0)
	return "+0";
    if (n2 == 0) {
        if(n1 < 0) {
            sprintf(numbuf, "%s%d", "", n1);
        }
        else {
            sprintf(numbuf, "%s%d", "+", n1);
        }

        return numbuf;
    }

    if(n1 < 0) {
        if(n2 < 0) {
            sprintf(numbuf, "%s%d,%s%d", "", n1, "", n2);
        }
        else {
            sprintf(numbuf, "%s%d,%s%d", "", n1, "+", n2);
        }
    }
    else {
        if(n2 < 0) {
            sprintf(numbuf, "%s%d,%s%d", "+", n1, "", n2);
        }
        else {
            sprintf(numbuf, "%s%d,%s%d", "+", n1, "+", n2);
        }
    }

    return numbuf;
}

// wield:
//     Pull out a certain weapon
int wield()
{
    struct linked_list *item;
    struct object *obj;
    struct object *oweapon;

    oweapon = cur_weapon;
    
    if(!dropcheck(cur_weapon)) {
	cur_weapon = oweapon;
        
	return 0;
    }
    
    cur_weapon = oweapon;
    item = get_item("wield", WEAPON);

    if(item == NULL) {
	after = FALSE;
        
	return 0;
    }

    obj = (struct object *)item->l_data;
    
    if(obj->o_type == ARMOR) {
	msg("You can't wield armor", 0);
        after = FALSE;
        
        return 0;
    }

    if (is_current(obj)) {
        after = FALSE;
        
        return 0;
    }

    if(terse) {
	addmsg("W", 0);
    }
    else {
	addmsg("You are now w", 0);
    }
    
    msg("ielding %s", inv_name(obj, TRUE));
    cur_weapon = obj;

    return 0;
}

/*
 * pick a random position around the give (y, x) coordinates
 */
int fallpos(coord *pos, coord *newpos, bool passages)
{
    register int y, x, cnt, ch;

    cnt = 0;
    for (y = pos->y - 1; y <= pos->y + 1; y++)
	for (x = pos->x - 1; x <= pos->x + 1; x++)
	{
	    /*
	     * check to make certain the spot is empty, if it is,
	     * put the object there, set it in the level list
	     * and re-draw the room if he can see it
	     */
	    if((y == player.t_pos.y) && (x == player.t_pos.x)) {
		continue;
            }

            if(mvwinch(mw, y, x) == ' ') {
                ch = mvwinch(stdscr, y, x);
            }
            else {
                ch = winch(mw);
            }
            
	    if(((ch == FLOOR) || (passages && ch == PASSAGE))
               && (rnd(++cnt) == 0)) {
		newpos->y = y;
		newpos->x = x;
	    }
	}
    return (cnt != 0);
}
