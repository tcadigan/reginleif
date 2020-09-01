/*
 * Omega copyright (C) by Laurence Raphael Brothers, 1987-1989
 *
 * oaux3.c
 *
 * Some functions called by ocom.c, also see oaux1.c and oaxu2.c. This is a real
 * grab bag file. It contains functions used by oaux1 and o.c, as well as
 * elsewhere. It is mainly here so oaux1.c and oaux2 are not huge
 */

#include "oaux3.h"

#include <stdlib.h>
#include <string.h>

#include "oaux1.h"
#include "oaux2.h"
#include "ochar.h"
#include "ocity.h"
#include "ocom2.h"
#include "ocom3.h"
#include "oeffect1.h"
#include "oeffect2.h"
#include "oeffect3.h"
#include "oetc.h"
#include "oglob.h"
#include "oinv.h"
#include "oitem.h"
#include "olev.h"
#include "omon.h"
#include "omove.h"
#include "oscr.h"
#include "osite1.h"
#include "outil.h"

/* Check every ten minutes */
void tenminute_check()
{
    if(Time % 60 == 0) {
        hourly_check();
    }
    else {
        if(Current_Environment == Current_Dungeon) {
            wandercheck();
        }

        minute_status_check();
        tenminute_status_check();

        if((Player.status[DISEASED] < 1) && (Player.hp < Player.maxhp)) {
            Player.hp = min(Player.maxhp, Player.hp + Player.level + 1);
        }

        if(Current_Environment != E_COUNTRYSIDE) {
            indoors_random_event();
        }
    }
}

/*
 * Hourly check is same as ten_minute check except food is also checked, and
 * since time moves in hours outdoors, also outdoors_random_event is possible
 */
void hourly_check()
{
    --Player.food;
    foodcheck();

    /* Midnight, a new day */
    if(hour() == 0) {
        moon_check();
        ++Date;
    }

    torch_check();

    if(Current_Environment == Current_Dungeon) {
        wandercheck();
    }

    minute_status_check();
    tenminute_status_check();

    if((Player.status[DISEASED] == 0) && (Player.hp < Player.maxhp)) {
        Player.hp = min(Player.maxhp, Player.hp + Player.level + 1);
    }

    if(Current_Environment != E_COUNTRYSIDE) {
        indoors_random_event();
    }
}

void indoors_random_event()
{
    pml ml;
    pol ol;

    switch(random_range(1000)) {
    case 0:
        print3("You feel an unexplainable elation.");
        morewait();

        break;
    case 1:
        print3("You hear a distant rumbling.");
        morewait();

        break;
    case 2:
        print3("You realize your fly is open.");
        morewait();

        break;
    case 3:
        print3("You have a sudden craving for a pecan twirl.");
        morewait();

        break;
    case 4:
        print3("A mysterious healing flux settles over the level.");
        morewait();

        for(ml = Level->mlist; ml != NULL; ml = ml->next) {
            if(ml->m->hp > 0) {
                ml->m->hp = Monsters[ml->m->id].hp;
            }
        }

        Player.hp = max(Player.hp, Player.maxhp);

        break;
    case 5:
        print3("You discover an itch just where you can't scratch it.");
        morewait();

        break;
    case 6:
        print3("A cosmic ray strikes!");
        p_damage(10, UNSTOPPABLE, "a cosmic ray");
        morewait();

        break;
    case 7:
        print3("You catch your second wind...");
        ++Player.maxhp;
        Player.hp = Player.maxhp;
        Player.mana = max(Player.mana, calcmana());
        morewait();

        break;
    case 8:
        print3("You find some spare change in a hidden pocket.");
        Player.cash += ((Player.level * Player.level) + 1);
        morewait();

        break;
    case 9:
        print3("You fell strangely lucky.");
        morewait();

        break;
    case 10:
        print3("You trip over something hidden in a shadow...");
        morewait();
        ol = ((pol)malloc(sizeof(oltype)));
        ol->thing = create_object(difficulty());
        ol->next = Level->site[Player.x][Player.y].things;
        Level->site[Player.x][Player.y].things = ol;
        pickup();

        break;
    case 11:
        print3("A mysterious voice echos all around you...");
        hint();
        morewait();

        break;
    case 12:
        if(Balance > 0) {
            print3("You get word of the failure of your bank!");
            Balance = 0;
        }
        else {
            print3("You feel lucky.");
        }

        break;
    case 13:
        if(Balance > 0) {
            print3("You get word of a bank error in your favor!");
            Balance += 5000;
        }
        else {
            print3("You feel unlucky.");
        }

        break;
    }

    showflags();
}

