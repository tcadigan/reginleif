#ifndef DESC_H_
#define DESC_H_

#include "types.h"

void objdes(char *out_val, int ptr, int pref);
void identify(treasure_type item);
void known2(char *object_str);
void known1(char *object_str);
void unquote(char *object_str);
void magic_init();

#endif
