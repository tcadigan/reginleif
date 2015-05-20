/*
 * map.cpp
 * 2006 Shamus Young
 *
 * This module handles the saving, loading, and creation of terrain DATA. The
 * terrain mesh is generated in terrain.cpp, but this is where the elevation
 * data comes from.
 *
 * This module loads in a bitmap and does some fancy stuff on it to make it
 * look nice. This takes a while, so the first time you run the app, it will
 * take some time to generate this. Once done, it will save the map so it can
 * skip the creation step next time.
 *
 * Really, in a production environment the stuff to CREATE a map would go in a
 * different program altogether.
 */

#include "map.hpp"

#include <SDL.h>
#include <cstring>
#include <cmath>

#include "camera.hpp"
#include "console.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "world.hpp"

struct cell {
    unsigned char layer[LAYER_COUNT - 1];
    bool shadow;
    
    // Move this to low-res sub-map?
    float distance;
    
    GLvector3 position;
    GLvector3 normal;
    GLrgba  light;
};

static struct cell map[MAP_AREA + 1][MAP_AREA + 1];
// static HDC dc;
// static HPEN pen;
static int bits;
static int scan_y;

/*
 * This will take the given elevations and calculate the resulting 
 * surface normal.
 */
static GLvector3 DoNormal(float north, float south, float east, float west)
{
    GLvector3 result;

    result.x = west - east;
    result.y = 2.0f;
    result.z = north - south;

    return glVectorNormalize(result);
}

/*
 * GetPixel is SLOW! It takes a ridiculous amount ot time to sample
 * colours using GetPixel, so we get the raw data and use this function
 * to extract arbitrary bits and used them to fill a full byte.
 */
static unsigned char rgb_sample(short val, int shift, int numbits)
{
    unsigned char r;

    r = val >> shift;
    r &= (int)(pow(2, numbits) - 1);
    r = r << (8 - numbits);

    return (r & 255);
}

void MapSave()
{
    FILE *f;

    f = fopen(MAP_FILE, "wb");
    fwrite(map, sizeof(map), 1, f);
    fclose(f);

    Console("Saved %s (%d bytes)", MAP_FILE, sizeof(map));
}

/*
 * This will generate the terrain data.
 *
 * This code is a mess. It's full of magic numbers that are tuned to work with
 * the current bitmap image, but a different terrain will probably need
 * all sorts of teaks.
 *
 * This one function is really our "level editor", and it should be a separate
 * program with all sorts of options for controlling these magic numbers.
 */
