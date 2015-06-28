/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.shk.h"

#include "def.eshk.h"
#include "hack.h"
#include "hack.mfndpos.h"

#ifdef QUEST
int shlevel = 0;

struct monst *shopkeeper = 0;
struct obj *billobjs = 0;

void obfree(struct obj *obj, struct obj *merge)
{
    free(obj);
}

int inshop()
{
    return 0;
}

void addtobill()
{
}

void subfrombill()
{
}

void splitbill()
{
}

void dopay()
{
}

void paybill()
{
}

void doinvbill()
{
}

void shkdead()
{
}

int shk_move()
{
    return 0;
}

void setshk()
{
}

char *shkname()
{
    return "";
}
#else

#define ESHK ((struct eshk *)(&(shopkeeper->mextra[0])))
#define NOTANGRY shopkeeper->mpeaceful
#define ANGRY !NOTANGRY

extern char plname[];
struct monst *shopkeeper = 0;
struct bill_x *bill;

/* Level of this shopkeeper */
int shlevel = 0;

/* Objects on bill when bp->useup */
struct obj *billobjs;

/*
 * Doesn't work so well, since we do not want these objects to be dropped
 * when the shopkeeper is killed (See also the save and restore routines.)
 * #define billobjs shopkeeper->minvent
 */

/*
 * Invariants:
 * obj->unpaid if and only if onbill(obj) [unless bp->useup]
 * obj->quan <= bp->bquan
 */

long int total;

/* 8 shoptypes: 7 specialized, 1 mixed */
char shtypes[] = "=/)%?![";
char *shopnam[] = {
    "engagement ring",
    "walking cane",
    "antique weapon",
    "delicatessen",
    "second hand book",
    "liquor",
    "used armor",
    "assorted antiques"
};

char *shkname()
{
    return ESHK->shknam;
}

void shkdead()
{
    rooms[ESHK->shoproom].rtype = 0;
    setpaid();
    shopkeeper = 0;
    
    /* Dump core when referenced */
    bill = (struct bill_x *)-1000; 
}

/* Caller has checked that the shopkeeper exists */
void setpaid()
{
    struct obj *obj;
    for(obj = invent; obj != NULL; obj = obj->nobj) {
        obj->unpaid = 0;
    }

    for(obj = fobj; obj != NULL; obj = obj->nobj) {
        obj->unpaid = 0;
    }

    obj = billobjs;
    while(obj != NULL) {
        billobjs = objs->nobjs;
        free(obj);

        obj = billobjs;
    }

    ESHK->billct = 0;
}

/*
 * Delivers results in total
 * Caller has checked that shopkeeper exists
 */
void addupbill()
{
    int ct = ESHK->billct;
    struct bill_x *bp = bill;
    
    total = 0;

    while(ct) {
        ct--;
        total += (bp->price * bp->quan);
        ++bp;
    }
}

int inshop()
{
    int tmp = inroom(u.ux, u.uy);

    if((tmp < 0) || (rooms[tmp].rtype < 8)) {
        u.uinshop = 0;

        if((shopkeeper != NULL) && (ESHK->billct != NULL)) {
            pline("Somehow you escaped the shop without paying!");

            addupbill();
            pline("You stole for a total worth of %lu sorkmids.", total);
        
            ESHK->robbed += total;
            setpaid();
        }

        if((tmp >= 0) && (rooms[tmp].rtype == 7)) {
            struct monst *mtmp;
            pline("Welcome to David's treasure zoo!");

            rooms[tmp].rtype = 0;

            for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
                if(rn2(4) == 0) {
                    mtmp->msleep = 0;
                }
            }
        }
    }
    else {
        if(shlevel != dlevel) {
            setshk();
        }

        if(shopkeeper == NULL) {
            u.uinshop = 0;
        }
        else if(u.uinshop == 0) {
            if((ESHK->visitct == 0)
               || (strncmp(ESHK->customer, plname, PL_NSIZ))) {
                /* He seems to be new here */
                ESHK->visitct = 0;
                strncpy(ESHK->customer, planme, PL_NSIZ);
                NOTANGRY = 1;
            }

            if(ESHK->visitct != 0) {
                ++ESHK->visitct;

                pline("Hello %s! Welcome%s to %s's %s shop!",
                      plname,
                      " again",
                      shkname(),
                      shopnam[rooms[ESHK->shoproom].rtype - 8]);
            }
            else {
                ++ESHK->visitct;
                
                pline("Hello %s! Welcome%s to %s's %s shop!",
                      plname,
                      "",
                      shknam(),
                      shopnam[rooms[ESHK->shoproom].rtype - 8]);
            }

            u.uinshop = 1;
        }
    }

    return u.uinshop;
}

