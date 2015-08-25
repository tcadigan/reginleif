#include "constants.h"
#include "config.h"

int set_1_2(int element)
{
    if((element == 1) || (element == 2)) {
        return TRUE;
    }

    return FALSE;
}

int set_1_2_4(int element)
{
    if((element == 1) || (element == 2) || (element == 4)) {
        return TRUE;
    }

    return FALSE;
}

int set_4(int element)
{
    if(element == 4) {
        return TRUE;
    }

    return FALSE;
}

int set_corrodes(int element)
{
    switch(element) {
    case 22:
    case 23:
    case 34:
    case 35:
    case 65:
        
        return TRUE;
    }

    return FALSE;
}

int set_flammable(int element)
{
    switch(element) {
    case 12:
    case 20:
    case 21:
    case 22:
    case 30:
    case 31:
    case 32:
    case 36:
    case 55:
    case 70:
    case 71:
        
        return TRUE;
    }

    return FALSE;
}

int set_frost_destroy(int element)
{
    int element;

    if((element == 75) || (element == 76)) {
        return TRUE;
    }

    return FALSE;
}

int set_acid_affect(int element)
{
    switch(element) {
    case 1:
    case 2:
    case 11:
    case 12:
    case 20:
    case 21:
    case 22:
    case 30:
    case 31:
    case 32:
    case 36:

        return TRUE;
    }

    return FALSE;
}

int set_floor(int element)
{
    switch(element) {
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
        
        return TRUE;
    }

    return FALSE;
}

int set_lightning_destroy(int element)
{
    switch(element) {
    case 45:
    case 60:
    case 65:

        return TRUE;
    }

    return FALSE;
}

/* ARGSUSED */
/* To shut up lint about unused arguments */
int set_null(int element)
{
    return FALSE;
}

int set_acid_destroy(int element)
{
    switch(element) {
    case 12:
    case 20:
    case 21:
    case 22:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 55:
    case 70:
    case 71:
    case 80:
    case 104:
    case 105:

        return TRUE;
    }

    return FALSE;
}

int set_fire_destroy(int element)
{
    switch(element) {
    case 12:
    case 20:
    case 21:
    case 22:
    case 30:
    case 31:
    case 32:
    case 36:
    case 55:
    case 70:
    case 71:
    case 75:
    case 76:
    case 80:
    case 104:
    case 105:

        return TRUE;
    }

    return FALSE;
}

int general_store(int element)
{
    switch(element) {
    case 13:
    case 15:
    case 25:
    case 30:
    case 32:
    case 77:
    case 80:

        return TRUE;
    }

    return FALSE;
}

int armory(int element)
{
    switch(element) {
    case 30:
    case 31:
    case 33:
    case 34:
    case 35:
    case 36:

        return TRUE;
    }

    return FALSE;
}

int weaponsmith(int element)
{
    switch(element) {
    case 10:
    case 11:
    case 12:
    case 20:
    case 21:
    case 22:
    case 23:

        return TRUE;
    }

    return FALSE;
}

int temple(int element)
{
    switch(element) {
    case 21:
    case 70:
    case 75:
    case 76:
    case 91:

        return TRUE;
    }

    return FALSE;
}

int alchemist(int element)
{
    switch(element) {
    case 70:
    case 71:
    case 75:
    case 76:

        return TRUE;
    }

    return FALSE;
}

int magic_shop(int element)
{
    switch(element) {
    case 40:
    case 45:
    case 55:
    case 65:
    case 70:
    case 71:
    case 75:
    case 76:
    case 90:

        return TRUE;
    }

    return FALSE;
}

int set_wall(int element)
{
    switch(element) {
    case 10:
    case 11:
    case 12:

        return TRUE;
    }

    return FALSE;
}

int set_pwall(int element)
{
    switch(element) {
    case 10:
    case 11:
    case 12:
    case 15:

        return TRUE;
    }

    return FALSE;
}

int set_corr(int element)
{
    if((element == 4) || (element == 5)) {
        return TRUE;
    }

    return FALSE;
}

int set_trap(int element)
{
    switch(element) {
    case 101:
    case 102:
    case 109:
    case 110:

        return TRUE;
    }

    return FALSE;
}

int set_light(int element)
{
    switch(element) {
    case 102:
    case 103:
    case 104:
    case 105:
    case 107:
    case 108:
    case 109:
    case 110:

        return TRUE;
    }

    return FALSE;
}
