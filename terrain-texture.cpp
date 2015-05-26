/*
 * terrain-texture.cpp
 * 2006 Shamus Young
 *
 * This generates the zone textures. I'm still not happy with how this works,
 * but it can't be improved until you decide how you want to use it.
 *
 * Want static lighting? then dump this incremental update stuff and just
 * make these tectures during your "loading screen". This will speed up
 * framerate and make everything much simpler.
 *
 * Want moving shadows? Then this needs work. There are actually two groups of
 * textures. One group is in use and SHOULD NOT BE CHANGED, or the user will
 * see the textures changing. The other group is the ones being worked on now.
 * Every time one set is complete, it goes into use. Once the old ones fall
 * out of use, you can start on them. All of this is a drag on FPS and you
 * still have the problem of the shadows jumping whenever you switch to the new
 * texture set.
 */

#include "terrain-texture.hpp"

#include <SDL.h>

#include "console.hpp"
#include "world.hpp"

terrain_texture::terrain_texture(world const &world_object)
    : world_(world_object)
{
}

terrain_texture::~terrain_texture()
{
}

void terrain_texture::init(texture &texture_mgr,
                           terrain_map const &terrain_map_entity,
                           camera const &camera_object,
                           ini_manager const &ini_mgr)
{
    map_ = &terrain_map_entity;
    camera_ = &camera_object;
    ini_mgr_ = &ini_mgr;

    max_resolution_ = ini_mgr_->get_int("Map Texture", "max_resolution");

    res_texture_ = new unsigned int[max_resolution_];

    map_texture_update_time_ = 
        ini_mgr_->get_int("Map Texture", "map_texture_update_time");

    show_resolution_ = ini_mgr_->get_int("Map Texture", "show_resolution");
    uv_scale_ = ini_mgr_->get_int("Map Texture", "uv_scale");
    zone_grid_ = ini_mgr_->get_int("Map Settings", "zone_grid");

    zone_texture_ = new ztexture*[terrainspace::GRID_COUNT];
    zone_texture_[0] = new ztexture[terrainspace::GRID_COUNT * (zone_grid_ * zone_grid_)];

    for(GLint i = 1; i < terrainspace::GRID_COUNT; ++i) {
        zone_texture_[i] = zone_texture_[i - 1] + (zone_grid_ * zone_grid_);
    }

    GLint grid;
    GLuint zone;

    for(grid = terrainspace::GRID_FRONT; grid < terrainspace::GRID_COUNT; ++grid) {
        for(zone = 0; zone < (zone_grid_ * zone_grid_); ++zone) {
            zone_texture_[grid][zone].set_size(0);
            zone_texture_[grid][zone].set_ready(false);
            glGenTextures(1, zone_texture_[grid][zone].get_texture_address());
        }
    }

    buffer_ = new GLushort[(max_resolution_ * max_resolution_) * 4];
    zone_size_ = map_->get_size() / zone_grid_;
    layer_texture_[terrainspace::LAYER_GRASS] = texture_mgr.from_name("grassa512");
    layer_texture_[terrainspace::LAYER_LOWGRASS] = texture_mgr.from_name("grassb512");
    layer_texture_[terrainspace::LAYER_SAND] = texture_mgr.from_name("sand512");
    layer_texture_[terrainspace::LAYER_ROCK] = texture_mgr.from_name("rock512");
    layer_texture_[terrainspace::LAYER_DIRT] = texture_mgr.from_name("dirt512");

    if(show_resolution_ != 0) {
        res_texture_[8] = texture_mgr.from_name("n8");
        res_texture_[16] = texture_mgr.from_name("n16");
        res_texture_[32] = texture_mgr.from_name("n32");
        res_texture_[64] = texture_mgr.from_name("n64");
        res_texture_[128] = texture_mgr.from_name("n128");
        res_texture_[256] = texture_mgr.from_name("n256");
        res_texture_[512] = texture_mgr.from_name("n512");
    }

    get_camera_zone();
}

