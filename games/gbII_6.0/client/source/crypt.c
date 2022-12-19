/*
 * crypt.c : Encryption routines. Got the encoding from some magazine a prof
 *           had. No known author.
 *
 * NOTE: The current encryption scheme is NOT secure. Several cracking programs
 * exist. You have been warned
 *
 * Written by Evan D. Koffler <evank@necom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "crypt.h"

#include "option.h"
#include "str.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern char pbuf[];

CryptRecall *cryptrecall = NULL;
CryptRecall *cryptcur = NULL;
char crypt_values[100];
static Crypt *crypt_list = NULL;

void crypt_update_index(void);
void encode(char *str, char *key);
void add_crypt_recall(char *key);

/*
 * find_crypt: Looks up nick in the crypt list and returns the Crypt structure
 *             if found, NULL otherwise.
 */
Crypt *find_crypt(char *id)
{
    Crypt *p = NULL;

    p = crypt_list;

    while (p && strcmp(id, p->nick)) {
        p = p->next;
    }

    return p;
}

/*
 * cmd_crypt: Adds the nickname and key pair to the crypt list. If the nickname
 *            is already in the list, then the key is changed to the supplied
 *            key.
 */
void cmd_crypt(char *args)
{
    Crypt *p;
    char nick[NORMSIZ];
    char key[NORMSIZ];

    if (!*args) {
        cmd_listcrypt(NULL);

        return;
    }

    /* If a - flag, then remove the crypt from the list */
    if (*args == '-') {
        cmd_uncrypt(args + 1);

        return;
    }

    split(args, nick, key);

    if (!*nick || !*key) {
        msg("-- Usage: crypt <pattern> <key>");

        return;
    }

    p = find_crypt(nick);

    if (!p) {
        p = (Crypt *)malloc(sizeof(Crypt));

        if (!crypt_list) {
            crypt_list = p;
            p->next = NULL;
            p->prev = NULL;
        } else {
            p->next = crypt_list;
            crypt_list->prev = p;
            p->prev = NULL;
            crypt_list = p;
        }

        p->key = NULL;
        p->nick = string(nick);
    } else {
        free(p->key);
    }

    p->key = string(key);
    crypt_update_index();
}

/* Remote the node that contains nick form the crypt list */
void cmd_uncrypt(char *nick)
{
    Crypt *p;
    int val = 0;

    if (*nick == '#') {
        val = atoi(nick + 1);
        p = crypt_list;

        while (p && p->indx != val) {
            p = p->next;
        }
    } else {
        p = find_crypt(nick);
    }

    if (!p) {
        msg("-- No such crypt %s defined.", nick);

        return;
    }

    msg("-- Removed crypt: %s", nick);

    if (!p->prev) {
        if (p->next) {
            crypt_list = p->next;
            crypt_list->prev = NULL;
        } else {
            crypt_list = NULL;
        }
    } else if (!p->next) {
        p->prev->next = NULL;
    } else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    free(p->key);
    free(p->nick);
    free(p);
    crypt_update_index();
}

/* Prints the entire crypt list */
void cmd_listcrypt(char *nop)
{
    Crypt *p;
    int i = 1;

    if (!crypt_list) {
        msg("-- No crypts defined.");

        return;
    }

    msg("-- Crypts:");

    for (p = crypt_list; p; p = p->next) {
        p->indx = i;
        msg("%3d) %s <%s>", i, p->nick, p->key);
        ++i;
    }

    msg("-- end of list");
}

void crypt_update_index(void)
{
    Crypt *p;
    int i = 1;

    if (!crypt_list) {
        return;
    }

    for (p = crypt_list; p; p = p->next) {
        p->indx = i;
        ++i;
    }
}

/*
 * Looks up nick in the crypt_list and returns the encryption key if found in
 * the list. If not found in the crypt_list, null is returned.
 */
char *is_crypted(char *nick)
{
    Crypt *p;

    if (crypt_list) {
        p = find_crypt(nick);

        if (p) {
            return p->key;
        }
    }

    return NULL;
}

/*
 * Takes the socket input and checks to see if it is supposed to be decrypted or
 * whatever.
 */
char *check_crypt(char *message, int type)
{
    char *key;
    char returnfmt[NORMSIZ];
    char buf[NORMSIZ];
    static char buf2[MAXSIZ];
    char pat[SMABUF];
    char *p;
    char *header = "CRYPT";

    if (strncmp(message, header, strlen(header))) {
        debug(4, "NO crypt header. Returning null");

        return NULL;
    }

    p = strchr(message, '|');

    if (!p || !*p) {
        return NULL;
    }

    *p = '\0';
    strcpy(buf, p + 1);
        strcpy(pat, message + strlen(header));

    /* Replace back for strlen in socket.c */
    *p = '|';

    debug(4, "pattern: \'%s\'", pat);
    debug(4, "buffer:  \'%s\'", buf);

    switch (type) {
    case NORM_BROADCAST:
    case GB_BROADCAST:
    case HAP_BROADCAST:
        sprintf(returnfmt, "%s >>", pat);

        break;
    case NORM_ANNOUNCE:
    case GB_ANNOUNCE:
    case HAP_ANNOUNCE:
        sprintf(returnfmt, "%s ::", pat);

        break;
    case GB_THINK:
        sprintf(returnfmt, "%s ==", pat);

        break;
    case HAP_THINK:
        sprintf(returnfmt, "%s ))", pat);

        break;
    case GB_EMOTE:
        sprintf(returnfmt, "%s ))", pat);

        break;
    default:
        *returnfmt = '\0';

        break;
    }

    key = is_crypted(pat);

    if (!key) {
        debug(3, "Crypt: no key for message");


        if (options[ENCRYPT / 8] & (1 << (ENCRYPT % 8))) {
            sprintf(buf2, "%s [encrypted]", returnfmt);

            return buf2;
        } else {
            return NULL;
        }
    }

    debug(4, "Key is: %s", key);
    encode(buf, key);
    sprintf(buf2, "%s %s", returnfmt, buf);

    return buf2;
}

