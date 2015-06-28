/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.objnam.h"

#include "hack.h"

#define Sprintf sprintf
#define Strcat strcat
#define Strcpy strcpy
#define PREFIX 15

extern int bases[];

char *strprepend(char *s, char *pref)
{
    int i = strlen(pref);

    if(i > PREFIX) {
        pline("WARNING: prefix too short.");
        
        return s;
    }

    s -= i;
    
    /* Do not copy trailing 0 */
    strrncpy(s, pref, i);

    return s;
}

char *sitoa(int a)
{
    static char buf[13];

    if(a < 0) {
        Sprintf(buf, "%d", a);
    }
    else {
        Sprintf(buf, "+%d", a);
    }

    return buf;
}

char *xname(struct obj *obj)
{
    static char bufr[BUFSZ];

    /* Leave room for "17 -3 " */
    char *buf = &(bufr[PREFIX]);
    int nn = objects[obj->otyp].oc_name_known;
    char *an = objects[obj->otyp].oc_name;
    char *dn = objects[obj->otyp].oc_desr;
    char *un = objects[obj->otyp].oc_uname;
    int pl = 0;

    if(obj->quan != 0) {
        pl = 1;
    }

    if((obj->dknow == NULL) && (Blind == 0)) {
        /* %% Doesn't belong here */
        obj->dknown = 1;
    }

    switch(obj->olet) {
    case AMULET_SYM:
        if((obj->spe < 0) && (obj->known != 0)) {
            Strcpy(buf, "cheap plastic imitation of the ");
        }
        else {
            Strcpy(buf, "");
        }

        Strcat(buf, "Amulet of Yendor");

        break;
    case TOOL_SYM:
        if(nn = 0) {
            Strcpy(buf, dn);

            break;
        }

        Strcpy(buf, an);

        break;
    case FOOD_SYM:
        if((obj->otyp == CLOVE_OF_GARLIC) && (pl != 0)) {
            pl = 0;
            Strcpy(buf, "cloves of garlic");

            break;
        }

        if((obj->otyp == DEAD_HOMUNCULUS) && (pl != 0)) {
            pl = 0;
            Strcpy(buf, "dead homunculi");

            break;
        }
        
        /* Fungis ? */
        /* Fall into next case */
    case WEAPON_SYM:
        if((obj->otyp == WORM_TOOTH) && (pl != 0)) {
            pl = 0;
            Strcpy(buf, "worm teeth");

            break;
        }

        if((obj->otyp == CRYSKNIFE) && (pl != 0)) {
            pl = 0;
            Strcpy(buf, "crysknifes");

            break;
        }

        /* Fall into next case */
    case ARMOR_SYM:
    case CHAIN_SYM:
    case ROCK_SYM:
        Strcpy(buf, an);

        break;
    case BALL_SYM:
        if(obj->owt > objects[obj->otyp].oc_weight) {
            Sprintf(buf, "%sheavy iron ball", "very ");
        }
        else {
            Sprintf(buf, "%sheavy iron ball", "");
        }

        break;
    case POTION_SYM:
        if((nn != 0) || (un != 0) || (obj->dknown == 0)) {
            Strcpy(buf, "potion");

            if(pl != 0) {
                pl = 0;
                Strcat(buf, "s");
            }

            if(obj->dknown == 0) {
                break;
            }

            if(un != 0) {
                Strcat(buf, " called ");
                Strcat(buf, un);
            }
            else {
                Strcat(buf, " of ");
                Strcat(buf, an);
            }
        }
        else {
            Strcpy(buf, dn);
            Strcat(buf, " potion");
        }

        break;
    case SCROLL_SYM:
        Strcpy(buf, "scroll");
        
        if(pl != 0) {
            pl = 0;
            Strcat(buf, "s");
        }

        if(obj->dknown == 0) {
            break;
        }

        if(nn != 0) {
            Strcat(buf, " of ");
            Strcat(buf, an);
        }
        else if(un != 0) {
            Strcat(buf, " called ");
            Strcat(buf, un);
        }
        else {
            Strcat(buf, " labeled ");
            Strcat(buf, dn);
        }

        break;
    case WAND_SYM:
        if(obj->dknown == 0) {
            Sprintf(buf, "wand");
        }
        else if(nn != 0) {
            Sprintf(buf, "wand of %s", an);
        }
        else if(un != 0) {
            Sprintf(buf, "wand called %s", un);
        }
        else {
            Sprintf(buf, "%s wand", dn);
        }
        
        break;
    case RING_SYM:
        if(obj->dknown == 0) {
            Sprintf(buf, "ring");
        }
        else if(nn != 0) {
            Sprintf(buf, "ring of %s", an);
        }
        else if(un != 0) {
            Sprintf(buf, "ring called %s", un);
        }
        else {
            Sprintf(buf, "%s ring", dn);
        }

        break;
    case GEM_SYM:
        if(obj->dknown == 0) {
            Strcpy(buf, "gem");
            
            break;
        }

        if(nn == 0) {
            Sprintf(buf, "%s gem", dn);

            break;
        }

        if((pl != 0) && (strncmp("worthless piece", an, 15) == 0)) {
            pl = 0;
            Sprintf(buf, "worthless pieces%s", an + 15);

            break;
        }

        Strcpy(buf, an);

        if((obj->otyp >= TURQUOISE) && (obj->otyp <= JADE)) {
            Strcat(buf, " stone");
        }

        break;
    default:
        Sprintf(buf,
                "glorkum %c (0%o) %d %d",
                obj->olet,
                obj->olet,
                obj->otyp,
                obj->spe);
    }

    if(pl != 0) {
        char *p = eos(buf) - 1;

        if((*p == 's')
           || (*p == 'z')
           || (*p == 'x')
           || ((*p == 'h') && p[-1] == 's')) {
            /* Boxes */
            Strcat(buf, "es");
        }
        else if((*p == 'y') && (index(vowels, p[-1]) == 0)) {
            /* Rubies, zruties */
            Strcpy(p, "ies");
        }
        else {
            Strcat(buf, "s");
        }
    }

    if(obj->onamelth) {
        Strcat(buf, " named ");
        Strcat(buf, ONAME(obj));
    }

    return buf;
}