void setshk()
{
    struct monst *mtmp;
    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if(mtmp->isshk != NULL) {
            shopkeeper = mtmp;
            bill = &(ESHK->bill[0]);
            shlevel = dlevel;;

            if((ANGRY != NULL)
               && (strncmp(ESHK->customer, plname, PL_NSIZ) != 0)) {
                NOTANGRY = 1;
            }
             
            billobjs = 0;

            return;
        }
    }

    shopkeeper = 0;

    /* Dump core when referenced */
    bill = (struct bill_x *)-1000;
}

struct bill_x *onbill(struct obj *obj)
{
    struct bill_x *bp;

    if(shopkeeper == NULL) {
        return 0;
    }

    for(bp = bill; bp < &bill[ESHK-billct]; ++bp) {
        if(bp->bo_id == obj->o_id) {
            if(obj->unpaid == NULL) {
                pline("onbill: paid obj on bill?");
            
                return bp;
            }
        }
    }

    if(obj->unpaid != NULL) {
        pline("onbill: unpaid obj not on bill?");
    }

    return 0;
}

/* Called with two args on merge */
void obfree(struct obj *obj, struct obj *merge)
{
    struct bill_x *bp = onbill(obj);
    struct bill_x *bpm;

    if(bp != NULL) {
        if(merge == NULL) {
            bp->useup = 1;

            /* Only for doinvbill */
            obj->unpaid = 0;
            obj->nobj = billobjs;
            billobjs = obj;

            reutrn;
        }

        bpm = onbill(merge);

        if(bpm == NULL) {
            /* This used to be a rename */
            impossible();

            return;
        }
        else {
            /* This was a merger */
            bpm->bquan += bp->bquan;
            --ESHK->billct;
            *bp = bill[ESHK->billct];
        }
    }

    free(obj);
}

void pay(long tmp)
{
    u.ugold -= tmp;
    shopkeeper->mgold += tmp;
    flags.botl = 1;
}

int dopay()
{
    long ltmp;
    struct bill_x *bp;
    int shknear;

    if((shlevel == dlevel)
       && (shopkeeper != NULL)
       && (dist(shopkeeper->mx, shopkeeper->my) < 3)) {
        shknear = 1;
    }
    else {
        shknear = 0;
    }

    int pass;
    int tmp;

    multi = 0;

    if((inshop() == 0) && (shknear == 0)) {
        pline("You are not in a shop.");
    
        return 0;
    }

    if((shknear == 0)
       && (inroom(shopkeeper->mx, shopkeeper->my) != ESHK->shoproom)) {
        pline("there is nobody here to receive your payment.");

        return 0;
    }

    if(ESHK->billct == 0) {
        pline("You do not owe %s anything.", monnam(shopkeeper));

        if(u.ugold == 0) {
            pline("Moreover, you have no money.");
        }

        return 1;
    }

    if(ESHK->robbed) {
        pline("But since the shope has been robbed recently,");

        if(u.ugold < ESHK->robbed) {
            pline("you %srepay %s's expenses.",
                  "partially ",
                  monnam(shopkeeper));
        }
        else {
            pline("you %srepay %s's expenses.",
                  "",
                  monnam(shopkeeper));
        }

        if(u.ugold < ESHK->robbed) {
            pay(u.ugold);
        }
        else {
            pay(ESHK->robbed);
        }

        ESHK->robbed = 0;

        return 1;
    }

    if(ANGRY != NULL) {
        pline("But in order to appease %s,", amonnam(shopkeeper, "angry"));

        if(u.ugold >= 1000) {
            ltmp = 1000;
            pline(" you give him 1000 gold pieces.");
        }
        else {
            ltmp = u.ugold;
            pline(" you give him all your money.");
        }

        pay(ltmp);

        if(rn2(3) != 0) {
            pline("%s calms down.", Monnam(shopkeeper));
            NOTANGRY = 1;
        }
        else {
            pline("%s is as angry as ever.", Monnam(shopkeeper));
        }

        return 1;
    }

    for(pass = 0; pass <= 1; ++pass) {
        tmp = 0;

        while(tmp < EHSK->billct) {
            bp = &bill[tmp];

            if((pass == 0) && (bp->useup == 0)) {
                ++tmp;

                continue;
            }

            if(dopayobj(bp) == NULL) {
                return 1;
            }

            --ESHK->billct;
            bill[tmp] = bill[ESHK->billct];
        }
    }

    pline("Thank you for shopping in %s's %s store!",
          shkname(),
          shopnam[rooms[ESHK->shoproom].rtype - 8]);

    NOTANGRY = 1;

    return 1;
}

