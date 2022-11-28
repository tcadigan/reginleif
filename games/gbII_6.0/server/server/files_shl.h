#ifndef FILES_SHL_H_
#define FILES_SHL_H_

void close_file(int);
void open_data_files(void);
void close_data_files(void);
void openstardata(int *);
void openshdata(int *);
void opencommoddata P_(int *);
void openpdata(int *);
void opensectdata(int *);
void openracedata(int *);
void getsdata(struct stardata *);
void getrace(racetype **, int);
void getstar(racetype **, int);
void getplanet(planettype **, int, int);
int getsector(sectortype **, planettype *, int, int);
int getsmap(sectortype **, planettype *);
int getship(shiptype **, int);
int getcommod(commodtype **, int);
int getdeadship(void);
int getdeadcommod(void);
void putsdata(struct stardata *);
void putrace(racetype *);
void putstar(startype *, int);
void putplanet(planettype *, int, int);
void putsector(sectortype *, planettype *, int, int);
void putsmap(sectortype *, planettype *);
void putship(shiptype *);
void putcommod(commodtype *, int);
int Numraces(void);
int Numships(void);
int Numcommands(void);
int Newslength(int);
void clr_shipfree(void);
void clr_commodfree(void);
void makeshipdead(int);
void makecommoddead(int);
void Putpower(powertype *);
void Getpower(powertype *);
void Putblock(blocktype *);
void Getblock(blocktype *);
void insert_dead_ship(shiptype *);
void clear_dead_ship(void);
int getdeadship_new(int, int);
int getFreeShip(int, int );
int initFreeShipList(void);
int doFreeshipList(shiptype **, int, int);
int shipSlotsAvail(void);
void create_pid_file(void);
void delete_pid_file(void);
int nextship(shiptype *);

#endif /* FILES_SHL_H_ */
