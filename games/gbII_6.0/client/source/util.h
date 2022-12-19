#ifndef UTIL_H_
#define UTIL_H_

#include "types.h"

int match_gag(char *pat);
void cmd_gag(char *args);
void cmd_ungag(char *name);
void cmd_listgag(char *args);
void gag_update_index(void);
void save_gags(FILE *fd);
void cmd_cleargag(char *args);
Gag *find_gag(char *name);
void add_history(char *line);
void free_history(void);
int recallf(char *line);
int recallb(char *line);
void recall(int n, int type);
void recall_n_m(int n, int m, int type);
void add_recall(char *line, int type);
void free_recall(void);
void recall_match(char *args, int type);
void history_sub(char *args);
Macro *find_macro(char *name);
void cmd_def(char *args);
void cmd_undef(char *name);
void remove_macro(char *name);
int do_macro(char *str);
void cmd_listdef(char *args);
void def_update_index(void);
void save_defs(FILE *fd);
void cmd_cleardef(char *nop);
void cmd_game(char *args);
void add_game(char *nick, char *host, char *port, char *type, char *racename, char *pripasswd, char *govname, char *secpasswd);
void free_game(void);
void cmd_ungame(char *args);
Game *find_game(char *nick);
void cmd_listgame(char *nop);
void game_update_index(void);
void save_games(FILE *fd);
void send_password(void);
void add_queue(char *args, int wait);
void remove_queue(char *args);
void process_queue(char *s);
int check_queue(void);
int have_queue(void);
int do_clear_queue(void);
void clear_queue(void);
void check_news(char *s);
int add_news(char *s);
RNode *find_news(char *date, char *line);
void print_news(void);

#endif // UTIL_H_