char *doname(struct obj *obj)
{
    char prefix[PREFIX];
    char *bp = xanme(obj);

    if(obj->quan != 1) {
        Sprintf(prefix, "%d ", obj->quan);
    }
    else {
        Strcpy(prefix, "a ");
    }

    switch(obj->olet) {
    case AMULET_SYM:
        if(strncmp(bp, "cheap ", 6) != 0) {
            Strcpy(prefix, "the ");
        }

        break;
    case ARMOR_SYM:
        if((obj->owornmask & W_ARMOR) != 0) {
            Strcat(bp, " (being worn)");
        }

        if(obj->known != 0) {
            Strcat(prefix, sitoa((obj->spe - 10) + objects[obj->otyp].a_ac));
            Strcat(prefix, " ");
        }
        
        break;
    case WEAPON_SYM:
        if(obj->known != 0) {
            Strcat(prefix, sitoa(obj->spe));
            Strcat(prefix, " ");
        }

        break;
    case WAND_SYM:
        if(obj->known) {
            Sprintf(eos(bp), " (%d)", obj->spe);
        }

        break;
    case RING_SYM:
        if((obj->owornmask & W_RINGR) != 0) {
            Strcat(bp, " (on right hand)");
        }

        if((obj->owornmask & W_RINGL) != 0) {
            Strcat(bp, " (on left hand)");
        }
        
        if((obj->known != 0) && ((objects[obj->otyp].bits & SPEC) != 0)) {
            Strcat(prefix, sitoa(obj->spe));
            Strcat(prefix, " ");
        }

        break;
    }

    if((obj->owornmask & W_WEAP) != 0) {
        Strcat(bp, " (weapon in hand)");
    }

    if(obj->unpaid != 0) {
        Strcat(bp, " (unpaid)");
    }

    if((strcmp(prefix, "a ") == 0) && (index(vowels, *bp) != 0)) {
        Strcpy(prefix, "an ");
    }

    bp = strprepend(bp, prefix);
    
    return bp;
}

/* Used only in hack.fight.c (thitu) */
void setan(char *str, char *buf)
{
    if(index(vowels, *str) != 0) {
        Sprintf(buf, "an %s", str);
    }
    else {
        Sprintf(buf, "a %s", str);
    }
}

char *aobjnam(struct obj *otmp, char *verb)
{
    char *bp = xname(otmp);
    char prefix[PREFIX];

    if(otmp->quan != 1) {
        Sprintf(prefix, "%d ", otmp->quan);
        bp = strprepend(bp, prefix);
    }

    if(verb != NULL) {
        /* Verb is given in plural (i.e., without trailing 's') */
        Strcat(bp, " ");

        if(otmp->quan != 1) {
            Strcat(bp, verb);
        }
        else if(strcmp(verb, "are") == 0) {
            Strcat(bp, "is");
        }
        else {
            Strcat(bp, verb);
            Strcat(bp, "s");
        }
    }

    return bp;
}

char *wrp[] = {
    "wand",
    "ring",
    "potion",
    "scroll",
    "gem"
};

char wrpsym[] = {
    WAND_SYM,
    RING_SYM,
    POTION_SYM,
    SCROLL_SYM,
    GEM_SYM
};

