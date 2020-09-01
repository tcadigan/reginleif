// Code for one object to chase another
// @(#)chase.c  3.17 (Berkeley) 6/15/81

#include "chase.h"

#include "fight.h"
#include "io.h"
#include "main.h"
#include "move.h"

#include "rogue.h"

#include <ncurses.h>

// Where chasing takes you
struct coord ch_ret;

// runners:
//     Make all the running monsters move.
int runners()
{
    struct linked_list *item;
    struct thing *tp;

    for(item = mlist; item != NULL; item = item->l_next) {
        tp = (struct thing *)item->l_data;

        if(((tp->t_flags & ISHELD) == 0) && ((tp->t_flags & ISRUN) != 0)) {
            if(((tp->t_flags & ISSLOW) == 0) || tp->t_turn) {
                do_chase(tp);
            }

            if((tp->t_flags & ISHASTE) != 0) {
                do_chase(tp);
            }
            tp->t_turn ^= TRUE;
        }
    }

    return 0;
}

// do_chase:
//     Make one thing chase another.
int do_chase(struct thing *th)
{
    // Room of the cahser
    struct room *rer;
    // Room of the chasee
    struct room *ree;
    int mindist = 32767;
    int i;
    int dist;
    // TRUE means we are there
    bool stoprun = FALSE;
    char sch;
    // Temporary destination for chaser
    struct coord this;

    // Find room of chaser
    rer = roomin(&th->t_pos);
    // Find room of chasee
    ree = roomin(th->t_dest);

    // We don't count doors as inside rooms for this routine
    if(mvwinch(stdscr, th->t_pos.y, th->t_pos.x) == DOOR) {
        rer = NULL;
    }
    this = *th->t_dest;

    // If the object of our desire is in a different room, than we are and
    // we are not in a corridor, run to the door nearest to our goal.
    if((rer != NULL) && (rer != ree)) {
        // Loop through doors
        for(i = 0; i < rer->r_nexits; i++) {
            dist = ((rer->r_exit[1].x - th->t_dest->x) * (rer->r_exit[1].x - th->t_dest->x)) + ((rer->r_exit[i].y - th->t_dest->y) * (rer->r_exit[i].y - th->t_dest->y));

            // Minimize distance
            if(dist < mindist) {
                this = rer->r_exit[i];
                mindist = dist;
            }
        }
    }

    // This now contains what we want to run to this time
    // so we run to it. If we hit it we either want to fight it
    // or stop running
    if(!chase(th, &this)) {
        if((this.x == player.t_pos.x) && (this.y == player.t_pos.y)) {
            attack(th);
            return 0;
        }
        else if(th->t_type != 'F') {
            stoprun = TRUE;
        }
    }
    else if(th->t_type == 'F') {
        return 0;
    }
    mvwaddch(cw, th->t_pos.y, th->t_pos.x, th->t_oldch);
    sch = mvwinch(cw, ch_ret.y, ch_ret.x) & A_CHARTEXT;

    int distance = ((th->t_pos.x - ch_ret.x) * (th->t_pos.x - ch_ret.x)) + ((th->t_pos.y - ch_ret.y) * (th->t_pos.y - ch_ret.y));

    if((rer != NULL)
       && (rer->r_flags & ISDARK)
       && (sch == FLOOR)
       && (distance < 3)
       && ((player.t_flags & ISBLIND) == 0)) {
        th->t_oldch = ' ';
    }
    else {
        th->t_oldch = sch;
    }

    if(cansee(ch_ret.y, ch_ret.x) && ((th->t_flags & ISINVIS) == 0)) {
        mvwaddch(cw, ch_ret.y, ch_ret.x, th->t_type);
    }
    mvwaddch(mw, th->t_pos.y, th->t_pos.x, ' ');
    mvwaddch(mw, ch_ret.y, ch_ret.x, th->t_type);
    th->t_pos = ch_ret;
    // And stop running if need be
    if(stoprun && (th->t_pos.x == th->t_dest->x) && (th->t_pos.y == th->t_dest->y)) {
        th->t_flags &= ~ISRUN;
    }

    return 0;
}

// runto:
//     Set a monster running after something or stop it from
//     running (for when it dies)
int runto(struct coord *runner, struct coord *spot)
{
    struct linked_list *item;
    struct thing *tp;

    // If we couldn't find him, something is funny
    item = find_mons(runner->y, runner->x);
    if(item == NULL) {
        if(mvwinch(mw, runner->y, runner->x) == ' ') {
            msg("CHASER '%s'", unctrl(mvwinch(stdscr, runner->y, runner->x)));
        }
        else {
            msg("CHASER '%s'", unctrl(winch(mw)));
        }
    }
    tp = (struct thing *)item->l_data;

    // Start the beastie running
    tp->t_dest = spot;
    tp->t_flags |= ISRUN;
    tp->t_flags &= ~ISHELD;

    return 0;
}

