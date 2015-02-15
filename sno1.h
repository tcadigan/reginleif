#ifndef SNO1_H_
#define SNO1_H_

#include "sno.h"

#include <string.h>

void mes(char *c);
struct node *init(char *s, int t);
struct node *syspit(void);
void syspot(struct node *string);
struct node *sno_strstr(char *s);
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
void delete(struct node *string);
void sysput(struct node *string);
void dump(void);
void dump1(struct node *base);
void writes(char *s);
struct node *sno_getc(void);

/* from string.h */
size_t strlen(const char *str);

#endif /* SNO1_H_ */
