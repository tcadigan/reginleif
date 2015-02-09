// Contains functions for dealing with things that happen in the future
//
// @(#)daemon.c 3.3 (Berkeley) 6/15/81

#include "daemon.h"

#include "io.h"
#include "rogue.h"

struct delayed_action d_list[20] = {
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, 
};

// d_slot:
//     Find an empty slot in the daemon/fuse list
struct delayed_action *d_slot()
{
    int i;
    struct delayed_action *dev;

    for(i = 0, dev = d_list; i < 20; ++i, ++dev) {
	if(dev->d_type == 0) {
	    return dev;
        }
    }

    if(wizard) {
        msg("Ran out of fuse slots");
    }

    return NULL;
}

// find_slot:
//     Find a particular slot in the table
struct delayed_action *find_slot(int (*func)())
{
    int i;
    struct delayed_action *dev;

    for(i = 0, dev = d_list; i < 20; ++i, ++dev) {
	if((dev->d_type != 0) && (func == dev->d_func)) {
	    return dev;
        }
    }
    
    return NULL;
}

// start_daemon:
//     Start a daemon, takes a function
int start_daemon(int (*func)(), int arg, int type)
{
    struct delayed_action *dev;

    dev = d_slot();
    dev->d_type = type;
    dev->d_func = func;
    dev->d_arg = arg;
    dev->d_time = -1;

    return 0;
}

// kill_daemon:
//     Remove a daemon from the list
int kill_daemon(int (*func)()) 
{
    struct delayed_action *dev;

    dev = find_slot(func);
    
    if(dev == NULL) {
	return 0;
    }

    // Take it out of the list
    dev->d_type = 0;

    return 0;
}

// do_daemons:
//     Run all the daemons that are active with the current flag,
//     passing the argument to the function
int do_daemons(int flag)
{
    struct delayed_action *dev;

    // Loop through the devil list
    for(dev = d_list; dev < &d_list[20]; ++dev) {
        // Executing each one, giving it the proper arguments
	if((dev->d_type == flag) && (dev->d_time == -1)) {
	    (*dev->d_func)(dev->d_arg);
        }
    }
    
    return 0;
}

// fuse:
//     Start a fuse to go off in a certain number of turns
int fuse(int (*func)(), int arg, int time, int type)
{
    struct delayed_action *wire;

    wire = d_slot();
    wire->d_type = type;
    wire->d_func = func;
    wire->d_arg = arg;
    wire->d_time = time;

    return 0;
}

// lengthen:
//     Increase the time until a fuse goes off
int lengthen(int (*func)(), int xtime)
{
    struct delayed_action *wire;

    wire = find_slot(func);
    
    if(wire == NULL) {
	return 0;
    }
    
    wire->d_time += xtime;

    return 0;
}

// extinguish:
//     Put out a fuse
int extinguish(int (*func)())
{
    struct delayed_action *wire;

    wire = find_slot(func);
    
    if(wire == NULL) {
	return 0;
    }
    
    wire->d_type = 0;

    return 0;
}

// do_fuses:
//     Decrement counters and start needed fuses
int do_fuses(int flag)
{
    struct delayed_action *wire;

    // Step through the list
    for(wire = d_list; wire < &d_list[20]; ++wire) {
        // Decrementing counters and starting things we want. We also need
        // to remove the fuse from the list once it has gone off.
	if((flag == wire->d_type) && (wire->d_time > 0)) {
            --wire->d_time;
            
            if(wire->d_time == 0) {
                wire->d_type = 0;
                (*wire->d_func)(wire->d_arg);
            }
        }
    }
    
    return 0;
}
