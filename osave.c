/*
 * Omega copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * osave.c
 *
 * Various functions for doing game saves and restores.  The game
 * remembers various player information, the city level, the country
 * level, and the last or current dungeon level
 */
#include "osave.h"

#include "oglob.h"

/*
 * Checks to see if save file already exists. Checks to see if save
 * file can be opened for write. The player, the city level, and the
 * current dungeon level are saved.
 */
int save_game(int compress, char *savestr)
{
    int i;
    int writeok = TRUE;
    char cmd[80];
    FILE *fd = fopen(savestr, "r");

    if(fd != NULL) {
        mprint("Overwrite old file?");

        if(ynq() == 'y') {
            writeok = 1;
        }
        else {
            writeok = 0;
        }

        fclose(fd);
    }

    if(writeok) {
        fd = fopen(savestr, "w");

        if(fd == NULL) {
            writeok = FALSE;
            mprint("Error opening file.");
        }
    }

    if(!writeok) {
        print2("Save aborted.");
    }
    else {
        print1("Saving game...");

        /* Write the version number */
        i = VERSION;
        fwrite((char *)&i, sizeof(int), 1, fd);

        /* Write game id to save file */
        save_player(fd);
        save_country(fd);
        save_level(fd, City);

        if((Current_Environment != E_CITY)
           && (Current_Environment != E_COUNTRYSIDE)) {
            i = TRUE;
            fwrite((char *)&i, sizeof(int), 1, fd);
            save_level(fd, Level);
        }
        else {
            i = FALSE;
            fwrite((char *)&i, sizeof(int), 1, fd);
        }

        fclose(fd);
        print1("Game saved.");

#ifdef COMPRESS_SAVE_FILES
        if(compress) {
            print2("Compressing save file...");
            strcpy(smd, "compress ");
            strcat(cmd, savestr);
            system(cmd);

            if(strlen(savestr) < 13) {
                strcpy(cmd, "mv ");
                strcat(cmd, savestr);
                strcat(cmd, ".Z ");
                strcat(cmd, savestr);
                system(cmd);
            }
        }
#endif

        morewait();
        clearmsg();
    }

    return writeok;
}

/*
 * Saves on SIGHUP.
 * no longer tries to compress, which hangs
 */
void signalsave(int signum)
{
    int i;
    FILE *fd = fopen("Omega.sav", "w");

    if(fd != NULL) {
        /* Write the version number */
        i = VERSION;
        fwrite((char *)&i, sizeof(int), 1, fd);

        /* Write game id to save file */
        save_player(fd);
        save_country(fd);
        save_level(fd, City);

        if((Current_Environment != E_CITY)
           && (Current_Environment != E_COUNTRYSIDE)) {
            i = TRUE;
            fwrite((char *)&i, sizeof(int), 1, fd);
            save_level(fd, level);
        }
        else {
            i = FALSE;
            fwrite((char *)&i, sizeof(int), 1, fd);
        }

        fclose(fd);
    }

    exit(0);
}