void MapBuild(void)
{
    // HBITMAP basemap;
    // BITMAPINFO bmi;
    GLrgba *cmap;
    cell *c;
    short val;
    int width;
    int x;
    int y;
    int xx;
    int yy;
    int max_x;
    int max_y;
    int left;
    int right;
    int top;
    int bottom;
    int samples;
    float high;
    float low;
    float smooth;
    float e;
    unsigned char r;
    unsigned char g;
    unsigned char *basebits;
    float *scale;

    // Get a couple of temp buffers to use below
    scale = new float[(MAP_AREA + 1) * (MAP_AREA + 1)];
    cmap = new GLrgba[(MAP_AREA + 1) * (MAP_AREA + 1)];
    Console("size = %s", sizeof(cell));

    // Now load the bitmap
    // dc = CreateCompatibleDC(GetDC(NULL));
    // bits = GetDeviceCaps(dc, BITSPIXEL);
    // pen = CreatePens(PS_SOLID, 1, RGB(0, 255, 0));
    
    // basemap = (HBITMAP)LoadImage(AppInstance(),
    //                              MAP_IMAGE,
    //                              IMAGE_BITMAP,
    //                              0,
    //                              0,
    //                              LR_LOADFROMFILE | LR_DEFAULTSIZE | LRVGACOLOR);

    // if(basemap == NULL) {
    //     Console("MapInit: Unable to load %s", MAP_IMAGE);
    
    //     return;
    // }

    Console("MapBuild: rebuilding map data.");

    // Call this to fill in the bmi with good values
    // ZeroMemory(&bmi, sizeof(BITMAPINFO));
    // bmi.bmiheader.biSize = sizeof(BITMAPINFOHEADER);
    // SelectObject(dc, basemap);
    // getDIBits(dc, basemap, 0, 0, NULL, &bmi, DIR_RGB_COLORS);
    // width = bmi.bmiHeader.biWidth;
    width = MAP_AREA + 1;
    // height = bmi.bmiHeader.biHeight;
    // basebits = 
    //     new unsigned char[(bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) * 4];
    basebits =
        new unsigned char[((MAP_AREA + 1) * (MAP_AREA + 1)) * 4];

    // GetBitmapBits(basemap, bmi.bmiHeader.biSizeImage, basebits);
    // max_x = MIN(bmi.bmiHeader.biWidth, MAP_AREA + 1);
    // max_y = MIN(bmi.bmiHeight, MAP_AREA + 1);

    if((MAP_AREA + 1) < (MAP_AREA + 1)) {
        max_x = MAP_AREA + 1;
    }
    else {
        max_x = MAP_AREA + 1;
    }

    if((MAP_AREA + 1) < (MAP_AREA + 1)) {
        max_y = MAP_AREA + 1;
    }
    else {
        max_y = MAP_AREA + 1;
    }

    high = -9999.0f;
    low = 9999.0f;
    
    // Now pass over the image and conver the color values to elevation values.
    for(y = 0; y < (MAP_AREA + 1); ++y) {
        for(x = 0; x < (MAP_AREA + 1); ++x) {
            if((x == MAP_AREA) && (y == MAP_AREA)) {
                x = x;
            }

            if(x < y) {
                xx = y;
            }
            else if(x > (max_x - 2)) {
                xx = max_x - 2;
            }
            else {
                xx = x;
            }

            if(x < y) {
                yy = x;
            }
            else if(x > (max_y - 2)) {
                yy = max_y - 2;
            }
            else {
                yy = x;
            }

            if(bits == 32) {
                g = basebits[((xx + (yy * width)) * 4) + 1];
                r = basebits[((xx + (yy * width)) * 4) + 2];
            }
            else {
                // We are dealing with a 16 bit color value, use first 5 bits
                memcpy(&val, &basebits[(xx + (yy * width)) + 2], 2);
                g = rgb_sample(val, 6, 5);
                r = rgb_sample(val, 11, 5);
            }

            // Now we have the rgb values, scale them however seems best
            e = (float)(r) / 30.0f;

            // (Not using the blue channel right now)
            e += ((float)(g) / 48.0f);

            // Now store the position in our grid of data
            map[x][y].position.x = (float)(x - (MAP_AREA / 2));
            map[x][y].position.y = e;
            map[x][y].position.z = (float)(y - (MAP_AREA / 2));
            map[x][y].shadow = false;

            // Keep track of high/low, which is used to normalize the data later
            if(high <= map[x][y].position.y) {
                high = map[x][y].position.y;
            }

            if(low >= map[x][y].position.y) {
                low = map[x][y].position.y;
            }
        }
    }

    // Convert elevations to scalar values 0.0 - 1.0
    // and copy them to the temp grid
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            scale[x + (y * (MAP_AREA + 1))] =
                (c->position.y - low) / (high - low);
        }
    }

    // Calculate the surface normals
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            top = (unsigned int)(y -1) % (MAP_AREA + 1);
            bottom = (unsigned int)(y + 1) % (MAP_AREA + 1);
            left = (unsigned int)(x - 1) % (MAP_AREA + 1);
            right = (unsigned int)(x + 1) % (MAP_AREA + 1);

            c->normal = 
                DoNormal(scale[x * (top * (MAP_AREA + 1))] * TERRAIN_SCALE,
                         scale[x * (bottom * (MAP_AREA + 1))] * TERRAIN_SCALE,
                         scale[right * (y * (MAP_AREA + 1))] * TERRAIN_SCALE,
                         scale[left * (y * (MAP_AREA + 1))] * TERRAIN_SCALE);
        }
    }

    // Blend the values in the temp grid and convert
    // back to elevation values
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            smooth = 0.0f;
            samples = 0;
            
            for(xx = -BLEND_RANGE; xx <= BLEND_RANGE; ++xx) {
                int tmp_x = x + xx;

                if(tmp_x < 0) {
                    tmp_x = 0;
                }
                else if(tmp_x > MAP_AREA) {
                    tmp_x = MAP_AREA;
                }

                for(yy = -BLEND_RANGE; yy <= BLEND_RANGE; ++yy) {
                    int tmp_y = y + yy;

                    if(tmp_y < 0) {
                        tmp_y = 0;
                    }
                    else if(tmp_y > MAP_AREA) {
                        tmp_y = MAP_AREA;
                    }

                    smooth += scale[tmp_x + (tmp_y * (MAP_AREA + 1))];

                    ++samples;
                }
            }

            c->position.y = (smooth / (float)samples) * TERRAIN_SCALE;
        }
    }

    // Calculate the distances
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            c->distance =
                glVectorLength(glVectorSubtract(c->position, CameraPosition()));

            c->distance /= FAR_VIEW;

            if(c->distance < 0.0f) {
                c->distance = 0.0f;
            }
            else if(c->distance > 1.0f) {
                c->distance = 1.0f;
            }
        }
    }

    // Calculate the layers. this will look at how high and steep each point is,
    // and then determine how much rock, sand, low gras, and dirt each point
    // has. Note, that we do this using the scalar data, which HAS NOT BEEN
    // SMOOTHED. This means all those coarse bumps on the terrain are still
    // available, and they give lots of detail and variation to this surface.
    // This data is used to generate the zone textures. Go nuts here. The more
    // complex the surface is, the more interesting it is to look at, and it
    // doesn't take any more time to render. If we did this using the smoothed
    // out data, it would be mostly grass everywhere, which would be boring.
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            c->layer[LAYER_LOWGRASS - 1] = 0;
            c->layer[LAYER_DIRT - 1] = 0;
            c->layer[LAYER_SAND - 1] = 0;
            c->layer[LAYER_ROCK - 1] = 0;

            // Sand is in the lowest parts of the map
            smooth = 
                MathSmoothStep(scale[x + (y * (MAP_AREA + 1))], 0.3f, 0.1f);

            c->layer[LAYER_SAND - 1] = (int)(smooth * 255.0f);

            // The deep lush grass likes lowlands and flat areas
            e = MathSmoothStep(c->normal.y, 0.75f, 0.1f);
            smooth = 
                MathSmoothStep(scale[x + (y * (MAP_AREA + 1))], 0.45f, 0.25f);

            smooth = (e * smooth) * 5.0f;

            if(smooth < 0) {
                smooth = 0;
            }
            else if(smooth > 1) {
                smooth = 1;
            }

            c->layer[LAYER_LOWGRASS - 1] = (int)(smooth * 255.0f);

            // Rock likes mild slopes and high elevations
            e = MathSmoothStep(c->normal.y, 0.8f, 0.5f);
            e += MathSmoothStep(scale[x + (y * (MAP_AREA + 1))], 0.7f, 1.0f);

            if(e < 0) {
                smooth = 0;
            }
            else if(e > 1) {
                smooth = 1;
            }
            else {
                smooth = e;
            }

            c->layer[LAYER_ROCK - 1] = (int)(smooth * 255.0f);

            // Dirt likes very steep slopes
            e = MathSmoothStep(c->normal.y, 0.7f, 0.4f);
            c->layer[LAYER_DIRT - 1] = (int)(e * 255.0f);
        }
    }

    // Now we recalculate the surface normals. The values we calculated
    // before were based on the un-smoothed terrain data, which we needed in
    // the previous set. Now update the normals with the (more correct)
    // smoothed data.
    for(x = 0; x < (MAP_AREA + 1); ++x) {
        for(y = 0; y < (MAP_AREA + 1); ++y) {
            c = &map[x][y];
            top = (unsigned int)(y - 1) % (MAP_AREA + 1);
            bottom = (unsigned int)(y + 1) % (MAP_AREA + 1);
            left = (unsigned int)(x - 1) % (MAP_AREA + 1);
            right = (unsigned int)(x + 1) % (MAP_AREA + 1);

            c->normal = DoNormal(map[x][top].position.y,
                                 map[x][bottom].position.y,
                                 map[right][y].position.y,
                                 map[left][y].position.y);
        }
    }

    // All done. Let's clean up and store this thing.
    delete[] cmap;
    delete[] scale;
    delete[] basebits;
    // DeleteObject(basemap);

    MapSave();
}

