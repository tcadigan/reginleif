/*
 * terrain.cpp
 * 2006 Shamus Young
 *
 * Member functions for the base terrain class.
 * Here is where the magic happens. This generates the terrain and renders it.
 */

#include "terrain.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

#include "console.hpp"
#include "terrain-texture.hpp"

terrain::terrain(world const &world_object)
    : entity()
    , world_(world_object)
    , stage_(terrainspace::STAGE_IDLE)
    , viewpoint_(gl_vector3(0.0f, 0.0f, 0.0f))
{
    entity_type_ = "terrain";
}

terrain::~terrain()
{
    delete[] point_;
    delete[] boundary_;
    delete[] zone_uv_;
}

void terrain::init(terrain_texture &terrain_texture_object,
                   terrain_map const &terrain_map_entity,
                   camera const &camera_object,
                   ini_manager const &ini_mgr)
{
    terrain_texture_ = &terrain_texture_object;
    map_ = &terrain_map_entity;
    camera_ = &camera_object;
    ini_mgr_ = &ini_mgr;

    map_size_ = map_->get_size();
    map_half_ = map_size_ / 2;
    zone_size_ = map_size_ / 2;
    boundary_ = new short[map_size_];
    point_ = new GLboolean[map_size_ * map_size_];

    tolerance_ = ini_mgr_->get_float("Terrain Settings", "tolerance");
    update_time_ = ini_mgr_->get_int("Terrain Settings", "update_time");
    do_wireframe_ = ini_mgr_->get_int("Terrain Settings", "do_wireframe");
    do_solid_ = ini_mgr_->get_int("Terrain Settings", "do_solid");
    zone_grid_ = ini_mgr_->get_int("Map Settings", "zone_grid");

    list_front_ = glGenLists(zone_grid_ * zone_grid_);
    list_back_ = glGenLists(zone_grid_ * zone_grid_);

    // This finds the largest power-of-two dimension for the given number.
    // This is used to determine what level of the quadtree a grid
    // position occupies.
    for(int n = 0; n < map_size_; ++n) {
        boundary_[n] = -1;

        if(n == 0) {
            boundary_[n] = map_size_;
        }
        else {
            for(int level = map_size_; level > 1; level /= 2) {
                if((n % level) == 0) {
                    boundary_[n] = level;
                    
                    break;
                }
            }

            if(boundary_[n] == -1) {
                boundary_[n] = 1;
            }
        }
    }

    // This maps out a grid of uv values so that a texture
    // will fit exactly over a zone
    zone_uv_ = new gl_vector2[(zone_size_ + 1) * (zone_size_ + 1)];
    
    for(int x = 0; x <= zone_size_; ++x) {
        for(int y = 0; y <= zone_size_; ++y) {
            zone_uv_[x + (y * (zone_size_ + 1))].set_x((float)x / (float)zone_size_);

            zone_uv_[x + (y * (zone_size_ + 1))].set_y((float)y / (float)zone_size_);
        }
    }
}

/*
 * This is called every frame. This is where the terrain mesh is evaluated,
 * cut into triangles, and compiled for rendering.
 */
void terrain::update()
{
    unsigned long end;
    unsigned long now;
    int xx;
    int yy;
    int level;
    gl_vector3 newpos;

    now = SDL_GetTicks();
    end = now + update_time_;

    while(SDL_GetTicks() < end) {
        switch(stage_) {
        case terrainspace::STAGE_IDLE:
            if(fade_) {
                return;
            }

            newpos = camera_->get_position();
            build_start_ = 0;
            viewpoint_ = camera_->get_position();
            ++stage_;

            break;
        case terrainspace::STAGE_CLEAR:
            memset(point_, 0, map_size_ * map_size_);
            ++stage_;
            zone_ = 0;
            layer_ = 0;
            x_ = 0;
            y_ = 0;
            build_time_ = 0;

            // No matter how simple the terrain is, we need to break it
            // up enough so that the blocks aren't bigger than the 
            // compile grid
            for(xx = 0; xx < zone_grid_; ++xx) {
                for(yy = 0; yy < zone_grid_; ++yy) {
                    point_activate((xx * zone_size_) + (zone_size_ / 2),
                                   (yy * zone_size_) + (zone_size_ / 2));

                    point_activate(xx * zone_size_, yy * zone_size_);
                }
            }

            break;
        case terrainspace::STAGE_QUADTREE:
            if(point_[x_ + (y_ * map_size_)]) {
                grid_step();
                
                break;
            }

            xx = boundary_[x_];
            yy = boundary_[y_];

            if(xx < yy) {
                level = xx;
            }
            else {
                level = yy;
            }

            do_quad(x_ - level, y_ - level, level * 2);
            grid_step();

            break;
        case terrainspace::STAGE_COMPILE:
            compile();

            break;
        case terrainspace::STAGE_WAIT_FOR_FADE:
            
            return;
        case terrainspace::STAGE_DONE:
            console("Build %d polygons, %d vertices in %dms (%d)",
                    triangles_,
                    vertices_,
                    build_time_,
                    compile_time_);

            stage_ = terrainspace::STAGE_IDLE;

            return;
        default:
            // Any stages not used end up here, skip it
            ++stage_;

            break;
        }
    }

    build_time_ += (SDL_GetTicks() - now);
}

