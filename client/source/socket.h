#ifndef SOCKET_H_
#define SOCKET_H_

int connectgb(char *gbhost, char *charport, char *s);
int is_connected(void);
void get_socket(void);
void process_gb(char *s);
void process_socket(char *s);
void socket_final_process(char *s, int type);
void oldcheck4_endprompt(char *s);
void connect_prompts(char *s);
void init_endprompt_connect(void);
void send_gb(char *s, int len);
int sendgb(char *buf, int len);
void cursor_output_window(void);
void scroll_output_window(void);
void cmd_connect(char *s);
void set_values_on_end_prompt(void);
void set_no_logout(void);
void check_no_logout(void);
char *build_scope(void);
void check_for_time_codes(char *s);
void check_for_special_formatting(char *s, int type);
void close_gb(void);
int read_socket(void);
int have_socket_output(void);
void loggedin(void);
void cmd_ping(char *s);
void null_func(void);
int on_endprompt(int eprompt);
void get_pass_info(void);
void chap_response(char *line);
void chap_abort(void);

#endif // SOCKET_H_
