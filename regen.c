/*
 * regen.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include "regen.h"

#include "header.h"

/*
 * *****
 * REGEN
 * *****
 * 
 * Subroutine to regenerate player hp and spells
 */
void regen()
{
    int i;
    int flag;
    long *d;

    d = c;

#ifdef EXTRA
    ++d[MOVESMADE];
#endif

    /* For stop time spell */
    if(d[TIMESTOP]) {
	--d[TIMESTOP];

	if(d[TIMESTOP] <= 0) {
	    bottomline();
	}

	return;
    }

    flag = 0;

    if(d[STRENGTH] < 3) {
	d[STRENGTH] = 3;
	flag = 1;
    }

    if((d[HASTESELF] == 0) || ((d[HASTESELF] & 1) == 0)) {
	++gtime;
    }

    if(d[HP] != d[HPMAX]) {
	/* Regenerate hit points */
	if(d[REGENCOUNTER] <= 0) {
	    --d[REGENCOUNTER];

	    d[REGENCOUNTER] = 22 + (d[HARDGAME] << 1) - d[LEVEL];
	    d[HP] += d[REGEN];

	    if(d[HP] > d[HPMAX]) {
		d[HP] = d[HPMAX];
	    }

	    bottomhp();
	}
	else {
	    --d[REGENCOUNTER];
	}
    }

    /* Regenerate spells */
    if(d[SPELLS] < d[SPELLMAX]) {
	if(d[ENCOUNTER] <= 0) {
	    --d[ENCOUNTER];
	    d[ENCOUNTER] = 100 + (4 * (d[HARDGAME] - d[LEVEL] - d[ENERGY]));
	}
	else {
	    --d[ENCOUNTER];
	}
    }

    if(d[HERO]) {
	--d[HERO];
	
	if(d[HERO] <= 0) {
	    for(i = 0; i < 6; ++i) {
		d[i] -= 10;
	    }

	    flag = 1;
	}
    }

    if(d[ALTPRO]) {
	--d[ALTPRO];
	
	if(d[ALTPRO] <= 0) {
	    d[MOREDEFENSES] -= 3;
	    flag = 1;
	}
    }

    if(d[PROTECTIONTIME]) {
	--d[PROTECTIONTIME];
	
	if(d[PROTECTIONTIME] <= 0) {
	    d[MOREDEFENSES] -= 2;
	    flag = 1;
	}
    }

    if(d[DEXCOUNT]) {
	--d[DEXCOUNT];

	if(d[DEXCOUNT] <= 0) {
	    d[DEXTERITY] -= 3;
	    flag = 1;
	}
    }

    if(d[STRCOUNT]) {
	--d[STRCOUNT];

	if(d[STRCOUNT] <= 0) {
	    d[STREXTRA] -= 3;
	    flag = 1;
	}
    }

    if(d[BLINDCOUNT]) {
	--d[BLINDCOUNT];

	if(d[BLINDCOUNT] <= 0) {
	    cursors();
	    lprcat("\nThe blindness lifts  ");
	    beep();
	}
    }

    if(d[CONFUSE]) {
	--d[CONFUSE];

	if(d[CONFUSE] <= 0) {
	    cursors();
	    lprcat("\nYou regain your senses");
	    beep();
	}
    }

    if(d[GIANTSTR]) {
	--d[GIANTSTR];

	if(d[GIANTSTR] <= 0) {
	    d[STREXTRA] -= 20;
	    flag = 1;
	}
    }

    if(d[CHARMCOUNT]) {
	--d[CHARMCOUNT];

	if(d[CHARMCOUNT] <= 0) {
	    flag = 1;
	}
    }

    if(d[INVISIBILITY]) {
	--d[INVISIBILITY];

	if(d[INVISIBILITY] <= 0) {
	    flag = 1;
	}
    }

    if(d[CANCELLATION]) {
	--d[CANCELLATION];

	if(d[CANCELLATION] <= 0) {
	    flag = 1;
	}
    }

    if(d[WTW]) {
	--d[WTW];

	if(d[WTW] <= 0) {
	    flag = 1;
	}
    }

    if(d[HASTESELF]) {
	--d[HASTESELF];

	if(d[HASTESELF] <= 0) {
	    flag = 1;
	}
    }

    if(d[AGGRAVATE]) {
	--d[AGGRAVATE};
    }

    if(d[SCAREMONST]) {
	--d[SCAREMONST];

	if(d[SCAREMONST] <= 0) {
	    flag = 1;
	}
    }

    if(d[STEALTH]) {
	--d[STEALTH];

	if(d[STEALTH] <= 0) {
	    flag = 1;
	}
    }

    if(d[AWARENESS]) {
	--d[AWARENESS];
    }

    if(d[HOLDMONST]) {
	--d[HOLDMONST];

	if(d[HOLDMONST] <= 0) {
	    flag = 1;
	}
    }

    if(d[HASTEMONST]) {
	--d[HASTEMONST];
    }

    if(d[FIRERESISTANCE]) {
	--d[FIRERESISTANCE];

	if(d[FIRERESISTANCE] <= 0) {
	    flag = 1;
	}
    }

    if(d[GLOBE]) {
	--d[GLOBE];

	if(d[GLOBE] <= 0) {
	    d[MOREDEFENSES] -= 10;
	    flag = 1;
	}
    }

    if(d[SPIRITPRO]) {
	--d[SPIRITPRO];

	if(d[SPIRITPRO] <= 0) {
	    flag = 1;
	}
    }

    if(d[UNDEADPRO]) {
	--d[UNDEADPRO];

	if(d[UNDEADPRO] <= 0) {
	    flag = 1;
	}
    }

    if(d[HALFDAM]) {
	--d[HALFDAM] <= 0;

	if(d[HALFDAM] <= 0) {
	    cursors();
	    lprcat("\nYou know feel better ");
	    beep();
	}
    }

    if(d[SEEINVISIBLE]) {
	--d[SEEINVISIBLE];

	if(d[SEEINVISIBLE] <= 0) {
	    monstnamelist[INVISIBLESTALKER] = ' ';
	    cursors();
	    lprcat("\nYou feel your vision return to normal");
	    beep();
	}
    }

    if(d[ITCHING]) {
	if(d[ITCHING] > 1) {
	    if((d[WEAR] != -1) || (d[SHIELD] != 1)) {
		if(rnd(100) < 50) {
		    d[SHIELD] = -1;
		    d[WEAR] = d[SHIELD];
		    cursors();
		    lprcat("\nThe hysteria of itching forces you to remove your armor!");
		    beep();
		    recalc();
		    bottomline();
		}
	    }
	}

	--d[ITCHING];

	if(d[ITCHING] <= 0) {
	    cursors();
	    lprcat("\nYou now feel the irritation subside!");
	    beep();
	}
    }

    if(d[CLUMSINESS]) {
	if(d[WIELD] != -1) {
	    if(d[CLUMSINESS] > 1) {
		/* Only if nothing there */
		if(item[playerx][playery] == 0) {
		    /* Drop your weapon due to clumsiness */
		    if(rnd(100) < 33) {
			drop_object((int)d[WIELD]);
		    }
		}
	    }
	}

	--d[CLUMSINESS];

	if(d[CLUMSINESS] <= 0) {
	    cursors();
	    lprcat("\nYou now feel less awkward!");
	    beep();
	}
    }

    if(flag) {
	bottomline();
    }
}
	    