void terrain::term()
{
}

/*
 * Cause the terrain to be rendered
 */
void terrain::render()
{
    unsigned int list;
    int i;

    if(compile_back_) {
        list = list_front_;
    }
    else {
        list = list_back_;
    }

    for(i = 0; i < (zone_grid_ * zone_grid_); ++i) {
        glCallList(list + i);
    }
}

/*
 * Cause the terrain to be rendered. This was part of a fade in/fade out
 * system I experimented with. It was too hard on framerate, and so I
 * disabled it, but the code is still here. You can re-enable the system
 * in Render.cpp
 */
void terrain::render_fade_in()
{
    unsigned int list;
    int i;

    // If we are not fading in, then just render normally
    if(!fade_) {
        render();
        
        return;
    }

    // Otherwise render OPPOSITE of the normal order
    if(!compile_back_) {
        list = list_front_;
    }
    else {
        list = list_back_;
    }

    for(i = 0; i < (zone_grid_ * zone_grid_); ++i) {
        glCallList(list + i);
    }
}

// Again, this is part of the disable fade in/fade out system
void terrain::fade_start()
{
    if((stage_ == terrainspace::STAGE_WAIT_FOR_FADE) && !fade_) {
        fade_ = true;
        ++stage_;
        
        return;
    }
    
    if(fade_) {
        fade_ = false;
        compile_back_ = !compile_back_;
    }
}


/*
 * Once the mesh is optimized and we know which points are active, we're
 * ready to compile. The terrain is a grid of zones. Each zone goes into
 * its own glList. So, we can compile zones a few at a time during updates.
 * (If we did them all at once it woulc cause a massive pause) Once they
 * are all complete, we start rendering the new grid of lists.
 */
