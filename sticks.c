// Functions to implement the various sticks one might
// find while wandering aroud the dungeon
//
// @(#)sticks.c 3.14 (Berkeley) 6/15/81
#include "sticks.h"

#include "chase.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "monsters.h"
#include "move.h"
#include "newlevel.h"
#include "pack.h"
#include "rip.h"
#include "rooms.h"
#include "weapons.h"

#include <ctype.h>

/* TC_DEBUG: Start */
#include "debug.h"
#include <stdio.h>
/* TC_DEBUG: Finish */

// fix_stick:
//     Something...
int fix_stick(struct object *cur)
{
    if(strcmp(ws_type[cur->o_which], "staff") == 0) {
        cur->o_damage = "2d3";
    }
    else {
        cur->o_damage = "1d1";
    }
    
    cur->o_hurldmg = "1d1";
    cur->o_ac = 3 + rnd(5);
    
    switch(cur->o_which) {
    case WS_HIT:
        cur->o_hplus = 3;
        cur->o_dplus = 3;
        cur->o_damage = "1d8";
        break;
    case WS_LIGHT:
        cur->o_ac = 10 + rnd(10);
    }
    
    return 0;
}

// do_zap:
//     Something...
int do_zap(bool gotdir)
{
    struct linked_list *item;
    struct object *obj;
    struct room *rp;
    struct thing *tp;
    int y;
    int x;

    item = get_item("zap with", STICK);
    if(item == NULL) {
        return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != STICK) {
        msg("You can't zap with that!");
        after = FALSE;
        return 0;
    }
    if(obj->o_ac == 0) {
        msg("Nothing happens.");
        return 0;
    }
    if(!gotdir) {
        delta.y = rnd(3) - 1;
        delta.x = rnd(3) - 1;

        while((delta.y == 0) && (delta.x == 0)) {
            delta.y = rnd(3) - 1;
            delta.x = rnd(3) - 1;
        }
    }

    switch(obj->o_which) {
    case WS_LIGHT:
        // Ready kilowatt want. Light up the room
        ws_know[WS_LIGHT] = TRUE;
        rp = roomin(&player.t_pos);
        
        if(rp == NULL) {
            msg("The corridor glows and then fades");
        }
        else {
            addmsg("The room is lit");
            if(!terse) {
                addmsg(" by a shimmering blue light.");
            }
            endmsg();
            rp->r_flags &= ~ISDARK;

            // Light the room and put the player back up
            light(&player.t_pos);
            mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
        }
        break;
    case WS_DRAIN:
        // Take away 1/2 of hero's hit poits, then take it away
        // evenly from the monster in the room (or next to hero
        // if he is in a passage)
        if(player.t_stats.s_hpt < 2) {
            msg("You are too weak to use it.");
            return 0;
        }
        
        rp = roomin(&player.t_pos);
        
        if(rp == NULL) {
            drain(player.t_pos.y - 1, player.t_pos.y + 1, player.t_pos.x - 1, player.t_pos.x + 1);
        }
        else {
            drain(rp->r_pos.y,
                  rp->r_pos.y + rp->r_max.y,
                  rp->r_pos.x,
                  rp->r_pos.x + rp->r_max.x);
        }
        break;
    case WS_POLYMORPH:
    case WS_TELAWAY:
    case WS_TELTO:
    case WS_CANCEL:
        {
            char monster;
            char oldch;
            int rm;
            
            y = player.t_pos.y;
            x = player.t_pos.x;
            
            char temp;
            if(mvwinch(mw, y, x) == ' ') {
                temp = mvwinch(stdscr, y, x) & A_CHARTEXT;
            }
            else {
                temp = winch(mw);
            }
            
            while(step_ok(temp)) {
                y += delta.y;
                x += delta.x;
                
                if(mvwinch(mw, y, x) == ' ') {
                    temp = mvwinch(stdscr, y, x) & A_CHARTEXT;
                }
                else {
                    temp = winch(mw);
                }
            }

            monster = mvwinch(mw, y, x) & A_CHARTEXT;
            if(isupper(monster)) {
                char omonst = monster;

                if(monster == 'F') {
                    player.t_flags &= ~ISHELD;
                }
                
                item = find_mons(y, x);
                tp = (struct thing *)item->l_data;
                if(obj->o_which == WS_POLYMORPH) {
                    _detach(&mlist, item);
                    oldch = tp->t_oldch;
                    delta.y = y;
                    delta.x = x;
                    new_monster(item, monster = rnd(26) + 'A', &delta);
                    if(!(tp->t_flags & ISRUN)) {
                        runto(&delta, &player.t_pos);
                    }
                    
                    if(isupper(mvwinch(cw, y, x))) {
                        mvwaddch(cw, y, x, monster);
                    }
                    
                    tp->t_oldch = oldch;
                    ws_know[WS_POLYMORPH] |= (monster != omonst);
                }
                else if(obj->o_which == WS_CANCEL) {
                    tp->t_flags |= ISCANC;
                    tp->t_flags &= ~ISINVIS;
                }
                else {
                    if(obj->o_which == WS_TELAWAY) {
                        rm = rnd_room();
                        rnd_pos(&rooms[rm], &tp->t_pos);

                        if(mvwinch(mw, tp->t_pos.y, tp->t_pos.x) == ' ') {
                            temp = mvwinch(stdscr, tp->t_pos.y, tp->t_pos.x) & A_CHARTEXT;;
                        }
                        else {
                            temp = winch(mw);
                        }
                        
                        while(temp != FLOOR) {
                            rm = rnd_room();
                            rnd_pos(&rooms[rm], &tp->t_pos);

                            if(mvwinch(mw, tp->t_pos.y, tp->t_pos.x) == ' ') {
                                temp = mvwinch(stdscr, tp->t_pos.y, tp->t_pos.x) & A_CHARTEXT;
                            }
                            else {
                                temp = winch(mw);
                            }
                        }
                    }
                    else {
                        tp->t_pos.y = player.t_pos.y + delta.y;
                        tp->t_pos.x = player.t_pos.x + delta.x;
                    }
                    
                    if(isupper(mvwinch(cw, y, x))) {
                        mvwaddch(cw, y, x, tp->t_oldch);
                    }
                    
                    tp->t_dest = &player.t_pos;
                    tp->t_flags |= ISRUN;
                    mvwaddch(mw, y, x, ' ');
                    mvwaddch(mw, tp->t_pos.y, tp->t_pos.x, monster);

                    if((tp->t_pos.y != y)
                       || (tp->t_pos.x != x)) {
                        tp->t_oldch = mvwinch(cw, tp->t_pos.y, tp->t_pos.x) & A_CHARTEXT;
                    }
                }
            }
        }
        break;
    case WS_MISSILE:
        {
            static struct object bolt = {
                "bolt",   /* Name */
                "",       /* Read text */
                '*',      /* Type */
                0,        /* Which of type */
                1,        /* Group */
                { 0, 0 }, /* Position */
                0,        /* Probability */
                1,        /* count of object */
                100,      /* Flags */
                0,        /* Worth */
                11,       /* Armor class */
                '\0',     /* Launcher */
                "",       /* Damage */
                "1d4",    /* Hurl damage */
                0,        /* Hit bonus */
                0         /* Damage bonus */
            };

            do_motion(&bolt, delta.y, delta.x);
            if(isupper(mvwinch(mw, bolt.o_pos.y, bolt.o_pos.x)) 
               && !save_throw(VS_MAGIC, (struct thing *)find_mons(bolt.o_pos.y, bolt.o_pos.y)->l_data)) {
                hit_monster(bolt.o_pos.y, bolt.o_pos.x, &bolt);
            }
            else if(terse) {
                msg("Missle vanishes");
            }
            else {
                msg("The missle vanishes with a puff of smoke");
            }
            
            ws_know[WS_MISSILE] = TRUE;
        }
        break;
    case WS_HIT:
        {
            char ch;
            
            delta.y += player.t_pos.y;
            delta.x += player.t_pos.x;
            
            if(mvwinch(mw, delta.y, delta.x) == ' ') {
                ch = mvwinch(stdscr, delta.y, delta.x) & A_CHARTEXT;
            }
            else {
                ch = winch(mw);
            }

            if(isupper(ch)) {
                if(rnd(20) == 0) {
                    obj->o_damage = "3d8";
                    obj->o_dplus = 9;
                }
                else {
                    obj->o_damage = "1d8";
                    obj->o_dplus = 3;
                }
                
                fight(&delta, ch, obj, FALSE);
            }
        }
        break;
    case WS_HASTE_M:
    case WS_SLOW_M:
        {
            y = player.t_pos.y;
            x = player.t_pos.x;
            
            char temp;
            if(mvwinch(mw, y, x) == ' ') {
                temp = mvwinch(stdscr, y, x) & A_CHARTEXT;
            }
            else {
                temp = winch(mw);
            }
            
            while(step_ok(temp)) {
                y += delta.y;
                x += delta.x;
                
                if(mvwinch(mw, y, x) == ' ') {
                    temp = mvwinch(stdscr, y, x) & A_CHARTEXT;
                }
                else {
                    temp = winch(mw);
                }
            }
            
            if(isupper(mvwinch(mw, y, x))) {
                item = find_mons(y, x);
                tp = (struct thing *)item->l_data;

                if(obj->o_which == WS_HASTE_M) {
                    if((tp->t_flags & ISSLOW) != 0) {
                        tp->t_flags &= ~ISSLOW;
                    }
                    else {
                        tp->t_flags |= ISHASTE;
                    }
                }
                else {
                    if((tp->t_flags & ISSLOW) != 0) {
                        tp->t_flags &= ~ISHASTE;
                    }
                    else {
                        tp->t_flags |= ISSLOW;
                    }
                    
                    tp->t_turn = TRUE;
                }
                delta.y = y;
                delta.x = x;
                runto(&delta, &player.t_pos);
            }
        }
        break;
    case WS_ELECT:
    case WS_FIRE:
    case WS_COLD:
        {
            char dirch;
            char ch;
            char *name;
            bool bounced;
            bool used;
            struct coord pos;
            struct coord spotpos[BOLT_LENGTH];
            static struct object bolt = {
                "bolt",    /* Name */
                "",        /* Read text */
                '*',       /* Type */
                0,         /* Type */
                1,         /* Which of type */
                { 0 , 0 }, /* Position */
                0,         /* Probability */
                1,         /* Count of object */
                100,       /* Flags */
                0,         /* Worth */
                11,        /* Armor class */
                '\0',      /* Launcher */
                "",        /* Damage */
                "6d6",     /* Hurl damage */
                0,         /* Hit bonus */
                0          /* Damage bonus */
            };
            
            
            switch (delta.y + delta.x) {
            case 0:
                dirch = '/';
                break;
            case 1:
            case -1:
                if(delta.y == 0) {
                    dirch = '-';
                }
                else {
                    dirch = '|';
                }
                break;
            case 2:
            default:
                dirch = '\\';
            }

            pos = player.t_pos;
            bounced = FALSE;
            used = FALSE;
            if(obj->o_which == WS_ELECT) {
                name = "bolt";
            }
            else if(obj->o_which == WS_FIRE) {
                name = "flame";
            }
            else {
                name = "ice";
            }
            
            for(y = 0; y < BOLT_LENGTH && !used; ++y) {
                if(mvwinch(mw, pos.y, pos.y) == ' ') {
                    ch = mvwinch(stdscr, pos.y, pos.x) & A_CHARTEXT;
                }
                else {
                    ch = winch(mw);
                }

                spotpos[y] = pos;
                switch(ch) {
                case DOOR:
                case SECRETDOOR:
                case '|':
                case '-':
                case ' ':
                    bounced = TRUE;
                    delta.y = -delta.y;
                    delta.x = -delta.x;
                    y--;
                    msg("The bolt bounces");
                    break;
                default:
                    if(!bounced && isupper(ch)) {
                        if(!save_throw(VS_MAGIC, (struct thing *)find_mons(pos.y, pos.x)->l_data)) {
                            bolt.o_pos = pos;
                            hit_monster(pos.y, pos.x, &bolt);
                            used = TRUE;
                        }
                        else if((ch != 'M') || (show(pos.y, pos.x) == 'M')) {
                            if(terse) {
                                msg("%s misses", name);
                            }
                            else {
                                msg("The %s whizzes past the %s", name, monsters[ch - 'A'].t_name);
                            }
                            runto(&pos, &player.t_pos);
                        }
                    }
                    else if(bounced && (pos.y == player.t_pos.y) && (pos.x == player.t_pos.x)) {
                        bounced = FALSE;
                        
                        if(!save(VS_MAGIC)) {
                            if(terse) {
                                msg("The %s hits", name);
                            }
                            else {
                                msg("You are hit by the %s", name);
                            }

                            player.t_stats.s_hpt -= roll(6, 6);
                            if(player.t_stats.s_hpt <= 0) {
                                death('b');
                            }
                            used = TRUE;
                        }
                        else {
                            msg("The %s whizzes by you", name);
                        }
                    }
                    
                    mvwaddch(cw, pos.y, pos.x, dirch);
                    wrefresh(cw);
                }
                
                pos.y += delta.y;
                pos.x += delta.x;
            }
            
            for(x = 0; x < y; ++x) {
                mvwaddch(cw, spotpos[x].y, spotpos[x].x, show(spotpos[x].y, spotpos[x].x));
            }
            
            ws_know[obj->o_which] = TRUE;
        }
        break;
    default:
        msg("What a bizarre schtick!");
    }
    --obj->o_ac;

    return 0;
}

