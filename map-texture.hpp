#ifndef MAP_TEXTURE_HPP_
#define MAP_TEXTURE_HPP_

// This defines the highest resolution of the zone textures. Note that becuase
// of the way this thing draws the textures, this cannot be bigger than the
// viewport. So, if you make the window 640x480, then textures can't be 512,
// because the window isn't tall enough to act as a canvas for drawing
// a 512x512!
#define MAX_RESOLUTION 256

// How much time to spedn drawing the next set of textures. Tradeoffs abound.
#define UPDATE_TIME 10

// For debugging: Put the texture size onto the texture
#define SHOW_RESOLUTION 0

// How many times textures should repeat over a zone. This has to be at least
// one. Tune this depending on how tight you want the detail.
#define UV_SCALE 1

unsigned int MapTexture(int zone);
void MapTextureInit(void);
void MapTextureTerm(void);
void MapTextureUpdate(void);

#endif
