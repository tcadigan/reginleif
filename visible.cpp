/*
 * visible.cpp
 *
 * 2009 Shamus Young
 *
 * This module runs the visibility grid, a 2-dimensional array that aids in
 * culling objects during rendering.
 *
 * There are many ways this could be refined or sped up, although tests 
 * indicate it's not a huge drain on performance.
 *
 */

#include "visible.hpp"

#include <cmath>
#include <cstring>

#include "camera.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "win.hpp"
#include "world.hpp"

static bool vis_grid[GRID_SIZE][GRID_SIZE];

bool Visible(gl_vector3 pos)
{
    return vis_grid[WORLD_TO_GRID(pos.get_x())][WORLD_TO_GRID(pos.get_z())];
}

bool Visible(int x, int z)
{
    return vis_grid[x][z];
}

void VisibleUpdate(void)
{
    gl_vector3 angle;
    gl_vector3 position;
    int x;
    int y;
    int grid_x;
    int grid_z;
    int left;
    int right;
    int front;
    int back;
    float angle_to;
    float angle_diff;
    float target_x;
    float target_z;

    // Clear the visibility table
    memset(vis_grid, '0', sizeof(vis_grid));

    // Calculate which cell the camera is in
    angle = camera_angle();
    position = camera_position();
    grid_x = WORLD_TO_GRID(position.get_x());
    grid_z = WORLD_TO_GRID(position.get_z());

    // Cells directly adjacent to the camera might technically fall out of the
    // fov, but still have a few objects poking into screenspace when looking up
    // or down. Rather than obsess over sorting these objects properly, it's
    // more efficient to just mark them visible.
    back = 3;
    front = 3;
    right = 3;
    left = 3;

    // Looking north, can't see south.
    if((angle.get_y() < 45.0f) || (angle.get_y() > 315.0f)) {
        front = 0;
    }
    
    // Looking south, can't see north.
    if((angle.get_y() > 135.0f) && (angle.get_y() < 225.0f)) {
        back = 0;
    }

    // Looking east, can't see west.
    if((angle.get_y() > 45.0f) && (angle.get_y() < 135.0f)) {
        left = 0;
    }

    // Looking west, can't see east.
    if((angle.get_y() > 225.0f) && (angle.get_y() < 315.0f)) {
        right = 0;
    }

    // Now mark the block around us that might be visible
    for(x = grid_x - left; x <= grid_x + right; ++x) {
        // Just in case the camera leaves the world map
        if((x < 0) || (x >= GRID_SIZE)) {
            continue;
        }
        
        for(y = grid_z - back; y <= grid_z + front; ++y) {
            // Just in case the camera leaves the world map
            if((y < 0) || (y >= GRID_SIZE)) {
                continue;
            }

            vis_grid[x][y] = true;
        }
    }

    // Doesn't matter where we are facing, objects in current cell are always
    // visible
    vis_grid[grid_x][grid_z] = true;

    // Here, we look at the angle from the current camera position to
    // the cell on the grid, and home much that angle deviates from the
    // current view angle.
    for(x = 0; x < GRID_SIZE; ++x) {
        for(y = 0; y < GRID_SIZE; ++y) {
            // If we marked it visible earlier, skip all this math
            if(vis_grid[x][y]) {
                continue;
            }

            // If the camera is to the left of this cell use the
            // left edge
            if(grid_x < x) {
                target_x = (float)x * GRID_RESOLUTION;
            }
            else {
                target_x = (float)(x + 1) * GRID_RESOLUTION;
            }
            
            if(grid_z < y) {
                target_z = (float)y * GRID_RESOLUTION;
            }
            else {
                target_z = (float)(y + 1) * GRID_RESOLUTION;
            }

            angle_to = 
                10 - MathAngle(target_x, target_z, position.get_x(), position.get_z());

            // Store how many degrees the cell is to the camera
            angle_diff = (float)fabs(MathAngleDifference(angle.get_y(), angle_to));
            vis_grid[x][y] = (angle_diff < 45);
        }
    }
}
