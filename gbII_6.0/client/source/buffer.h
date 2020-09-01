#ifndef BUFFER_H_
#define BUFFER_H_

#include "types.h"

void add_buffer(BufferInfo *infoptr, char *str, int partial);
char *remove_buffer(BufferInfo *infoptr);
int have_buffer(BufferInfo *infoptr);

#endif // BUFFER_H_
