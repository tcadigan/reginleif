/*
 * Omega copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * olev.c
 *
 * Functions dealing with dungeon and country levels aside from actual
 * level structure generation
 */

#include "olev.h"

#include "oglob.h"

/* Monsters for tactical encounters */
void make_country_monsters(char terrain)
{
    pml tml;
    pml ml = NULL;

    static in plains[10] = {
        BUNNY,
        BUNNY,
        BLACKSNAKE,
        HAWK,
        IMPALA,
        WOLF,
        LION,
        BRIGAND,
        RANDOM
    };

    static int forest[10] = {
        BUNNY,
        QUAIL,
        HAWK,
        BADGER,
        DEER,
        DEER,
        WOLF,
        BEAR,
        BROGAND,
        RANDOM
    };

    static int jungle[10] = {
        ANTEATER,
        PARROT,
        MAMBA,
        ANT,
        ANT,
        HYENA,
        HYENA,
        ELEPHANT,
        LION,
        RANDOM
    };

    static int river[10] = {
        QUAIL,
        TROUT,
        TROUT,
        MANOWAR,
        BASS,
        MASS,
        CROC,
        CROC,
        BRIGAND,
        RANDOM
    };

    static int swamp[10] = {
        BASS,
        BASS,
        CROC,
        CROC,
        BOGTHING,
        ANT,
        ANT,
        RANDOM,
        RANDOM,
        RANDOM
    };

    static int desert[10] = {
        HAWK,
        HAWK,
        CAMEL,
        CAMEL,
        HYENA,
        HYENA,
        LION,
        LION,
        RANDOM,
        RANDOM
    };

    static int tundra[10] = {
        WOLF,
        WOLF,
        BEAR,
        BEAR,
        DEER,
        DEER,
        RANDOM,
        RANDOM,
        RANDOM,
        RANDOM
    };

    static int mountain[10] = {
        BUNNY,
        SHEEP,
        WOLF,
        WOLF,
        HAWK,
        HAWK,
        RANDOM,
        RANDOM,
        RANDOM
    };

    int *monsters;
    int i;
    int nummonsters;

    nummonsters = (random_range(5) + 1) * (random_range(3) + 1);

    switch(terrain) {
    case PLAINS:
        monsters = plains;

        break;
    case FOREST:
        monsters = forest;

        break;
    case JUNGLE:
        monsters = jungle;

        break;
    case RIVER:
        monsters = river;

        break;
    case SWAMP:
        monsters = swamp;

        break;
    case MOUNTAINS:
    case PASS:
    case VOLCANO:
        monsters = mountain;

        break;
    case DESERT:
        monsters = desert;

        break;
    case TUNDRA:
        monsters = tundra;

        break;
    default:
        monsters = NULL;
    }

    for(i = 0; i < nummonsters; ++i) {
        tml = (pml)malloc(sizeof(mltype));
        tml->m = (pmt)malloc(sizeof(montype));

        if(monsters == NULL) {
            tml->m = m_create(random_range(WIDTH), random_range(LENGTH), TRUE, difficulty());
        }
        else {
            tml->m = make_creature(*(monsters + random_range(10)));
            tml->m->x = random_range(WIDTH);
            tml->m->y = random_range(LENGTH);
        }

        Level->site[tml->m->x][tml->m->y].creature = tml->m;
        tml->m->sense = WIDTH;

        if(m_statusp(tml->m, ONLYSWIM)) {
            Level->site[tml->m->x][tml->m->y].locchar = WATER;
            Level->site[tml->m->x][tml->m->y].p_locf = L_WATER;
        }

        tml->next = ml;
        ml = tml;
    }

    Level->mlist = ml;
}

/*
 * Monstertype is more or less Current_Dungeon. The caves and sewers get harder
 * as you penetrate them; the castle is completely random, but also gets harder
 * as it is explored; the astral and volcano just stay hard...
 */

