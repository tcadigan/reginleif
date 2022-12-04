#ifndef WIN_HPP_
#define WIN_HPP_

#include <SDL2/SDL.h>

// Versioning info
#define APP_TITLE "PixelCity"
#define APP "pixelcity"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_REVISION 10

// Do we hide scene building behind a loading screen or show it?
#define LOADING_SCREEN 1

// Controls the density of cars
#define CARS 500

// The "dead zone" along the edge of the world, with super-low detail
#define WORLD_EDGE 200

// How often to rebuild the city
#define RESET_INTERVAL (999999) // Milliseconds

// How long the screen fade takes when transitioning to a new city
#define FADE_TIME (1) // Milliseconds

// Debug ground texture that shows traffic lanes
#define SHOW_DEBUG_GROUND 0

// Controls the amount of space available for buildings.
// Other code is written for assuming this will be a power of two
#define WORLD_SIZE 1024
#define WORLD_HALF (WORLD_SIZE / 2)

// Bitflags use to track how world space is being used.
#define CLAIM_ROAD 1
#define CLAIM_WALK 2
#define CLAIM_BUILDING 4
#define MAP_ROAD_NORTH 8
#define MAP_ROAD_SOUTH 16
#define MAP_ROAD_EAST 32
#define MAP_ROAD_WEST 64

// Random SATURATED color
#define RANDOM_COLOR (glRgbaFromHsl((float)RandomVal(255) / 255, 1.0f, 1.0f))

// Used in orienting roads and cars
enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

void WinPopup(char *message, ...);
void WinTerm(void);
bool WinInit(void);
int winWidth(SDL_Window *window);
int winHeight(SDL_Window *window);
void WinMousePosition(int *x, int *y);

extern SDL_Window *window;

#endif /* WIN_HPP_ */
