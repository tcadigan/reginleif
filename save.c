#include "save.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>

#else

#include <strings.h>
#endif

/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

#ifdef ultrix
void sleep();
#endif

#ifdef USG
unsigned sleep();
#endif

#define IREAD 00400
#define IWRITE 00200

/*
 * This save package was brought to you by -JWT- and -RAK-
 * And has been completely rewritten for UNIX by -JEW-
 */

/* Wizard command for restoring character    -RAK- */
int restore_char()
{
    vtype fnam;
    int i;
    int j;
    FILE *f1;
    int error;
    vtype temp;
    double version;
    struct stat buf2;
    char char_tmp;
    char char_tmp_array[3];
    cave_type *c_ptr;

    clear_screen(0, 0);
    prt("Enter Filename:", 0, 0);

    if(!get_string(fnam, 0, 16, 60)) {
	return FALSE;
    }

    no_controlz();

    if(chmod(fnam, (IREAD | IWRITE)) == -1) {
	sprintf(temp, "Can not change file mode for %s", fnam);
	prt(temp, 0, 0);
    }

    f1 = fopen(fnam, "r");

    if(f1 == NULL) {
	sprintf(temp, "Cannot open file %s for reading.", fnam);
	prt(temp, 0, 0);

	return FALSE;
    }

    prt("Restoring Character...", 0, 0);
    put_qio();
    error = 0;
    error |= !fread((char *)&version, sizeof(version), 1, f1);
    error |= !fread((char *)&py, sizeof(py), 1, f1);
    error |= !fread((char *)&char_row, sizeof(char_row), 1, f1);
    error |= !fread((char *)&char_col, sizeof(char_col), 1, f1);
    error |= !fread((char *)&inven_ctr, sizeof(inven_ctr), 1, f1);
    error |= !fread((char *)&inven_weight, sizeof(inven_weight), 1, f1);
    error |= !fread((char *)&equip_ctr, sizeof(equip_ctr), 1, f1);
    error |= !fread((char *)&dun_level, sizeof(dun_level), 1, f1);
    error |= !fread((char *)&missile_ctr, sizeof(missile_ctr), 1, f1);
    error |= !fread((char *)&mon_tot_mult, sizeof(mon_tot_mult), 1, f1);
    error |= !fread((char *)&turn, sizeof(turn), 1, f1);
    error |= !fread((char *)inventory, sizeof(inventory), 1, f1);
    error |= !fread((char *)magic_spell[py.misc.pclass], sizeof(spell_type), 31, f1);
    error |= !fread((char *)cur_height, sizeof(cur_height), 1, f1);
    error |= !fread((char *)cur_width, sizeof(cur_width), 1, f1);
    error |= !fread((char *)max_panel_rows, sizeof(max_panel_rows), 1, f1);
    error |= !fread((char *)max_panel_cols, sizeof(max_panel_cols), 1, f1);

    for(i = 0; i < MAX_HEIGHT, ++i) {
	for(j = 0; j < MAX_WIDTH; ++j) {
	    c_ptr = &cave[i][j];
	    error |= !fread((char *)&c_ptr->cptr, sizeof(c_ptr->cptr), 1, f1);
	    error |= !fread((char *)&c_ptr->tptr, sizeof(c_ptr->tptr), 1, f1);
	    error |= !fread((char *)&char_tmp, sizeof(char_tmp), 1, f1);
	    c_ptr->fval = char_tmp & 0xF;
	    c_ptr->fopen = (char_tmp >> 4) & 0x1;
	    c_ptr->fm = (char_tmp >> 5) & 0x1;
	    c_ptr->pl = (char_tmp >> 6) & 0x1;
	    c_ptr->tl = (char_tmp >> 7) & 0x1;
	}
    }

    error |= !fread((char *)t_list, sizeof(t_list), 1, f1);
    error |= !fread((char *)&tcptr, sizeof(tcptr), 1, f1);
    error |= !fread((char *)object_ident, sizeof(object_ident), 1, f1);
    error |= !fread((char *)m_list, sizeof(m_list), 1, f1);
    error |= !fread((char *)&mfptr, sizeof(mfptr), 1, f1);
    error |= !fread((char *)&muptr, sizeof(muptr), 1, f1);

    if(version == 4.83) {
	/* insult_cur was a byte in 4.83, but is now a short */
	for(i = 0; i < MAX_STORES; ++i) {
	    error |= !fread((char *)&store[i].store_open, sizeof(short), 1, f1);

	    /* This is different */
	    error |= !fread((char *)&char_tmp, sizeof(char), 1, f1);
	    store[i].insult_cur = (short)char_tmp;
	    error |= !fread((char *)&store[i].owner, sizeof(char), 1, f1);
	    error |= !fread((char *)&store[i].store_ctr, sizeof(char), 1, f1);

	    /* 
	     * Quick compatibility hack for a local vax ignore three bytes of
	     * fill character
	     */
	    error |= !fread((char *)char_tmp_array, sizeof(char), 3, f1);
	    error |= !fread((char *)store[i].store_inven, sizeof(inven_record), STORE_INVEN_MAX, f1);
	}
    }
    else {
	error |= !fread((char *)store, sizeof(store), 1, f1);
    }

    error |= !fread((char *)&buf2, sizeof(buf2), 1, f1);
    error |= !fread((char *)norm_state, sizeof(norm_state), 1, f1);
    error |= !fread((char *)randes_state, sizeof(randes_state), 1, f1);
    error |= !fread((char *)&randes_state, sizeof(randes_seed), 1, f1);
    error |= !fread((char *)town_state, sizeof(town_state), 1, f1);
    error |= !fread((char *)&town_seed, sizeof(town_seed), 1, f1);

    if(version >= 4.87) {
	error |= fread((char *)&panic, sizeof(panic_size), 1, f1);

	/* Clear the panic_save condition, which is used to indicate cheating */
	panic_save = 0;
    }

    error |= fclose(f1);
    controlz();

    if(error) {
	sprintf(temp, "Error reading in file %s", fnam);
	prt(temp, 0, 0);

	return FALSE;
    }

    if(unlink(fnam) == -1) {
	sprintf(temp, "Cannot delete file %s", fnam);
	prt(temp, 0, 0);
    }

    /* 
     * Reidentify objects.
     * very inefficient, should write new routine perhaps? 
     */
    for(i = 0; i < MAX_OBJECTS, ++i) {
	if(object_ident[i] == TRUE) {
	    identify(object_list[i]);
	}
    }

    return FALSE;
}