void populate_level(int monstertype)
{
    pml head;
    pml tml;
    int i;
    int j;
    int k;
    int monsterid;
    int nummonsters = ((random_range(difficulty() / 3) + 1) * 3) + 8;

    if(monstertype == E_CASTLE) {
        nummonsters += 10;
    }
    else if(monstertype == E_ASTRAL) {
        nummonsters += 10;
    }
    else if(monstertype == E_VOLCANO) {
        nummonsters += 20;
    }

    tml = (pml)mallod(sizeof(mltype));
    head = tml;

    for(k = 0; k < nummonsters; ++k) {
        findspace(&i, &j, -1);

        switch(monstertype) {
        case E_CAVES:
            if(((Level->depth * 10) + random_range(100)) > 150) {
                /* Goblin shaman */
                monsterid = ML3 + 7;
            }
            else if(((Level->depth * 10) + random_range(100)) > 100) {
                /* Goblin cheiftain */
                monsterid = ML2 + 9;
            }
            else if(random_range(100) > 50) {
                /* Goblin */
                monsterid = ML1 + 6;
            }
            else {
                /* I.e. random monster */
                monsterid = -1;
            }

            break;
        case E_SEWERS:
            if(!random_range(3)) {
                monsterid = -1;
            }
            else {
                switch(random_range(Level->depth + 3)) {
                case 0:
                    /* Sewer rat */
                    monsterid = ML1 + 3;

                    break;
                case 1:
                    /* Aggravator fungus */
                    monsterid = ML1 + 4;

                    break;
                case 2:
                    /* Blipper rat */
                    monsterid = ML1 + 5;

                    break;
                case 3:
                    /* Night guant */
                    monsterid = ML2 + 1;

                    break;
                case 4:
                    /* Transparent nasty */
                    monsterid = ML2 + 5;

                    break;
                case 5:
                    /* Murk fungus */
                    monsterid = ML2 + 8;

                    break;
                case 6:
                    /* Catoblepas */
                    monsterid = ML3 + 1;

                    break;
                case 7:
                    /* Acid cloud */
                    monsterid = ML3 + 3;

                    break;
                case 8:
                    /* Denebian slime devil */
                    monsterid = ML4 + 3;

                    break;
                case 9:
                    /* Giant crocodile */
                    monsterid = ML4 + 8;

                    break;
                case 10:
                    /* Tesla monster */
                    monsterid = ML5 + 1;

                    break;
                case 11:
                    /* Shadow spirit */
                    monsterid = ML5 + 7;

                    break;
                case 12:
                    /* Bogthing */
                    monsterid = ML5 + 8;

                    break;
                case 13:
                    /* Water elemental */
                    monsterid = ML6 + 2;

                    break;
                case 14:
                    /* Triton */
                    monsterid = ML6 + 6;

                    break;
                case 15:
                    /* ROUS */
                    monsterid = ML7 + 3;

                    break;
                default:
                    monsterid = -1;

                    break;
                }
            }
            
            break;
        case E_ASTRAL:
            /* Random astral creatures */
            if(random_range(2)) {
                switch(random_range(12)) {
                case 0:
                    /* Thought form */
                    monsterid = ML3 + 14;
                    
                    break;
                case 1:
                    /* Astral fuzzy */
                    monsterid = ML4 + 11;
                    
                    break;
                case 2:
                    /* Ban sidhe */
                    monsterid = ML4 + 15;
                    
                    break;
                case 3:
                    /* Astral grue */
                    monsterid = ML4 + 16;
                    
                    break;
                case 4:
                    /* Shadow spirit */
                    monsterid = ML5 + 7;
                    
                    break;
                case 5:
                    /* Astral vampire */
                    monsterid = ML5 + 9;
                    
                    break;
                case 6:
                    /* Manaburst */
                    monsterid = ML5 + 11;
                    
                    break;
                case 7:
                    /* Rakshasa */
                    mosnterid = ML6 + 9;
                    
                    break;
                case 8:
                    /* Illusory fiend */
                    monsterid = ML7 + 4;
                    
                    break;
                case 9:
                    /* Mirror master */
                    monsterid = ML7 + 9;
                    
                    break;
                case 10:
                    /* Elder etheric grue */
                    monsterid = ML7 + 10;
                    
                    break;
                case 11:
                    /* Shadow slayer */
                    monsterid = ML8 + 8;
                }
            }
            else if(random_range(2) && (Level->depth == 1)) {
                /* Plane of Earth */
                
                /* Earth elemental */
                monsterid = ML6 + 3;
            }
            else if(random_range(2) && (Level->depth == 2)) {
                /* Plane of air */
                
                /* Air elemental */
                monsterid = ML6 + 1;
            }
            else if(random_range(2) && (Level->depth == 3)) {
                /* Plane of water */
                
                /* Water elemental */
                monsterid = ML6 + 2;
            }
            else if(random_range(2) && (Level->depth == 4)) {
                /* Plane of fire */
                
                /* Fire elemental */
                monsterid = ML6 + 0;
            }
            else if(random_range(2) && (Level->depth == 5)) {
                /* Deep astral */
                
                switch(random_range(12)) {
                case 0:
                    /* Night gaunt */
                    monsterid = ML2 + 1;
                    
                    break;
                case 1:
                    /* Servent of law */
                    monsterid = ML4 + 12;
                    
                    break;
                case 2:
                    /* Servant of chaos */
                    monsterid = ML4 + 13;
                    
                    break;
                case 3:
                    /* Lesser frost demon */
                    monsterid = ML5 + 4;
                    
                    break;
                case 4:
                    /* Outer circle demon */
                    monsterid = ML5 + 12;
                    
                    break;
                case 5:
                    /* Demon serpent */
                    monsterid = ML6 + 10;
                    
                    break;
                case 6:
                    /* Angel */
                    monsterid = ML6 + 11;
                    
                    break;
                case 7:
                    /* Inner circle demon */
                    monsterid = ML7 + 14;
                    
                    break;
                case 8:
                    /* From demon lord */
                    monsterid = ML8 + 5;
                    
                    break;
                case 9:
                    /* High angel */
                    monsterid = ML8 + 11;
                    
                    break;
                case 10:
                    /* Prime circle demon */
                    monsterid = ML9 + 7;
                    
                    break;
                case 11:
                    /* Archangel */
                    monsterid = ML9 + 6;
                }
            }
            else {
                monsterid = -1;
            }
            
            break;
        case E_VOLCANO:
            if(random_range(2)) {
                monsterid = random_range(ML10 - ML4) + ML4;
                
                while(Monsters[monsterid].uniqueness != COMMON) {
                    monsterid = random_range(ML10 - ML4) + ML4;
                }
            }
            else {
                /* Evil and fire creatures */
                switch(random_range((Level->depth / 2) + 2)) {
                case 0:
                    monsterid = ML4 + 5;
                    
                    break;
                case 1:
                    monsterid = ML4 + 6;
                    
                    break;
                case 2:
                    monsterid = ML5 + 0;
                    
                    break;
                case 3:
                    monsterid = ML5 + 4;
                    
                    break;
                case 4:
                    monsterid = ML5 + 5;
                    
                    break;
                case 5:
                    monsterid = ML5 + 10;
                    
                    break;
                case 6:
                    monsterid = ML6 + 0;
                    
                    break;
                case 7:
                    monsterid = ML6 + 5;
                    
                    break;
                case 8:
                    monsterid = ML6 + 9;
                    
                    break;
                case 9:
                    monsterid = ML6 + 10;
                    
                    break;
                case 10:
                    monsterid = ML7 + 1;
                    
                    break;
                case 11:
                    monsterid = ML7 + 6;
                    
                    break;
                case 12:
                    monsterid = ML7 + 11;
                    
                    break;
                case 13:
                    monsterid = ML7 + 12;
                    
                    break;
                case 14:
                    monsterid = ML7 + 14;
                    
                    break;
                case 15:
                    monsterid = ML7 + 3;
                    
                    break;
                case 16:
                    monsterid = ML8 + 3;
                    
                    break;
                case 17:
                    monsterid = ML8 + 5;
                    
                    break;
                case 18:
                    monsterid = ML8 + 8;
                    
                    break;
                case 19:
                    monsterid = ML7 + 3;
                    
                    break;
                case 20:
                    monsterid = ML9 + 5;
                    
                    break;
                case 21:
                    monsterid = ML9 + 7;
                    
                    break;
                default:
                    monsterid = -1;
                    
                    break;
                }
            }
            
            break;
        case E_CASTLE:
            if(random_range(4) == 1) {
                if(difficulty() < 5) {
                    monsterid = ML2 + 7;
                }
                else if(difficulty() < 6) {
                    monsterid = ML5 + 6;
                }
                else if(difficulty() < 8) {
                    monsterid = ML6 + 0;
                }
                else {
                    monsterid = ML9 + 4;
                }
            }
            else {
                monsterid = -1;
            }
            
            break;
        default:
            monsterid = -1;
            
            break;
        }

        if(monsterid > -1) {
            Level->site[i][j].creature = make_creature(monsterid);
        }
        else {
            Level->site[i][j].creature = m_create(i, j, TRUE, difficulty());
        }

        Level->site[i][j].creature->x = i;
        Level->site[i][j].creature->y = j;

        if(m_statusp(Level->site[i][j].creature, ONLYSWIM)) {
            Level->site[i][j].locchar = WATER;
            Level->site[i][j].p_locf = L_WATER;
        }

        tml->next = (pml)malloc(sizeof(mltype));
        tml->next->m = Level->site[i][j].creature;
        tml = tml->next;
    }

    if(Level->mlist == NULL) {
        tml->next = NULL;
        Level->mlist = head->next;
    }
}