void outdoors_random_event()
{
    int num;
    int i;
    int j;
    pob ob;

    switch(random_range(300)) {
    case 0:
        switch(Country[Player.x][Player.y].current_terrain_type) {
        case TUNDRA:
            mprint("It begins to snow. Heavily.");

            break;
        case DESERT:
            mprint("A sandstorm swirls around you.");

            break;
        default:
            if((Date > 75) && (Date < 330)) {
                mprint("You are drenched by a sudden downpour!");
            }
            else {
                mprint("It begins to snow. Heavily.");
            }
        }

        mprint("Due to inclement weatch conditions, you have become lost.");
        Precipitation += (random_range(12) + 1);
        setgamestatus(LOST);

        break;
    case 1:
        mprint("You enter a field of brightly colored flowers...");
        mprint("Wow, man! These are some pretty poppies...");
        morewait();
        mprint("poppies...");
        morewait();
        mprint("poppies...");
        morewait();
        print3("You become somewhat disoriented...");
        setgamestatus(LOST);

        break;
    case 2:
        mprint("You discover a sprig of athelas growing lonely in the wild.");
        mprint("Using your herbalist lore you cook a cake of lembas...");
        ob = ((pob)malloc(sizeof(objtype)));
        *ob = Objects[FOODID + 1];
        gain_item(ob);

        break;
    case 3:
        if(Precipitation > 0) {
            mprint("You are struck by a bolt of lightning!");
            p_damage(random_range(25), ELECTRICITY, "a lightning strike");
        }
        else {
            mprint("You feel static cling");
        }

        break;
    case 4:
        mprint("You find a fast-food establishment.");
        l_commandant();

        break;
    case 5:
        mprint("A weird howling tornado hits from out of the West!");
        mprint("You've been caught in a chaos storm!");
        num = random_range(300);

        if(num < 10) {
            mprint("You cell-structure was disrupted!");
            p_damage(random_range(100), UNSTOPPABLE, "a chaos storm");
        }
        else if(num < 20) {
            mprint("The chaos storm warps your frame!");
            mprint("You statistical entropy has been maximised.");
            mprint("You feel average...");
            Player.maxpow = (Player.maxstr + Player.maxcon + Player.maxdex + Player.maxagi + Player.maxiq + Player.maxpow + 12) / 6;
            Player.pow = Player.maxpow;
            Player.maxiq = Player.pow;
            Player.iq = Player.maxiq;
            Player.maxagi = Player.iq;
            Player.agi = Player.maxagi;
            Player.maxdex = Player.maxdex;
            Player.dex = Player.maxdex;
            Player.maxcon = Player.dex;
            Player.con = Player.maxcon;
            Player.maxstr = Player.con;
            Player.str = Player.maxstr;
        }
        else if(num < 30) {
            mprint("Your entire body glows with an eerie flickering light.");

            for(i = 1; i < MAXITEMS; ++i) {
                if(Player.possessions[i] != NULL) {
                    ++Player.possessions[i]->plus;

                    if(Player.possessions[i]->objchar == STICK) {
                        Player.possessions[i]->charge += 10;
                    }

                    Player.possessions[i]->blessing += 10;
                }
            }

            cleanse(1);
            mprint("You feel filled with energy!");
            Player.maxpow += 5;
            Player.pow += 5;
            Player.maxmana = calcmana() * 5;
            Player.mana = Player.maxmana;
            mprint("You also feel weaker. Paradoxical, no?");
            Player.con -= 5;
            Player.maxcon -= 5;

            if(Player.con < 3) {
                p_death("congestive heart failure");
            }
        }
        else if(num < 40) {
            mprint("Your entire body glows black.");
            dispel(-1);
            dispel(-1);
            Player.pow -= 10;
            Player.mana = 0;
        }
        else if(num < 60) {
            mprint("The storm deposits you in a strange place...");
            Player.x = random_range(WIDTH);
            Player.y = random_range(LENGTH);
            screencheck(Player.y);
        }
        else if(num < 70) {
            mprint("A tendril of the storm condenses and falls into your hands.");
            ob = ((pob)malloc(sizeof(objtype)));
            make_artifact(ob, -1);
            gain_item(ob);
        }
        else if(num < 80) {
            if(gamestatusp(MOUNTED)) {
                mprint("Your horse screams as he is transformed into an");
                mprint("imaginary dead tortoise.");
                mprint("You are no foot.");
                resetgamestatus(MOUNTED);
            }
            else {
                mprint("You notice you are riding a horse. Odd. Very odd...");
                mprint("Now that's a horse of a different color!");
                setgamestatus(MOUNTED);
            }
        }
        else if(num < 90) {
            mprint("You feel imbued with godlike power...");
            wish(1);
        }
        else if(num < 100) {
            mprint("The chaos storm has wiped your memory!");
            mprint("You feel extroardinarily naive...");
            mprint("You can't remember a thing! Not even your name.");
            Player.xp = 0;
            Player.level = 0;

            for(i = 0; i < NUMRANKS; ++i) {
                Player.rank[i] = 0;
            }

            for(i = 0; i < NUMSPELLS; ++i) {
                Spells[i].known = FALSE;
            }

            rename_player();
        }
        else {
            mprint("You survive the chaos storm relatively unscathed.");
            mprint("It was a learning experience.");
            gain_experience(1000);
        }

        break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        mprint("An encounter!");
        change_environment(E_TACTICAL_MAP);

        break;
    case 11:
        mprint("You find a Traveller's Aid station with maps of the local area.");

        if(gamestatusp(LOST)) {
            resetgamestatus(LOST);
            mprint("You know where you are now.");
        }

        for(i = Player.x - 5; i < (Player.x + 6); ++i) {
            for(j = Player.y - 5; j < (Player.y + 6); ++j) {
                if(inbounds(i, j)) {
                    Country[i][j].explored = TRUE;
                    Country[i][j].current_terrain_type = Country[i][j].base_terrain_type;
                }
            }
        }

        show_screen();

        break;
    case 12:
        if(!gamestatusp(MOUNTED)) {
            mprint("You develop blisters...");
            p_damage(1, UNSTOPPABLE, "blisters");
        }

        break;
    case 13:
        mprint("You discover an itch just where you can't scratch it.");

        break;
    case 14:
        mprint("A cosmic ray strikes!");
        p_damage(10, UNSTOPPABLE, "a cosmic ray");

        break;
    case 15:
        mprint("You feel strangely lucky.");

        break;
    case 16:
        mprint("The west wind carries with it a weird echoing voice...");
        hint();

        break;
    }

    dataprint();
    showflags();
}