/* Also saves some globals like Level->depth... */
void save_player(FILE *fd)
{
    int i;
    int twohand;

    if((Player.possessions[O_READY_HAND] == Player.possessions[O_WEAPON_HAND])
       && (Player.possessions[O_READY_HAND] != NULL)) {
        twohand = 1;
    }
    else {
        twohand = 0;
    }

    /* Save random global state information */
    fwrite((char *)&Player, sizeof(Player), 1, fd);
    fprintf(fd, "%s\n", Password);
    fprintf(fd, "%s\n", Player.name);
    fwrite((char *)CitySiteList, sizeof(CitySiteList), 1, fd);
    fwrite((char *)&GameStatus, sizeof(long), 1, fd);
    fwrite((char *)&Current_Environment, sizeof(int), 1, fd);
    fwrite((char *)&Last_Environment, sizeof(int), 1, fd);
    fwrite((char *)&Current_Dungeon, sizeof(int), 1, fd);
    fwrite((char *)&Villagenum, sizeof(int), 1, fd);
    fwrite((char *)&Verbosity, sizeof(char), 1, fd);
    fwrite((char *)&Time, sizeof(int), 1, fd);
    fwrite((char *)&Tick, sizeof(int), 1, fd);
    fwrite((char *)&Searchnum, sizeof(int), 1, fd);
    fwrite((char *)&Phase, sizeof(int), 1, fd);
    fwrite((char *)&Date, sizeof(int), 1, fd);
    fwrite((char *)&Spellsleft, sizeof(int), 1, fd);
    fwrite((char *)&SymbolUseHour, sizeof(int), 1, fd);
    fwrite((char *)&ViewHour, sizeof(int), 1, fd);
    fwrite((char *)&Helmhour, sizeof(int), 1, fd);
    fwrite((char *)&Constriction, sizeof(int), 1, fd);
    fwrite((char *)&Blessing, sizeof(int), 1, fd);
    fwrite((char *)&LastDay, sizeof(int), 1, fd);
    fwrite((char *)&RitualHour, sizeof(int), 1, fd);
    fwrite((char *)&Lawstone, sizeof(int), 1, fd);
    fwrite((char *)&chaostone, sizeof(int), 1, fd);
    fwrite((char *)&Mindstone, sizeof(int), 1, fd);
    fwrite((char *)&Arena_Opponent, sizeof(int), 1, fd);
    fwrite((char *)&Imprisonment, sizeof(int), 1, fd);
    fwrite((char *)&Gymcredit, sizeof(int), 1, fd);
    fwrite((char *)&Balance, sizeof(int), 1, fd);
    fwrite((char *)&StarGemUse, sizeof(int), 1, fd);
    fwrite((char *)&HiMagicUse, sizeof(int), 1, fd);
    fwrite((char *)&HiMagic, sizeof(int), 1, fd);
    fwrite((char *)&FixedPoints, sizeof(int), 1, fd);
    fwrite((char *)&LastCountryLocX, sizeof(int), 1, fd);
    fwrite((char *)&LastCountryLocY, sizeof(int), 1, fd);
    fwrite((char *)&LastTownLocX, sizeof(int), 1, fd);
    fwrite((char *)&LastTownLocY, sizeof(int), 1, fd);
    fwrite((char *)&Pawndate, sizeof(int), 1, fd);
    fwrite((char *)&twohand, sizeof(int), 1, fd);
    fwrite((char *)Spells, sizeof(Spells), 1, fd);
    fwrite((char *)&Command_Duration, sizeof(Command_Duration), 1, fd);
    fwrite((char *)&Precipitation, sizeof(Precipitation), 1, fd);
    fwrite((char *)&Lunarity, sizeof(Lunarity), 1, fd);
    fwrite((char *)&ZapHour, sizeof(ZapHour), 1, fd);
    fwrite((char *)&RitualRoom, sizeof(RitualRoom), 1, fd);

    /* High score stuff */
    fwrite((char *)&Shadowlordbehavior, sizeof(Shadowlordbehavior), 1, fd);
    fwrite((char *)&Archmagebehavior, sizeof(Archmagebehavior), 1, fd);
    fwrite((char *)&Primebehavior, sizeof(Primebehavior), 1, fd);
    fwrite((char *)&Justiciarbehavior, sizeof(Justiciarbehavior), 1, fd);
    fwrite((char *)&Commandantbehavior, sizeof(Commandantbehavior), 1, fd);
    fwrite((char *)&Choaslordbehavior, sizeof(Chaoslordbehavior), 1, fd);
    fwrite((char *)&Lawlordbehavior, sizeof(Lawlordbehavior), 1, fd);
    fwrite((char *)&Championbehavior, sizeof(Championbehavior), 1, fd);
    fwrite((char *)Priestbehavior, sizeof(Priestbehavior), 1, fd);
    fwrite((char *)&Hibehavior, sizeof(Hibehavior), 1, fd);
    fwrite((char *)&Dukebehavior, sizeof(Bukebehavior), 1, fd);
    fwrite((char *)Shadowlord, sizeof(Shadowlord), 1, fd);
    fwrite((char *)Archmage, sizeof(Archmage), 1, fd);
    fwrite((char *)Prime, sizeof(Prime), 1, fd);
    fwrite((char *)Commandant, sizeof(Commandant), 1, fd);
    fwrite((char *)Duke, sizeof(Duke), 1, fd);
    fwrite((char *)Champion, sizeof(Champion), 1, fd);
    fwrite((char *)Priest, sizeof(Priest), 1, fd);
    fwrite((char *)Hiscorer, sizeof(Hiscorer), 1, fd);
    fwrite((char *)Hidescrip, sizeof(Hidescrip), 1, fd);
    fwrite((char *)Chaoslord, sizeof(Chaoslord), 1, fd);
    fwrite((char *)Lawlord, sizeof(Lawlord), 1, fd);
    fwrite((char *)Justiciar, sizeof(Justiciar), 1, fd);
    fwrite((char *)&Shadowlordlevel, sizeof(Shadowlordlevel), 1, fd);
    fwrite((char *)&Archmagelevel, sizeof(Archmagelevel), 1, fd);
    fwrite((char *)&Primelevel, sizeof(Primelevel), 1, fd);
    fwrite((char *)&Commandantlevel, sizeof(Commandantlevel), 1, fd);
    fwrite((char *)&Dukelevel, sizeof(Dukelevel), 1, fd);
    fwrite((char *)&Championlevel, sizeof(Championlevel), 1, fd);
    fwrite((char *)Priestlevel, sizeof(Priestlevel), 1, fd);
    fwrite((char *)&Hiscore, sizeof(Hiscore), 1, fd);
    fwrite((char *)&Hilevel, sizeof(Hilevel), 1, fd);
    fwrite((char *)&Justiciarlevel, sizeof(Justiciarlevel), 1, fd);
    fwrite((char *)&Chaoslordlevel, sizeof(Chaoslordlevel), 1, fd);
    fwrite((char *)&Lawlordlevel, sizeof(Lawlordlevel), 1, fd);
    fwrite((char *)&Chaos, sizeof(Chaos), 1, fd);
    fwrite((char *)&Law, sizeof(Law), 1, fd);

    /* Stuff which used to be statics */
    fwrite((char *)&twiddle, sizeof(twiddle), 1, fd);
    fwrite((char *)&saved, sizeof(saved), 1, fd);
    fwrite((char *)&onewithchaos, sizeof(onewithchaos), 1, fd);
    fwrite((char *)&club_hinthour, sizeof(club_hinthour), 1, fd);
    fwrite((char *)&winnings, sizeof(winnings), 1, fd);
    fwrite((char *)&tavern_hinthour, sizeof(tavern_hinthour), 1, fd);
    fwrite((char *)scroll_ids, sizeof(scroll_ids), 1, fd);
    fwrite((char *)potion_ids, sizeof(potion_ids), 1, fd);
    fwrite((char *)stick_ids, sizeof(stick_ids), 1, fd);
    fwrite((char *)ring_ids, sizeof(ring_ids), 1, fd);
    fwrite((char *)cloak_ids, sizeof(cloak_ids), 1, fd);
    fwrite((char *)boot_ids, sizeof(boot_ids), 1, fd);
    fwrite((char *)deepest, sizeof(deepest), 1, fd);

    /* Save player possessions */
    for(i = 0; i < MAXITEMS; ++i) {
        save_item(fd, Player.possessions[i]);
    }

    for(i = 0; i < MAXPACK; ++i) {
        save_item(fd, Player.pack[i]);
    }

    for(i = 0; i < PAWNITEMS; ++i) {
        save_item(fd, Pawnitems[i]);
    }

    /* Save items in condo vault */
    save_itemlist(fd, Condoitems);

    /* Save player item knowledge */
    for(i = 0; i < TOTALITEMS; ++i) {
        fwrite((char *)&Objects[i].known, sizeof(Objects[i].known), 1, fd);
    }
}