void terrain_texture::update()
{
    ztexture *z;
    GLuint end;
    GLuint now;
    GLint delta_x;
    GLint delta_y;
    GLint delta;
    GLint zone_x;
    GLint zone_y;
    GLint origin_x;
    GLint origin_y;

    if(ref_count_ < 2) {
        return;
    }

    now = SDL_GetTicks();
    end = now + map_texture_update_time_;

    while(SDL_GetTicks() < end) {
        z = &zone_texture_[current_grid_][current_zone_];
        zone_x = current_zone_ % zone_grid_;
        zone_y = (current_zone_ - zone_x) / zone_grid_;
        origin_x = zone_x * zone_size_;
        origin_y = zone_y * zone_size_;
        glBindTexture(GL_TEXTURE_2D, z->get_texture());

        if(current_layer_ == 0) {
            delta_x = (camera_zone_x_ - zone_x);

            if((camera_zone_x_ - zone_x) < 0) {
                delta_x *= -1;
            }

            delta_y = (camera_zone_y_ - zone_y);

            if((camera_zone_y_ - zone_y) < 0) {
                delta_y *= -1;
            }

            if(delta_x > delta_y) {
                delta = delta_x;
            }
            else {
                delta = delta_y;
            }

            if(delta < 2) {
                z->set_size(max_resolution_);
            }
            else if(delta < 3) {
                z->set_size(max_resolution_ / 2);
            }
            else if(delta < 4) {
                z->set_size(max_resolution_ / 4);
            }
            else if(delta < 6) {
                z->set_size(max_resolution_ / 8);
            }
            else if(delta < 7) {
                z->set_size(max_resolution_ / 16);
            }
            else if(delta < 8) {
                z->set_size(max_resolution_ / 32);
            }
            else {
                z->set_size(max_resolution_ / 64);
            }

            if(z->get_size() < 8) {
                z->set_size(8);
            }
            else if(z->get_size() > 512) {
                z->set_size(512);
            }

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         3,
                         z->get_size(), 
                         z->get_size(),
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         buffer_);
        }

        glEnable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glLoadIdentity();
        glViewport(0, 0, z->get_size(), z->get_size());
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, z->get_size(), 0, z->get_size(), 0, 1.0f);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, z->get_texture());
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, z->get_size());
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(z->get_size(), 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(z->get_size(), z->get_size());
        glEnd();

        while((current_layer_ != terrainspace::LAYER_COUNT) && (SDL_GetTicks() < end)) {
            draw_layer(origin_x, origin_y, z->get_size(), current_layer_);
            ++current_layer_;
        }

        if((show_resolution_ != 0) && (current_layer_ == terrainspace::LAYER_COUNT)) {
            draw_layer(origin_x, origin_y, z->get_size(), terrainspace::LAYER_SPECIAL);
        }
        
        if(current_layer_ == terrainspace::LAYER_COUNT) {
            draw_layer(origin_x, origin_y, z->get_size(), terrainspace::LAYER_LIGHTING);
        }

        // Save the result in our block texture
        glBindTexture(GL_TEXTURE_2D, z->get_texture());
        glReadPixels(0, 0, z->get_size(), z->get_size(), GL_RGB, GL_UNSIGNED_BYTE, buffer_);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     3,
                     z->get_size(),
                     z->get_size(),
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     buffer_);

        // Enable the following line to enable mipmapping
        // gluBuild2DMipmaps(GL_TEXTURE_2D,
        //                   4,
        //                   TEXTURE_SIZE,
        //                   TEXTURE_SIZE,
        //                   GL_RGB,
        //                   GL_UNSIGNED_BYTE,
        //                   buffer_);

        // Restore the projection matrix and cleanup
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        if(current_layer_ == terrainspace::LAYER_COUNT) {
            // Did we just finish this texture?
            pixel_count_ += (z->get_size() * z->get_size());
            current_layer_ = 0;
            z->set_ready(true);
            ++current_zone_;

            if(current_zone_ == (zone_grid_ * zone_grid_)) {
                // Little debug stuff here. Figure out how many 
                // Mb of memory we just ate.
                GLfloat meg = (float)pixel_count_ / 1048576.0f;
                console("Zonemap: %1.2fMb of data in %dms",
                        meg * 4.0f,
                        build_time_);

                pixel_count_ = 0;
                build_time_ = 0;
                
                if(current_grid_ == terrainspace::GRID_FRONT) {
                    current_grid_ = terrainspace::GRID_BACK;
                }
                else {
                    current_grid_ = terrainspace::GRID_FRONT;
                }

                current_zone_ = 0;
                ref_count_ = 0;
                get_camera_zone();
            }
        }
    }

    build_time_ += (SDL_GetTicks() - now);
}

void terrain_texture::term(void)
{
    delete[] res_texture_;
    delete[] buffer_;
}


// Get the current texture for the requested zone
GLuint terrain_texture::get_texture(GLuint zone)
{
    GLint grid;

    if(current_grid_ == terrainspace::GRID_FRONT) {
        grid = terrainspace::GRID_BACK;
    }
    else {
        grid = terrainspace::GRID_FRONT;
    }

    // If the very last texture is requested, then we know the terrain is about
    // to change over to a new set, and we can increment our ref count.
    // Once this reaches 2, we know both terrains (the one being built and the
    // one in use) are using our latest texture set, and it is safe to work on
    // the old set.
    if(zone == ((zone_grid_ * zone_grid_) - 1)) {
        ++ref_count_;
    }
    if(!zone_texture_[grid][zone].get_ready()) {
        return 0;
    }

    return zone_texture_[grid][zone].get_texture();
}

