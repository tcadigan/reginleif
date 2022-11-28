/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

struct edog {
    long hungrytime; /* At this time dog gets hungry */
    long eattime; /* Dog is eating */
    long droptime; /* Moment dog dropped object */
    unsigned int dropdist; /* Distance of dropped object from @ */
    unsigned int apport; /* Amount of training */
    unsigned int whistletime; /* Last time he whistled */
};

#define EDOG(mp) ((struct edog *)(&(mp->mextra[0])))
