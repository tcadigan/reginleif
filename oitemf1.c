/*
 * Omega copyright (C) 1987-1989 by Laurance Raphael Brothers
 *
 * oitemf1.c
 *
 * Various item functions: potions, scrolls, boots, cloaks, things, food
 */

#include "oglob.h"

/* General item functions */
void i_no_op(pob o)
{
}

void i_nothing(pob o)
{
}

/* Scroll functions */
void i_knowledge(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    knowledge(o->blessing);
}

void i_jane_to(pob o)
{
    int volume = random_range(6);
    int i;
    int j;
    int k;

    print1("Jane's Guide to the World's Treasures: ");

    switch(volume) {
    case 0:
	nprint1("SCROLLS");
	j = SCROLLID;
	k = POTIONID;

	break;
    case 1:
	nprint1("POTIONS");
	j = POTIONID;
	k = WEAPONID;

	break;
    case 2:
	nprint1("CLOAKS");
	j = CLOAKID;
	k = BOOTID;

	break;
    case 3:
	nprint1("BOOTS");
	j = BOOTID;
	k = RINGID;

	break;
    case 4:
	nprint1("RINGS");
	j = RINGID;
	k = STICKID;

	break;
    case 5:
	nprint1("STICKS");
	j = STICKID;
	k = ARTIFACTID;

	break;
    }

    for(i = j; i < k; ++i) {
	Objects[i].known = 1;
    }
}

void i_flux(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    flux(o->blessing);
}

/* Enchantment */
void i_enchant(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    if(o->blessing < 0) {
	enchant(-1 - o->plus);
    }
    else {
	enchant(o->plus + 1);
    }
}

/* Scroll of clairvoyance */
void i_clairvoyance(struct object *o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    if(o->blessing < 0) {
	amnesia();
    }
    else {
	clairvoyance(5 + (o->blessing * 5));
    }
}

void i_acquire(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    acquire(o->blessing);

    /* Blank out the scroll */
    *o = Objects[SCROLLID + 0];
}

void i_teleport(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    p_teleport(o->blessing);
}

void i_spells(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    mprint("A scroll of spells.");
    learnspell(o->blessing);
}

/* Scroll of blessing */
void i_bless(pob o)
{
    Objects[o->id].known = 1;
    bless(o->blessing);
}

/* Scroll of wishing */
void i_wish(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    wish(o->blessing);

    /* Blank out the scroll */
    *o = Objects[SCROLLID + 0];
}

/* Scroll of displacement */
void i_displace(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    displace(o->blessing);
}

/* Scroll of deflection */
void i_deflect(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    deflection(o->blessing);
}

/* Scroll of identification */
void i_id(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    identify(o->blessing);
}

/* Potion functions */

/* Potion of healing */
void i_heal(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
	heal(1 + o->plus);
    }
    else {
	heal(-1 - abs(o->plus));
    }
}

/* Potion of monster detection */
void i_mondet(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    mondet(o->blessing);
}

/* Potion of objects detection */
void i_objdet(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    objdet(o->blessing);
}

/* Potion of neutralize poison */
void i_neutralize_poison(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
	mprint("You feel vital!");
	Player.status[POISONED] = 0;
    }
    else {
	p_poison(random_range(20) + 5);
    }
}

/* Potion of sleep */
void i_sleep_self(pob o)
{
    sleep_player(6);
    Objects[o->id].known = 1;
}

/* Potion of speed */
void i_speed(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    haste(o->blessing);
}

/* Potion of restoration */
void i_restore(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    recover_stat(o->blessing);
}

void i_augment(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    augment(o->blessing);
}

void i_azoth(pob o)
{
    if(o->plus < 0) {
	mprint("The mercury was poisonous!");
	p_poison(25);
    }
    else if(o->plus == 0) {
	mprint("The partially enchanted azoth makes you sick!");
	Player.con = (int)(Player.con / 2);
	calc_melee();
    }
    else if(o->blessing < 1) {
	mprint("The unblessed azoth warps your soul!");
	Player.maxpow = (int)(Player.maxpow / 2);
	Player.pow = Player.maxpow;
	level_drain(random_range(10), "cursed azoth");
    }
    else {
	mprint("the azoth fills you with cosmic power!");

	if(Player.str > (Player.maxstr * 2)) {
	    mprint("The power rages out of control!");
	    p_death("overdose of azoth");
	}
	else {
	    heal(10);
	    cleanse(1);
	    Player.mana = calcmana() * 3;
	    ++Player.maxstr;
	    Player.str = Player.maxstr * 3;
	}
    }
}

