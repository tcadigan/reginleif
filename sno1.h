#ifndef SNO1_H_
#define SNO1_H_

#include "sno.h"

void mes(char *c);
struct node *init(char *s, int t);
struct node *syspit(void);
int syspot(struct node *string);
struct node *sno_strstr(char *s);
int class(int c);
struct node *alloc(void);
void sno_free(struct node *pointer);
int nfree(void);
struct node *look(struct node *string);
struct node *copy(struct node *string);
int equal(struct node *string1, struct node *string2);
int strbin(struct node *string);
struct node *binstr(int binary);
struct node *add(struct node *string1, struct node *string2);
struct node *sub(struct node *string1, struct node *string2);
struct node *mult(struct node *string1, struct node *string2);
struct node *sno_div(struct node *string1, struct node *string2);
struct node *cat(struct node *string1, struct node *string2);
struct node *dcat(struct node *a, struct node *b);
int delete(struct node *string);
void sysput(struct node *string);
void dump(void);
int dump1(struct node *base);
int writes(char *s);
struct node *sno_getc(void);

/* from unistd.h */
void *sbrk(int increment);

#endif /* SNO1_H_ */
