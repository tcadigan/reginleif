#ifndef OAUX3_H_
#define OAUX3_H_

void default_maneuvers();
int maneuvers();
void destroy_order();
void expandsiteabbrevs(char prefix[80]);
char *countryid(char terrain);
void indoors_random_event();
void minute_status_check();
void tenminute_status_check();
void hourly_check();
void countrysearch();

#endif
