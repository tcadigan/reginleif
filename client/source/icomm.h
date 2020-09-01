#ifndef ICOMM_H_
#define ICOMM_H_

void init_icommand(void);
void init_start_commands(int flag);
int start_command(int val, int flag);
void icomm_command_done(char ch);
int icomm_valid_csp(int num);
int icomm_valid_csp_end(int num);
void icomm_handling_command(char *s);
void icomm_issue_command(char *command, int flag);

#ifdef SMART_CLIENT
void icomm_profile(char *s);
void icomm_relation(char *s);
void icomm_done_relation(void);
int type_relation(char *s);
void icomm_status(char *s);
void icomm_done_status(void);
#endif // SMART_CLIENT

#ifdef OPTTECH
double find_derivative(long int pop, long int inv);
void switch_planets(long int a, long int b);
void cmd_opttech(char *s);
#endif // OPTTECH

#endif // ICOMM_H_
