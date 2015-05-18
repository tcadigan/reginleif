/*
 * terrain.cpp
 * 2006 Shamus Young
 *
 * Member functions for the base terrain class.
 * Here is where the magic happens. This generates the terrain and renders is.
 */

#define POINT(x, y) point_[(x) + ((y) * map_size_)]

// Magic number: Fiddle with this to adjust how aggressive the program should be
// in removing polygons. Higher numbers result in fewer terrain polygons
#define TOLERANCE 0.07f

// How many milliseconds to spend each frame on the terrain
#define UPDATE_TIME 10

// Set this to 1 to force a wireframe overlay on the terrain, so you can
// see the polygons
#define DO_WIREFRAME 0

// Set this to 0 to skip rendering the terrains as solid. You'll ne to set
// either this one of DO_WIREFRAME to 1, or you won't render anything!
// You can set both to 1 if you like as well.
#define DO_SOLID 1

#include "terrain.hpp"

#include <SDL.h>
#include <GL/gl.h>

#include "camera.hpp"
#include "console.hpp"
#include "macro.hpp"
#include "map.hpp"
#include "map-texture.hpp"

// Used during compile: Add a vertex
void CTerrain::CompileVertex(int x, int y)
{
    glTexCoord2fv(&zone_uv_[(x - zone_origin_x_) + ((y - zone_origin_y_) * (zone_size_ + 1))].x);
    GLvector p = MapPosition(x, y);
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
    if((size == 1) || POINT(xc, yc)) {
        CompileStrip(x, y, x, y2, x2, y, x2, y2);

        return;
    }

    // If the edges are inactive, we need 4 triangles (Figure b)
    if(!POINT(xc, y) && !POINT(xc, y2) && !POINT(x, yc) && !POINT(x2, yc)) {
        CompileFan(xc, yc, x, y, x2, y, x2, y2, x, y2, x, y);

        return;
    }

    // If the top and bottom edges are inactive, it is impossible
    // to have sub-blocks, so we can make a single fan
    if(!POINT(xc, y) && !POINT(xc, y2)) {
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        CompileVertex(xc, yc);
        CompileVertex(x, y);

        if(POINT(x, yc)) {
            // W
            CompileVertex(x, yc);
            ++triangles_;
        }

        CompileVertex(x, y2);
        CompileVertex(x2, y2);
        
        if(POINT(x2, yc)) {
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
    if(!POINT(x, yc) && !POINT(x2, yc)) {
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        CompileVertex(xc, yc);
        CompileVertex(x, y);
        CompileVertex(x, y2);
        
        if(POINT(xc, y2)) {
            // S
            CompileVertex(xc, y2);
            ++triangles_;
        }

        CompileVertex(x2, y2);
        CompileVertex(x2, y);
        
        if(POINT(xc, y)) {
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
    if(!POINT(xc, y)) {
        // Is the top edge inactive?
        if(POINT(x, yc) && POINT(x2, yc)) {
            // Left and right edge active?
            CompileFan(xc, yc, x, yc, x, y, x2, y, x2, yc);
        }
        else {
            // Either left or right edge is inactive
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(POINT(x2, yc)) {
                // EL
                CompileVertex(x2, yc);
                ++triangles_;
            }

            CompileVertex(x2, y);
            CompileVertex(x, y);
            
            if(POINT(x, yc)) {
                // WR
                CompileVertex(x, yc);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!POINT(xc, y2)) {
        // Is the bottom edge inactive
        if(POINT(x, yc) && POINT(x2, yc)) {
            // Top and bottom edge active?
            CompileFan(xc, yc, x2, yc, x2, y2, x, y2, x, yc);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(POINT(x, yc)) {
                CompileVertex(x, yc);
                ++triangles_;
            }

            CompileVertex(x, y2);
            CompileVertex(x2, y2);

            if(POINT(x2, yc)) {
                CompileVertex(x2, yc);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!POINT(x, yc)) {
        // Is the left edge inactive?
        if(POINT(xc, y) && POINT(xc, y2)) {
            // Top and bottom edge active?
            CompileFan(xc, yc, xc, y2, x, y2, x, y, xc, y);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(POINT(xc, y)) {
                // NL
                CompileVertex(xc, y);
                ++triangles_;
            }

            CompileVertex(x, y);
            CompileVertex(x, y2);

            if(POINT(xc, y2)) {
                // SR
                CompileVertex(xc, y2);
                ++triangles_;
            }

            ++triangles_;
            glEnd();
            glBegin(GL_TRIANGLES);
        }
    }

    if(!POINT(x2, yc)) {
        // Right edge inactive?
        if(POINT(xc, y) && POINT(xc, y2)) {
            CompileFan(xc, yc, xc, y, x2, y, x2, y2, xc, y2);
        }
        else {
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            CompileVertex(xc, yc);

            if(POINT(xc, yc)) {
                // SL
                CompileVertex(xc, y2);
                ++triangles_;
            }

            CompileVertex(x2, y2);
            CompileVertex(x2, y);
            
            if(POINT(xc, y)) {
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
    if(POINT(xc, y) && POINT(x, yc)) {
        // Sub-block A
        CompileBlock(x, y, next_size);
    }
    
    if(POINT(xc, y) && POINT(x2, yc)) {
        // Sub-block B
        CompileBlock(x + next_size, y, next_size);
    }

    if(POINT(x, yc) && POINT(xc, y2)) {
        // Sub-block C
        CompileBlock(x, y + next_size, next_size);
    }

    if(POINT(x2, yc) && POINT(xc, y2)) {
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

    x = zone_ % ZONE_GRID;
    y = (zone_ - x) / ZONE_GRID;
    zone_origin_x_ = x * zone_size_;
    zone_origin_y_ = y * zone_size_;
    glNewList(list, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, MapTexture(zone_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    if(DO_SOLID != 0) {
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

    if(DO_WIREFRAME != 0) {
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

    if(zone_ == ZONES) {
        if((DO_WIREFRAME != 0) && (DO_SOLID != 0)) {
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
    : list_front_(glGenLists(ZONES))
    , list_back_(glGenLists(ZONES))
    , stage_(STAGE_IDLE)
    , map_size_(size)
    , map_half_(size / 2)
    , tolerance_(TOLERANCE)
    , zone_size_(map_size_ / 2)
    , viewpoint_(glVector(0.0f, 0.0f, 0.0f))
    , boundary_(new short[size])
    , point_(new bool[size * size])
{
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

    for(i = 0; i < ZONES; ++i) {
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

    for(i = 0; i < ZONES; ++i) {
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

    if(POINT(x, y)) {
        return;
    }

    POINT(x, y) = true;
    xl = boundary_[x];
    yl = boundary_[y];
    level = MIN(xl, yl);

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
    GLvector pos;

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
    delta = ABS((average - center)) * 5.0f;

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
 * cut into triangles, and compiled fro rendering.
 */
void CTerrain::Update(void)
{
    unsigned long end;
    unsigned long now;
    int xx;
    int yy;
    int level;
    GLvector newpos;

    now = SDL_GetTicks();
    end = now + UPDATE_TIME;

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
            for(xx = 0; xx < ZONE_GRID; ++xx) {
                for(yy = 0; yy < ZONE_GRID; ++yy) {
                    PointActivate((xx * zone_size_) + (zone_size_ / 2),
                                  (yy * zone_size_) + (zone_size_ / 2));

                    PointActivate(xx * zone_size_, yy * zone_size_);
                }
            }

            break;
        case STAGE_QUADTREE:
            if(POINT(x_, y_)) {
                GridStep();
                
                break;
            }

            xx = boundary_[x_];
            yy = boundary_[y_];
            level = MIN(xx, yy);
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