void save_char(int exit, int no_ask)
{
    int i;
    int j;
    int flag;
    int error;
    vtype fnam;
    vtype temp;
    doule version;
    struct stat buf;
    FILE *f1;
    char char_tmp;
    cave_type *c_ptr;

    flag = FALSE;

    if(!no_ask) {
	prt("Enter Filename:", 0, 0);

	if(!get_string(fnam, 0, 16, 60)) {
	    /* Only return if exit TRUE, i.e. this is not a panic save */
	    if(exit) {
		return;
	    }
	    else {
		strcpy(fnam, "MORIACHR.SAV");
	    }
	}
	else if(strlen(fnam) == 0) {
	    /* If get_string succeeded, but returned zero length */
	    strcpy(fnam, "MORIACHR.SAV");
	}
    }
    else {
	strcpy(fnam, "MORIACHR.SAV");
    }

    no_controlz();

    /* Open the user's save file    -JEW- */
    f = fopen(fnam, "w");

    if(f1 == NULL) {
	sprintf(temp, "Error creating %s", fnam);
	msg_print(temp);

	return;
    }

    flag = TRUE;
    clear_screen(0, 0);
    prt("Saving character...", 0, 0);
    put_qio();
    version = CUR_VERSION;
    error = 0;
    error |= !fwrite((char *)&version, sizeof(version), 1, f1);
    error |= !fwrite((char *)&py, sizeof(py), 1, f1);
    error |= !fwrite((char *)&char_row, sizeof(char_row), 1, f1);
    error |= !fwrite((char *)&char_col, sizeof(char_col), 1, f1);
    error |= !fwrite((char *)&inven_ctr, sizeof(inven_ctr), 1, f1);
    error |= !fwrite((char *)&inven_weight, sizeof(inven_weight), 1, f1);
    error |= !fwrite((char *)&equip_ctr, sizeof(equip_ctr), 1, f1);
    error |= !fwrite((char *)&dun_level, sizeof(dun_level), 1, f1);
    error |= !fwrite((char *)&missile_ctr, sizeof(missile_ctr), 1, f1);
    error |= !fwrite((char *)&mon_tot_mult, sizeof(mon_tot_mult), 1, f1);
    error |= !fwrite((char *)&turn, sizeof(turn), 1, f1);
    error |= !fwrite((char *)inventory, sizeof(inventory), 1, f1);
    error |= !fwrite((char *)magic_spell[py.misc.pclass], sizeof(spell_type), 31, f1);
    error |= !fwrite((char *)&cur_height, sizeof(cur_height), 1, f1);
    error |= !fwrite((char *)&cur_width, sizeof(cur_width), 1, f1);
    error |= !fwrite((char *)&max_panel_rows, sizeof(max_panel_rows), 1, f1);
    error |= !fwrite((char *)&max_panel_cols, sizeof(max_panel_cols), 1, f1);

    for(i = 0; i < MAX_HEIGHT; ++i) {
	for(j = 0; j < MAX_WIDTH; ++j) {
	    c_ptr = &cave[i][j];
	    char_tmp = c_ptr->fval | (c_ptr->fopen << 4) | (c_ptr->fm << 5) | (c_ptr->pl << 6) | (c_ptr->tl << 7);
	    error |= !fwrite((char *)&c_ptr->cptr, sizeof(c_ptr->cptr), 1, f1);
	    error |= !fwrite((char *)&c_ptr->tptr, sizeof(c_ptr->tptr), 1, f1);
	    error |= !fwrite((char *)&char_tmp, sizeof(char_tmp), 1, f1);
	}
    }

    error |= !fwrite((char *)t_list, sizeof(t_list), 1, f1);
    error |= !fwrite((char *)&tcptr, sizeof(tcptr), 1, f1);
    error |= !fwrite((char *)object_ident, sizeof(object_iden), 1, f1);
    error |= !fwrite((char *)m_list, sizeof(m_list), 1, f1);
    error |= !fwrite((char *)&mfptr, sizeof(mfptr), 1, f1);
    error |= !fwrite((char *)&muptr, sizeof(muptr), 1, f1);
    error |= !fwrite((char *)store, sizeof(store), 1, f1);

    if(stat(fnam, &buf) == -1) {
	sprintf(temp, "Can not stat file %s", fnam);
	msg_print(temp);

	return;
    }

    error |= !fwrite((char *)&buf, sizeof(buf), 1, f1);
    error |= !fwrite((char *)norm_state, sizeof(norm_state), 1, f1);
    error |= !fwrite((char *)randes_state, sizeof(randes_state), 1, f1);
    error |= !fwrite((char *)&randes_seed, sizeof(randes_seed), 1, f1);
    error |= !fwrite((char *)town_state, sizeof(town_state), 1, f1);
    error |= !fwrite((char *)&town_seed, sizeof(town_seed), 1, f1);

    /* This indicates 'cheating' if it is a one */
    error |= !fwrite((char *)&panic_save, sizeof(panic_save), 1, f1);
    error |= fclose(f1);
    character_saved = 1;

    if(!wizard1) {
	if(chmod(fnam, 0) == -1) {
	    sprintf(temp, "Can not change file mode for %s", fnam);
	    msg_print(temp);

	    return;
	}
    }

    /* Make sure user can't touch save file for 5 seconds */
    sleep(5);
    controlz();

    if(error) {
	sprintf(temp, "Error writing to file %s", fnam);
	prt(tmp, 0, 0);
	prt("Game not saved.", 0, 0);
    }
    else if(flag) {
	sprintf(temp, "Character saved. [Moria Version %lf]", CUR_VERSION);
	prt(temp, 0, 0);

	if(exit) {
	    exit_game();
	}
    }
}

