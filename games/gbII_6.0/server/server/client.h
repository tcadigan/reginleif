#ifndef CLIENT_H_
#define CLIENT_H_

void CSP_knowledge(int, int);
void CSP_process_command2(int, int);
void CSP_server_qsort(void);
void CSP_client_qsort(void);
void CSP_send_knowledge(int, int);
void CSP_receive_knowledge(int, int);
void CSP_query(int, int);
void CSP_developer(int, int);
void CSP_client_on(int, int);
void CSP_client_off(int, int);
void CSP_client_toggle(int, int, int);
void CSP_client_version(int, int);
int client_can_understand(int, int, int);
void stropargs(int);
void CSP_prompt(int, int);
int CSP_print_planet_number(int, int, int, char *);
int CSP_print_star_number(int, int, int, char *);
int qsort_csp(void const *, void const *);

#endif /* CLIENT_H_ */
