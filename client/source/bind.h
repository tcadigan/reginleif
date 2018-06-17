/*
 * bind.h: Contains the data structs for bind.c
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file.
 */

#ifndef BIND_H_
#define BIND_H_

#include <stdio.h>

void cmd_bind(char *args);
void init_binding();
void save_binds(FILE *fd);

#endif // BIND_H_
