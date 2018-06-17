#ifndef MAP_H_
#define MAP_H_

void plot_surface(char *t);
void print_X(int Maxx);
void plot_orbit(char *t);

#ifdef ARRAY
void DispArray(int x, int y, int maxx, int maxy, char *array[], float mag);
#endif // ARRAY

#endif // MAP_H_