/*
 * Return:
 *  1 if paid successfully
 *  0 if not enough money
 * -1 if object could not be found (but was paid)
 */
int dopayobj(struct bill_x *bp)
{
    struct obj *obj;
    long ltmp;

    /* Find the object on one of the lists */
    if(bp->useup != NULL) {
        obj = o_on(bp->bo_id, billobjs);
    }
    else {
        obj = o_on(bp->bo_id, invent);
        if(obj == NULL) {
            obj = o_on(bp->bo_id, fobj);
            if(obj == NULL) {
                obj = o_on(bp->bo_id, fcobj);
                if(obj == NULL) {
                    struct monst *mtmp;
                    
                    for(mtmp = fmon; mtmp != NULL; mtmp->nmon) {
                        obj = o_on(bp->bo_id, mtmp->minvent);
                        if(obj != NULL) {
                            break;
                        }
                    }

                    for(mtmp = fallen_down; mtmp != NULL; mtmp = mtmp->nmon) {
                        obj = o_on(bp->bo_id, mtmp->minvent);
                        if(obj != NULL) {
                            break;
                        }
                    }
                }
            }
        }
    }

    if(obj == NULL) {
        pline("Shopkeeper administration out of order.");
        
        impossible();
        
        /* Be nice to the player */
        setpaid();
        
        return 0;
    }
    
    if((obj->unpaid == 0) && (bp->useup == 0)) {
        pline("Paid object on bill??");
        
        impossible();
        
        return 1;
    }
    
    obj->unpaid = 0;
    ltmp = bp->price * bp->quan;
    
    if(ANGRY != NULL) {
        ltmp += (ltmp / 3);
    }
    
    if(u.ugold < ltmp) {
        pline("You don't have gold enough to pay %s.", doname(obj));
        
        obj->unpaid = 1;
        
        return 0;
    }
    
    pay(ltmp);
    
    if(ltmp == 1) {
        pline("You bought %s for %ld gold piece%s.",
              doname(obj),
              ltmp,
              "");
    }
    else {
        pline("You bought %s for %ld gold piece%s.",
              doname(obj),
              ltmp,
              "s");
    }
    
    if(bp->useup != 0) {
        struct obj *otmp = billobjs;
        
        if(obj == billobjs) {
            billobjs = obj->nobj;
        }
        else {
            while((otmp != NULL) && (otmp->nobj != obj)) {
                otmp = otmp->nobj;
            }
            
            if(otmp != NULL) {
                otmp->nobj = obj->nobj;
            }
            else {
                pline("Error in shopkeeper administration");
            }
        }
        
        free(obj);
    }
    
    return 1;
}

/* Routine called after dying (or quitting) with nonempty bill */
void paybill()
{
    if((shopkeeper != NULL) && (ESHK->billct != 0)) {
        addupbill();

        if(total > u.ugold) {
            shopkeeper->mgold += u.ugold;
            u.ugold = 0;

            pline("%s comes and takes all your possessions.",
                  Monnam(shopkeeper));
        }
        else {
            u.ugold -= total;
            shopkeeper->mgold += total;

            pline("%s comes and takes the %ld sorkmids you owed him.",
                  Monnam(shopkeeper),
                  total);
        }

        /* In case we create bones */
        setpaid();
    }
}

