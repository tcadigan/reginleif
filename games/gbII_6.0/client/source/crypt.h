#ifndef CRYPT_H_
#define CRYPT_H_

#include "types.h"

#include <stdio.h>

void cmd_crypt(char *args);
void cmd_uncrypt(char *nick);
void cmd_listcrypt(char *nop);
char *check_crypt(char *message, int type);
void cmd_cr(char *args);
void init_crypt(void);
void save_crypts(FILE *fd);
int do_crypt_recall(char *str);
void crypt_test(void);

#endif // CRYPT_H_
