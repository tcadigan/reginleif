#include "main.h"
#include "rogue.h"
#include "machdep.h"

#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

int Uid;

int main(int argc, char *argv[])
{
    static int amount;

    while(1) {
	printf("> ");
	scanf("%d %d %s", &Uid, &amount, whoami);
	score(amount, 1, 'B');
    }
}

struct sc_ent {
    int sc_score;
    int sc_uid;
    char sc_name[80];
    int sc_flags;
    int sc_level;
    char sc_monster;
};

typedef struct sc_ent SCORE;

SCORE *scores;

// sccomp:
//     Something...
int sccomp(SCORE *s1, SCORE *s2)
{
    return (s2->sc_score - s1->sc_score);
}

// prscore:
//     Something...
int prscore(SCORE *sp)
{
    register char *killer;
    static char *reason[] = {
	"killed",
	"quit",
	"A total winner"
    };
    char *killname();

    printf("%d\t%d\t%s: %s on level %d",
           sp - scores + 1,
           sp->sc_score,
           sp->sc_name,
           reason[sp->sc_flags],
           sp->sc_level);
    if(sp->sc_flags == 0) {
	killer = killname(sp->sc_monster);
	printf(" by a%s %s", vowelstr(killer), killer);
    }
    printf(".\n");

    return 0;
}

// score:
//     Something...
int score(int amount, int flags, char monst)
{
    SCORE *sp;
    SCORE *myscore;
    int fd;
    FILE *outf;
    int uid;
    int i;
    int size;
    SCORE *endsp;
    SCORE sbuf;
    static struct stat stbuf;

    // Open file and read list
    fd = open("testroll", 2);
    if(fd < 0) {
	return;
    }
    outf = fdopen(fd, "w");

    signal(SIGINT, SIG_DFL);
    sp = &sbuf;
    uid = Uid;
    fstat(fd, &stbuf);
    size = -1;
    for(i = 0; encread(sp, sizeof(SCORE), fd) > 0; ++i) {
	if(sp->sc_uid == uid) {
	    size = stbuf.st_size;
	    break;
	}
    }
    printf("uid = %d, size = %d", uid, size);
    if(size == -1) {
	size = stbuf.st_size + sizeof(SCORE);
        printf("(%d)", size);
    }
    scores = (SCORE *) malloc(size);
    printf(", i = %d\n", i);
    lseek(fd, 0L, 0);
    for(sp = scores; encread(sp, sizeof(SCORE), fd) > 0; ++sp) {
	continue;
    }
    // Insert him in the list if need be
    sp = &scores[i];
    if(!waswizard && ((sp->sc_score < amount) || (sp->sc_uid != uid))) {
	sp->sc_score = amount;
	strcpy(sp->sc_name, whoami);
	sp->sc_flags = flags;
	if(flags == 2) {
	    sp->sc_level = max_level;
        }
	else {
	    sp->sc_level = level;
        }
	sp->sc_monster = monst;
	sp->sc_uid = uid;
    }

    // Print the list
    printf("Rank\tScore\tName\n");
    size /= sizeof(SCORE);
    qsort(scores, size, sizeof(SCORE), sccomp);
    for(myscore = scores; myscore->sc_uid != uid; ++myscore) {
	continue;
    }

    if((sp - 4) < scores) {
        sp = scores;
    }
    else {
        sp = sp - 4;
    }

    if((sp + 4) >= &scores[size]) {
        endsp = &scores[size - 1];
    }
    else {
        endsp = sp + 4;
    }
    
    while (sp <= endsp) {   
	prscore(sp);
        ++sp;
    }
    
    fseek(outf, (long) i * sizeof (SCORE), 0);
    encwrite(myscore, sizeof (SCORE), outf);
    fclose(outf);

    return 0;
}

