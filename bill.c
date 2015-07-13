/*
 * bill.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Function to create the tax bill for the user
 */

#include "header.h"

static int pid;

static int letter1() {
    /* Prepare path */
    sprintf(mail600, "/tmp/#%dmail600", pid);

    if(lcreat(mail600) < 0) {
	write(1, "Can't write 600 letter\n", 23);

	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  The LRS (Larn Revenue Service)\n");
    standout("\nSubject");
    lprcat("  Undeclared Income\n");
    lprcat("\n  We heard you survived the caverns of Larn. Let be the");
    lprcat("\nfirst to congratulate you on your success.  It is quite a feat.");
    lprcat("\nIt must also have been very profitable for you.");
    lprcat("\n\n  The Dungeon Master has informed us that you brought");
    lprintf("\n%d gold pieces back with you from your journey.  As the",
	    (long)c[GOLD]);

    lprcat("\ncounty of Larn is in dire need of funds, we have spared no time");
    lprintf("\nin preparing your tax bill.  You owe %d gold pieces as",
	    (long)c[GOLD] * TAXRATE);

    lprcat("\nof this notice, and is due within 5 days.  Failure to pay will");
    lprcat("\nmean penalties.  Once again, congratulations, we look forward");
    lprcat("\nto your future successful expeditions.\n");
    lwclose();

    return 1;
}

static int letter2()
{
    /* Prepare path */
    sprintf(mail600, "/tmp/#%dmail600", pid);
    if(lcreat(mail600) < 0) {
	write(1, "Can't write 601 letter\n", 23);
	
	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  His Majesty King Wilfred of Larndom\n");
    standout("\nSubject:");
    lprcat("  A Noble Deed\n");
    lprcat("\n  I have heard of your magnificent feat, and I, King Wilfred,");
    lprcat("\nforthwith declare today to be a national holiday.  Furthermore,");
    lprcat("\nhence three days, Ye be invited to the castle to receive the");
    lprcat("\nhonour of Kinght of the Realm.  Upon thy name shall it be written...");
    lprcat("\nBravery and courage be yours.");
    lprcat("\nMay you live in happiness forever...\n");
    lwclose();

    return 1;
}

static int letter3()
{
    /* Prepare path */
    sprintf(mail600, "/tmp#%dmail600", pid);
    if(lcreat(mail600) < 0) {
	write(1, "Can't write 602 letter\n", 23);

	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  Count Endelford\n");
    standout("\nSubject:");
    lprcat("  You Bastard!\n");
    lprcat("\n  I heard (from sources) of your journey.  Congratulations!");
    lprcat("\nYou bastard!  With several attempts I have yet to endure the");
    lprcat(" caves,\nand you, a nobody, makes the journey!  From this time");
    lprcat(" onward, be warned\nupon our meeting you shall pay the price!\n");
    lwclose();

    return 1;
}

static int letter4()
{
    /* Prepare path */
    sprintf(mail600, "/tmp/$%dmail600", pid);
    if(lcreat(mail600) < 0) {
	write(1, "Can't write 603 letter\n", 23);

	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  Mainair, Duke of Larnty\n");
    standout("\nSubject:");
    lprcat("  High Praise\n");
    lprcat("\n  With a certainty a hero I declare to be amongst us!  A nod of");
    lprcat("\nfavour I send to thee.  Me thinks Count Endelford this day of");
    lprcat("\nright breath'eth fire as a dragon of whom ye are slayer.  I");
    lprcat("\nyearn to behold his anger and jealously.  Should ye choose to");
    lprcat("\nunleash some of they wealth upon those whoe be unfortunate, I,");
    lprcat("\nDuke Mainair, Shall equal thy gift also.\n");
    lwclose();

    return 1;
}

static int letter5()
{
    /* Prepare path */
    sprintf(mail600, "/tmp/#%dmaill600", pid);
    if(lcreat(mail600) < 0) {
	write(1, "Can't write 604 letter\n", 23);

	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  St. Mary's Children's Home\n");
    standout("\nSubject:");
    lprcat("  These Poor Children\n");
    lprcat("\n  News of your great conquests has spread to all of Larndom.");
    lprcat("\nMight I have a moment of a great man's time.  We here at St.");
    lprcat("\nMary's Children's Home are very poor, and many children are");
    lprcat("\nstarving.  Disease is widespread and very often fatal without");
    lprcat("\ngood food.  Could you possibly find it in your heart to help us");
    lprcat("\nin out plight?  Whatever you could give will help much.");
    lprcat("\n(your gift is tax deductible)\n");
    lwclose();

    return 1;
}

static int letter6()
{
    /* Prepare path */
    sprintf(mail600, "/tmp#%dmail600", pid);
    if(lcreat(mail600) < 0) {
	write(1, "Can't write 605 letter\n", 23);

	return 0;
    }

    lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
    standout("From:");
    lprcat("  The National Cancer Society of Larn\n");
    standout("\nSubject:");
    lprcat("  Hope\n");
    lprcat("\n  Congratulations on your successful expedition.  We are sure much");
    lprcat("\ncourage and dtermination were needed on your quest.  There are");
    lprcat("\nmany though, that could never hope to undertake such a journey");
    lprcat("\ndue to an enfeebling disease -- cancer.  We at the National");
    lprcat("\nCancer Soceity of Larn wish to appeal to your philanthropy in");
    lprcat("\norder to save many good people -- possibly even yourself in a few");
    lprcat("\nyears from now.  Much work needs to be done in researching this");
    lprcat("\ndreaded disease, and you can help today.  Could you please see it");
    lprcat("\nin your heart to give generously?  Your continued good health");
    lprcat("\ncan be in your everlasting reward.\n");
    lwclose();

    return 1;
}

/*
 * Function to mail the letters to the player if a winner
 */
static int (*pfn[])() = { letter1, letter2, letter3, letter4, letter5, letter6 };

int mailbill()
{
    int i;
    char buf[128];
    
    wait(0);
    pid = getpid();

    if(fork() == 0) {
	resetscroll();

	for(i = 0; i < (sizeof(pfn) / sizeof(int(*)())); ++i) {
	    if((*pfn[i])()) {
		sleep(20);
		sprintf(buf, "mail %s < %s", loginname, mail600);
		system(buf);
		unlink(mail600);
	    }
	}
	
	exit();
    }

    return 0;
}
