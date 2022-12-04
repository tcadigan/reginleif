#ifndef DATABASE_H_
#define DATABASE_H_

void useobj(char *oldname);
void infername(char *oldname, char *name);
int used(char *oldname);
int know(char *name);
void dumpdatabase();
char *realname(char *oldname);

#endif
