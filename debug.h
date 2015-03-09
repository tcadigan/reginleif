#ifndef DEBUG_H_
#define DEBUG_H_

#include "rogue.h"

void print_h_list(struct h_list *item, FILE *output);
void print_h_list_internal(struct h_list *item, int level, int embedded, FILE *output);

void print_coord(coord *item, FILE *output);
void print_coord_internal(coord *item, int level, int embedded, FILE *output);

void print_linked_list(struct linked_list *item, FILE *output);
void print_linked_list_internal(struct linked_list *item, int level, int embedded, FILE *output);

void print_magic_item(struct magic_item *item, FILE *output);
void print_magic_item_internal(struct magic_item *item, int level, int embedded, FILE *output);

void print_room(struct room *item, FILE *output);
void print_room_internal(struct room *item, int level, int embedded, FILE *output);

void print_stats(struct stats *item, FILE *output);
void print_stats_internal(struct stats *item, int level, int embedded, FILE *output);

void print_thing(struct thing *item, FILE *output);
void print_thing_internal(struct thing *item, int level, int embedded, FILE *output);

void print_object(struct object *item, FILE *output);
void print_object_internal(struct object *item, int level, int embedded, FILE *output);

void print_indent(int x, FILE *output);

#endif