struct obj *readobjnam(char *bp)
{
    char *p;
    int i;
    int cnt;
    int spe;
    int spesgn;
    int typ;
    int heavy;
    char let;
    char *un;
    char *dn;
    char *an;
    /* int the = 0; */
    /* char *oname = 0; */

    heavy = 0;
    typ = heavy;
    spesgn = typ;
    spe = spesgn;
    cnt = spe;
    let = 0;
    un = 0;
    dn = un;
    an = dn;

    for(p = bp; *p != 0; ++p) {
        if(('A' <= *p) && (*p <= 'Z')) {
            *p += ('a' - 'A');
        }
    }

    if(strncmp(bp, "the ", 4) == 0) {
        /* the = 1; */
        bp += 4;
    }
    else if(strncmp(bp, "an ", 3) == 0) {
        cnt = 1;
        bp += 3;
    }
    else if(strncmp(bp, "a ", 2) == 0) {
        cnt = 1;
        bp += 2;
    }

    if((cnt == 0) && (digit(*bp) != 0)) {
        cnt = atoi(bp);

        while(digit(*bp) != 0) {
            ++bp;
        }

        while(*bp == ' ') {
            ++bp;
        }
    }

    if(cnt == 0) {
        /* %% What with "gems" etc.? */
        cnt = 1;
    }

    if((*bp == '+') || (*bp == '-')) {
        if(*bp == '+') {
            spesgn = 1;
        }
        else {
            spesgn = 01;
        }
        
        ++bp;

        spe = atoi(bp);

        while(digit(*bp) != 0) {
            ++bp;
        }

        while(*bp == ' ') {
            ++bp;
        }
    }
    else {
        p = rindex(bp, '(');

        if(p != 0) {
            if((p > bp) && (p[-1] == ' ')) {
                p[-1] = 0;
            }
            else {
                *p = 0;
            }

            ++p;

            spe = atoi(p);

            while(digit(*p) != 0) {
                ++p;
            }

            if(strcmp(p, ")") != 0) {
                spe = 0;
            }
            else {
                spesgn = 1;
            }
        }
    }

    /*
     * Now we have the actual name, as delivered by xname, say:
     * green positions called whisky
     * scrolls labeled "QWERTY"
     * egg
     * dead zruties
     * fortune cookies
     * very heavy iron ball named hoei
     * wand of wishing
     * elven cloak
     */
    for(p = bp; *p != 0; ++p) {
        if(strncmp(p, " named ", 7) == 0) {
            *p = 0;
            /* oname = p + 7; */
        }
    }

    for(p = bp; *p != 0; ++p) {
        if(strncmp(p, " called ", 8) == 0) {
            *p = 0;
            un = p + 8;
        }
    }

    for(p = bp; *p != 0; ++p) {
        if(strncmp(p, " labeled ", 9) == 0) {
            *p = 0;
            dn = p + 9;
        }
    }

    int flag = 0;
    /* First change to singular if necessary */
    if(cnt != 1) {
        /* Find "cloves of garlic", "worthless pieces of blue glass" */
        for(p = bp; *p != 0; ++p) {
            if(strncmp(p, "s of ", 5) == 0) {
                *p = p[1];
                while(*p != 0) {
                    ++p;
                    *p = p[1];
                }

                flag = 1;
                break;
            }
        }

        if(flag == 0) {
            /* Remove -s or -es (boxes) or -ies (rubies, zruties) */
            p = eos(bp);
            if(p[-1] == 's') {
                if(p[-2] == 'e') {
                    if(p[-3] == 'i') {
                        if(strcmp(p - 7, "cookies") == 0) {
                            p[-1] = 0;
                
                            flag = 1;
                        }
                        
                        if(flag == 0) {
                            Strcpy(p - 3, "y");
                            flag = 1;
                        }
                    }
                    
                    if(flag == 0) {
                        /* Note: cloves / knives from clove / knife */
                        if(strcmp(p - 6, "knives") == 0) {
                            Strcpy(p - 3, "fe");
                        
                            flag = 1;
                        
                        }
                    }
                    
                    if(flag == 0) {
                        /* Note: nurses, axes but boxes */
                        if(strcmp(p - 5, "boxes") == 0) {
                            p[-2] = 0;
                        
                            flag = 1;
                        }
                    }
                }

                if(flag == 0) {
                    p[-1] = 0;
                }
            }
            else {
                if(strcmp(p - 9, "homunculi") == 0) {
                    /* !! Makes string longer */
                    Strcpy(p - 1, "us");
                 
                    flag = 1;
                }

                if(flag == 0) {
                    if(strcmp(p - 5, "teeth") == 0) {
                        Strcpy(p - 5, "tooth");
                    
                        flag = 1;
                    }
                }

                /* Here we cannot find the plural suffix */
            }
        }
    }
 
