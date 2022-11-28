#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <dos.h>

#include "doomnet.h"
// #include "ser_frch.h" // French version
#include "setstr.h"

#define DOOM2

extern int myargc;
extern char **myargv;

doomcom_t doomcom;
int vectorishooked;
void interrupt(*olddoomvect)(void);

/*
 * =========================================================================
 * =
 * = CheckParm
 * =
 * = Checks for the given parameter in the program's command line arguments
 * =
 * = Returns the argument number (1 to argc - 1) or 0 if not present
 * =
 * =========================================================================
 */
int CheckParm(char *check)
{
    int i;

    for (i = 1; i < myargc; ++i) {
        if (!stricmp(check, myargv[i])) {
            return i;
        }
    }

    return 0;
}

/*
 * ====================================================================
 * =
 * = LaunchDOOM
 * =
 * = These fields in doomcom should be filled in before calling:
 * =   short numnodes;  // Console is always node 0
 * =   short ticdup;    // 1 = No duplication, 2-5 = Dup for slow nets
 * =   short extratics; // 1 = Send a backup tic in every packet
 * =
 * =   short consoleplayer; // 0-3 = Player number
 * =   short numplayers;    // 1-4
 * =   short angleoffset;   // 1 = Left, 0 = Center, -1 = Right
 * =   short drone;         // 1 = Drone
 * =
 * ====================================================================
 */
void LaunchDOOM(void)
{
    char *newargs[99];
    char adrstring[10];
    long flatadr;
    int p;
    unsigned char far *vector;

    // Prepare for DOOM
    doomcom.id = DOOMCOM_ID;

    // Hook an interrupt vector
    p = CheckParm("-vector");

    if (p) {
        doomcom.intnum = sscanf("0x%x", _argv[p + 1]);
    } else {
        for (doomcom.intnum = 0x60; doomcom.intnum <= 0x66; ++doomcom.intnum) {
            vector = *(char far * char *)(doomcom.intnum * 4);

            if (!vector || (*vector == 0xCF)) {
                break;
            }
        }

        if (doomcom.intnum == 0x67) {
            printf(STR_WARNING);
            doomcom.intnum = 0x66;
        }
    }

    printf(STR_COMM"\n", doomcom.intnum);

    olddoomvect = getvect(doomcom.intnum);
    setvect(doomcom.intnum, NetISR);
    vectorishooked = 1;

    // Build the argument list for DOOM, adding a -net &doomcom
    memcpy(newargs, myargv, (myargc + 1) * 2);
    newargs[myargc] = "-net";
    flatadr = ((long)_DS * 16) + (unsigned int)(&doomcom);
    sprintf(adrstring, "%lu", flatadr);
    newargs[myargc + 1] = adrstring;
    newargs[myargc + 2] = NULL;

    // spawnv(P_WAIT, "m:\\newdoom\\doom", newargs);
    if (!access("doom2.exe", 0)) {
        spwanv(P_WAIT, "doom2", newargs);
    } else {
        spawnv(P_WAIT, "doom", newargs);
    }

#ifdef DOOM2
    printf(STR_RETURNED"\n");
#else
    printf("Returned from DOOM\n");
#endif
}