void i_regenerate(pob o)
{
    regenerate(o->blessing);
}

/* Boots functions */
void i_perm_speed(pob o)
{
    if(o->blessing > -1) {
	if(o->used) {
	    o->known = 2;
	    Objects[o->id].known = 1;

	    if(Player.status[SLOWED] > 0) {
		Player.status[SLOWED] = 0;
	    }

	    mprint("The world slows down!");
	    Player.status[HASTED] += 1500;
	}
	else {
	    Player.status[HASTED] -= 1500;

	    if(Player.status[HASTED] < 1) {
		mprint("The world speeds up again.");
	    }
	}
    }
    else {
	if(o->used) {
	    if(Player.status[HASTED] > 0) {
		Player.status[HASTED] = 0;
	    }

	    mprint("You feel slower.");
	    Player.status[SLOWED] += 1500;
	}
	else {
	    Player.status[SLOWED] -= 1500;

	    if(Player.status[SLOWED] < 1) {
		mprint("You feel quicker again.");
	    }
	}
    }
}

/* Cloak functions */
void i_perm_displace(pob o)
{
    if(o->blessing > -1) {
	if(o->used) {
	    mprint("You feel dislocated.");
	    Player.status[DISPLACED] += 1500;
	}
	else {
	    Player.status[DISPLACED] -= 1500;

	    if(Player.status[DISPLACED] < 1) {
		mprint("You feel a sense of position.");
		Player.status[DISPLACED] = 0;
	    }
	}
    }
    else {
	if(o->used) {
	    mprint("You have a forboding of bodily harm!");
	    Player.status[VULNERABLE] += 1500;
	}
	else {
	    Player.status[VULNERABLE] -= 1500;

	    if(Player.status[VULNERABLE] < 1) {
		mprint("You feel less endangered.");
		Player.status[VULNERABLE] = 0;
	    }
	}
    }
}

void i_perm_negimmune(pob o)
{
    if(o->blessing > -1) {
	if(o->used) {
	    ++Player.immunity[NEGENERGY];
	}
	else {
	    --Player.immunity[NEGENERGY];
	}
    }
    else if(o->used) {
	level_drain(abs(o->blessing), "cursed cloak of level drain");
    }
}

/* Food functions */
void i_food(pob o)
{
    switch(random_range(5)) {
    case 0:
	mprint("That tasted horrible!");

	break;
    case 1:
	mprint("Yum!");

	break;
    case 2:
	mprint("How nauseous!");

	break;
    case 3:
	mprint("Can I have some more? Please?");

	break;
    case 4:
	mprint("Your mouth feels like it is growing hair!");

	break;
    }
}

void i_stim(pob o)
{
    mprint("You feel Hyper!");
    i_speed(o);
    Player.str += 3;
    Player.con -= 1;
    calc_melee();
}

void i_pow(pob o)
{
    mprint("You feel a surge of mystic power!");
    Player.mana = 2 * calcmana();
}

void i_poison_food(pob o)
{
    mprint("This food was contaminated with cynaide!");
    p_poison(random_range(20) + 5);
}

void i_pepper_food(pob o)
{
    mprint("You innocently start to chec the szechuan pepper...");
    morewait();
    mprint("hot.");
    morewait();
    mprint("Hot.");
    morewait();
    mprint("Hot!");
    morewait();
    mprint("HOT!!!!!!");
    morewait();
    p_damage(1, UNSTOPPABLE, "a szechuan pepper");
    mprint("Your sinuses melt and run out your ears.");
    mprint("Your mouth and throat seem to be permanently on fire.");
    mprint("You feel much more awake now...");
    ++Player.immunity[SLEEP];
}

void i_lembas(pob o)
{
    heal(10);
    cleanse(0);
    Player.food = 40;
}

void i_cure(pob o)
{
    if(o->blessing > 0) {
	mprint("You feel a sense of innoculation");
	++Player.immunity[INFECTION];
	cure(o->blessing);
    }
}

