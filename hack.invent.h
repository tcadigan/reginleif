#ifndef HACK_INVENT_H_
#define HACK_INVENT_H_

#include "def.gen.h" /* gen */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void freeobj(struct obj *obj);
struct obj *getobj(char *let, char *word);
int ggetobj(char *word, int (*fn)(struct obj *obj), int max);
void stackobj(struct obj *obj);
void delobj(struct obj *obj);
struct obj *addinv();
void freeinv(struct obj *obj);
void prinv(struct obj *obj);
int askchain(struct obj *objchain, 
             char *olets,
             int allflag, 
             int (*fn)(struct obj *obj), 
             int (*ckfn)(struct obj *otmp), 
             int max);
void useup(struct obj *obj);
void freegold(struct gen *gold);
int carried(struct obj *obj);
void deltrap(struct gen *trap);
struct obj *sobj_at(int n, int x, int y);
struct obj *o_at(int x, int y);
struct monst *m_at(int x, int y);
struct gen *g_at(int x, int y, struct gen *ptr);
int dorwep();
int doprwep();
int doprarm();
int doprring();

#endif 
