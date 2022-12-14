#ifndef ORDER_H_
#define ORDER_H_

#include "ships.h"
#include "vars.h"

void order(int, int, int, int, orbitinfo *);
void give_orders(int, int, int, shiptype *, int);
char const *prin_aimed_at(int, int, shiptype *);
char const *prin_ship_dest(int, int, shiptype *);
void mk_expl_aimed_at(int, int, shiptype *);
void DispOrdersHeader(int, int );
void DispOrders(int, int, shiptype *);
int AddOrderToString(int, int, char *);
void route(int, int, int, int, orbitinfo *);
char const *prin_ship_dest_brief(int, int, shiptype *);
void inf_usage(int, int, char *);

#endif /* ORDER_H_ */