bool MapLoad()
{
    FILE *f;
    int r;
    
    f = fopen(MAP_FILE, "rb");
    
    if(f == NULL) {
        Console("MapLoad: Unable to load %s", MAP_FILE);

        return false;
    }

    r = fread(map, sizeof(map), 1, f);

    if(r < 1) {
        Console("MapLoad: Error loading %s", MAP_FILE);

        return false;
    }

    fclose(f);

    return true;
}

void MapInit()
{
    if(!MapLoad() || FORCE_REBUILD) {
        MapBuild();
    }
}

float MapElevation(int x, int y)
{
    if(x < y) {
        x = y;
    }
    else if(x > MAP_AREA) {
        x = MAP_AREA;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > MAP_AREA) {
        y = MAP_AREA;
    }

    return map[x][y].position.y;
}

// Get the elevation of an arbitrary point over the terrain. This will
// interpolate between points so that we can have collision with the surface.
float MapElevation(float x, float y)
{
    int cell_x;
    int cell_y;
    float a;
    float b;
    float c;
    float y0;
    float y1;
    float y2;
    float y3;
    float dx;
    float dy;

    cell_x = (int)x;
    cell_y = (int)y;
    dx = x - (float)cell_x;
    dy = y - (float)cell_y;
    cell_x += (MAP_AREA / 2);
    cell_y += (MAP_AREA / 2);
    y0 = MapElevation(cell_x, cell_y);
    y1 = MapElevation(cell_x + 1, cell_y);
    y2 = MapElevation(cell_x, cell_y + 1);
    y3 = MapElevation(cell_x + 1, cell_y + 1);

    if(dx < dy) {
        c = y2 - y0;
        b = y3 - y2;
        a = y0;
    }
    else {
        c = y3 - y1;
        b = y1 - y0;
        a = y0;
    }

    return ((a + (b * dx)) + (c * dy));
}

