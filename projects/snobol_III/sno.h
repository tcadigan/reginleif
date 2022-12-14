#ifndef SNO_H_
#define SNO_H_

struct node {
    struct node *p1;
    struct node *p2;
    char typ;
    char ch;
};

extern int freesize;
extern struct node *lookf;
extern struct node *looks;
extern struct node *lookend;
extern struct node *lookstart;
extern struct node *lookdef;
extern struct node *lookret;
extern struct node *lookfret;
extern int cfail;
extern int rfail;
extern struct node *freelist;
extern struct node *namelist;
extern int lc;
extern struct node *schar;

#endif /* SNO_H_ */