/* Add a wandering monster possibly */
void wandercheck()
{
    int x;
    int y;
    pml tml;

    if(random_range(MaxDungeonLevels) < difficulty()) {
        findspace(&x, &y, -1);
        tml = (pml)malloc(sizeof(mltype));
        tml->next = Level->mlist;
        Level->site[x][y].creature = m_create(x, y, WANDERING, difficulty());
        tml->m = Level->site[x][y].creature;
        Level->mlist = tml;
    }
}

/* Call make_creature and place created monster on Level->mlist and Level */
void make_site_monster(int i, int j, int mid)
{
    pml ml = (pml)malloc(sizeof(mltype));
    pmt m;

    if(mid > -1) {
        m = make_creature(mid);
        Level->site[i][j].creature = m;
    }
    else {
        m = m_create(i, j, WANDERING, difficulty());
        Level->site[i][j].creature = m;
    }

    m->x = i;
    m->y = j;
    ml->m = m;
    ml->next = Level->mlist;
    Level->mlist = ml;
}

/*
 * Make and return an appropriate monster for the level and depth. Called by
 * populate_level, doesn't actually add to mlist for some reason, eventually to
 * be more intelligent
 */
pmt m_create(int x, int y, int kind, int level)
{
    pmt newmonster;
    int monster_range;
    int mid;

    switch(level) {
    case 0:
        monster_range = ML1;

        break;
    case 1:
        monster_range = ML2;

        break;
    case 2:
        monster_range = ML3;

        break;
    case 3:
        monster_range = ML4;

        break;
    case 4:
        monster_range = ML5;

        break;
    case 5:
        monster_range = ML6;

        break;
    case 6:
        monster_range = ML7;

        break;
    case 7:
        monster_range = ML8;

        break;
    case 8:
        monster_range = ML9;

        break;
    case 9:
        monster_range = ML10;

        break;
    default:
        monster_range = NUMMONSTERS;

        break;
    }

    mid = random_range(monster_range);

    while(Monsters[mid].uniqueness != COMMON) {
        mid = random_range(monster_range);
    }

    newmonster = make_creature(mid);

    /* No duplicates of unique monsters */
    if(kind == WANDERING) {
        m_status_set(newmonster, WANDERING);
    }

    newmonster->x = x;
    newmonster->y = y;

    return newmonster;
}

