/*
 * terrain.cpp
 * 2006 Shamus Young
 *
 * Member functions for the base terrain class.
 * Here is where the magic happens. This generates the terrain and renders is.
 */

#include "terrain.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

#include "camera.hpp"
#include "console.hpp"
#include "ini.hpp"
#include "macro.hpp"
#include "map.hpp"
#include "map-texture.hpp"

// Used during compile: Add a vertex
void CTerrain::CompileVertex(int x, int y)
{
    glTexCoord2fv(&zone_uv_[(x - zone_origin_x_) + ((y - zone_origin_y_) * (zone_size_ + 1))].x);
    GLvector3 p = MapPosition(x, y);
    glVertex3fv(&p.x);
    ++vertices_;
}

// Used during compile: Add a triangle to the render list
void CTerrain::CompileTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    CompileVertex(x3, y3);
    CompileVertex(x2, y2);
    CompileVertex(x1, y1);
    ++triangles_;
}

// Used during compile: Add a triangle fan to the render list
void CTerrain::CompileFan(int x1,
                          int y1,
                          int x2,
                          int y2,
                          int x3,
                          int y3,
                          int x4,
                          int y4,
                          int x5,
                          int y5)
{
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    CompileVertex(x1, y1);
    CompileVertex(x5, y5);
    CompileVertex(x4, y4);
    CompileVertex(x3, y3);
    CompileVertex(x2, y2);
    glEnd();
    triangles_ += 3;
    glBegin(GL_TRIANGLES);
}

// Used during compile: Add a triangle fan to the render list
void CTerrain::CompileFan(int x1,
                          int y1,
                          int x2, 
                          int y2,
                          int x3,
                          int y3,
                          int x4,
                          int y4,
                          int x5,
                          int y5,
                          int x6,
                          int y6)
{
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    CompileVertex(x1, y1);
    CompileVertex(x6, y6);
    CompileVertex(x5, y5);
    CompileVertex(x4, y4);
    CompileVertex(x3, y3);
    CompileVertex(x2, y2);
    glEnd();
    triangles_ += 4;
    glBegin(GL_TRIANGLES);
}