void terrain::compile()
{
    unsigned long compile_start;
    unsigned int list;
    int x;
    int y;

    compile_start = SDL_GetTicks();
    
    if(compile_back_ != 0) {
        list = list_back_ + zone_;
    }
    else {
        list = list_front_ + zone_;
    }

    if(zone_ == 0) {
        vertices_ = 0;
        triangles_ = 0;
        compile_time_ = 0;
    }

    x = zone_ % zone_grid_;
    y = (zone_ - x) / zone_grid_;
    zone_origin_x_ = x * zone_size_;
    zone_origin_y_ = y * zone_size_;
    glNewList(list, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, terrain_texture_->get_texture(zone_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    if(do_solid_ != 0) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 1.0f, 1.0f);
        use_color_ = false;
        glBegin(GL_TRIANGLES);
        compile_block(x * zone_size_, y * zone_size_, zone_size_);
        glEnd();
        glDisable(GL_BLEND);
    }

    if(do_wireframe_ != 0) {
        glDisable(GL_CULL_FACE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor4f(0.4f, 0.4f, 1.0f, 0.1f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.0f);
        glBegin(GL_TRIANGLES);
        glEnable(GL_COLOR_MATERIAL);
        compile_block(x * zone_size_, y * zone_size_, zone_size_);
        glEnd();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEndList();
    ++zone_;

    if(zone_ == (zone_grid_ * zone_grid_)) {
        if((do_wireframe_ != 0) && (do_solid_ != 0)) {
            vertices_ /= 2;
            triangles_ /= 2;
        }

        zone_ = 0;
        ++stage_;
        // Console("Compiled %d polygons, %d vertices in %dms",
        //         triangles_,
        //         vertices_,
        //         compile_time_);
    }

    compile_time_ += (SDL_GetTicks() - compile_start);
}

/*
 *                   North              N
 * *-------*       *---+---*        *---*---*    *---+---*
 * |\      |       |\     /|        |\Nl|Nr/|    |   |   |
 * | \ Sup |       | \   / |        | \ | / |    | A | B |
 * |  \    |       |  \ /  |        |Wr\|/Wl|    |   |   |
 * |   \   |   West+   *   +East   W*---*---*E   *---+---*
 * |    \  |       |  / \  |        |Wl/|\Er|    |   |   |
 * | Inf \ |       | /   \ |        | / | \ |    | C | D |
 * |      \|       |/     \|        |/Sr|Sl\|    |   |   |
 * *-------*       *-------*        *---*---*    *---*---*
 *                   South              S
 *
 * Figure a        Figure b         Figure c     Figure d
 *
 * This takes a single quadtree block and decides how to divide it for
 * rendering. If the center point is not included in the mesh (or if
 * there IS no center because we are at the lowest level of the tree),
 * then the block is simple cut into two triangles. (Figure a)
 *
 * If the center point is active, but none of the edges, the block is
 * cut into four triangles (Figure b). If the edges are active, then
 * the block is cut into a combination of smaller triangles (figure c)
 * and then sub-blocks (figure d).
 */
void terrain::compile_block(GLint x, GLint y, GLint size)
{
    int x2;
    int y2;
    int xc;
    int yc;
    int next_size;

    // Define the shape of this block. x and y are the
    // upper-left (Northwest) origin, xc and yc define the center,
    // and x2, y2 mark the lower-right (Southeast) corner, and next_size
    // is half the size of this block
    next_size = size / 2;
    x2 = x + size;
    y2 = y + size;
    xc = x + next_size;
    yc = y + next_size;

    // If this is the smallest block, or the center is inactive, then
    // just cut into two triangles as shown in Figure a
    if((size == 1) || point_[xc + (yc * map_size_)]) {
        compile_strip(x, y, x, y2, x2, y, x2, y2);

        return;
    }

    // If the edges are inactive, we need 4 triangles (Figure b)
    if(!point_[xc + (y * map_size_)]
       && !point_[xc + (y2 * map_size_)]
       && !point_[x + (yc * map_size_)]
       && !point_[x2 + (yc * map_size_)]) {
        compile_fan(xc, yc, x, y, x2, y, x2, y2, x, y2, x, y);

        return;
    }

    // If the top and bottom edges are inactive, it is impossible
    // to have sub-blocks, so we can make a single fan
    if(!point_[xc + (y * map_size_)] && !point_[xc * (y2 * map_size_)]) {
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        compile_vertex(xc, yc);
        compile_vertex(x, y);

        if(point_[x + (yc * map_size_)]) {
            // W
            compile_vertex(x, yc);
            ++triangles_;
        }

        compile_vertex(x, y2);
        compile_vertex(x2, y2);
        
        if(point_[x2 + (yc * map_size_)]) {
            // E
            compile_vertex(x2, yc);
            ++triangles_;
        }

        compile_vertex(x2, y);
        compile_vertex(x, y);
        triangles_ += 4;
        glEnd();
        glBegin(GL_TRIANGLES);

        return;
    }

    // If the left and right edges are inactive, it is impossible
    // to have sub-blocks, so we can make a single fan
    if(!point_[x + (yc * map_size_)] && !point_[x2 + (yc * map_size_)]) {
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        compile_vertex(xc, yc);
        compile_vertex(x, y);
        compile_vertex(x, y2);
        
        if(point_[xc + (y2 * map_size_)]) {
            // S
            compile_vertex(xc, y2);
            ++triangles_;
        }

        compile_vertex(x2, y2);
        compile_vertex(x2, y);
        
        if(point_[xc + (y * map_size_)]) {
            // N
            compile_vertex(xc, y);
            ++triangles_;
        }

        compile_vertex(x, y);
        triangles_ += 4;
        glEnd();
        glBegin(GL_TRIANGLES);
        
        return;
    }

    // None of the other tests worked, which means this block is a
    // combination of triangle strips and sub-blocks. Brace yourself,
    // this is not for the timid. The frist test is to find out
    // which triangles we need
    if(!point_[xc + (y * map_size_)]) {
        // Is the top edge inactive?
        if(point_[x + (yc * map_size_)] && point_[x2 + (yc * map_size_)]) {
            // Left and right edge active?
            compile_fan(xc, yc, x, yc, x, y, x2, y, x2, yc);
        }
        else {
            // Either left or right edge is inactive
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            compile_vertex(xc, yc);

            if(point_[x2 + (yc * map_size_)]) {
                // EL
                compile_vertex(x2, yc);
                ++triangles_;
            }

            compile_vertex(x2, y);
            compile_vertex(x, y);
            
            if(point_[x + (yc * map_size_)]) {
                // WR
                compile_vertex(x, yc);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!point_[xc + (y2 * map_size_)]) {
        // Is the bottom edge inactive
        if(point_[x + (yc * map_size_)] && point_[x2 + (yc * map_size_)]) {
            // Top and bottom edge active?
            compile_fan(xc, yc, x2, yc, x2, y2, x, y2, x, yc);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            compile_vertex(xc, yc);

            if(point_[x + (yc * map_size_)]) {
                compile_vertex(x, yc);
                ++triangles_;
            }

            compile_vertex(x, y2);
            compile_vertex(x2, y2);

            if(point_[x2 + (yc * map_size_)]) {
                compile_vertex(x2, yc);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!point_[x + (yc * map_size_)]) {
        // Is the left edge inactive?
        if(point_[xc + (y * map_size_)] && point_[xc + (y2 * map_size_)]) {
            // Top and bottom edge active?
            compile_fan(xc, yc, xc, y2, x, y2, x, y, xc, y);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            compile_vertex(xc, yc);

            if(point_[xc + (y * map_size_)]) {
                // NL
                compile_vertex(xc, y);
                ++triangles_;
            }

            compile_vertex(x, y);
            compile_vertex(x, y2);

            if(point_[xc + (y2 * map_size_)]) {
                // SR
                compile_vertex(xc, y2);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!point_[x2 + (yc * map_size_)]) {
        // Right edge inactive?
        if(point_[xc + (y * map_size_)] && point_[xc + (y2 * map_size_)]) {
            compile_fan(xc, yc, xc, y, x2, y, x2, y2, xc, y2);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            compile_vertex(xc, yc);

            if(point_[xc + (yc * map_size_)]) {
                // SL
                compile_vertex(xc, y2);
                ++triangles_;
            }

            compile_vertex(x2, y2);
            compile_vertex(x2, y);
            
            if(point_[xc + (y * map_size_)]) {
                // NR
                compile_vertex(xc, y);
                ++triangles_;
            }
            
            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }
    
    // Now that the various triangles hanve been added, we add th
    // various sub-blocks. This is recursive.
    if(point_[xc + (y * map_size_)] && point_[x + (yc * map_size_)]) {
        // Sub-block A
        compile_block(x, y, next_size);
    }
    
    if(point_[xc + (y * map_size_)] && point_[x2 + (yc * map_size_)]) {
        // Sub-block B
        compile_block(x + next_size, y, next_size);
    }

    if(point_[x + (yc * map_size_)] && point_[xc + (y2 * map_size_)]) {
        // Sub-block C
        compile_block(x, y + next_size, next_size);
    }

    if(point_[x2 + (yc * map_size_)] && point_[xc + (y2 * map_size_)]) {
        // Sub-block D
        compile_block(x + next_size, y + next_size, next_size);
    }
}

// Used during compile: Add a triangle to the render list
void terrain::compile_triangle(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3)
{
    compile_vertex(x3, y3);
    compile_vertex(x2, y2);
    compile_vertex(x1, y1);
    ++triangles_;
}

// Used during compile: Add a vertex
void terrain::compile_vertex(GLint x, GLint y)
{
    glTexCoord2fv(zone_uv_[(x - zone_origin_x_) + ((y - zone_origin_y_) * (zone_size_ + 1))].get_data());
    gl_vector3 p = map_->get_position(x, y);
    glVertex3fv(p.get_data());
    ++vertices_;
}

// Used during compile: Add triangle strip to the render list
void terrain::compile_strip(GLint x1,
                            GLint y1,
                            GLint x2,
                            GLint y2,
                            GLint x3,
                            GLint y3,
                            GLint x4,
                            GLint y4)
{
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    compile_vertex(x1, y1);
    compile_vertex(x2, y2);
    compile_vertex(x3, y3);
    compile_vertex(x4, y4);
    glEnd();
    triangles_ += 2;
    glBegin(GL_TRIANGLES);
}

// Used during compile: Add a triangle fan to the render list
void terrain::compile_fan(GLint x1,
                          GLint y1,
                          GLint x2,
                          GLint y2,
                          GLint x3,
                          GLint y3,
                          GLint x4,
                          GLint y4,
                          GLint x5,
                          GLint y5)
{
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    compile_vertex(x1, y1);
    compile_vertex(x5, y5);
    compile_vertex(x4, y4);
    compile_vertex(x3, y3);
    compile_vertex(x2, y2);
    glEnd();
    triangles_ += 3;
    glBegin(GL_TRIANGLES);
}

// Used during compile: Add a triangle fan to the render list
void terrain::compile_fan(GLint x1,
                          GLint y1,
                          GLint x2, 
                          GLint y2,
                          GLint x3,
                          GLint y3,
                          GLint x4,
                          GLint y4,
                          GLint x5,
                          GLint y5,
                          GLint x6,
                          GLint y6)
{
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    compile_vertex(x1, y1);
    compile_vertex(x6, y6);
    compile_vertex(x5, y5);
    compile_vertex(x4, y4);
    compile_vertex(x3, y3);
    compile_vertex(x2, y2);
    glEnd();
    triangles_ += 4;
    glBegin(GL_TRIANGLES);
}


/*
 * This is used by the incremental building code in Update(). This will
 * cause X and Y to traverse the entire mesh, and then advance to the
 * next stage of building.
 */
void terrain::grid_step()
{
    ++x_;
    
    if(x_ > map_size_) {
        x_ = 0;
        ++y_;

        if(y_ > map_size_) {
            x_ = 0;
            y_ = 0;
            ++stage_;
        }
    }
}

/*
 *    upper
 * ul-------ur
 *  |\      |
 * l| \     |r
 * e|  \    |i
 * f|   c   |g
 * t|    \  |h
 *  |     \ |t
 *  |      \|
 * ll-------lr
 *    lower
 *
 * this considers a quad for splitting. This is done by looking to see how
 * coplaner the quad is. The elevation of the corner are average, and
 * compared to the elvation of the center. Teh greater the difference between
 * these two values, the more non-coplaner this quad is
 */
void terrain::do_quad(GLint x1, GLint y1, GLint size)
{
    int xc;
    int yc;
    int x2;
    int y2;
    int half;
    float ul;
    float ur;
    float ll;
    float lr;
    float center;
    float average;
    float delta;
    float dist;
    float size_bias;
    gl_vector3 pos;

    half = size / 2;
    xc = x1 + half;
    x2 = x1 + size;
    yc = y1 + half;
    y2 = y1 + size;
    
    if((x2 > map_size_) || (y2 > map_size_) || (x1 < 0) || (y1 < 0)) {
        return;
    }

    dist = map_->get_distance(xc, yc);
    pos = map_->get_position(x1, y1);
    ul = pos.get_y();
    pos = map_->get_position(x2, y1);
    ur = pos.get_y();
    pos = map_->get_position(x1, y2);
    ll = pos.get_y();
    pos = map_->get_position(x2, y2);
    lr = pos.get_y();
    pos = map_->get_position(xc, yc);
    center = pos.get_y();
    average = (((ul + lr) + ll) + ur) / 4.0f;

    // Look for a delta between the center point and the average calculation
    delta = (average - center) * 5.0f;

    if((average - center) < 0) {
        delta *= -1;
    }

    // Scale the delta based on the size of the quad we are dealing with
    delta /= (float)size;
    
    // Scale based on distance
    delta *= (1.0f - (dist * 0.85f));

    // If the distance is very close, then we want a lot more detail
    if(dist < 0.15f) {
        delta *= 10.0f;
    }

    // Smaller quads are much less important
    size_bias = (float)(map_size_ + size) / (float)(map_size_ * 2);
    delta *= size_bias;

    if(delta > tolerance_) {
        point_activate(xc, yc);
    }
}


/*
 * This is tricky stuff. When there is called, it means that the
 * given point is needed for the terrain we are working on. Each
 * point, when activated, will recursively require two other points
 * at the next lowest level of detail. This is what causes the
 * "shattering" effect that breaks terrain into triangles. If you
 * want to know more, Google for Peter Lindstrom, the inventor of
 * this very clever system.
 */
void terrain::point_activate(GLint x, GLint y)
{
    int xl;
    int yl;
    int level;

    if((x < 0) || (x > (int)map_size_) || (y < 0) || (y > (int)map_size_)) {
        return;
    }

    if(point_[x + (y * map_size_)]) {
        return;
    }

    point_[x + (y * map_size_)] = true;
    xl = boundary_[x];
    yl = boundary_[y];

    if(xl < yl) {
        level = xl;
    }
    else {
        level = yl;
    }

    if(xl > yl) {
        point_activate(x - level, y);
        point_activate(x + level, y);
    }
    else if(xl < yl) {
        point_activate(x, y + level);
        point_activate(x, y - level);
    }
    else {
        int x2;
        int y2;
        
        x2 = x & (level * 2);
        y2 = y & (level * 2);
        
        if(x2 == y2) {
            point_activate(x - level, y + level);
            point_activate(x + level, y - level);
        }
        else {
            point_activate(x + level, y + level);
            point_activate(x - level, y - level);
        }
    }
}