/* Save whatever is pointed to by level */
void save_level(FILE *fd, plv level)
{
    int i;
    int j;

    fwrite((char *)level, sizeof(levtype), 1, fd);

    save_monsters(fd, level->mlist);

    for(i = 0; i < MAXWIDTH; ++i) {
        for(j = 0; j < MAXLENGTH; ++j) {
            save_itemlist(fd, level->site[i][j].things);
        }
    }
}

void save_monsters(FILE *fd, pml ml)
{
    pml tml;
    int nummonsters = 0;

    /* First count monsters */
    for(tml = ml; tml != NULL; tml = tml->next) {
        if(tml->m->hp > 0) {
            ++nummonsters;
        }
    }

    fwrite((char *)&nummonsters, sizeof(int), 1, fd);

    /* Now save monsters */
    for(tml = ml; tml != NULL; tml = tml->next) {
        if(tml->m->hp > 0) {
            fwrite((char *)tml->m, sizeof(montype), 1, fd);
            fprintf(fd, "%s\n", tml->m->monstring);
            save_itemlist(fd, tml->m->possessions);
        }
    }
}

/* Save o unless it's null, then save a null item instead */
void save_item(FILE *fd, pob o)
{
    objtype nullobject;
    nullobject.id = -1;

    if(o == NULL) {
        fwrite((char *)&nullobjects, sizeof(objtype), 1, fd);
        fprintf(fd, "Null object. Report if you see this!\n");
    }
    else {
        fwrite((char *)o, sizeof(objtype), 1, fd);
        fprintf(fd, "%s\n", o->truename);
    }
}