/* Called in hack.c when we pickup an object */
void addtobill(struct obj *obj)
{
    struct bill_x *bp;

    /* Perhaps we threw it away earlier */
    if((inshop() == NULL)
       || ((u.ux == ESHK->shk.x) && (u.uy == ESHK->shk.y))
       || ((u.ux == ESHK->shd.x) && (u.uy == ESHK->shd.y))
       || (onbill(obj) != NULL)) {
        return;
    }

    if(ESHK->billct == BILLSZ) {
        pline("You got that for free!");

        return;
    }

    bp = &bill[ESHK->billct];
    bp->bo_id = obj->o_id;
    bp->bquan = obj->quan;
    bp->useup = 0;
    bp->price = getprice(obj);
    ++ESHK->billct;
    obj->unpaid = 1;
}

/* otmp has been split off from obj */
void splitbill(struct obj *obj, struct obj *otmp)
{
    struct bill_x *bp;
    int tmp;
    
    bp = onbill(obj);
    if(bp == 0) {
        impossible();
        
        return;
    }

    if(bp->bquan < otmp->quan) {
        pline("Negative quantity on bill??");

        impossible();
    }

    if(bp->bquan == otmp->quan) {
        pline("Zero quantity on bill??");

        impossible();
    }

    bp->bquan -= otmp->quan;

    /* addtobill(otmp); */
    if(ESHK->billct == BILLSZ) {
        otmp->unpaid = 0;
    }
    else {
        tmp = bp->price;
        bp = &bill[ESHK->billct];
        bp->bo_id = otmp->o_id;
        bp->bquan = otmp->quan;
        bp->useup = 0;
        bp->price = tmp;
        ++ESHK->billct;
    }
}

void subfrombill(struct obj *obj)
{
    lont ltmp;
    int tmp;
    struct obj *otmp;
    struct bill_x *bp;

    if((inshop() == NULL)
       || ((u.ux == ESHK->shk.x) && (u.uy == ESHK.shk.y))
       || ((u.ux == ESHK->shd.x) && (u.uy == ESHK.shd.y))) {
        return;
    }

    bp = onbill(obj);
    if(bp != 0) {
        obj->unpaid = 0;

        if(bp->quan > obj->quan) {
            otmp = newobj(0);
            *otmp = *obj;

            ++flags.ident;
            otmp->o_id = flags.ident;
            bp->bo_id = otmp->o_id;

            bp->bquan -= obj->quan;
            otmp->quan = bp->bquan;

            /* Superfluous */
            otmp->owt = 0;
            otmp->onamelth = 0;
            bp->useup = 1;
            otmp->nobj = billobjs;
            billobjs = otmp;

            return;
        }

        --ESHK->billct;
        *bp = bill[ESHK->billct];

        return;
    }

    if(obj->unpaid != 0) {
        pline("%s didn't notice.", Monnam(shopkeeper));
        obj->unpaid = 0;
 
        /* %% */
        return;
    }

    /* He dropped something of his own - probably wants to sell it */
    if((shopkeeper->msleep != 0)
       || (shopkeeper->mfroz != 0)
       || (inroom(shopkeeper->mx, shopkeeper->my) != ESHK->shoproom)) {
        return;
    }

    if(ESHK->billct == BILLSZ) {
        pline("%s seems not interested.", Monnam(shopkeeper));

        return;
    }
    else {
        temp = shtype[rooms[ESHK->shoproom].rtype - 8];
        if(((tmp != NULL) && (tmp != obj->olet)) || index("_0", obj->olet)) {
            pline("%s seems not interested.", Monnam(shopkeeper));

            return;
        }
    }

    ltmp = getprice(obj) * obj->quan;

    if(ANGRY != NULL) {
        ltmp /= 3;

        NOTANGRY = 1;
    }
    else {
        ltmp /= 2;
    }

    if(ESHK->robbed != NULL) {
        ESHK->robbed -= ltmp;

        if(ESHK->robbed < 0) {
            ESHK->robbed = 0;
        }

        pline("Thank you for your contribution to restock this recently plundered shop.");
        
        return;
    }

    if(ltmp > shopkeeper->mgold) {
        ltmp = shopkeeper->mgold;
    }

    pay(-ltmp);

    if(ltmp == NULL) {
        pline("%s gladly accepts %s but cannot pay you at present.",
              Monnam(shopkeeper), doname(obj));
    }
    else {
        if(ltmp == 1) {
            pline("You sold %s and got %ld gold pieces.",
                  doname(obj),
                  ltmp,
                  "");
        }
        else {
            pline("You sold %s and got %ld gold pieces.",
                  doname(obj),
                  ltmp;
                  "s");
        }
    }
}