void i_breathing(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    breathe(o->blessing);
}

void i_invisible(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    invisible(o->blessing);
}

void i_perm_invisible(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    if(o->used) {
	if(o->blessing > -1) {
	    mprint("You feel transparent!");
	    Player.status[INVISIBLE] += 1500;
	}
	else {
	    mprint("You feel a forboding of bodily harm!");
	    Player.status[VULNERABLE] += 1500;
	}
    }
    else {
	if(o->blessing > -1) {
	    Player.status[INVISIBLE] -= 1500;

	    if(Player.status[INVISIBLE] < 1) {
		mprint("You feel opaque again.");
		Player.status[INVISIBLE] = 0;
	    }
	}
	else {
	    Player.status[VULNERABLE] -= 1500;

	    if(Player.status[VULNERABLE] < 1) {
		mprint("You feel less endangered now.");
		Player.status[VULNERABLE] = 0;
	    }
	}
    }
}

void i_warp(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    warp(o->blessing);
}

void i_alert(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;
	alert(o->blessing);
    }
}

void i_charge(pob o)
{
    int i;

    if(o->blessing > -1) {
	Objects[o->id].known = 1;
    }

    mprint("A scroll of charging.");
    mprint("Charge: ");
    i = getitem(STICK);

    if(i != ABORT) {
	if(o->blessing < 0) {
	    mprint("The stick glows black!");
	    Player.possessions[i]->charge = 0;
	}
	else {
	    mprint("The stick glows blue!");
	    Player.possessions[i]->charge += ((random_range(10) + 1) * (o->blessing + 1));

	    if(Player.possessions[i]->charge > 99) {
		Player.possessions[i]->charge = 99;
	    }
	}
    }
}

void i_fear_resist(pob o)
{
    if(o->blessing > -1) {
	Objects[o->id].known = 1;

	if(Player.status[AFRAID] > 0) {
	    mprint("You feel stauncher now.");
	    Player.status[AFRAID] = 0;
	}
    }
    else if(!p_immune(FEAR)) {
	mprint("You panic!");
	Player.status[AFRAID] += random_range(100);
    }
}

/* Use a thieves pick */
void i_pick(pob o)
{
    int dir;
    int ox;
    int oy;
    o->used = FALSE;

    if(!o->known && !Player.rank[THIEVES]) {
	mprint("You have no idea what to do with a piece of twisted metal.");
    }
    else {
	o->known = 1;
	Objects[o->id].known = 1;
	mprint("Pick lock:");
	dir = getdir();

	if(dir == ABORT) {
	    resetgamestatus(SKIP_MONSTERS);
	}
	else {
	    ox = Player.x + Dirs[0][dir];
	    oy = Player.y + Dirs[1][dir];

	    if((Level->site[ox][oy].locchar != CLOSED_DOOR)
	       || loc_statusp(ox, oy, SECRET)) {
		mprint("You can't unlock that!");
		resetgamestatus(SKIP_MONSTERS);
	    }
	    else if(Level->site[ox][oy].aux == LOCKED) {
		if(Level->depth == (MaxDungeonLevels - 1)) {
		    mprint("The lock is too complicated for you!!!");
		}
		else if(((Level->depth * 2) + random_range(50)) < (Player.dex + Player.level + (Player.rank[THIEVES] * 10))) {
		    mprint("You picked the lock!");
		    Level->site[ox][oy].aux = UNLOCKED;
		    gain_experience(max(3, Level->depth));
		}
		else {
		    mprint("You failed to pick the lock.");
		}
	    }
	    else {
		mprint("That door is already unlocked!");
	    }
	}
    }
}