/*
 * Takes the given input and creates the appropriate output for sending to the
 * socket.
 *
 * Formerly build_crypt
 */
void cmd_cr(char *args)
{
    char pat[SMABUF];
    int type = NORM_BROADCAST;
    char *p;
    char *key;
    char buf[MAXSIZ];

    if (*args == '-') {
        switch (*(args + 1)) {
        case 'a':
            type = NORM_ANNOUNCE;

            break;
        case 'b':
            type = NORM_BROADCAST;

            break;
        case 'e':
            type = GB_EMOTE;

            break;
        case 't':
            type = GB_THINK;

            break;
        default:
            type = NORM_BROADCAST;

            break;
        }

        p = rest(args);

        if (p) {
            strcpy(args, p);
        }
    }

    p = first(args);

    if (p) {
        strcpy(pat, p);
    } else {
        msg("-- Usage: cr <pat> <message>");

        return;
    }

    p = rest(args);

    if (p) {
        strcpy(args, p);
    } else {
        msg("-- Usage: cr <pat> <message>");

        return;
    }

    key = is_crypted(pat);

    if (!key) {
        msg("-- Pattern %s does not have a key.", pat);

        return;
    }

    debug(4, "cr buffer before encode: \'%s\'", args);
    encode(args, key);
    debug(4, "cr buffer after encode:  \'%s\'", args);

    sprintf(buf,
            "%s %s%s|%s\n",
            (type == NORM_BROADCAST ? "broadcast"
             : (type == NORM_ANNOUNCE ? "announce"
                : (type == GB_EMOTE ? "emote"
                   : "think"))),
            "CRYPT",
            pat,
            args);

    strcpy(args, buf);
    add_crypt_recall(pat);
}

/*
 * Fill the crypt array with the ascii values for use by the encoding
 * process.
 */
void init_crypt(void)
{
    int i;
    int j = 127;

    for (i = 0; i < 96; ++i) {
        --j;
        crypt_values[i] = j;
    }
}

/*
 * This is a double code. Encoding, and then encoding again creates the orginal
 * string. So it is the encryption and decryption process.
 */
void encode(char *str, char *key)
{
    char *kptr = key;
    int klen;
    int slen;

    if (!str || !*str) {
        return;
    }

    klen = strlen(key) - 1;
    slen = strlen(str) - 1;

    while (*str) {
        *str = crypt_values[(int)((*str - 32) + (int)((*kptr + ((klen + key[klen] + slen) % 95)) % 95)) % 95];
        ++str;
        ++kptr;

        if (!*kptr) {
            kptr = key;
        }
    }
}

/*
 * Saves the current crypts to the file descriptor provided in a format that can
 * be reloaded vai loadf.
 */
void save_crypts(FILE *fd)
{
    Crypt *p;

    if (!crypt_list) {
        return;
    }

    fprintf(fd, "\n#\n# Crypt keys\n#\n");

    for (p = crypt_list; p; p = p->next) {
        fprintf(fd, "crypt %s %s\n", p->nick, p->key);
    }
}

int do_crypt_recall(char *str)
{
    if (cryptrecall) {
        if (!cryptcur) {
            cryptcur = cryptrecall;
        }

        sprintf(str, "cr %s ", cryptcur->key);

        if (cryptcur->next) {
            cryptcur = cryptcur->next;
        } else {
            cryptcur = cryptrecall;
        }

        return true;
    } else {
        return false;
    }
}

void add_crypt_recall(char *key)
{
    CryptRecall *new = cryptrecall;

    cryptcur = NULL;

    /* Try to find the crypt first, if it exists, remove to front */
    while (new && strcmp(key, new->key)) {
        new = new->next;
    }

    /* Key already exists */
    if (new) {
        /* Found 1st node, leave alone */
        if (new == cryptrecall) {
            return;
        }

        if (new->next) {
            new->next->prev = new->prev;
        }

        if (new->prev) {
            new->prev->next = new->next;
        }
    } else {
        /* Does not exist, create */
        new = (CryptRecall *)malloc(sizeof(CryptRecall));

        if (!new) {
            msg("-- cryptrecall: could not malloc memory needed.");

            return;
        }

        new->key = string(key);
    }

    new->next = cryptrecall;

    if (cryptrecall) {
        cryptrecall->prev = new;
    }

    new->prev = NULL;
    cryptrecall = new;
}

void crypt_test(void)
{
    int i;

    for (i = 0; i < 96; ++i) {
        msg("%c = %c", i, crypt_values[i]);
    }
}