void terrain_texture::draw_layer(GLint origin_x, 
                                 GLint origin_y,
                                 GLint size,
                                 GLint layer)
{
    gl_rgba color1;
    gl_rgba color2;
    GLint step;
    GLint x;
    GLint y;
    GLint xx;
    GLint yy;
    GLint y2;
    GLfloat cell_size;
    GLboolean drawing;
    GLboolean blank;

    glBindTexture(GL_TEXTURE_2D, layer_texture_[layer]);
    cell_size = size / (GLfloat)zone_size_;

    if(cell_size >= 1.0f) {
        step = 1;
    }
    else {
        step = (int)(1.0f / cell_size);
    }

    if(layer == terrainspace::LAYER_GRASS) {
        glColor3f(1, 1, 1);
        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, uv_scale_);
        glVertex2i(0, size);
        glTexCoord2f(uv_scale_, 0);
        glVertex2i(size, 0);
        glTexCoord2f(uv_scale_, uv_scale_);
        glVertex2i(size, size);
        glEnd();

        return;
    }

    if(layer == terrainspace::LAYER_LIGHTING) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);

        for(y = -1; y < ((zone_size_ + step) + 1); y += step) {
            yy = origin_y + y;
            y2 = y + 1;
            glBegin(GL_QUAD_STRIP);
            for(x = -1; x < ((zone_size_ + step) + 1); x += step) {
                xx = origin_x + x;
                color1 = map_->get_light(xx, yy);
                color2 = map_->get_light(xx, yy + step);
                glColor3fv(color1.get_data());
                glVertex2i((GLint)((GLfloat)x * cell_size),
                           (GLint)((GLfloat)y * cell_size));

                glColor3fv(color2.get_data());
                glVertex2i((GLint)((GLfloat)x * cell_size),
                           (GLint)((GLfloat)y2 * cell_size));
            }

            glEnd();
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return;
    }

    if(layer == terrainspace::LAYER_SPECIAL) {
        glBindTexture(GL_TEXTURE_2D, res_texture_[size]);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);

        if(current_grid_ != terrainspace::GRID_FRONT) {
            glColor4f(0.6f, 0.0f, 0.6f, 0.3f);
        }
        else {
            glColor4f(0.6f, 0.6f, 0.0f, 0.3f);
        }

        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, size);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(size, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(size, size);
        glEnd();
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return;
    }

    color2 = gl_rgba(1.0f);
    color1 = color2;

    for(y = -1; y < ((zone_size_ + step) + step); y += step) {
        drawing = false;
        blank = true;
        yy = origin_y + y;
        y2 = y + 1;

        for(x = -1; x < ((zone_size_ + step) + 1); x += step) {
            xx = origin_x + x;
            color1.set_alpha(map_->get_layer(xx, yy, layer));
            color2.set_alpha(map_->get_layer(xx, yy + step, layer));

            if((color1.get_alpha() == 0.0f) && (color2.get_alpha() == 0.0f)) {
                if(blank) {
                    if(drawing) {
                        glEnd();
                        drawing = false;
                    }
                }

                blank = true;
            }
            else if(!drawing) {
                glBegin(GL_QUAD_STRIP);
                blank = false;
                drawing = true;
            }

            if(drawing) {
                glColor4fv(color1.get_data());
                glTexCoord2f(((GLfloat)x / (GLfloat)zone_size_) * uv_scale_,
                             ((GLfloat)y / (GLfloat)zone_size_) * uv_scale_);

                glVertex2i((GLint)((GLfloat)x * cell_size),
                           (GLint)((GLfloat)y * cell_size));

                glColor4fv(color2.get_data());
                glTexCoord2f(((GLfloat)x / (GLfloat)zone_size_) * uv_scale_,
                             ((GLfloat)y / (GLfloat)zone_size_) * uv_scale_);

                glVertex2i((GLint)((GLfloat)x * cell_size),
                           (GLint)((GLfloat)y * cell_size));
            }
        }

        if(drawing) {
            glEnd();
        }
    }
}

// Which zone is the camera over?
void terrain_texture::get_camera_zone()
{
    gl_vector3 cam;
    GLint zone_size_;

    cam = camera_->get_position();
    zone_size_ = map_->get_size() / zone_grid_;
    camera_zone_x_ = ((GLint)cam.get_x() + (map_->get_size() / 2)) / zone_size_;
    camera_zone_y_ = ((GLint)cam.get_y() + (map_->get_size() / 2)) / zone_size_;
}