int MapSize()
{
    return MAP_AREA;
}

GLvector3 MapPosition(int x, int y)
{
    if(x < 0) {
        x = 0;
    }
    else if(x > MAP_AREA) {
        x = MAP_AREA;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > MAP_AREA) {
        y = MAP_AREA;
    }

    return map[x][y].position;
}

// This is a little goofy. There are several different texture layers on the
// terrain, but the lowest one (grass) is always opaque. So, we leave that
// entry out of the array. This means we have to subtract 1 from the given
// index. Confusing, but this grid is big and there is no sense in storing
// half a million redundant values.
float MapLayer(int x, int y, int layer)
{
    // The base later is always opaque
    if(layer == LAYER_GRASS) {
        return 1.0f;
    }

    layer -= 1;

    if(x < 0) {
        x = 0;
    }
    else if(x > MAP_AREA) {
        x = MAP_AREA;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > MAP_AREA) {
        y = MAP_AREA;
    }

    return ((float)map[x][y].layer[layer] / 255.0f);
}

// How far is the given point from the camera? These values are updated during
// TerrainUpdate() and are rarely 100% accurate. These are used when calculating
// detail on the terrain.
float MapDistance(int x, int y)
{
    if(x < 0) {
        x = 0;
    }
    else if(x > MAP_AREA) {
        x = MAP_AREA;
    }


    if(y < 0) {
        y = 0;
    }
    else if(y > MAP_AREA) {
        y = MAP_AREA;
    }

    return map[x][y].distance;
}