void get_char(char *fnam)
{
    int i;
    int j;
    FILE *f1;
    int error;
    vtype temp;
    double version;

#ifdef USG
    struct stat buf;
    struct stat buf2;

#else

    struct stat lbuf;
    struct stat buf;
    struct stat buf2;
#endif

    char char_tmp;
    char char_tmp_array[3];
    cave_type *c_ptr;
    long age;

    clear_screen(0, 0);
    no_controlz();

#ifdef USG
    /* No symbolic links */
    if(stat(fnam, &buf) == -1) {
	sprintf(temp, "Cannot stat file &s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }
    
#endif

    if((lstat(fnam, &lbuf) == -1) || (state(fnam, &buf) == -1)) {
	sprintf(temp, "Cannot stat file %s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }
#endif

#ifdef USG
    /* No symbolic links */
    
#else
    
    if(lbuf.st_ino != buf.st_ino) {
	sprintf(temp, "Cannot restore from symbolic link %s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }
#endif

    if(buf.st_nlink != 1) {
	sprintf(temp, "Too many links to file %s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }

    if(chmod(fnam, (IREAD | IWRITE)) == -1) {
	sprintf(temp, "Can not change file mode for %s", fnam);
	prt(temp, 0, 0);
    }

    f1 = fopen(fnam, "r");

    if(f1 == NULL) {
	sprintf(temp, "Cannot open file %s for reading", fnam);
	prt(temp, 0, 0);

	exit_game();
    }

    prt("Restoring Character...", 0, 0);
    put_qio();
    error = 0;
    error |= !fread((char *)&version, sizeof(version), 1, f1);
    error |= !fread((char *)&py, sizeof(py), 1, f1);
    error |= !fread((char *)&char_row, sizeof(char_row), 1, f1);
    error |= !fread((char *)&char_col, sizeof(char_col), 1, f1);
    error |= !fread((char *)&inven_ctr, sizeof(inven_ctr), 1, f1);
    error |= !fread((char *)&inven_weight, sizeof(inven_weight), 1, f1);
    error |= !fread((char *)&equip_ctr, sizeof(equip_ctr), 1, f1);
    error |= !fread((char *)&dun_level, sizeof(dun_level), 1, f1);
    error |= !fread((char *)&missile_ctr, sizeof(missile_ctr), 1, f1);
    error |= !fread((char *)&mon_tot_mult, sizeof(mon_tot_mult), 1, f1);
    error |= !fread((char *)&turn, sizeof(turn), 1, f1);
    error |= !fread((char *)inventory, sizeof(inventory), 1, f1);
    error |= !fread((char *)magic_spell[py.misc.pclass], sizeof(spell_type), 31, f1);
    error |= !fread((char *)&cur_height, sizeof(cur_height), 1, f1);
    error |= !fread((char *)&cur_width, sizeof(cur_width), 1, f1);
    error |= !fread((char *)&max_panel_rows, sizeof(max_panel_rows), 1, f1);
    error |= !fread((char *)&max_panel_cols, sizeof(max_panel_cols), 1, f1);

    for(i = 0; i < MAX_HEIGHT; ++i) {
	for(j = 0; j < MAX_WIDTH; ++j) {
	    c_ptr = &cave[i][j];
	    error |= !fread((char *)&c_ptr->cptr, sizeof(c_ptr->cptr), 1, f1);
	    error |= !fread((char *)&c_ptr->tptr, sizeof(c_ptr->tptr), 1, f1);
	    error |= !fread((char *)&char_tmp, sizeof(char_tmp), 1, f1);
	    c_ptr->fval = char_tmp & 0xF;
	    c_ptr->fopen = (char_tmp >> 4) & 0x1;
	    c_ptr->fm = (char_tmp >> 5) & 0x1;
	    c_ptr->pl = (char_tmp >> 6) & 0x1;
	    c_ptr->tl = (char_tmp >> 7) & 0x1;
	}
    }

    error |= !fread((char *)t_list, sizeof(t_list), 1, f1);
    error |= !fread((char *)&tcptr, sizeof(tcptr), 1, f1);
    error |= !fread((char *)object_ident, sizeof(object_ident), 1, f1);
    error |= !fread((char *)m_list, sizeof(m_list), 1, f1);
    error |= !fread((char *)&mfptr, sizeof(mfptr), 1, f1);
    error |= !fread((char *)&muptr, sizeof(muptr), 1, f1);

    if(version == 4.83) {
	/* insult_cur was a byte in 4.83, but is now a short */
	for(i = 0; i < MAX_STORES; ++i) {
	    error |= !fread((char *)&store[i].store_open, sizeof(short), 1, f1);

	    /* This is different */
	    error |= !fread((char *)&char_tmp, sizeof(char), 1, f1);
	    store[i].insult_cur = (short)char_tmp;
	    error |= !fread((char *)&store[i].store_owner, sizeof(char), 1, f1);
	    error |= !fread((char *)&store[i].store_ctr, sizeof(char), 1, f1);

	    /*
	     * Quick compatibility hack for a local vax ignore three bytes of
	     * fill character
	     */
	    error |= !fread((char *)char_tmp_array, sizeof(char), 3, f1);
	    error |= !fread((char *)store[i].store_inven, sizeof(inven_record), STORE_INVEN_MAX, f1);
	}
    }
    else {
	error |= !fread((char *)store, sizeof(store), 1, f1);
    }

    error |= !fread((char *)&buf2, sizeof(buf2), 1, f1);
    error |= !fread((char *)norm_state, sizeof(norm_state), 1, f1);
    error |= !fread((char *)randes_state, sizeof(randes_state), 1, f1);
    error |= !fread((char *)&randes_seed, sizeof(randes_seed), 1, f1);
    error |= !fread((char *)town_state, sizeof(town_state), 1, f1);
    error |= !fread((char *)&town_seed, sizeof(town_seed), 1, f1);

    if(version >= 4.87) {
	error |= !fread((char *)&panic_save, sizeof(panic_save), 1, f1);
    }

    error |= fclose(f1);
    controlz();

    if(buf.st_atime >= (buf2.st_atime + 5)) {
	sprintf(temp, "File %s has been touched, sorry.", fnam);
	prt(temp, 0, 0);

	exit_game();
    }

    if(error) {
	sprintf(temp, "Error reading in file %s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }

    /*
     * Rotate store inventory, depending on how old the save file is foreach day
     * or fraction thereof old, call store_maint once must do this before delete
     * file
     */
    if(stat(fnam, &buf2) == -1) {
	sprintf(temp, "Cannot stat file %s?", fnam);
	prt(temp, 0, 0);
    }
    else {
	/* Age in seconds */
	age = (long)buf2.st_atime - (long)buf.st_atime;

	/* Age in days */
	age = (age / 86400) + 1;

	for(i = 0; i < age; ++i) {
	    store_maint();
	}
    }

    if(unlink(fnam) == -1) {
	sprintf(temp, "Cannot delete file %s", fnam);
	prt(temp, 0, 0);

	exit_game();
    }

    if(panic_save == 1) {
	sprintf(temp, "This game is from a panic save. Score will not be added to scoreboard.");
	msg_print(temp);

	/* Make sure player will see message before change_name is called */
	msg_print(" ");
    }

    /* 
     * Reidentify objects.
     * Very inefficient, should write new routine perhaps?
     */
    for(i = 0; i < MAX_OBJECTS; ++i) {
	if(object_ident[i] == TRUE) {
	    identify(object_list[i]);
	}
    }

    /*
     * In case restoring a dead character, this can happen if a signal is caught
     * after a character's hit points go below zero, but before the game ends
     */
    if(py.misc.chp <= -1) {
	prt("Your character has already died.", 23, 0);
	strcpy(died_from, "Unknown.");
	death = 1;
    }

    return FALSE;
}