char getlocation()
{
    char response;
    char result = 'H';
    menuprint(" (enter location [HCL])");

    response = mcigetc();

    while((response != 'h') && (response != 'c') && (response != 'l')) {
        response = mcigetc();
    }

    switch(response) {
    case 'h':
        menuprint(" High.");
        result = 'H';

        break;
    case 'c':
        menuprint(" Center.");
        result = 'C';

        break;
    case 'l':
        menuprint(" Low.");
        result = 'L';

        break;
    }

    return result;
}

/*
 * Chance for player to resist magic somehow. hostile_magic ranges in power from
 * 0 (weak) to 10 (strong)
 */
int magic_resist(int hostile_magic)
{
    if(((Player.rank[COLLEGE] + Player.rank[CIRCLE]) > 0) && (((Player.level / 2) + random_range(20)) > (hostile_magic + random_range(20)))) {
        if(Player.mana > (hostile_magic * hostile_magic)) {
            mprint("Thinking fast, you defend yourself with a counterspell!");
            Player.mana -= (hostile_magic * hostile_magic);
            dataprint();

            return TRUE;
        }
    }

    if(((Player.level / 4) + Player.status[PROTECTION] + random_range(20)) > (hostile_magic + random_range(30))) {
        mprint("You make your saving throw!");

        return TRUE;
    }
    else {
        return FALSE;
    }
}

