#ifndef KEY_H_
#define KEY_H_

void init_key(void);
void quit_gb(int exitstatus, char *s, char *a1, char *a2);
void signal_int(int sig);
void signal_tstp(int sig);
void signal_usr1(int sig);
void signal_watch(int sig);
void reset_key(void);
void cursor_to_window(void);
void cursor_to_input(void);
void clear_buf(void);
void input_string_into_buf(char *s);
void add_key_buf(char ch);
void erase_space_left(void);
void rease_space_right(void);
void erase_input(int position);
void set_marks(void);
void update_key(int mode);
void set_edit_buffer(char *s);
void func_default(char ch);
void print_key_string(int parse);
void process_key(char *s, int interactive);
void get_key(void);
void promptfor(char *prompt, char *s, int mode);
void update_input_prompt(char *str);
void put_input_prompt(void);
int in_talk_mode(void);
int trans_func_key_to_num(char c);
void backspace(char ch);
void cancel_input(char ch);
void clear_screen(char ch);
void cmd_talk(char *args);
void cmd_talk_off(char ch);
void cursor_backward(char ch);
void cursor_begin(char ch);
void cursor_end(char ch);
void cursor_forward(char ch);
void delete_under_cursor(char ch);
void delete_word_left(char ch);
void delete_word_right(char ch);
void do_edit_mode(char ch);
void do_recallb(char ch);
void do_recallf(char ch);
void esc_default(char ch);
void esc_escape(char ch);
void escape_key(char ch);
void handle_key_buf(char ch);
void input_ch_into_buf(char ch);
void kill_to_end_line(char ch);
void recall_crypts(char ch);
void refresh_input(char ch);
void refresh_screen(char ch);
void stop_things(char ch);
void transpose_chars(char ch);
void quote_key(char ch);
void arrow_default(char ch);

#endif // KEY_H_