/* Use a magic key */
void i_key(pob o)
{
    int dir;
    int ox;
    int oy;

    o->used = FALSE;
    mprint("Unlock door: ");
    dir = getdir();

    if(dir == ABORT) {
	resetgamestatus(SKIP_MONSTERS);
    }
    else {
	ox = Player.x + Dirs[0][dir];
	oy = Player.y + Dirs[1][dir];

	if((Level->site[ox][oy].locchar != CLOSED_DOOR)
	   || loc_statusp(ox, oy, SECRET)) {
	    mprint("You can't unlock that!");
	    resetgamestatus(SKIP_MONSTERS);
	}
	else if(Plevel->site[ox][oy].aux == LOCKED) {
	    mprint("The lock clicks open!");
	    Level->site[ox][oy].aux = UNLOCKED;
	    --o->blessing;

	    if((o->blessing < 0) || (Level->depth == (MaxDungeonLevels - 1))) {
		mprint("The key disintegrates!");
		conform_lost_objects(1, o);
	    }
	    else {
		mprint("Your key glows faintly.");
		--o->blessing;
	    }
	}
	else {
	    mprint("That door is already unlocked!");
	}
    }
}

void i_corpse(pob o)
{
    switch(o->aux) {
    case ML0 + 1:
    case ML0 + 2:
    case ML0 + 3:
    case ML0 + 4:
    case ML2 + 0:
    case ML2 + 2:
    case ML7 + 3:
    case ML10 + 0:
	/* Cannibalism */
	mprint("Yechh! How could you!? You didn't even cooke him first!");

	if(Player.alignment > 0) {
	    Player.food = 25;
	}

	Player.food += 8;
	Player.alignment -= 10;
	foodcheck();

	break;
    case ML1 + 2:
	/* Fnord */
	mprint("You feel illuminated!");
	++Player.iq;

	break;
    case ML4 + 3:
	/* Denebian slime devil */
	mprint("I don't believe this. You ate Denebian Slime?");
	mprint("You deserve a horrible wasting death, uncurable by any means!");

	break;
    case ML5 + 0:
	mprint("Those dragon-steaks were fantastic!");
	Player.food = 24;
	foodcheck();

	break;
    case ML7 + 0:
	mprint("You feel infinitely more virile now.");
	Player.str = max(Player.str, Player.maxstr + 10);
	Player.food = 24;
	foodcheck();

	break;
    case ML9 + 1:
	mprint("Guess what? You're invisible.");

	if(Player.status[INVISIBLE] < 1000) {
	    Player.status[INVISIBLE] = 666;
	}

	Player.food += 6;
	foodcheck();

	break;
    case ML7 + 2:
	mprint("You ATE a unicorn's horn!?!?!?");
	Player.immunity[POISON] = 1000;

	break;
    case ML0 + 0:
    case ML1 + 0:
    case ML1 + 1:
    case ML1 + 4:
    case ML1 + 5:
    case ML1 + 6:
    case ML1 + 10:
    case ML2 + 1:
    case ML2 + 4:
    case ML4 + 1:
    case ML4 + 4:
    case ML5 + 3:
	mprint("Well, you forced it down. Not much nutrition though.");
	++Player.food;
	foodcheck();

	break;
    case ML1 + 3:
    case ML1 + 7:
    case ML2 + 3:
    case ML2 + 5:
    case ML3 + 1:
    case ML4 + 9:
    case ML9 + 3:
    case ML10 + 1:
	mprint("Oh, yuck. The \'food\' seems to be tainted.");
	mprint("You feel very sick. You throw up.");
	Player.food = min(Player.food, 4);

	if(!Player.immunity[INFECTION]) {
	    Player.status[DISEASED] += 24;
	}

	p_poison(10);

	break;
    default:
	mprint("It proved completely inedible, but you tried anyhow.");
    }
}

void i_accuracy(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;
    accuracy(o->blessing);
}

void i_perm_accuracy(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;

    if(o->used && (o->blessing > -1)) {
	Player.status[ACCURACTE] += 1500;
	mprint("You feel skillful and see bulls; eyes everywhere.");
    }
    else {
	Player.status[ACCURATE] -= 1500;

	if(Player.status[ACCURATE] < 1) {
	    Player.status[ACCURATE] = 0;
	    calc_melee();
	    mprint("Your vision blurs...");
	}
    }
}

void i_hero(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;
    hero(o->blessing);
}

