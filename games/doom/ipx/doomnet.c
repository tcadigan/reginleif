#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <dos.h>

#include "doomnet.h"
#include "ipxstr.h"
// #include "ipx_frch.h" // French version

doomcom_t doomcom;
int vectorishooked;
void interrupt (*olddoomvect) (void);

/*
 * =============
 * =
 * = LaunchDOOM
 * =
 * These field in doomcom should be filled in before calling:
 *     short numnodes;      // Console is always node 0
 *     short ticdump;       // 1 = No duplication
 *                          // 2-5 = Duplication for slow networks
 *     short extratics;     // 1 = Send a backup tic in every packet
 *
 *     short consoleplayer; // 0-3 = Player number
 *     short numplayers;    // 1-4
 *     short angleoffset;   // 1 = Left, 0 = Center, -1 = Right
 *     short drone;         // 1 = Drone
 * =============
 */

void LaunchDOOM(void)
{
    char *newargs[99];
    char adrstring[10];
    long flatadr;

    // Prepare for DOOM
    doomcom.id = DOOMCOM_ID;

    // Hook the interrupt vector
    olddoomvect = getvect(doomcom.intnum);
    setvect(doomcom.intnum, (void interrupt(*)(void))MK_FP(_CS, (int)NetISR));
    vectorishooked = 1;

    // Build the argument list for DOOM, adding a -new &doomcom
    memcpy(newargs, _argv, (_argc + 1) * 2);
    newargs[_argc] = "-net";
    flatadr = ((long)_DS * 16) + (unsigned)&doomcom;
    sprintf(adrstring, "%lu", flatadr);
    newargs[_argc + 1] = adrstring;
    newargs[_argc + 2] = NULL;

    if (!access("doom2.exe", 0)) {
        spawnv(P_WAIT, "doom2", newargs);
    } else {
        spawnv(P_WAIT, "doom", newargs);
    }

#ifdef DOOM2
    printf(STR_RETURNED"\n");
#else
    printf("Returned from DOOM\n");
#endif
}
