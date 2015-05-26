/*
 * terrain-map.cpp
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

#include "terrain-map.hpp"

#include <SDL.h>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <string>

#include "camera.hpp"
#include "cell.hpp"
#include "console.hpp"
#include "enums.hpp"
#include "ini-manager.hpp"
#include "math.hpp"
#include "world.hpp"

terrain_map::terrain_map(world const &world_object)
    : world_(world_object)
{
}

terrain_map::~terrain_map()
{
}

void terrain_map::init(camera const &camera,
                       ini_manager const &ini_mgr)
{
    camera_ = &camera;
    ini_mgr_ = &ini_mgr;

    zone_grid_ = ini_mgr_->get_int("Map Settings", "zone_grid");
    map_area_ = ini_mgr_->get_int("Map Settings", "map_area");
    map_image_ = ini_mgr_->get_string("Map Settings", "map_image");
    map_file_ = ini_mgr_->get_string("Map Settings", "map_file");
    blend_range_ = ini_mgr_->get_int("Map Settings", "blend_range");
    map_update_time_ = ini_mgr_->get_int("Map Settings", "map_update_time");
    far_view_ = ini_mgr_->get_int("Map Settings", "far_view");
    terrain_scale_ = ini_mgr_->get_int("Map Settings", "terrain_scale");
    force_rebuild_ = ini_mgr_->get_int("Map Settings", "force_rebuild");

    terrain_data_ = new cell*[map_area_ + 1];
    terrain_data_[0] = new cell[(map_area_ + 1) * (map_area_ + 1)];

    for(int i = 1; i < (map_area_ + 1); ++i) {
        terrain_data_[i] = terrain_data_[i - 1] + (map_area_ + 1);
    }

    if(!load() || force_rebuild_) {
        build();
    }
}

void terrain_map::update()
{
    int x;
    int samples;
    int start;
    int end;
    int step;
    gl_vector3 light;
    gl_rgba ambient;
    gl_rgba sun;
    gl_rgba shadow;
    float dot;
    float top;
    float drop;
    float shade;
    cell *c;
    unsigned int update_end;

    light = world_.get_light_vector();
    sun = world_.get_light_color();
    ambient = world_.get_ambient_color();
    shadow = ambient * gl_rgba(0.3f, 0.5f, 0.9f);

    if(light.get_x() > 0.0f) {
        start = map_area_;
        end = -1;
        step = -1;
    }
    else {
        start = 0;
        end = map_area_ + 1;
        step = 1;
    }

    if(light.get_x() == 0.0f) {
        drop = FLT_MAX;
    }
    else {
        drop = light.get_y() / light.get_x();
    }

    if(drop < 0) {
        drop *= -1;
    }

    update_end = SDL_GetTicks() + map_update_time_;

    while(SDL_GetTicks() < update_end) {
        // Pass over the map (either east to west or vice versa) and
        // see which points are being hit with sunlight.
        for(x = start; x != end; x += step) {
            c = &terrain_data_[x][scan_y_];
            
            gl_vector3 temp_pos(c->get_position() - camera_->get_position());
            c->set_distance(temp_pos.length() / far_view_);

            if(c->get_distance() < 0.0f) {
                c->set_distance(0.0f);
            }
            else if(c->get_distance() > 1.0f) {
                c->set_distance(1.0f);
            }
            
            if(x == start) {
                // First point is always in sunlight
                top = c->get_position().get_y();
                c->set_shadow(false);
            }
            else {
                top -= drop;

                if(c->get_position().get_y() > top) {
                    // Is this point high enough to be out of the shadow?
                    c->set_shadow(false);
                    top = c->get_position().get_y();
                }
                else {
                    // Nope!
                    c->set_shadow(true);
                }
            }
            
            dot = light.dot_product(c->get_normal());

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
                else if(tmp_x > (map_area_ + 1)) {
                    tmp_x = map_area_ + 1;
                }

                for(int yy = -1; yy <= 1; ++yy) {
                    int tmp_y = scan_y_ + yy;

                    if(tmp_y < 0) {
                        tmp_y = 0;
                    }
                    else if(tmp_y > (map_area_ + 1)) {
                        tmp_y = map_area_ + 1;
                    }

                    if(terrain_data_[tmp_x][tmp_y].get_shadow() != 0) {
                        shade += 1.0f;
                    }

                    ++samples;
                }
            }

            // Finally! We know how much light is hitting this point and
            // if it is in shadow, now figure out what colour this
            // point is
            gl_rgba scaled_ambient(ambient + (sun * dot));
            gl_rgba scaled_shadow(shadow + (ambient * dot));
            c->set_light(scaled_ambient.interpolate(scaled_shadow,
                                                    shade / (float)samples));
        }

        scan_y_ = (scan_y_ + 1) % (map_area_ + 1);
    }
}

void terrain_map::term()
{
    delete terrain_data_[0];
    delete terrain_data_;
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
void terrain_map::build()
{
    // HBITMAP basemap;
    // BITMAPINFO bmi;
    gl_rgba *cmap;
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
    scale = new float[(map_area_ + 1) * (map_area_ + 1)];
    cmap = new gl_rgba[(map_area_ + 1) * (map_area_ + 1)];
    console("size = %s", sizeof(cell));

    // Now load the bitmap
    // dc = CreateCompatibleDC(GetDC(NULL));
    // bits = GetDeviceCaps(dc, BITSPIXEL);
    // pen = CreatePens(PS_SOLID, 1, RGB(0, 255, 0));
    
    // basemap = (HBITMAP)LoadImage(AppInstance(),
    //                              map_image,
    //                              IMAGE_BITMAP,
    //                              0,
    //                              0,
    //                              LR_LOADFROMFILE | LR_DEFAULTSIZE | LRVGACOLOR);

    // if(basemap == NULL) {
    //     console("MapInit: Unable to load %s", map_image);
    
    //     return;
    // }

    console("MapBuild: rebuilding map data.");

    // Call this to fill in the bmi with good values
    // ZeroMemory(&bmi, sizeof(BITMAPINFO));
    // bmi.bmiheader.biSize = sizeof(BITMAPINFOHEADER);
    // SelectObject(dc, basemap);
    // getDIBits(dc, basemap, 0, 0, NULL, &bmi, DIR_RGB_COLORS);
    // width = bmi.bmiHeader.biWidth;
    width = map_area_ + 1;
    // height = bmi.bmiHeader.biHeight;
    // basebits = 
    //     new unsigned char[(bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) * 4];
    basebits =
        new unsigned char[((map_area_ + 1) * (map_area_ + 1)) * 4];

    // GetBitmapBits(basemap, bmi.bmiHeader.biSizeImage, basebits);
    // max_x = MIN(bmi.bmiHeader.biWidth, map_area + 1);
    // max_y = MIN(bmi.bmiHeight, map_area + 1);

    if((map_area_ + 1) < (map_area_ + 1)) {
        max_x = map_area_ + 1;
    }
    else {
        max_x = map_area_ + 1;
    }

    if((map_area_ + 1) < (map_area_ + 1)) {
        max_y = map_area_ + 1;
    }
    else {
        max_y = map_area_ + 1;
    }

    high = -9999.0f;
    low = 9999.0f;
    
    // Now pass over the image and convert the color values to elevation values.
    for(y = 0; y < (map_area_ + 1); ++y) {
        for(x = 0; x < (map_area_ + 1); ++x) {
            if((x == map_area_) && (y == map_area_)) {
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

            if(bits_ == 32) {
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
            terrain_data_[x][y].get_position().set_x((float)(x - (map_area_ / 2)));
            terrain_data_[x][y].get_position().set_y(e);
            terrain_data_[x][y].get_position().set_z((float)(y - (map_area_ / 2)));
            terrain_data_[x][y].set_shadow(false);

            // Keep track of high/low, which is used to normalize the data later
            if(high <= terrain_data_[x][y].get_position().get_y()) {
                high = terrain_data_[x][y].get_position().get_y();
            }

            if(low >= terrain_data_[x][y].get_position().get_y()) {
                low = terrain_data_[x][y].get_position().get_y();
            }
        }
    }

    // Convert elevations to scalar values 0.0 - 1.0
    // and copy them to the temp grid
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];
            scale[x + (y * (map_area_ + 1))] =
                (c->get_position().get_y() - low) / (high - low);
        }
    }

    // Calculate the surface normals
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];
            top = (unsigned int)(y -1) % (map_area_ + 1);
            bottom = (unsigned int)(y + 1) % (map_area_ + 1);
            left = (unsigned int)(x - 1) % (map_area_ + 1);
            right = (unsigned int)(x + 1) % (map_area_ + 1);

            c->set_normal(do_normal(scale[x * (top * (map_area_ + 1))] * terrain_scale_,
                                    scale[x * (bottom * (map_area_ + 1))] * terrain_scale_,
                                    scale[right * (y * (map_area_ + 1))] * terrain_scale_,
                                    scale[left * (y * (map_area_ + 1))] * terrain_scale_));
        }
    }

    // Blend the values in the temp grid and convert
    // back to elevation values
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];
            smooth = 0.0f;
            samples = 0;
            
            for(xx = -blend_range_; xx <= blend_range_; ++xx) {
                int tmp_x = x + xx;

                if(tmp_x < 0) {
                    tmp_x = 0;
                }
                else if(tmp_x > map_area_) {
                    tmp_x = map_area_;
                }

                for(yy = -blend_range_; yy <= blend_range_; ++yy) {
                    int tmp_y = y + yy;

                    if(tmp_y < 0) {
                        tmp_y = 0;
                    }
                    else if(tmp_y > map_area_) {
                        tmp_y = map_area_;
                    }

                    smooth += scale[tmp_x + (tmp_y * (map_area_ + 1))];

                    ++samples;
                }
            }

            c->get_position().set_y((smooth / (float)samples) * terrain_scale_);
        }
    }

    // Calculate the distances
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];

            gl_vector3 temp_pos(c->get_position() - camera_->get_position());
            c->set_distance(temp_pos.length() / far_view_);

            if(c->get_distance() < 0.0f) {
                c->set_distance(0.0f);
            }
            else if(c->get_distance() > 1.0f) {
                c->set_distance(1.0f);
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
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];
            c->set_layer(0, terrainspace::LAYER_LOWGRASS - 1);
            c->set_layer(0, terrainspace::LAYER_DIRT - 1);
            c->set_layer(0, terrainspace::LAYER_SAND - 1);
            c->set_layer(0, terrainspace::LAYER_ROCK - 1);

            // Sand is in the lowest parts of the map
            smooth = 
                math_smooth_step(scale[x + (y * (map_area_ + 1))], 0.3f, 0.1f);

            c->set_layer((int)(smooth * 255.0f), terrainspace::LAYER_SAND - 1);

            // The deep lush grass likes lowlands and flat areas
            e = math_smooth_step(c->get_normal().get_y(), 0.75f, 0.1f);
            smooth = 
                math_smooth_step(scale[x + (y * (map_area_ + 1))], 0.45f, 0.25f);

            smooth = (e * smooth) * 5.0f;

            if(smooth < 0) {
                smooth = 0;
            }
            else if(smooth > 1) {
                smooth = 1;
            }

            c->set_layer((int)(smooth * 255.0f), terrainspace::LAYER_LOWGRASS - 1);

            // Rock likes mild slopes and high elevations
            e = math_smooth_step(c->get_normal().get_y(), 0.8f, 0.5f);
            e += math_smooth_step(scale[x + (y * (map_area_ + 1))], 0.7f, 1.0f);

            if(e < 0) {
                smooth = 0;
            }
            else if(e > 1) {
                smooth = 1;
            }
            else {
                smooth = e;
            }

            c->set_layer((int)(smooth * 255.0f), terrainspace::LAYER_ROCK - 1);

            // Dirt likes very steep slopes
            e = math_smooth_step(c->get_normal().get_y(), 0.7f, 0.4f);
            c->set_layer((int)(e * 255.0f), terrainspace::LAYER_DIRT - 1);
        }
    }

    // Now we recalculate the surface normals. The values we calculated
    // before were based on the un-smoothed terrain data, which we needed in
    // the previous set. Now update the normals with the (more correct)
    // smoothed data.
    for(x = 0; x < (map_area_ + 1); ++x) {
        for(y = 0; y < (map_area_ + 1); ++y) {
            c = &terrain_data_[x][y];
            top = (unsigned int)(y - 1) % (map_area_ + 1);
            bottom = (unsigned int)(y + 1) % (map_area_ + 1);
            left = (unsigned int)(x - 1) % (map_area_ + 1);
            right = (unsigned int)(x + 1) % (map_area_ + 1);

            c->set_normal(do_normal(terrain_data_[x][top].get_position().get_y(),
                                    terrain_data_[x][bottom].get_position().get_y(),
                                    terrain_data_[right][y].get_position().get_y(),
                                    terrain_data_[left][y].get_position().get_y()));
        }
    }

    // All done. Let's clean up and store this thing.
    delete[] cmap;
    delete[] scale;
    delete[] basebits;
    // DeleteObject(basemap);

    save();
}

void terrain_map::save()
{
    FILE *f;

    f = fopen(map_file_.c_str(), "wb");
    fwrite(terrain_data_, sizeof(terrain_data_), 1, f);
    fclose(f);

    console("Saved %s (%d bytes)", map_file_.c_str(), sizeof(terrain_data_));
}

GLboolean terrain_map::load()
{
    FILE *f;
    int r;
    
    f = fopen(map_file_.c_str(), "rb");
    
    if(f == NULL) {
        console("MapLoad: Unable to load %s", map_file_.c_str());

        return false;
    }

    r = fread(terrain_data_, sizeof(terrain_data_), 1, f);

    if(r < 1) {
        console("MapLoad: Error loading %s", map_file_.c_str());

        return false;
    }

    fclose(f);

    return true;
}

int terrain_map::get_size() const
{
    return map_area_;
}

float terrain_map::get_elevation(GLint x, GLint y) const
{
    if(x < y) {
        x = y;
    }
    else if(x > map_area_) {
        x = map_area_;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > map_area_) {
        y = map_area_;
    }

    return terrain_data_[x][y].get_position().get_y();
}

// Get the elevation of an arbitrary point over the terrain. This will
// interpolate between points so that we can have collision with the surface.
float terrain_map::get_elevation(GLfloat x, GLfloat y) const
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
    cell_x += (map_area_ / 2);
    cell_y += (map_area_ / 2);
    y0 = get_elevation(cell_x, cell_y);
    y1 = get_elevation(cell_x + 1, cell_y);
    y2 = get_elevation(cell_x, cell_y + 1);
    y3 = get_elevation(cell_x + 1, cell_y + 1);

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

// This is a little goofy. There are several different texture layers on the
// terrain, but the lowest one (grass) is always opaque. So, we leave that
// entry out of the array. This means we have to subtract 1 from the given
// index. Confusing, but this grid is big and there is no sense in storing
// half a million redundant values.
GLfloat terrain_map::get_layer(GLint x, GLint y, GLint layer) const
{
    // The base later is always opaque
    if(layer == terrainspace::LAYER_GRASS) {
        return 1.0f;
    }

    layer -= 1;

    if(x < 0) {
        x = 0;
    }
    else if(x > map_area_) {
        x = map_area_;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > map_area_) {
        y = map_area_;
    }

    return ((float)terrain_data_[x][y].get_layer(layer) / 255.0f);
}

// How far is the given point from the camera? These values are updated during
// TerrainUpdate() and are rarely 100% accurate. These are used when calculating
// detail on the terrain.
GLfloat terrain_map::get_distance(GLint x, GLint y) const
{
    if(x < 0) {
        x = 0;
    }
    else if(x > map_area_) {
        x = map_area_;
    }


    if(y < 0) {
        y = 0;
    }
    else if(y > map_area_) {
        y = map_area_;
    }

    return terrain_data_[x][y].get_distance();
}

// The lighting color of the given point.
gl_rgba terrain_map::get_light(GLint x, GLint y) const
{
    if(x < 0) {
        x = 0;
    }
    else if(x > map_area_) {
        x = map_area_;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > map_area_) {
        y = map_area_;
    }

    return terrain_data_[x][y].get_light();
}

gl_vector3 terrain_map::get_position(GLint x, GLint y) const
{
    if(x < 0) {
        x = 0;
    }
    else if(x > map_area_) {
        x = map_area_;
    }

    if(y < 0) {
        y = 0;
    }
    else if(y > map_area_) {
        y = map_area_;
    }

    return terrain_data_[x][y].get_position();
}

/*
 * This will take the given elevations and calculate the resulting 
 * surface normal.
 */
gl_vector3 terrain_map::do_normal(GLfloat north,
                                  GLfloat south, 
                                  GLfloat east,
                                  GLfloat west) const
{
    gl_vector3 result(west - east,
                      2.0f,
                      north - south);

    result.normalize();

    return result;
}

/*
 * GetPixel is SLOW! It takes a ridiculous amount ot time to sample
 * colours using GetPixel, so we get the raw data and use this function
 * to extract arbitrary bits and used them to fill a full byte.
 */
GLushort terrain_map::rgb_sample(GLshort val, GLint shift, GLint numbits) const
{
    unsigned char r;

    r = val >> shift;
    r &= (int)(pow(2, numbits) - 1);
    r = r << (8 - numbits);

    return (r & 255);
}