// drain:
//     do drain hit points from player attack
int drain(int ymin, int ymax, int xmin, int xmax)
{
    int i;
    int j;
    int count;
    struct thing *ick;
    struct linked_list *item;

    // First count how many things we need to spread the hit points among
    count = 0;
    for(i = ymin; i <= ymax; i++) {
        for(j = xmin; j <= xmax; j++) {
            if(isupper(mvwinch(mw, i, j))) {
                ++count;
            }
        }
    }
    if(count == 0) {
        msg("You have a tingling feeling");

        return 0;
    }
    count = player.t_stats.s_hpt / count;
    player.t_stats.s_hpt /= 2;

    // Now zot all of the monsters
    for(i = ymin; i <= ymax; ++i) {
        for(j = xmin; j <= xmax; ++j) {
            if(isupper(mvwinch(mw, i, j))) {
                item = find_mons(i, j);
                
                if(item != NULL) {
                    ick = (struct thing *)item->l_data;
                    ick->t_stats.s_hpt -= count;
                    
                    if(ick->t_stats.s_hpt < 1) {
                        killed(item, cansee(i, j) && ((ick->t_flags & ISINVIS) == 0));
                    }
                }
            }
        }
    }

    return 0;
}

// charge_str:
//     Chared a wand for wizards
char *charge_str(struct object *obj)
{
    static char buf[20];

    if(!(obj->o_flags & ISKNOW)) {
        buf[0] = '\0';
    }
    else if(terse) {
        sprintf(buf, " [%d]", obj->o_ac);
    }
    else {
        sprintf(buf, " [%d charges]", obj->o_ac);
    }
    
    return buf;
}