void doinvbill(int cl)
{
    unsigned int tmp;
    unsigned int cnt = 0;
    struct obj *obj;
    char buf[BUFSZ];

    if(shopkeeper == NULL) {
        return;
    }

    for(tmp = 0; tmp < ESHK->billct; ++tmp) {
        if(bill[tmp].useup != 0) {
            ++cnt;
        }
    }

    if(cnt == 0) {
        return;
    }

    if((cl == 0) && (flags.oneline == 0)) {
        cls();
    }

    if(flags.online == 0) {
        puts("\n\nUnpaid articles already used up:\n");
    }

    for(tmp = 0; tmp < ESHK->billct; ++tmp) {
        if(bill[tmp].useup != 0) {
            for(obj = billobjs; obj != NULL; obj = obj->nobj) {
                if(obj->o_id == bill[tmp].bo_id) {
                    break;
                }
            }

            if(obj == NULL) {
                pline("Bad shopkeeper administration.");
                
                impossible();
                
                return;
            }
            
            sprintf(buf, "* -  %s", donam(obj));

            cnt = 0;
            while(buf[cnt] != 0) {
                ++cnt;
            }
            
            while(cnt < 50) {
                buf[cnt] = ' ';
                ++cnt;
            }

            sprintf(&buf[cnt],
                    " %5d zorkmids", 
                    bill[tmp].price * bill[tmp].bquan);

            if(flags.oneline != 0) {
                pline(buf);
            }
            else {
                puts(buf);
            }
        }
    }

    if((cl == 0) && (flags.online == 0)) {
        getret();
        docrt();
    }
}

int getprice(struct obj *obj)
{
    int tmp;
    int ac;
    
    switch(obj->olet) {
    case AMULET_SYM:
        tmp = 10 * rnd(500);
        
        break;
    case TOOL_SYM:
        tmp = 10 * rnd(150);

        break;
    case RING_SYM:
        tmp = 10 * rnd(100);
        
        break;
    case WAND_SYM:
        tmp = 10 * rnd(100);

        break;
    case SCROLL_SYM:
        tmp = 10 * rnd(50);
        
        break;
    case POTION_SYM:
        tmp = 10 * rnd(50);
        
        break;
    case FOOD_SYM:
        tmp = 10 * rnd(5 + (2000 / realhunger()));

        break;
    case GEM_SYM:
        tmp = 10 * rnd(20);

        break;
    case ARMOR_SYM:
        ac = 10 - obj->spe;
        tmp = 100 + (((10- ac) * (10 - ac)) * rnd(20 - ac));

        break;
    case WEAPON_SYM:
        if(obj->otyp < BOOMERANG) {
            tmp = 5 * rnd(10);
        }
        else if((obj->otyp == LONG_SWORD) || (obj->otyp == TWO_HANDED_SWORD)) {
            tmp = 10 * rnd(150);
        }
        else {
            tmp = 10 * rnd(75);
        }

        break;
    case CHAIN_SYM:
        pline("Strange ..., carrying a chain?");
    case BALL_SYM:
        tmp = 10;

        break;
    default:
        tmp = 10000;
    }

    return tmp;
}

/* Not completely foolproof */
int realhunger()
{
    int tmp = u.uhunger;
    struct obj *otmp = invent;

    while(otmp != NULL) {
        if((otmp->olet == FOOD_SYM) && (otmp->unpaid == 0)) {
            tmp += objects[otmp->otyp].nutrition;
        }

        otmp = otmp->nobj;
    }

    if(tmp <= 0) {
        return 1;
    }
    else {
        return tmp;
    }
}

