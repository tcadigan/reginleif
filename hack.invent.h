#ifndef HACK_INVENT_H_
#define HACK_INVENT_H_

#include "def.obj.h" /* obj */

struct obj *addinv();
void freeinv(struct obj *obj);
int askchain(struct obj *objchain, 
             char *olets,
             int allflag, 
             int (*fn)(struct obj *obj), 
             int (*ckfn)(struct obj *otmp), 
             int max);

#endif 