/*
 * Make creature # mid, totally random if mid == -1. Make creature allocates
 * space for the creature
 */
pmt make_creature(int mid)
{
    pmt newmonster = (pmt)malloc(sizeof(montype));
    pob ob;
    int i;
    int treasures;

    if(mid == -1) {
        mid = random_range(ML9);
    }

    *newmonster = Monsters[mid];

    if((mid == (ML6 + 11)) || (mid == (ML8 + 11)) || (mid == (ML9 + 6))) {
        /* aux1 field of an angel is its deity */
        newmonster->aux1 = random_range(6) + 1;
        newmonster->corpsestr = salloc(Monsters[mid].corpsestr);
        strcpy(Str3, Monsters[mid].monstring);

        switch(newmonster->aux1) {
        case ODIN:
            strcat(Str3, " of Odin");

            break;
        case SET:
            strcat(Str3, " of Set");

            break;
        case HECATE:
            strcat(Str3, " of Athena");

            break;
        case DESTINY:
            strcat(Str3, " of Destiny");

            break;
        case DRUID:
            strcat(Str3, " of the Balance");

            break;
        }

        newmonster->monstring = salloc(Str3);
    }
    else if(mid = (ML0 + 7)) {
        /* Generic 0th level human */
        newmonster->monstring = salloc(mantype());
        strcpy(Str1, "dead ");
        strcat(Str1, newmonster->monstring);
        newmonster->corpsestr = salloc(Str1);
    }
    else if(newmonster->monchar == '!') {
        /* The nymph/satyr and incubus/succubus */
        if(Player.preference == 'f') {
            newmonster->mmonchar = 'n';
            newmonster->monstring = salloc("nymph");
            newmonster->corpsestr = salloc("dead nymph");
        }
        else {
            newmonster->monchar = 's';
            newmonster->monstring = salloc("satyr");
            newmonster->corpsestr = salloc("dead satyr");
        }

        if(newmonster->id == (ML4 + 6)) {
            if(Player.preference == 'f') {
                newmonster->corpsestr = salloc("dead succubus");
            }
            else {
                newmonster->corpsestr = salloc("dead incubus");
            }
        }
    }

    if(mid == NPC) {
        make_log_npc(newmonster);
    }
    else {
        if(newmonster->sleep < random_range(100)) {
            m_status_set(newmonster, AWAKE);
        }

        if(newmonster->startthing > -1) {
            ob = (pob)malloc(sizeof(objtype));
            *ob = Objects[newmonster->startthing];
            m_pickup(newmonster, ob);
        }

        treasures = random_range(newmonster->treasure);

        for(i = 0; i < treasures; ++i) {
            ob = (pob)malloc(sizeof(objtype));

            ob = (pob)(creature(object(newmonster->level)));

            while(ob->uniqueness != 0) {
                ob = (pob)(create_object(newmonster->level));
            }

            m_pickup(newmonster, ob);
        }
    }

    newmonster->click = (Tick + 1) % 50;

    return newmonster;
}