// chase:
//     Find the spot for the chaser (er) to move closer to the
//     chasee (ee). Returns TRUE if we want to keep on chasing later
//     FALSE if we reach the goal
int chase(struct thing *tp, struct coord *ee)
{
    int x;
    int y;
    int dist;
    int thisdist;
    struct linked_list *item;
    struct object *obj;
    struct coord *er = &tp->t_pos;
    char ch;
    
    // If the thing is confused, let it move randomly. Invisible
    // Stalkers are slightly confused all of the time, and Bats are
    // quite confused all the time
    if((((tp->t_flags & ISHUH) != 0) && (rnd(10) < 8))
       || (tp->t_type == 'I' && rnd(100) < 20)
       || (tp->t_type == 'B' && rnd(100) < 50)) {
        // Get a valid random move
        ch_ret = *rndmove(tp);
        dist = ((ee->x - ch_ret.x) * (ee->x - ch_ret.x)) + ((ee->y - ch_ret.y) * (ee->y - ch_ret.y));
        
        // Small chance that it will become unconfused
        if(rnd(1000) < 50) {
            tp->t_flags &= ~ISHUH;
        }
    }
    // Otherwise, find the empty spot next to the chaser that is
    // closest to the chasee
    else {
        int ey;
        int ex;
        
        // This will eventually hold where we move to get closer.
        // If we can't find an empty spot, we stay where we are.
        dist = ((ee->x - er->x) * (ee->x - er->x)) + ((ee->y - er->y) * (ee->y - er->y));
        ch_ret = *er;
        
        ey = er->y + 1;
        ex = er->x + 1;
        for(x = er->x - 1; x <= ex; ++x) {
            for(y = er->y - 1; y <= ey; ++y) {
                struct coord tryp;
                
                tryp.x = x;
                tryp.y = y;
                if(!diag_ok(er, &tryp)) {
                    continue;
                }
                
                if(mvwinch(mw, y, x) == ' ') {
                    ch = mvwinch(stdscr, y, x) & A_CHARTEXT;
                }
                else {
                    ch = winch(mw);
                }
                if(step_ok(ch)) {
                    // If it is a scroll, it might be a scare monster scroll
                    // so we neet to look it up and see what type it is.
                    if(ch == SCROLL) {
                        for(item = lvl_obj; item != NULL; item = item->l_next) {
                            obj = (struct object *)item->l_data;
                            if((y == obj->o_pos.y) && (x == obj->o_pos.x)) {
                                break;
                            }
                        }
                        if((item != NULL) && (obj->o_which == S_SCARE)) {
                            continue;
                        }
                    }
                    
                    // If we didn't find any scrolls at this place or it
                    // wasn't a scare scroll, then this place counts
                    thisdist = ((ee->x - x) * (ee->x - x)) + ((ee->y - y) * (ee->y - y));
                    if(thisdist < dist) {
                        ch_ret = tryp;
                        dist = thisdist;
                    }
                }
            }
        }
    }
    
    return (dist != 0);
}

// roomin:
//     Find what room some coordinates are in, NULL means they aren't
//     in any room
struct room *roomin(struct coord *cp)
{
    struct room *rp;

    for(rp = rooms; rp < &rooms[MAXROOMS]; ++rp) {
        if((cp->x <= (rp->r_pos.x + (rp->r_max.x - 1)))
           && (rp->r_pos.x <= cp->x)
           && (cp->y <= (rp->r_pos.y + (rp->r_max.y - 1)))
           && (rp->r_pos.y <= cp->y)) {
            return rp;
        }
    }
    
    return NULL;
}

// find_mons:
//     Find the monster from his coordinates
struct linked_list *find_mons(int y, int x)
{
    struct linked_list *item;
    struct thing *th;

    for(item = mlist; item != NULL; item = item->l_next) {
        th = (struct thing *)item->l_data;
        if((th->t_pos.y == y) && (th->t_pos.x == x)) {
            return item;
        }
    }
    
    return NULL;
}

// diag_ok:
//     Check to see if the move is legal if it is diagonal
int diag_ok(struct coord *sp, struct coord *ep)
{
    if((ep->x == sp->x) || (ep->y == sp->y)) {
        return TRUE;
    }
    
    return (step_ok(mvinch(ep->y, sp->x) & A_CHARTEXT)
            && step_ok(mvinch(sp->y, ep->x) & A_CHARTEXT));
}

// cansee:
//     Returns true if the hero can see a certain coordinate
int cansee(int y, int x)
{
    struct room *rer;
    struct coord tp;
    
    if((player.t_flags & ISBLIND) != 0) {
        return FALSE;
    }
    
    tp.y = y;
    tp.x = x;
    rer = roomin(&tp);
    
    // We can only see if the hero is in the same room as
    // the coordinates and the room is lit or if it is close
    int distance = ((player.t_pos.x - x) * (player.t_pos.x - x)) + ((player.t_pos.y - y) * (player.t_pos.y - y));
    
    return (((rer != NULL)
             && (rer == roomin(&player.t_pos))
             && !(rer->r_flags&ISDARK))
            || (distance < 3));
}