// Used during compile: Add triangle strip to the render list
void CTerrain::CompileStrip(int x1,
                            int y1,
                            int x2,
                            int y2,
                            int x3,
                            int y3,
                            int x4,
                            int y4)
{
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    CompileVertex(x1, y1);
    CompileVertex(x2, y2);
    CompileVertex(x3, y3);
    CompileVertex(x4, y4);
    glEnd();
    triangles_ += 2;
    glBegin(GL_TRIANGLES);
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
void CTerrain::CompileBlock(int x, int y, int size)
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
        CompileStrip(x, y, x, y2, x2, y, x2, y2);

        return;
    }

    // If the edges are inactive, we need 4 triangles (Figure b)
    if(!point_[xc + (y * map_size_)]
       && !point_[xc + (y2 * map_size_)]
       && !point_[x + (yc * map_size_)]
       && !point_[x2 + (yc * map_size_)]) {
        CompileFan(xc, yc, x, y, x2, y, x2, y2, x, y2, x, y);

        return;
    }

    // If the top and bottom edges are inactive, it is impossible
    // to have sub-blocks, so we can make a single fan
    if(!point_[xc + (y * map_size_)] && !point_[xc * (y2 * map_size_)]) {
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        CompileVertex(xc, yc);
        CompileVertex(x, y);

        if(point_[x + (yc * map_size_)]) {
            // W
            CompileVertex(x, yc);
            ++triangles_;
        }

        CompileVertex(x, y2);
        CompileVertex(x2, y2);
        
        if(point_[x2 + (yc * map_size_)]) {
            // E
            CompileVertex(x2, yc);
            ++triangles_;
        }

        CompileVertex(x2, y);
        CompileVertex(x, y);
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
        CompileVertex(xc, yc);
        CompileVertex(x, y);
        CompileVertex(x, y2);
        
        if(point_[xc + (y2 * map_size_)]) {
            // S
            CompileVertex(xc, y2);
            ++triangles_;
        }

        CompileVertex(x2, y2);
        CompileVertex(x2, y);
        
        if(point_[xc + (y * map_size_)]) {
            // N
            CompileVertex(xc, y);
            ++triangles_;
        }

        CompileVertex(x, y);
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
            CompileFan(xc, yc, x, yc, x, y, x2, y, x2, yc);
        }
        else {
            // Either left or right edge is inactive
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(point_[x2 + (yc * map_size_)]) {
                // EL
                CompileVertex(x2, yc);
                ++triangles_;
            }

            CompileVertex(x2, y);
            CompileVertex(x, y);
            
            if(point_[x + (yc * map_size_)]) {
                // WR
                CompileVertex(x, yc);
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
            CompileFan(xc, yc, x2, yc, x2, y2, x, y2, x, yc);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(point_[x + (yc * map_size_)]) {
                CompileVertex(x, yc);
                ++triangles_;
            }

            CompileVertex(x, y2);
            CompileVertex(x2, y2);

            if(point_[x2 + (yc * map_size_)]) {
                CompileVertex(x2, yc);
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
            CompileFan(xc, yc, xc, y2, x, y2, x, y, xc, y);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(point_[xc + (y * map_size_)]) {
                // NL
                CompileVertex(xc, y);
                ++triangles_;
            }

            CompileVertex(x, y);
            CompileVertex(x, y2);

            if(point_[xc + (y2 * map_size_)]) {
                // SR
                CompileVertex(xc, y2);
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
            CompileFan(xc, yc, xc, y, x2, y, x2, y2, xc, y2);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(point_[xc + (yc * map_size_)]) {
                // SL
                CompileVertex(xc, y2);
                ++triangles_;
            }

            CompileVertex(x2, y2);
            CompileVertex(x2, y);
            
            if(point_[xc + (y * map_size_)]) {
                // NR
                CompileVertex(xc, y);
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
        CompileBlock(x, y, next_size);
    }
    
    if(point_[xc + (y * map_size_)] && point_[x2 + (yc * map_size_)]) {
        // Sub-block B
        CompileBlock(x + next_size, y, next_size);
    }

    if(point_[x + (yc * map_size_)] && point_[xc + (y2 * map_size_)]) {
        // Sub-block C
        CompileBlock(x, y + next_size, next_size);
    }

    if(point_[x2 + (yc * map_size_)] && point_[xc + (y2 * map_size_)]) {
        // Sub-block D
        CompileBlock(x + next_size, y + next_size, next_size);
    }
}

/*
 * Once the mesh is optimized and we know which points are active, we're
 * ready to compile. The terrain is a grid of zones. Each zone goes into
 * its own glList. So, we can compile zones a few at a time during updates.
 * (If we did them all at once it woulc cause a massive pause) Once they
 * are all complete, we start rendering the new grid of lists.
 */
void CTerrain::Compile(void)
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
    glBindTexture(GL_TEXTURE_2D, MapTexture(zone_));
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
        CompileBlock(x * zone_size_, y * zone_size_, zone_size_);
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
        CompileBlock(x * zone_size_, y * zone_size_, zone_size_);
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
 * This is used by the incremental building code in Update(). This will
 * cause X and Y to traverse the entire mesh, and then advance to the
 * next stage of building.
 */
void CTerrain::GridStep(void)
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
 * Constructor
 */
CTerrain::CTerrain(int size)
    : stage_(STAGE_IDLE)
    , map_size_(size)
    , map_half_(size / 2)
    , zone_size_(map_size_ / 2)
    , viewpoint_(glVector(0.0f, 0.0f, 0.0f))
    , boundary_(new short[size])
    , point_(new bool[size * size])
{
    IniManager ini_mgr;

    tolerance_ = ini_mgr.get_float("Terrain Settings", "tolerance");
    update_time_ = ini_mgr.get_int("Terrain Settings", "update_time");
    do_wireframe_ = ini_mgr.get_int("Terrain Settings", "do_wireframe");
    do_solid_ = ini_mgr.get_int("Terrain Settings", "do_solid");
    zone_grid_ = ini_mgr.get_int("Map Settings", "zone_grid");

    list_front_ = glGenLists(zone_grid_ * zone_grid_);
    list_back_ = glGenLists(zone_grid_ * zone_grid_);

    entity_type_ = "terrain";

    // This finds the largest power-of-two dimension for the given number.
    // This is used to determine what level of the quadtree a grid
    // position occupies.
    for(int n = 0; n < size; ++n) {
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
    zone_uv_ = new GLvector2[(zone_size_ + 1) * (zone_size_ + 1)];
    
    for(int x = 0; x <= zone_size_; ++x) {
        for(int y = 0; y <= zone_size_; ++y) {
            zone_uv_[x + (y * (zone_size_ + 1))].x = 
                (float)x / (float)zone_size_;

            zone_uv_[x + (y * (zone_size_ + 1))].y =
                (float)y / (float)zone_size_;
        }
    }
}

CTerrain::~CTerrain()
{
    delete[] point_;
    delete[] boundary_;
    delete[] zone_uv_;
}

/*
 * Cause the terrain to be rendered. This was part of a fade in/fade out
 * system I experimented with. It was too hard on framerate, and so I
 * disabled it, but the code is still here. You can re-enable the system
 * in Render.cpp
 */
void CTerrain::RenderFadeIn()
{
    unsigned int list;
    int i;

    // If we are not fading in, then just render normally
    if(!fade_) {
        Render();
        
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

/*
 * Cause the terrain to be rendered
 */
void CTerrain::Render()
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
 * This is tricky stuff. When there is called, it means that the
 * given point is needed for the terrain we are working on. Each
 * point, when activated, will recursively require two other points
 * at the next lowest level of detail. This is what causes the
 * "shattering" effect that breaks terrain into triangles. If you
 * want to know more, Google for Peter Lindstrom, the inventor of
 * this very clever system.
 */
void CTerrain::PointActivate(int x, int y)
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
        PointActivate(x - level, y);
        PointActivate(x + level, y);
    }
    else if(xl < yl) {
        PointActivate(x, y + level);
        PointActivate(x, y - level);
    }
    else {
        int x2;
        int y2;
        
        x2 = x & (level * 2);
        y2 = y & (level * 2);
        
        if(x2 == y2) {
            PointActivate(x - level, y + level);
            PointActivate(x + level, y - level);
        }
        else {
            PointActivate(x + level, y + level);
            PointActivate(x - level, y - level);
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
void CTerrain::DoQuad(int x1, int y1, int size)
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
    GLvector3 pos;

    half = size / 2;
    xc = x1 + half;
    x2 = x1 + size;
    yc = y1 + half;
    y2 = y1 + size;
    
    if((x2 > map_size_) || (y2 > map_size_) || (x1 < 0) || (y1 < 0)) {
        return;
    }

    dist = MapDistance(xc, yc);
    pos = MapPosition(x1, y1);
    ul = pos.y;
    pos = MapPosition(x2, y1);
    ur = pos.y;
    pos = MapPosition(x1, y2);
    ll = pos.y;
    pos = MapPosition(x2, y2);
    lr = pos.y;
    pos = MapPosition(xc, yc);
    center = pos.y;
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
        PointActivate(xc, yc);
    }
}

// Again, this is part of the disable fade in/fade out system
void CTerrain::FadeStart(void)
{
    if((stage_ == STAGE_WAIT_FOR_FADE) && !fade_) {
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
 * This is called every frame. This is where the terrain mesh is evaluated,
 * cut into triangles, and compiled for rendering.
 */
void CTerrain::Update(void)
{
    unsigned long end;
    unsigned long now;
    int xx;
    int yy;
    int level;
    GLvector3 newpos;

    now = SDL_GetTicks();
    end = now + update_time_;

    while(SDL_GetTicks() < end) {
        switch(stage_) {
        case STAGE_IDLE:
            if(fade_) {
                return;
            }

            newpos = CameraPosition();
            build_start_ = 0;
            viewpoint_ = CameraPosition();
            ++stage_;

            break;
        case STAGE_CLEAR:
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
                    PointActivate((xx * zone_size_) + (zone_size_ / 2),
                                  (yy * zone_size_) + (zone_size_ / 2));

                    PointActivate(xx * zone_size_, yy * zone_size_);
                }
            }

            break;
        case STAGE_QUADTREE:
            if(point_[x_ + (y_ * map_size_)]) {
                GridStep();
                
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

            DoQuad(x_ - level, y_ - level, level * 2);
            GridStep();

            break;
        case STAGE_COMPILE:
            Compile();

            break;
        case STAGE_WAIT_FOR_FADE:
            
            return;
        case STAGE_DONE:
            Console("Build %d polygons, %d vertices in %dms (%d)",
                    triangles_,
                    vertices_,
                    build_time_,
                    compile_time_);

            stage_ = STAGE_IDLE;

            return;
        default:
            // Any stages not used end up here, skip it
            ++stage_;

            break;
        }
    }

    build_time_ += (SDL_GetTicks() - now);
}