void terrain_check(int takestime)
{
    if(!takestime) {
        switch(random_range(32)) {
        case 0:
            print2("Boingg!");

            break;
        case 1:
            print2("Whooosh!");

            break;
        case 2:
            print2("Over hill, over dale...");

            break;
        case 3:
            print2("...able to leap over 7 leagues in a single bound...");

            break;
        }
    }
    else if(gamestatusp(MOUNTED)) {
        switch(random_range(32)) {
        case 0:
        case 1:
            print2("Clippity Clop.");

            break;
        case 2:
            print2("...my spurs go jingle jangle jingle...");

            break;
        case 3:
            print2("...as I go riding merrily along...");

            break;
        }
    }
    else {
        switch(random_range(32)) {
        case 0:
            print2("Trudge. Trudge.");

            break;
        case 1:
            print2("The road goes ever onward...");

            break;
        }
    }

    switch(Country[Player.x][Player.y].current_terrain_type) {
    case RIVER:
        if((Player.y < 6) && (Player.x > 20)) {
            locprint("Star Lake.");
        }
        else if(Player.y < 41) {
            if(Player.x < 10) {
                locprint("Aerie River.");
            }
            else {
                locprint("The Great Flood.");
            }
        }
        else if(Player.x < 42) {
            locprint("The Swamp Runs.");
        }
        else {
            locprint("River Greenshriek.");
        }

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
        }

        break;
    case ROAD:
        locprint("A well-maintained road.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        break;
    case PLAINS:
        locprint("A rippling sea of grass.");

        if(takestime) {
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
            }
        }

        break;
    case TUNDRA:
        locprint("The Great Northern Wastes.");

        if(takestime) {
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
            }
        }

        break;
    case FOREST:
        if(Player.y < 10) {
            locprint("The Deepwood.");
        }
        else if(Player.y < 18) {
            locprint("The Forest of Erelon.");
        }
        else if(Player.y < 46) {
            locprint("The Great Forest.");
        }

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
            }
        }

        break;
    case JUNGLE:
        locprint("Greenshriek Jungle.");

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
                Time += 60;
                hourly_check();
            }
        }

        break;
    case DESERT:
        locprint("The Waste of Time.");

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
                Time += 60;
                hourly_check();
            }
        }

        break;
    case MOUNTAINS:
        if((Player.y < 9) && (Player.x < 12)) {
            locprint("The Magic Mountains");
        }
        else if((Player.y < 9) && (Player.y > 2) && (Player.x < 40)) {
            locprint("The Peaks of the Fist.");
        }
        else if(Player.x < 52) {
            locprint("The Rift Mountains.");
        }
        else {
            locprint("Borderland Mountains.");
        }

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
                Time += 60;
                hourly_check();
                Time += 60;
                hourly_check();
            }
        }

        break;
    case PASS:
        locprint("A hidden pass.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        break;
    case CHAOS_SEA:
        locprint("The Sea of Chaos.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        l_chaos();
        mprint("You have entered the sea of chaos...");

        break;
    case SWAMP:
        locprint("The Loathly Swamp.");

        if(takestime) {
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();
            Time += 60;
            hourly_check();

            if(!gamestatusp(MOUNTED)) {
                Time += 60;
                hourly_check();
                Time += 60;
                hourly_check();
            }
        }

        break;
    case CITY:
        if(gamestatusp(LOST)) {
            resetgamestatus(LOST);
            mprint("Well, I guess you know where you are now...");
        }

        locprint("Outside Rampart, the city.");

        break;
    case VILLAGE:
        if(gamestatusp(LOST)) {
            resetgamestatus(LOST);
            mprint("The village guards let you know where you are...");
        }
        
        locprint("Outside a small village.");

        break;
    case CAVES:
        locprint("A deserted hillside.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("You notice a concealed entrance into the hill.");

        break;
    case CASTLE:
        locprint("Near a fortified castle.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("The castle is hewn from solid granite. The drawbridge is down.");

        break;
    case TEMPLE:
        switch(Country[Player.x][Player.y].aux) {
        case ODIN:
            locprint("A rough-hewn grantie temple.");

            break;
        case SET:
            locprint("A black pyramidal temple made of sandstone.");

            break;
        case ATHENA:
            locprint("A classical marble-columned temple.");

            break;
        case HECATE:
            locprint("A temple of ebony adorned with ivory.");

            break;
        case DRUID:
            locprint("A temple formed of living trees.");

            break;
        case DESTINY:
            locprint("A temple of some mysterious blue crystal.");

            break;
        }

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("You notice an entrance conveniently at hand.");

        break;
    case MAGIC_ISLE:
        locprint("A strange island in the midst of the Sea of Chaos.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("There is a narrow causway to the island from here.");

        break;
    case STARPEAK:
        locprint("Star Peak.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("The top of the mountain seems to glow with an allochroous aura.");

        break;
    case DRAGONLAIR:
        locprint("A rocky chasm.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("You are at a cave entrance from which you see the glint of gold.");

        break;
    case VOLCANO:
        locprint("HellWell Volcano.");

        if(takestime) {
            Time += 60;
            hourly_check();
        }

        mprint("A shimmer of heat lightning plays about the crater rim.");

        break;
    default:
        locprint("I haven't any idea where you are!!!");

        break;
    }

    outdoors_random_event();
}

void countrysearch()
{
    int x;
    int y;

    Time += 60;
    hourly_check();

    for(x = Player.x - 1; x < (Player.x + 2); ++x) {
        for(y = Player.y - 1; y < (Player.y + 2); ++y) {
            if(inbounds(x, y)) {
                if(Country[x][y].current_terrain_type != Country[x][y].base_terrain_type) {
                    mprint("Your search was fruitful!");
                    Country[x][y].current_terrain_type = Country[x][y].base_terrain_type;
                    mprint("You discovered:");
                    mprint(countryid(Country[x][y].base_terrain_type));
                }
            }
        }
    }
}

char *countryid(char terrain)
{
    switch(terrain) {
    case MOUNTAINS:
        strcpy(Str1, "Almost impassable mountains");

        break;
    case PLAINS:
        strcpy(Str1, "Seemingly endless plains");

        break;
    case TUNDRA:
        strcpy(Str1, "A frosty stretch of tundra");

        break;
    case ROAD:
        strcpy(Str1, "A paved highway");

        break;
    case PASS:
        strcpy(Str1, "A secret mountain pass");

        break;
    case RIVER:
        strcpy(Str1, "A rolling river");

        break;
    case CITY:
        strcpy(Str1, "The city of Rampart");

        break;
    case VILLAGE:
        strcpy(Str1, "A rural village");

        break;
    case FOREST:
        strcpy(Str1, "A verdant forest");

        break;
    case JUNGLE:
        strcpy(Str1, "A densely overgrown jungle");

        break;
    case SWAMP:
        strcpy(Str1, "A swampy fen");

        break;
    case VOLCANO:
        strcpy(Str1, "A huge active volcano");

        break;
    case CASTLE:
        strcpy(Str1, "An imposing castle");

        break;
    case STARPEAK:
        strcpy(Str1, "A mysterious mountain.");

        break;
    case DRAGONLAIR:
        strcpy(Str1, "A cavern filled with treasure.");

        break;
    case MAGIC_ISLE:
        strcpy(Str1, "An island emanating magic.");

        break;
    case CAVES:
        strcpy(Str1, "A hidden cave entrance");

        break;
    case TEMPLE:
        strcpy(Str1, "A neoclassical temple");

        break;
    case DESERT:
        strcpy(Str1, "A sere desert");

        break;
    case CHAOS_SEA:
        strcpy(Str1, "The Sea of Chaos");

        break;
    default:
        strcpy(Str1, "I have no idea.");

        break;
    }

    return Str1;
}

/* i is the offset from CITYSITEBASE, not the actual constant */
char * citysiteid(int i)
{
    switch(i + CITYSITEBASE) {
    case L_CHARITY:
        strcpy(Str4, "hospice");

        break;
    case L_ORDER:
        strcpy(Str4, "order of paladins");

        break;
    case L_ARMORER:
        strcpy(Str4, "armorer");

        break;
    case L_CLUB:
        strcpy(Str4, "explorers' club");

        break;
    case L_GYM:
        strcpy(Str4, "gymnasium");

        break;
    case L_THIEVES_GUILD:
        strcpy(Str4, "thieves' guild");

        break;
    case L_COLLEGE:
        strcpy(Str4, "collegium magii");

        break;
    case L_HEALER:
        strcpy(Str4, "healer");

        break;
    case L_CASINO:
        strcpy(Str4, "casino");

        break;
    case L_TAVERN:
        strcpy(Str4, "tavern");

        break;
    case L_MERC_GUILD:
        strcpy(Str4, "mercenary guild");

        break;
    case L_ALCHEMIST:
        strcpy(Str4, "alchemist");

        break;
    case L_SORCERORS:
        strcpy(Str4, "sorcerors' guild");

        break;
    case L_CASTLE:
        strcpy(Str4, "castle");

        break;
    case L_ARENA:
        strcpy(Str4, "arena");

        break;
    case L_DPW:
        strcpy(Str4, "department of public works");

        break;
    case L_LIBRARY:
        strcpy(Str4, "library");

        break;
    case L_PAWN_SHOP:
        strcpy(Str4, "pawn shop");

        break;
    case L_BANK:
        strcpy(Str4, "bank");

        break;
    case L_CONDO:
        strcpy(Str4, "condo");

        break;
    case L_ORACLE:
        strcpy(Str4, "oracle");

        break;
    case L_BROTHEL:
        strcpy(Str4, "brothel");

        break;
    case L_DINER:
        strcpy(Str4, "diner");

        break;
    case L_COMMANDANT:
        strcpy(Str4, "fast food");

        break;
    case L_CRAP:
        strcpy(Str4, "les crapleux");

        break;
    case L_TEMPLE:
        strcpy(Str4, "temple");

        break;
    case L_COUNTRYSIDE:
        strcpy(Str4, "city gates");

        break;
    default:
        strcpy(Str4, "???");

        break;
    }

    return Str4;
}

void expandsiteabbrevs(char prefix[80])
{
    int i;
    int printed = FALSE;

    menuclear();
    menuprint("\nPossible Sites:\n");

    for(i = 0; i < NUMCITYSITES; ++i) {
        if(CitySiteList[i][0] && strprefix(prefix, citysiteid(i))) {
            menuprint(citysiteid(i));
            menuprint("\n");
            printed = TRUE;
        }
    }

    if(!printed) {
        menuprint("\nno known sites match that prefix!");
    }
}

int expandsite(char prefix[80])
{
    int i;
    int site = 0;
    int matched = 0;

    for(i = 0; i < NUMCITYSITES; ++i) {
        if(CitySiteList[i][0] && strprefix(prefix, citysiteid(i))) {
            site = i;
            ++matched;
        }
    }

    if(matched == 0) {
        print3("No known locations match that prefix!");

        return ABORT;
    }
    else if(matched > 1) {
        print3("That is an ambiguous abbreviation!");

        return ABORT;
    }
    else {
        return site;
    }
}

int parsecitysite()
{
    int site = -1;
    int place = 0;
    char byte;
    char prefix[80];

    byte = mgetc();

    if((byte >= 'A') && (byte <= 'Z')) {
        maddch(byte + 'a' - 'A');
        prefix[place] = byte + 'a' - 'A';
        prefix[place + 1] = 0;
        ++place;
    }
    else if((byte == ' ') || ((byte >= 'a') && (byte <= 'z'))) {
        maddch(byte);
        prefix[place] = byte;
        prefix[place + 1] = 0;
        ++place;
    }
    else if((byte == 8) || (byte == 127)) {
        /* ^h or delete */
        prefix[place] = 0;

        if(place > 0) {
            --place;
            dobackspace();
        }
    }
    else if(byte == '?') {
        maddch(byte);
        expandsiteabbrevs(prefix);
        dobackspace();
    }
    else if(byte == '\n') {
        site = expandsite(prefix);
    }
    else if(byte == ESCAPE) {
        site = ABORT;
    }

    while(site == -3) {
        byte = mgetc();

        if((byte >= 'A') && (byte <= 'Z')) {
            maddch(byte + 'a' - 'A');
            prefix[place] = byte + 'a' - 'A';
            prefix[place + 1] = 0;
            ++place;
        }
        else if((byte == ' ') || ((byte >= 'a') && (byte <= 'z'))) {
            maddch(byte);
            prefix[place] = byte;
            prefix[place + 1] = 0;
            ++place;
        }
        else if((byte == 8) || (byte == 127)) {
            /* ^h or delete */
            prefix[place] = 0;

            if(place > 0) {
                --place;
                dobackspace();
            }
        }
        else if(byte == '?') {
            maddch(byte);
            expandsiteabbrevs(prefix);
            dobackspace();
        }
        else if(byte == '\n') {
            site = expandsite(prefix);
        }
        else if(byte == ESCAPE) {
            site = ABORT;
        }
    }

    xredraw();

    return site;
}

/* Are there hostile monsters within 2 moves? */
int hostilemonstersnear()
{
    int i;
    int j;
    int hostile = FALSE;

    for(i = Player.x - 2; (i < (Player.x + 3)) && !hostile; ++i) {
        for(j = Player.x - 2; (j < (Player.y + 3)) && !hostile; ++j) {
            if(inbounds(i, j)) {
                if(Level->site[i][j].creature != NULL) {
                    hostile = m_statusp(Level->site[i][j].creature, HOSTILE);
                }
            }
        }
    }

    return hostile;
}

/*
 * Random effects from some of stones in villages. It alignment of stone is
 * alignment of player, gets done sooner.
 */
int stonecheck(int alignment)
{
    int *stone;
    int match = FALSE;
    int cycle = FALSE;
    int i;

    if(alignment == 1) {
        stone = &Lawstone;

        if(Player.alignment > 0) {
            match = TRUE;
        }
        else {
            match = FALSE;
        }
    }
    else if(alignment == -1) {
        stone = &Chaostone;

        if(Player.alignment < 0) {
            match = TRUE;
        }
        else {
            match = FALSE;
        }
    }
    else {
        stone = &Mindstone;
        match = FALSE;
    }

    if(match) {
        *stone += (random_range(4) + random_range(4));
    }
    else {
        *stone += random_range(4);
    }

    switch((*stone)++) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    case 22:
    case 24:
    case 26:
    case 28:
    case 30:
    case 32:
    case 34:
    case 36:
    case 38:
    case 40:
        print1("The stone glows grey.");
        print2("Not much seems to happen this time.");
        --(*stone);

        break;
    case 1:
        print1("The stone glows black");
        print2("A burden has been removed from your shoulders...");
        print3("Your pack has disintegrated!");

        for(i = 0; i < MAXPACK; ++i) {
            if(Player.pack[i] != NULL) {
                free((char *)Player.pack[i]);
                Player.pack[i] = NULL;
            }
        }

        Player.packptr = 0;

        break;
    case 3:
        print1("The stone glows microwave");
        print2("A vortex of antimana spins about you!");
        dispel(-1);

        break;
    case 5:
        print1("The stone glows infrared");
        print2("A portal opens nearby and an obviously confused monster appears!");
        summon(-1, -1);

        break;
    case 7:
        print1("The stone glows brick red");
        print2("A gold piece falls from the heaves into your money pouch!");
        ++Player.cash;

        break;
    case 9:
        print1("The stone glows cherry red");
        print2("A flush of warmth spreads through your body.");
        augment(1);

        break;
    case 11:
        print1("The stone glows orange");
        print2("A flux of energy blasts you!");
        manastorm(Player.x, Player.y, random_range(Player.maxhp) + 1);

        break;
    case 13:
        print1("The stone glows lemon yellow");
        print2("You're surrounded by enemies! You begin to foam at the mouth.");
        Player.status[BERSERK] += 10;

        break;
    case 15:
        print1("The stone clows yellow");
        print2("Oh no! the DREADED AQUAE MORTIS!");
        morewait();
        print2("No, wait, it's just your imagination.");

        break;
    case 17:
        print1("The stone glows chartreuse");
        print2("You joints stiffen up.");
        Player.agi -= 3;

        break;
    case 19:
        print1("The stone glows green");
        print2("You come down with an acute case of Advanced Leprosy.");
        Player.status[DISEASED] = 1100;
        Player.hp = 1;
        Player.dex -= 5;

        break;
    case 21:
        print1("The stone glows forest green");
        print2("You feel wonderful!");
        Player.status[HERO] += 10;

        break;
    case 23:
        print1("The stone glows cyan");
        print2("You feel a strange twisting sensation...");
        strategic_teleport(-1);

        break;
    case 25:
        print1("The stone glows blue");
        morewait();
        print1("You feel a tingle of an unearthly intuition:");
        hint();

        break;
    case 27:
        print1("The stone glows navy blue");
        print2("A sudden shock of knowledge overcomes you.");
        morewait();
        clearmsg();
        identify(1);
        knowledge(1);

        break;
    case 29:
        print1("The stone glows blue-violet");
        print2("You feel forgetful.");

        for(i = 0; i < NUMSPELLS; ++i) {
            if(Spells[i].known) {
                Spells[i].known = FALSE;

                break;
            }
        }

        break;
    case 31:
        print1("The stone glows violet");
        acquire(0);

        break;
    case 33:
        print1("The stone glows deep purple");
        print2("You vanish.");
        Player.status[INVISIBLE] += 10;

        break;
    case 35:
        print1("The stone glows ultraviolet");
        print2("All your hair raises up on end...A bolt of lightning hits you!");
        p_damage(random_range(Player.maxhp), ELECTRICITY, "mystic lightning");

        break;
    case 37:
        print1("The stone glows roentgen");
        print2("You feel more experienced.");
        gain_experience((Player.level + 1) * 250);

        break;
    case 39:
        print1("The stone glows gamma");
        print2("Your left hand shines silvery, and your right emits a golden aura.");
        enchant(1);
        bless(1);
        print3("Your hands stop glowing.");

        break;
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
        print1("The stone glows cosmic!");
        print2("The stone's energy field quiets for a moment...");
        *stone = 50;
        cycle = TRUE;

        break;
    default:
        print1("The stone glows polka-dot (?!?!?!?)");
        print2("You feel a strange twisting sensation...");
        *stone = 0;
        strategic_teleport(-1);

        break;
    }

    calc_melee();

    return cycle;
}

void alert_guards()
{
    int foundguard = FALSE;
    pml ml;

    for(ml = Level->mlist; ml != NULL; ml = ml->next) {
        if(((ml->m->id == (ML0 + 3)) || ((ml->m->id == (ML0 + 8)) && (ml->m->aux2 == 15))) && (ml->m->hp > 0)) {
            /* Guard and Justiciar respectively */
            foundguard = TRUE;
            m_status_set(ml->m, AWAKE);
            m_status_set(ml->m, HOSTILE);
        }
    }

    if(foundguard) {
        print2("You hear a whistle and the sound of running feet!");
    }

    if(!foundguard && (Current_Environment == E_CITY)) {
        print2("The last member of the Order of Paladins dies...");
        gain_experience(1000);
        Player.alignment -= 250;

        if(!gamestatusp(KILLED_LAWBRINGER)) {
            morewait();
            print1("A chime sounds from far away...The sounds grows stronger...");
            print2("Suddenly the great shadowy form of the LawBringer appears over");
            print3("the city. He points his finger at you...");
            morewait();
            print1("Cursed art though, minion of chaos! May thy strength fail thee");
            print2("in they hour of need! You feel an unearthly shiver as the");
            print3("LawBringer waves his palm across the city skies...");
            Player.str /= 2;
            morewait();
            print1("You hear a bell tolling, and eerie moans all around you...");
            print2("Suddenly, the image of the LawBringer is gone.");
            print3("You hear a guardsman's whistle in the distance!");
            morewait();
            resurrect_guards();
        }
        else {
            print1("The Order's magical defenses have dropped, and the");
            print2("Legions of Chaos strike...");
            morewait();
            print1("The city shakes! An earthquake has struck!");
            print2("Cracks open in the street, and a chasm engulfs the Order HQ!");
            print3("Flames lick across the sky and you hear wild laughter...");
            gain_experience(5000);
            destroy_order();
        }
    }
}

/* Can only occur when player is in city, so OK to use Level */
void destroy_order()
{
    int i;
    int j;
    setgamestatus(DESTROYED_ORDER);

    if(Level != City) {
        print1("Zounds! A Serious Mistake!");
    }
    else {
        for(i = 35; i < 46; ++i) {
            for(j = 61; j < 64; ++j) {
                Level->site[i][j].locchar = RUBBLE;
                Level->site[i][j].p_locf = L_RUBBLE;

                if(Level->site[i][j].creature != NULL) {
                    free((char *)Level->site[i][j].creature);
                }

                make_site_monster(i, j, ML2 + 6);
                Level->site[i][j].creature->monstring = salloc("ghost of a Paladin");
                m_status_set(Level->site[i][j].creature, HOSTILE);
            }
        }
    }
}

int maneuvers()
{
    int m;
    float times;

    if(Player.status[HASTED]) {
        times = 2.0;
    }
    else {
        times = 1.0;
    }

    if(Player.status[SLOWED]) {
        times *= 0.5;
    }
    else {
        times *= 1.0;
    }

    m = 2 + (Player.level / 7);

    if(Player.rank[ARENA]) {
        ++m;
    }

    m = (int)(m * times);
    m = min(8, max(1, m));

    return m;
}

/* For when hase runs out, etc. */
void default_maneuvers()
{
    int i;

    morewait();
    clearmsg();
    print1("Warning, resetting your combat options to the default.");
    print2("Use the 'F' command to select which options you prefer.");
    morewait();

    for(i = 0; i < maneuvers(); i += 2) {
        Player.meleestr[i * 2] = 'A';
        Player.meleestr[(i + 1) * 2] = 'B';
        Player.meleestr[((i + 1) * 2) + 1] = 'C';
        Player.meleestr[(i * 2) + 1] = 'C';
    }

    Player.meleestr[maneuvers() * 2] = 0;
}