void i_perm_hero(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;

    if(o->used) {
	if(o->blessing > -1) {
	    Player.status[HERO] += 1500;
	    calc_melee();
	    mprint("You feel super!");
	}
	else {
	    Player.status[HERO] = 0;
	    calc_melee();

	    if(!Player.immunity[FEAR]) {
		Player.status[AFRAID] += 1500;
		mprint("You feel cowardly...");
	    }
	}
    }
    else {
	if(o->blessing > -1) {
	    Player.status[HERO] -= 1500;

	    if(Player.status[HERO] < 1) {
		calc_melee();
		mprint("You feel less super now.");
		Player.status[HERO] = 0;
	    }
	}
	else {
	    Player.status[AFRAID] -= 1500;

	    if(Player.status[AFRAID] < 1) {
		mprint("You finally conquer your fear.");
		Player.status[AFRAID] = 0;
	    }
	}
    }
}

void i_levitate(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;
    levitate(o->blessing);
}

void i_perm_levitate(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;

    if(o->blessing > -1) {
	if(o->used) {
	    Player.status[LEVITATING] += 1400;
	    mprint("You start to float a few inches above the floor");
	    mprint("You find you can easily control your altitude");
	}
	else {
	    Player.status[LEVITATING] -= 1500;

	    if(Player.status[LEVITATING] < 1) {
		Player.status[LEVITATING] = 0;
		mprint("You sink to the floor.");
	    }
	}
    }
    else {
	i_perm_burden(o);
    }
}

void i_perm_protection(pob o)
{
    if(o->used) {
	if(o->blessing > -1) {
	    Player.status[PROTECTION] += (abs(o->plus) + 1);
	}
	else {
	    Player.status[PROTECTION] -= (abs(o->plus) + 1);
	}
    }
    else {
	if(o->blessing > -1) {
	    Player.status[PROTECTION] -= (abs(o->plus) + 1);
	}
	else {
	    Player.status[PROTECTION] += (abs(o->plus) + 1);
	}
    }

    calc_melee();
}

void i_perm_agility(pob o)
{
    o->known = 2;
    Objects[o->id].known = 1;

    if(o->used) {
	if(o->blessing > -1) {
	    Player.agi += (abs(o->plus) + 1);
	}
	else {
	    Player.agi -= (abs(o->plus) + 1);
	}
    }
    else {
	if(o->blessing > -1) {
	    Player.agi -= (abs(o->plus) + 1);
	}
	else {
	    Player.agi += (abs(o->plus) + 1);
	}
    }

    calc_melee();
}

void i_truesight(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;
    truesight(o->blessing);
}

void i_perm_truesight(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;

    if(o->used) {
	if(o->blessing > -1) {
	    Player.status[TRUESIGHT] += 1500;
	    mprint("You feel sharp!");
	}
	else {
	    Player.status[BLINDED] += 1500;
	    mprint("You've been blinded!");
	}
    }
    else {
	if(o->blessing > -1) {
	    Player.status[TRUESIGHT] -= 1500;

	    if(Player.status[TRUESIGHT] < 1) {
		mprint("You feel less keen now.");
		Player.status[TRUESIGHT] = 0;
	    }
	}
	else {
	    Player.status[BLINDED] -= 1500;

	    if(Player.status[BLINDED] < 1) {
		mprint("You can see again!");
		Player.status[BLINDED] = 0;
	    }
	}
    }
}

void i_illuminate(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;
    illuminate(o->blessing);
}

void i_perm_illuminate(pob o)
{
    o->known = 1;
    Objects[o->id].known = 1;

    if(o->used) {
	Player.status[ILLUMINATION] += 1500;
    }
    else {
	Player.status[ILLUMINATION] = max(0, Player.status[ILLUMINATION] - 1500);
    }
}

void i_trap(pob o)
{
    Objects[o->id].known = 1;

    if((Level->site[Player.x][Player.y].locchar != FLOOR)
       || (Level->site[Player.x][Player.y].p_locf != L_NO_OP)) {
	mprint("Your attempt fails.");
    }
    else if(!o->known) {
	mprint("Fiddling with the thin, you have a small accident...");
	p_movefunctions(o->aux);
    }
    else {
	mprint("You successfully set a trap at your location.");
	Level->site[Player.x][Player.y].p_locf = o->aux;
    }

    dispose_lost_objects(1, o);
}

void i_raise_portcullis(pob o)
{
    l_raise_portcullis();
    mprint("The box beeps once and explodes in your hands!");
    confrom_lost_objects(1, o);
}
