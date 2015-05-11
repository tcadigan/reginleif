/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#define BILLSZ 200

struct bill_x {
    unsigned int bo_id;
    unsigned int useup:1;
    unsigned int bquan:7;
    unsigned int price; /* Price per unit */
};

struct eshk {
    long int robbed; /* Amount stolen by most recent customer */
    schar shoproom; /* Index in rooms; set by inshop() */
    coord shk; /* Usual position shopkeeper */
    coord shd; /* Position shop door */
    int billct;
    struct bill_x bill[BILLSZ];
    int visitct; /* Number of visits by most recent customer */
    char customer[PL_NSIZ]; /* Most recent customer */
    char shknam[PL_NSIZ];
};
