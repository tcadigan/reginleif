/*
 * command.h -- Client commands.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1991-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#include "vars.h"

void cmd_cd(char *args);
void cmd_clear(char *args);
void cmd_convo(char *args);
void cmd_detach(char *args);
void cmd_echo(char *args);
void cmd_help(char *args);     /* Server command */
void cmd_helpc(char *args);
void cmd_helps(char *args);
void cmd_hide(char *args);
void cmd_info(char *args);
void cmd_internal(char *args);
void cmd_post(char *args);
void cmd_quit(char *args);
void cmd_quote(char *args);
void cmd_savef(char *args);
void cmd_read(char *args);     /* Server command */
void cmd_recall(char *args);
void cmd_repeat(char *args);
void cmd_msg(char *args);
void cmd_queue(char *args);
void cmd_sleep(char *args);
void cmd_version(char *args);
void cmd_wait(char *args);
void cmd_watch4(char *args);
Command *binary_search(char *cmd);

int client_command(char *s, int interactive);
void done_rwho(void);
void start_rwho(void);
int can_log(char *s);

#ifdef RWHO
void icomm_rwho(char *s);
#endif

void init_rwho(void);
void process_spoken(char *race, char *gov, int rid, int gid, char *message);
void robo_say(char *message, int proc);
void cancel_post(void);
void add_to_post(char *s);
int handle_pipes_and_redirects(char *str);

#endif // COMMAND_H_
