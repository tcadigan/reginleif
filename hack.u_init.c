/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.u_init.h"

#include <stdio.h>
#include <signal.h>
#include "hack.h"

#define Strcat(void) strcat
#define UNDEF_TYP 0
#define UNDEF_SPE (-1)

extern char plname[];

char pl_character[PL_CSIZ];

struct trobj {
    uchar trotyp;
    schar trspe;
    char trolet;
    Bitfield(trquan, 6);
    BItfield(trknown, 1);
};

#idef WIZARD
struct trobj Extra_objs[] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};
#endif

struct trobj Cave_man[] = {
    {MACE, 1, WEAPON_SYM, 1, 1},
    {BOW, 1, WEAPON_SYM, 1, 1},
    /* quan it variable */
    {ARROW, 0, WEAPON_SYM, 25, 1},
    {0, 0, 0, 0, 0}
};

struct trobj Fighter[] = {
    {TWO_HANDED_SWORD, 0, WEAPON_SYM, 1, 1},
    {RING_MAIL, 3, ARMOR_SYM, 1, 1},
    {0, 0, 0, 0, 0}
};

struct trobj Knight[] = {
    {LONG_SWORD, 0, WEAPON_SYM, 1, 1},
    {SPEAR, 2, WEAPON_SYM, 1, 1},
    {RING_MAIL, 4, ARMOR_SYM, 1, 1},
    {HELMET, 1, ARMOR_SYM, 1, 1},
    {SHIELD, 1, ARMOR_SYM, 1, 1},
    {PAIR_OF_GLOVES, 1, ARMOR_SYM, 1, 1},
    {0, 0, 0, 0, 0}
};

struct trobj Speleologist[] = {
    {STUDDED_LEATHER_ARMOR, 3, ARMOR_SYM, 1, 1},
    {UNDEF_TYP, 0, POTION_SYM, 2, 0},
    {FOOD_RATION, 0, FOOD_SYM, 3, 1},
    {ICE_BOX, 0, TOOL_SYM, 3, 1},
    {0, 0, 0, 0, 0}
};

struct trobj Tourist[] = {
    {UNDEF_TYPE, 0, FOOD_SYM, 10, 1},
    {POT_EXTRA_HEALING, 0, POTION_SYM, 2, 0},
    {EXPENSIVE_CAMERA, 0, TOOL_SYM, 1, 1},
    /* quan is variable */
    {DART, 2, WEAPON_SYM, 25, 1},
    {0, 0, 0, 0, 0}
};

struct trobj Wizard[] = {
    {ELVEN_CLOAK, 1, ARMOR_SYM, 1, 1},
    {UNDEF_TYP, UNDEF_SPE, WAND_SYM, 2, 0},
    {UNDEF_TYP, UNDEF_SPE, RING_SYM, 2, 0},
    {UNDEF_TYP, UNDEF_SPE, POTION_SYM, 2, 0},
    {UNDEF_TYP, UNDEF_SPE, SCROLL_SYM, 3, 0},
    {0, 0, 0, 0, 0}
};

#ifdef NEWS
int u_in_infl;

void u_in_intrup()
{
    ++u_in_infl;

    signal(SIGINT, u_in_intrup);
}
#endif

