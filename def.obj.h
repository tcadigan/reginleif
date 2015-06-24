/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

struct obj {
    struct obj *nobj;
    unsigned int o_id;
    unsigned int o_cnt_id; /* Id of container object is in */
    xchar ox;
    xchar oy;
    xchar odx;
    xchar ody;
    uchar owt;
    unsigned int quan; /* Small in general but large in case of gold */
    schar spe;
    char olet;
    Bitfield(oinvis, 1);
    Bitfield(odispl, 1);
    Bitfield(known, 1); /* Exact nature known */
    Bitfield(dknown, 1); /* Color or text known */
    Bitfield(cursed, 1);
    Bitfield(unpaid, 1); /* On some bill */
    Bitfield(rustfree, 1);
    Bitfield(onamelth, 6);
    long age; /* Creation date */
    long owornmask;
#define W_ARM 01L
#define W_ARM2 02L
#define W_ARMH 04L
#define W_ARMS 010L
#define W_ARMG 020L
#define W_ARMOR (W_ARM | W_ARM2 | W_ARMH | W_ARMS | W_ARMG)
#define W_RINGL 010000L /* Make W_RINGL = RING_LEFT (see uprop) */
#define W_RINGR 020000L
#define W_RING (W_RINGL | W_RINGR)
#define W_WEP 01000L
#define W_BALL 02000L
#define W_CHAIN 04000L
    long oextra[1];
};

extern struct obj *fobj;

#define newobj(xl) (struct obj *)alloc((unsigned int)(xl) + sizeof(struct obj))

#define ONAME(otmp) ((char *)otmp->oextra)