void save_itemlist(FILE *fd, pol ol)
{
    int numitems = 0;
    pol tol;

    for(tol = ol; tol != NULL; tol = tol->next) {
        ++numitems;
    }

    fwrite((char *)&numitems, sizeof(int), 1, fd);

    for(tol = ol; tol != NULL; tol = tol->next) {
        save_item(fd, tol->thing);
    }
}

void save_country(FILE *fd)
{
    fwrite((char *)Country, sizeof(Country), 1, fd);
}

/*
 * Read player data, city level, dungeon leve, check validity of save file,
 * etc. Return TRUE if game restored, FALSE otherwise
 */
int restore_game(char *savestr)
{
    int i;
    int version;
    char cmd[80];
    FILE *fd;

#ifdef COMPRESS_SAVE_FILES
    print1("Uncompressing save file...");

    if(strlen(savestr) < 13) {
        strcpy(cmd, "mv ");
        strcat(cmd, savestr);
        strcat(cmd, " ");
        strcat(savestr, ".Z");
        strcat(cmd, savestr);
        system(cmd);
    }

    strcpy(cmd, "uncompress ");
    strcat(cmd, savestr);
    system(cmd);
    print2("Save file uncompressed.");
    morewait();

    if((savestr[strlen(savestr) - 1] == 'Z')
       && (savestr[strlen(savestr) - 2] == '.')) {
        savestr[strlen(savestr)-2] = 0;
    }
#endif

    fd = fopen(savestr, "r");

    if(fd == NULL) {
        print1("Error restoring game -- aborted.");
        print2("File name was: ");
        nprint2(savestr);
        morewait();

        return FALSE;
    }
    else {
        print1("Restoring...");
        fread((char *)&version, sizeof(int), 1, fd);

        if(VERSION != version) {
            print2("Sorry, I can't restore an outdated save file!");
            morewait();

            return FALSE;
        }

        restore_player(fd);
        restore_country(fd);

        /* The city level */
        restore_level(fd);

        fread((char *)&i, sizeof(int), 1, fd);

        if(i == TRUE) {
            restore_level(fd);
        }

        print3("Restoration complete.");
        ScreenOffset = -1000;
        screencheck(Player.y);
        drawvision(Player.x, Player.y);
        setgamestatus(SKIP_MONSTERS);
        dataprint();
        showflags();
        fclose(fd);

        return TRUE;
    }
}