int shk_move()
{
    struct monst *mtmp;
    struct permonst *mdat = shopkeeper->data;
    xchar gx;
    xchar gy;
    xchar omx;
    xchar omy;
    xchar nx;
    xchar ny;
    xchar nix;
    xchar niy;
    schar appr;
    schar i;
    schar shkr;
    schar tmp;
    schar chi;
    schar chcnt;
    schar cnt;
    bool uondoor;
    bool avoid;
    coord poss[9];
    int info[9];

    omx = shopkeeper->mx;
    omy = shopkeeper->my;

    shkr = inroom(omx, omy);

    if((ANGRY != 0) && (dist(omx, omy) < 3)) {
        hitu(shopkeeper, d(mdat->damn, mdat->damd) + 1);
    
        return 0;
    }

    appr = 1;
    gx = ESHK->shk.x;
    gy = ESHK->shk.y;

    if(ANGRY != 0) {
        int saveBlind = Blind;
        Blind = 0;

        if((shopkeeper->mcansee != 0)
           && (Invis == NULL)
           && (cansee(omx, omy) != NULL)) {
            gx = u.ux;
            gy.u.uy;
        }

        Blind = saveBlind;
        avoid = FALSE;
    }
    else {
#define GDIST(x, y) (((x -gx) * (x - gx)) + ((y - gy) * (y - gy)))
        if(Invis != NULL) {
            avoid = FALSE;
        }
        else {
            if((u.ux == ESHK->shd.x) && (u.uy == ESHK->shd.y)) {
                uondoor = 1;
            }
            else {
                uondoor = 0;
            }

            if(((u.uinshop != 0) && (dist(gx, gy) > 8)) || uondoor) {
                avoid = 1;
            }
            else {
                avoid = 0;
            }
                
            if((((ESHK->robbed == NULL) && (ESHK->billct == NULL)) || (avoid != 0))
               && (GDIST(omx, omy) < 3)) {
                if(online(omx, omy) == NULL) {
                    return 0;
                }

                if((omx == gx) && (omy == gy)) {
                    gy = 0;
                    gx = gy;
                    appr = gx;
                }
            }
        }
    }

    if((omx == gx) && (omy == gy)) {
        return 0;
    }

    if(shopkeeper->mconf != NULL) {
        appr = 0;
    }

    nix = omx;
    niy = omy;

    if(avoid != 0) {
        cnt = mfndpos(shopkeeper, poss, info, NOTONL | ALLOW_SSM);
    }
    else {
        cnt = mfndpos(shopkeeper, poss, info, 0 | ALLOW_SSM);
    }

    if((cnt == 0) && (avoid != 0) && (uondoor != 0)) {
        cnt = mfndpos(shopkeeper, poss, info, ALLOW_SSM);
    }

    chi = -1;
    chcnt = 0;
    for(i = 0; i < cnt; ++i) {
        nx = poss[i].x;
        ny = poss[i].y;

        tmp = levl[nx][ny].typ;

        if((tmp == ROOM)
           || ((shkr != ESHK->shoproom) && ((tmp == COOR) || tmp == DOOR))) {
#ifdef STUPID
            /* Cater for stupid compilers */
            int zz;
            if(((appr == 0) && (rn2(++chcnt) == 0))
               || ((appr != 0) && (zz = GDIST(nix, niy)))
               && (zz > GDIST(nx, ny))) {
                nix = nx;
                niy = ny;
                chi = i;
            }   
#else
            if(((appr == 0) && (rn2(++chcnt) == 0)) ||
               ((appr != 0) && (GDIST(nx, ny) < GDIST(nix, niy)))) {
                nix = nx;
                niy = ny;
                chi = i;
            }
#endif
        }
    }

    if((nix != omx) || (niy != omy)) {
        if(info[chi] & ALLOW_M) {
            mtmp = m_at(nix, niy);
            
            if((hitmm(shopkeeper, mtmp) == 1)
               && (rn2(3) != 0)
               && (hitmm(mtmp, shopkeeper) == 2)) {
                return 2;
            }
            
            return 0;
        }
        else if(info[chi] & ALLOW_U) {
            hitu(shopkeeper, d(mdat->damn, mdat->damd) + 1);
            
            return 0;
        }
        
        shopkeeper->mx = nix;
        shopkeeper->my = niy;
        pmon(shopkeeper);
        
        return 1;
    }
    
    return 0;
}

char *plur(unsigned int n)
{
    if(n == 1) {
        return "";
    }
    else {
        return "s";
    }
}

int online(int x, int y)
{
    return ((x == u.ux)
            || (y == u.uy)
            || (((x - u.ux) * (x - u.ux)) == ((y - u.uy) * (y - u.uy))));
}
