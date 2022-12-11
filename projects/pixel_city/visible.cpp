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
#include "math.hpp"
#include "win.hpp"
#include "world.hpp"

static std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> vis_grid;

bool visible(gl_vector3 pos)
{
    int row = pos.get_x() / GRID_RESOLUTION;
    int col = pos.get_z() / GRID_RESOLUTION;
    return vis_grid.at(row).at(col);
}

bool visible(int x, int z)
{
    return vis_grid.at(x).at(z);
}

void visible_update()
{
    float target_x;
    float target_z;

    // Clear the visibility table
    for (std::array<bool, GRID_SIZE> &row : vis_grid) {
        row.fill(false);
    }

    // Calculate which cell the camera is in
    gl_vector3 angle = camera_angle();
    gl_vector3 position = camera_position();
    int grid_x = position.get_x() / GRID_RESOLUTION;
    int grid_z = position.get_z() / GRID_RESOLUTION;

    // Cells directly adjacent to the camera might technically fall out of the
    // fov, but still have a few objects poking into screenspace when looking up
    // or down. Rather than obsess over sorting these objects properly, it's
    // more efficient to just mark them visible.
    int back = 3;
    int front = 3;
    int right = 3;
    int left = 3;

    // Looking north, can't see south.
    if ((angle.get_y() < 45.0f) || (angle.get_y() > 315.0f)) {
        front = 0;
    }

    // Looking south, can't see north.
    if ((angle.get_y() > 135.0f) && (angle.get_y() < 225.0f)) {
        back = 0;
    }

    // Looking east, can't see west.
    if ((angle.get_y() > 45.0f) && (angle.get_y() < 135.0f)) {
        left = 0;
    }

    // Looking west, can't see east.
    if ((angle.get_y() > 225.0f) && (angle.get_y() < 315.0f)) {
        right = 0;
    }

    // Now mark the block around us that might be visible
    for (int x = grid_x - left; x <= grid_x + right; ++x) {
        // Just in case the camera leaves the world map
        if ((x < 0) || (x >= GRID_SIZE)) {
            continue;
        }

        for (int y = grid_z - back; y <= grid_z + front; ++y) {
            // Just in case the camera leaves the world map
            if ((y < 0) || (y >= GRID_SIZE)) {
                continue;
            }

            vis_grid.at(x).at(y) = true;
        }
    }

    // Doesn't matter where we are facing, objects in current cell are always
    // visible
    vis_grid.at(grid_x).at(grid_z) = true;

    // Here, we look at the angle from the current camera position to
    // the cell on the grid, and home much that angle deviates from the
    // current view angle.
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int y = 0; y < GRID_SIZE; ++y) {
            // If we marked it visible earlier, skip all this math
            if (vis_grid.at(x).at(y)) {
                continue;
            }

            // If the camera is to the left of this cell use the
            // left edge
            if (grid_x < x) {
                target_x = x * GRID_RESOLUTION;
            } else {
                target_x = (x + 1) * GRID_RESOLUTION;
            }

            if (grid_z < y) {
                target_z = y * GRID_RESOLUTION;
            } else {
                target_z = (y + 1) * GRID_RESOLUTION;
            }

            float angle_to =
                10 - math_angle(target_x, target_z, position.get_x(), position.get_z());

            // Store how many degrees the cell is to the camera
            float angle_diff = fabs(math_angle_difference(angle.get_y(), angle_to));
            vis_grid.at(x).at(y) = (angle_diff < 45);
        }
    }
}