void restore_player(FILE *fd)
{
    int i;
    int twohand;

    fread((char *)&Player, sizeof(Player), 1, fd);
    filescanstring(fd, Password);
    filescanstring(fd, Player.name);
    fread((char *)CitySiteList, sizeof(CitySiteList), 1, fd);
    fread((char *)&GameStatus, sizeof(long), 1, fd);
    fread((char *)&Current_Environment, sizeof(int), 1, fd);
    fread((char *)&Last_Environment, sizeof(int), 1, fd);
    fread((char *)&Current_Dungeon, sizeof(int), 1, fd);
    fread((char *)&Last_Environment, sizeof(int), 1, fd);
    fread((char *)&Current_Dungeon, sizeof(int), 1, fd);
    fread((char *)&Villagenum, sizeof(int), 1, fd);

    switch(Current_Dungeon) {
    case E_ASTRAL:
        MaxDungeonLevels = ASTRALLEVELS;

        break;
    case E_SEWERS:
        MaxDungeonLevels = SEWERLEVELS;

        break;
    case E_CASTLE:
        MaxDungeonLevels = CASTLELEVELS;

        break;
    case E_CAVES:
        MaxDungeonLevels = CAVELEVELS;

        break;
    case E_VOLCANO:
        MaxDungeonLevels = VOLCANOLEVELS;

        break;
    }

    fread((char *)&Verbosity, sizeof(char), 1, fd);
    fread((char *)&Time, sizeof(int), 1, fd);
    fread((char *)&Tick, sizeof(int), 1, fd);
    fread((char *)&Searchnum, sizeof(int), 1, fd);
    fread((char *)&Phase, sizeof(int), 1, fd);
    fread((char *)&Date, sizeof(int), 1, fd);
    fread((char *)&Spellsleft, sizeof(int), 1, fd);
    fread((char *)&SymbolUseHour, sizeof(int), 1, fd);
    fread((char *)&ViewHour, sizeof(int), 1, fd);
    fread((char *)&HelmHour, sizeof(int), 1, fd);
    fread((char *)&Constriction, sizeof(int), 1, fd);
    fread((char *)&Blessing, sizeof(int), 1, fd);
    fread((char *)&LastDay, sizeof(int), 1, fd);
    fread((char *)&RitualHour, sizeof(int), 1, fd);
    fread((char *)&Lawstone, sizeof(int), 1, fd);
    fread((char *)&Chaostone, sizeof(int), 1, fd);
    fread((char *)&Mindstone, sizeof(int), 1, fd);
    fread((char *)&Area_Opponent, sizeof(int), 1, fd);
    fread((char *)&Imprisonment, sizeof(int), 1, fd);
    fread((char *)&Gymcredit, sizeof(int), 1, fd);
    fread((char *)&Balance, sizeof(int), 1, fd);
    fread((char *)&StarGemUse, sizeof(int), 1, fd);
    fread((char *)&HiMagicUse, sizeof(int), 1, fd);
    fread((char *)&HiMagic, sizeof(int), 1, fd);
    fread((char *)&FixedPoints, sizeof(int), 1, fd);
    fread((char *)&LastCountryLocX, sizeof(int), 1, fd);
    fread((char *)&LastCountryLocY, sizeof(int), 1, fd);
    fread((char *)&LastTownLocX, sizeof(int), 1, fd);
    fread((char *)&LastTownLocY, sizeof(int), 1, fd);
    fread((char *)&PawnDate, sizeof(int), 1, fd);
    fread((char *)&twohand, sizeof(int), 1, fd);
    fread((char *)Spells, sizeof(int), 1, fd);
    fread((char *)&Command_Duration, sizeof(Command_Duration), 1, fd);
    fread((char *)&Precipitation, sizeof(Precipitation), 1, fd);
    fread((char *)&Lunarity, sizeof(Lunarity), 1, fd);
    fread((char *)&ZapHour, sizeof(ZapHour), 1, fd);
    fread((char *)&RitualRoom, sizeof(RitualRoom), 1, fd);

    /* High score stuff */
    fread((char *)&Shadowlordbehavior, sizeof(Shadowlordbehavior), 1, fd);
    fread((char *)&Archmagebehavior, sizeof(Archmagebehavior), 1, fd);
    fread((char *)&Primebehavior, sizeof(Primebehavior), 1, fd);
    fread((char *)&Justiciarbehavior, sizeof(Justiciarbehavior), 1, fd);
    fread((char *)&Commandantbehavior, sizeof(Commandantbehavior), 1, fd);
    fread((char *)&Chaoslordbehavior, sizeof(Chaoslordbehavior), 1, fd);
    fread((char *)&Lawlordbehavior, sizeof(Lawlordbehavior), 1, fd);
    fread((char *)&Championbehavior, sizeof(Championbehavior), 1, fd);
    fread((char *)Priestbehavior, sizeof(Priestbehavior), 1, fd);
    fread((char *)&Hibehavior, sizeof(Hibehavior), 1, fd);
    fread((char *)&Dukebehavior, sizeof(Dukebehaivor), 1, fd);
    fread((char *)Shadowlord, sizeof(Shadowlord), 1, fd);
    fread((char *)Archmage, sizeof(Archmage), 1, fd);
    fread((char *)Prime, sizeof(Prime), 1, fd);
    fread((char *)Commandant, sizeof(Commandant), 1, fd);
    fread((char *)Duke, sizeof(Duke), 1, fd);
    fread((char *)Champion, sizeof(Champion), 1, fd);
    fread((char *)Priest, sizeof(Priest), 1, fd);
    fread((char *)Hiscorer, sizeof(Hiscorer), 1, fd);
    fread((char *)Hidescrip, sizeof(Hidescrip), 1, fd);
    fread((char *)Chaoslord, sizeof(Chaoslord), 1, fd);
    fread((char *)Lawlord, sizeof(Lawlord), 1, fd);
    fread((char *)Justiciar, sizeof(Justiciar), 1, fd);
    fread((char *)&Shadorlordlevel, sizeof(Shadowlordlevel), 1, fd);
    fread((char *)&Archmagelevel, sizeof(Archmagelevel), 1, fd);
    fread((char *)&Primelevel, sizeof(Primelevel), 1, fd);
    fread((char *)&Commandantlevel, sizeof(Commandantlevel), 1, fd);
    fread((char *)&Dukelevel, sizeof(Dukelevel), 1, fd);
    fread((char *)&Championlevel, sizeof(Championlevel), 1, fd);
    fread((char *)Priestlevel, sizeof(Priestlevel), 1, fd);
    fread((char *)&Hiscore, sizeof(Hiscore), 1, fd);
    fread((char *)&Hilevel, sizeof(Hilevel), 1, fd);
    fread((char *)&Justiciarlevel, sizeof(Justiciarlevel), 1, fd);
    fread((char *)&Chaoslordlevel, sizeof(Chaoslordlevel), 1, fd);
    fread((char *)&Lawlordlevel, sizeof(Lawlordlevel), 1, fd);
    fread((char *)&Chaos, sizeof(Chaos), 1, fd);
    fread((char *)&Law, sizeof(Law), 1, fd);

    /* Stuff which used to be statics */
    fread((char *)&twiddle, sizeof(twiddle), 1, fd);
    fread((char *)&saved, sizeof(saved), 1, fd);
    fread((char *)&onewithchaos, sizeof(onewithchaos), 1, fd);
    fread((char *)&club_hinthout, sizeof(club_hinthour), 1, fd);
    fread((char *)&winnings, sizeof(winnings), 1, fd);
    fread((char *)&tavern_hinthour, sizeof(tavern_hinthour), 1, fd);
    fread((char *)scroll_ids, sizeof(scroll_ids), 1, fd);
    fread((char *)potion_ids, sizeof(potion_ids), 1, fd);
    fread((char *)stick_ids, sizeof(stick_ids), 1, fd);
    fread((char *)ring_ids, sizeof(ring_ids), 1, fd);
    fread((char *)cloak_ids, sizeof(cloak_ids), 1, fd);
    fread((char *)boot_ids, sizeof(boot_ids), 1, fd);
    fread((char *)deepest, sizeof(deepest), 1, fd);

    /* Set up the string for the ids */
    inititem(0);

    for(i = 0; i < MAXITEMS; ++i) {
        Player.possessions[i] = restore_item(fd);
    }

    if(twohand) {
        free((char *)Player.possessions[O_READY_HAND]);
        Player.possessions[O_READY_HAND] = Player.possessions[O_WEAPON_HAND];
    }

    for(i = 0; i < MAXPACK; ++i) {
        Player.pack[i] = restore_item(fd);
    }

    for(i = 0; i < PAWNITEMS; ++i) {
        Pawnitems[i] = restore_item(fd);
    }

    Condoitems = restore_itemlist(fd);

    for(i = 0; i < TOTALITEMS; ++i) {
        fread((char *)&Objects[i].known, sizeof(Objects[i].known), 1, fd);
    }

    calc_melee();
}

