#ifndef HACK_U_INIT_H_
#define HACK_U_INIT_H_

#include "config.h" /* schar, uchar, Bitfield */

struct trobj {
    uchar trotyp;
    schar trspe;
    char trolet;
    Bitfield(trquan, 6);
    Bitfield(trknown, 1);
};

void u_init();
void plnamesuffix();
void setpl_char(char *plc);
void wiz_inv();
void ini_inv(struct trobj *trop);

#endif