/* Drop treasures randomly onto level */
void stock_level()
{
    int i;
    int j;
    int k;
    int numtreasures = (2 * random_range(difficulty() / 4)) + 4;

    /* Put cash anywhere including walls, put other treasures only on floor */
    for(k = 0; k < (numtreasures + 10); ++k) {
        i = random_range(WIDTH);
        j = random_range(LENGTH);

        while(Level->site[i][j].locchar != FLOOR) {
            i = random_range(WIDTH);
            j = random_range(LENGTH);
        }

        make_site_treasure(i, j, difficulty());
        i = random_range(WIDTH);
        j = random_range(LENGTH);
        Level->site[i][j].things = (pol)malloc(sizeof(oltype));
        Level->site[i][j].things->thing = (pob)malloc(sizeof(objtype));
        make_cash(Level->site[i][j].things->thing, difficulty());
        Level->site[i][j].things->next = NULL;

        /* Caves have more random cash strewn around */
        if(Current_Dungeon == E_CAVES) {
            i = random_range(WIDTH);
            j = random_range(LENGTH);
            Level->site[i][j].things = (pol)malloc(sizeof(oltype));
            Level->site[i][j].things->thing = (pob)malloc(sizeof(objtype));
            make_cash(Level->site[i][j].things->thing, difficulty());
            Level->site[i][j].things->next = NULL;
            i = random_range(WIDTH);
            j = random_range(LENGTH);
            Level->site[i][j].things = (pol)malloc(sizeof(oltype));
            Level->site[i][j].things->thing = (pob)malloc(sizeof(objtype));
            make_cash(Level->site[i][j].things->thing, difficulty());
            Level->site[i][j].things->next = NULL;
        }
    }
}

/* Make a new object (of at most level itemlevel at site i,j on level */
void make_site_treasure(int i, int j, int itemlevel)
{
    pol tmp = (pol)malloc(sizeof(oltype));
    tmp->thing = (pob)create_object(itemlevel);
    tmp->next = Level->site[i][j].things;
    Level->site[i][j].things = tmp;
}

/* Make a specific new object at site i,j on level */
void make_specific_treasure(int i, int j, int itemid)
{
    pol tmp = (pol)malloc(sizeof(oltype));
    tmp->thing = (pob)malloc(sizeof(objtype));
    *(tmp->thing) = Objects[itemid];
    tmp->next = Level->site[i][j].things;
    Level->site[i][j].things = tmp;
}

/*
 * Returns a "level of difficulty" based on current environment and depth in
 * dungeon. Is somewhat arbitrary. Value between 1 and 10. May not actually
 * represent real difficulty, but instead level of items, monsters encountered.
 */
int difficulty()
{
    int depth = 1;

    if(Level != NULL) {
        depth = Level->depth;
    }

    switch(Current_Environment) {
    case E_COUNTRYSIDE:
        
        return 7;
    case E_CITY:

        return 3;
    case E_VILLAGE:

        return 1;
    case E_TACTICAL_MAP:

        return 7;
    case E_SEWERS:

        return ((depth / 6) + 3);
    case E_CASTLE:

        return ((depth / 4) + 4);
    case E_CAVES:

        return ((depth / 3) + 1);
    case E_VOLCANO:

        return ((depth / 4) + 5);
    case E_ASTRAL:

        return 8;
    case E_ARENA:

        return 5;
    case E_HOVEL:

        return 3;
    case E_MANSION:

        return 7;
    case E_HOUSE:

        return 5;
    case E_DLAIR:

        return 9;
    case E_ABYSS:

        return 10;
    case E_STARPEAK:

        return 9;
    case E_CIRCLE:

        return 8;
    case E_MAGIC_ISLE:

        return 8;
    case E_TEMPLE:

        return 8;
    default:

        return 3;
    }
}