pob restore_item(FILE *fd)
{
    char tempstr[80];
    pob obj = (pob)malloc(sizeof(objtype));

    fread((char *)obj, sizeof(objtype), 1, fd);
    filescanstring(fd, tempstr);
    obj->truename = salloc(tempstr);

    if(obj->id < 0) {
        free((char *)obj);

        return NULL;
    }
    else {
        obj->objstr = Objects[obj->id].objstr;
        obj->cursestr = Objects[obj->id].cursestr;

        return obj;
    }
}

pol restore_itemlist(FILE *fd)
{
    pol ol = NULL;
    pol cur = NULL;
    pol new = NULL;
    int i;
    int numitems;
    int firsttime = TRUE;

    fread((char *)&numitems, sizeof(int), 1, fd);

    for(i = 0; i < numitems; ++i) {
        new = (pol)malloc(sizeof(oltype));
        new->thing = restore_item(fd);
        new->next = NULL;

        if(firsttime == TRUE) {
            cur = new;
            ol = cur;
            firsttime = FALSE;
        }
        else {
            cur->next = new;
            cur = new;
        }
    }

    return ol;
}

void restore_level(FILE *fd)
{
    int i;
    int j;

    Level = (plv)malloc(sizeof(levtype));
    fread((char *)Level, sizeof(levtype), 1, fd);

    Level->next = NULL;

    restore_monsters(fd, Level);

    for(i = 0; i < MAXWIDTH; ++i) {
        for(j = 0; j < MAXLENGTH; ++j) {
            Level->site[i][j].things = restore_itemlist(fd);
        }
    }

    if(Level->environment == E_CITY) {
        City = Level;
    }
    else if(Level->environment == Current_Dungeon) {
        Dungeon = Level;
    }
}

void restore_monsters(FILE *fd, plv level)
{
    pml ml = NULL;
    int i;
    int nummonsters;
    char tmpstr[80];

    level->mlist = NULL;
    fread((char *)&nummonsters, sizeof(int), 1, fd);

    for(i = 0; i < nummonsters, ++i) {
        ml = (pml)malloc(sizeof(mltype));
        ml->m = (pmt)malloc(sizeof(montype));
        ml->next = NULL;
        fread((char *)ml->m, sizeof(montype), 1, fd);
        filescanstring(fd, tempstr);
        ml->m->monstring = salloc(tempstr);
        ml->m->corpsestr = salloc(Monsters[ml->m->id].corpsestr);
        ml->m->meleestr = salloc(Monsters[ml->m->id].meleestr);
        level->site[ml->m->x][ml->m->y].creature = ml->m;
        ml->m->possessions = restore_itemlist(fd);
        ml->next = level->mlist;
        level->mlist = ml;
    }
}

void restore_country(FILE *fd)
{
    fread((char *)Country, sizeof(Country), 1, fd);
}