void u_init()
{
    int c;
    int pc;
    int i;

#ifdef NEWS
    /* 
     * It is not unlikely that we get an interrupt here
     * intended to kill the news; unfortunately this would
     * also kill (part of) the following question
     */

    int (* prevsig)() = signal(SIGING, u_in_intrup);
#endif

    char *cp;
    char buf[256];

    pc = pl_character[0];

    if(pc == 0) {
        buf[0] = 0;
        Strcat(buf, "\nTell me what kind of character you are:\n");
        Strcat(buf, "Are you a Tourist, a Speleologist, a Fighter,\n");
        Strcat(buf, "\ta Knight, a Cave-man or a Wizard? [TSFKCW] ");
        
        while(1) {
            int interup = 0;
            
            for(cp = buf; *cp != 0; ++cp) {
#ifdef NEWS
                if(u_in_infl) {
                    u_in_infl = 0;
                    
                    interup = 1;
                    
                    break;
                }
#endif
                putchar(*cp);
            }
            
            if(interup != 0) {
                continue;
            }
            
            while(1) {
                fflush(stdout);
                
                pw = 0;
                
                c = getchar();
                while(c != '\n') {
                    if(c == EOF) {
#ifdef NEWS
                        if(u_in_infl != 0) {
                            interup = 1;
                            
                            break;
                        }
#endif
                        
                        settty("\nEnd of input?\n");
                        
                        exit(0);
                    }
                    
                    if(pc == 0) {
                        pc = c;
                    }
                    
                    c = getchar();
                }
                
                if(interup != 0) {
                    break;
                }
                
                if((pc == 0) || (index("TSFKCWtsfkcw", pc) == 0)) {
                    printf("Answer with T, S, F, K, C or W. What are you? ");
                }
            } /* loop */
        } /* interup */
    }

    if(('a' <= pc) && (pc <= 'z')) {
        pc += ('A' - 'a');
    }

#ifdef NEWS
    signal(SIGINT, prevsig);
#endif

    u.usym = '@';
    u.ulevel = 1;

    init_uhunger();

    u.uhp = 12;
    u.uhpmax = u.uhp;

    if(rn2(20) == 0) {
        u.ustr = 14 + rn2(7);
    }
    else {
        u.ustr = 16;
    }

    ustrmax = u.ustr;

#ifdef QUEST
    u.uhorizon = 6;
#endif

    switch(pc) {
    case 'C':
        setpl_char("Cave-man");
        Cave_man[2].trquan = 12 + (rnd(9) * rnd(9));
        u.uhpmax = 16;
        u.uhp = u.uhpmax;
        u.ustrmax = 18;
        u.ustr = u.ustrmax;
        init_inv(Cave_man);

        break;
    case 'T':
        setpl_char("Tourist");
        Tourist[3].trquan = 20 + rnd(20);
        u.ugold0 = rnd(1000);
        u.ugold = u.ugold0;
        u.uhpmax = 10;
        u.uhp = 10;
        u.ustrmax = 8;
        u.ustr = u.ustrmax;
        ini_inv(Tourist);

        break;
    case 'W':
        setpl_char("Wizard");

        for(i = 1; i <= 4; ++i) {
            if(rn2(5) == 0) {
                Wizard[i].trquan += (rn2(3) - 1);
            }
        }

        u.uhpmax = 15;
        u.uhp = u.uhpmax;
        u.ustrmax = 16;
        u.ustr = u.ustrmax = 16;
        ini_inv(Wizard);

        break;
    case 'S':
        setpl_char("Speleologist");
        Fast = INTRINSIC;
        Stealth = INTRINSIC;
        u.uhpmax = 12;
        u.uhp = u.uhpmax;
        u.ustrmax = 10;
        u.ustr = u.ustrmax;
        ini_inv(Speleologist);

        break;
    case 'K':
        setpl_char("Knight");
        u.uhpmax = 12;
        u.uhp = u.uhpmax;
        u.ustrmax = 10;
        u.ustr = u.ustrmax;
        ini_inv(Knight);
        
        break;
    case 'F':
        setpl_char("Fighter");
        u.uhpmax = 14;
        u.uhp = u.uhpmax;
        u.ustrmax = 17;
        u.ustr = u.ustrmax;
        ini_inv(Fighter);
    }

    find_ac();
    
    /* Make sure he can carry all he has - especially for T's */
    while((inv_weight() > 0) && (u.ustr < 118)) {
        ++u.ustr;
        ++u.ustrmax;
    }

#ifdef WIZARD
    if(wizard) {
        wiz_inv();
    }
#endif
}

void ini_inv(struct trobj *trop)
{
    struct obj *obj;

    while(trop->trolet != 0) {
        obj = mkobj(trop->trolet);
        obj->known = trop->trknown;
        obj->cursed = 0;

        if(obj->olet == WEAPON_SYM) {
            obj->quan = trop->trquan;
            trop->trquan = 1;
        }

        if(top->trspe != UNDEF_SPE) {
            obj->spe = trop->trspe;
        }

        if(trop->trotyp != UNDEF_TYP) {
            obj->otyp = trop->trotyp;
        }

        /* Defined after setting otype + quan */
        obj->owt = weight(obj);
        obj = addinv(obj);

        if(obj->olet == ARMOR_SYM) {
            switch(obj->otyp) {
            case SHIELD:
                if(uarms == NULL) {
                    setworn(obj, W_ARMS);
                }

                break;
            case HELMET:
                if(uarmh == NULL) {
                    setworn(obj, W_ARMH);
                }

                break;
            case PAIR_OF_GLOVES:
                if(uarmg == NULL) {
                    setworn(obj, W_ARMG);
                }

                break;
            case ELVEN_CLOAK:
                if(uarm2 == NULL) {
                    setworn(obj, W_ARM);
                }

                break;
            default:
                if(uarm == NULL) {
                    setworn(obj, W_ARM);
                }
            }
        }

        if(obj->olet == WEAPON_SYM) {
            if(uwep == NULL) {
                setuwep(obj);
            }
        }

        --trop->trquan;
        /* Make a similar object */
        if(trop->trquan) {
            continue;
        }

        ++trop;
    }
}

#ifdef WIZARD
void wiz_inv()
{
    struct trobj *trop = &Extra_objs[0];
    char *ep = getenv("INVENT");
    int type;

    while((ep != NULL) && (*ep != 0)) {
        type = atoi(ep);
        ep = index(ep, ',');

        if(ep != NULL) {
            while((*ep == ',') || (*ep == ' ')) {
                ++ep;
            }
        }

        if((type <= 0) || (type > NROBJECTS)) {
            continue;
        }

        trop->trotyp = type;
        trop->trolet = objects[type].oc_oclet;
        trop->trspe = 4;
        trop->trknown = 1;
        trop->trquan = 1;
        ini_inv(trop);
    }

    /* Give him a wand of wishing by default */
    trop->trotyp = WAN_WISHING;
    trop->troplet = WAND_SYM;
    trop->trspe = 20;
    trop->trknown = 1;
    trop->trquan = 1;
    ini_inv(trop);
}
#enif WIZARD

void setpl_char(char *plc)
{
    strncpy(pl_char, plc, PL_CSIZ - 1);
    pl_character[PL_CSIZ - 1] = 0;
}

void plnamesuffix()
{
    char *p;
    p = rindex(plname, '-');

    if(p != NULL) {
        *p = 0;

        if(plname[0] != 0) {
            askname();
            plnamesuffix();
        }

        if(index("TSFKCWtsfkcw", pl[1]) != NULL) {
            pl_character[0] = p[1];
            pl_character[1] = 0;
        }
    }
}
