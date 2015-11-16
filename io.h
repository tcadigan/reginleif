#ifndef IO_H_
#define IO_H_

void clear_screen(int row, int col);
void pause_exit(int prt_line, int delay);
void inkey(char *ch);
void move_cursor(int row, int col);
void put_buffer(char *out_str, int row, int col);
void prt(char *str_buf, int row, int col);
void msg_print(char *str_buf);
void flush();
void print(char *str_buf, int row, int col);
void exit_game();
int get_string(char *in_str, int row, int column, int slen);
int get_com(char *prompt, char *command);
void pause_line(int prt_line);
void shell_out();
void really_clear_screen();
void repeat_msg();
void erase_line(int row, int col);
void put_qio();

#endif