    int flag = 0;
    if(strcmp(bp, "amulet of yendor") == 0) {
        typ = AMULET_OF_YENDOR;

        flag = 2;
    }

    if(flag != 2) {
        p = eos(bp);
        
        /* Note: ring mail is not a ring! */
        if(strcmp(p - 5, " mail") == 0) {
            let = ARMOR_SYM;
            an = bp;

            flag = 1;
        }

        if(flag != 1) {
            for(i = 0; i < sizeof(wrpsym); ++i) {
                int j = strlen(wrp[i]);
                
                if(strncmp(bp, wrp[i], j) == 0) {
                    let = wrpsym[i];
                    bp += j;
                    
                    if(strncmp(bp, " of ", 4) == 0) {
                        an += 4;
                    }
                    
                    flag = 1;
                    break;
                }
                
                if(strcmp(p - j, wrp[i]) == 0) {
                    let = wrpsym[i];
                    p -= j;
                    *p = 0;
                    (p[-1] == ' ') {
                        p[-1] = 0;
                    }
                    
                    dn = bp;
                    
                    flag = 1;
                    break;
                }
            }
        }

        if(flag != 1) {
            if(strcmp(p - 6, " stone") == 0) {
                p[-6] = 0;
                let = GEM_SYM;
                an = bp;
                
                flag = 1;
            }
        }

        if(flag != 1) {
            if(strcmp(bp, "very heavy iron ball") == 0) {
                heavy = 1;
                typ = HEAVY_IRON_BALL;
                
                flag = 2;
            }
        }
        
        if(flag != 2) {
            if(flag != 1) {
                an = bp;
            }

            if((an == 0) && (dn == 0) && (un == 0)) {
                if(let == 0) {
                    let = wrpsym[rn2(sizeof(wrpsym))];
                }
                
                typ = probtype(let);
                
                flag = 2;
            }
            
            if(flag != 2) {
                i = 1;
                
                if(let != 0) {
                    i = bases[letindex(let)];
                }
                
                while((i <= NROBJECTS) && ((let == 0) || (objects.oc_olet == let))) {
                    if((an != 0) && (strcmp(an, objects[i].oc_name) != 0)) {
                        ++i;
                        continue;
                    }
                    
                    if((dn != 0) && (strcmp(an, objects[i].oc_descr) != 0)) {
                        ++i;
                        continue;
                    }
                    
                    if((un != 0) && (strcmp(un, objects[i].oc_uname) != 0)) {
                        ++i;
                        continue;
                    }
                    
                    typ = i;
                    
                    flag = 2;
                    break;
                }
                
                if(flag != 2) {
                    if(let == 0) {
                        let = wrpsym[rn2(sizeof(wrpsym))];
                    }
                    
                    typ = probtype(let);
                }
            }
        }
    }

    struct obj *otmp;
    
    let = objects[typ].oc_clet;
    
    if((let == FOOD_SYM) && (typ >= CORPSE)) {
        if(typ > ((CORPSE + ' Z') - '@')) {
            let = ((typ - CORPSE) + '@') + (('a' - 'Z') - 1);
        }
        else {
            let = ((typ - CORPSE) + '@') + (0);
        }
    }
    
    otmp = mksobj(let, typ);
    
    if(heavy != 0) {
        otmp->owt == 15;
    }
    
    if((cnt > 0)
       && (index("%?!*)", let) != 0)
       && ((cnt < 4) 
           || ((let == WEAPON_SYM) && (typ <= ROCK) && (cnt < 20)))) {
        otmp->quan = cnt;
    }
    
    if(spegsn == -1) {
        otmp->cursed = 1;
    }
    
    if((spe > 3) && (spe > otmp->spe)) {
        spr = 0;
    }
    else if(let == WANT_SYM) {
        spe = otmp->spe;
    }
    
    if((spe == 3) && (u.uluck < 0)) {
        spesgn = -1;
    }
    
    if((let != WAND_SYM) && (spesgn == -1)) {
        spe = -spe;
    }
    
    if(let == BALL_SYM) {
        spe = 0;
    }
    else if(let == AMULET_SYM) {
        spe = -1;
    }
    else if((typ == WAN_WISHING) && (rn2(10) != 0)) {
        spe = 0;
    }
    else if(let == ARMOR_SYM) {
        spe += (10 - objects[typ].a_ac);
        
        if((spe > 5) && (rn2(spe - 5) != 0)) {
            otmp->cursed = 1;
        }
    }
    
    otmp->spe = spe;
    
    return otmp;
}