// The lighting color of the given point.
GLrgba MapLight(int x, int y)
{
    if(x < 0) {
        x = 0;
    }
    else if(x > MAP_AREA) {
        x = MAP_AREA;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > MAP_AREA) {
        y = MAP_AREA;
    }

    return map[x][y].light;
}

void MapTerm(void)
{
}

void MapUpdate(void)
{
    int x;
    int samples;
    int start;
    int end;
    int step;
    GLvector3 light;
    GLrgba ambient;
    GLrgba sun;
    GLrgba shadow;
    float dot;
    float top;
    float drop;
    float shade;
    cell *c;
    unsigned int update_end;

    light = WorldLightVector();
    sun = WorldLightColor();
    ambient = WorldAmbientColor();
    shadow = glRgbaMultiply(ambient, glRgba(0.3f, 0.5f, 0.9f));

    if(light.x > 0.0f) {
        start = MAP_AREA;
        end = -1;
        step = -1;
    }
    else {
        start = 0;
        end = MAP_AREA + 1;
        step = 1;
    }

    if(light.x == 0.0f) {
        drop = 9999999.0f;
    }
    else {
        drop = light.y / light.x;
    }

    if(drop < 0) {
        drop *= -1;
    }

    update_end = SDL_GetTicks() + MAP_UPDATE_TIME;

    while(SDL_GetTicks() < update_end) {
        // Pass over the map (either east to west of vice versa) and
        // see which points are being hit with sunlight.
        for(x = start; x != end; x += step) {
            c = &map[x][scan_y];
            c->distance = 
                glVectorLength(glVectorSubtract(c->position, CameraPosition()));

            c->distance /= FAR_VIEW;

            if(c->distance < 0.0f) {
                c->distance = 0.0f;
            }
            else if(c->distance > 1.0f) {
                c->distance = 1.0f;
            }
            
            if(x == start) {
                // First point is always in sunlight
                top = c->position.y;
                c->shadow = false;
            }
            else {
                top -= drop;

                if(c->position.y > top) {
                    // Is this point high enough to be out of the shadow?
                    c->shadow = false;
                    top = c->position.y;
                }
                else {
                    // Nope!
                    c->shadow = true;
                }
            }

            dot = glVectorDotProduct(light, c->normal);

            if(dot < 0.0f) {
                dot = 0.0f;
            }
            else if(dot > 1.0f) {
                dot = 1.0f;
            }

            samples = 0;
            shade = 0.0f;

            // Blend this shadow with adjoining ones to soften the edges
            // of shadows. Totally not needed, and it slows this down a bit.
            // You only need this if the terrain is going to be viewed in
            // close a lot
            for(int xx = -1; xx <= 1; ++xx) {
                int tmp_x = x + xx;

                if(tmp_x < 0) {
                    tmp_x = 0;
                }
                else if(tmp_x > (MAP_AREA + 1)) {
                    tmp_x = MAP_AREA + 1;
                }

                for(int yy = -1; yy <= 1; ++yy) {
                    int tmp_y = scan_y + yy;

                    if(tmp_y < 0) {
                        tmp_y = 0;
                    }
                    else if(tmp_y > (MAP_AREA + 1)) {
                        tmp_y = MAP_AREA + 1;
                    }

                    if(map[tmp_x][tmp_y].shadow != 0) {
                        shade += 1.0f;
                    }

                    ++samples;
                }
            }

            // Finally! We know how much light is hitting this point and
            // if it is in shadow, now figure out what colour this
            // point is
            c->light = 
                glRgbaInterpolate(glRgbaAdd(ambient, glRgbaScale(sun, dot)),
                                  glRgbaAdd(shadow, glRgbaScale(ambient, dot)),
                                  shade / (float)samples);
        }

        scan_y = (scan_y + 1) % (MAP_AREA + 1);
    }
}
