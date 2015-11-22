#ifndef STORE1_H_
#define STORE1_H_

#include "types.h"

void store_maint();
void store_init();
void store_destroy(int store_num, int item_val, int one_of);
void store_carry(int store_num, int *ipos);
int  store_check_num(int store_num);
void insert_store(int store_num, int pos, int icost);
int item_value(treasure_type item);
int sell_price(int snum, int *max_sell, int *min_sell, treasure_type item);

#endif
