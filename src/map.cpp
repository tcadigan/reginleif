/*
 * Copyright (C) 1995-2002 FSGames. Ported by Sean Ford and Yan Shosh
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "map.hpp"

#include "util.hpp"

#include <SDL2/SDL.hpp>

#define MAP_WIDTH 400
// Should not really be duplicating this from screen.cpp
#define GRID_SIZE 16

#define MAKE_STATE(x, y) static_cast<void *>((((y) / GRID_SIZE) * MAP_WIDTH) + ((x) / GRID_SIZE))
#define GET_STATE_X(state) ((static_cast<Sint32>(state) % MAP_WIDTH) * GRID_SIZE)
#define GET_STATE_Y(state) ((static_cast<Sint32>(state) / MAP_WIDTH) * GRID_SIZE)
#define ALIGN_TO_GRID(x) (((x) / GRID_SIZE) * GRID_SIZE)

float Map::LeastCostEstimate(void *stateStart, void *stateEnd)
{
    Sint32 x1 = GET_STATE_X(stateStart);
    Sint32 y1 = GET_STATE_Y(stateStart);
    Sint32 x2 = GET_STATE_X(stateEnd);
    Sint32 y2 = GET_STATE_Y(stateEnd);

    return sqrtf(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

void Map::AdjacentCost(void *state, std::vector<StateCost> *adjacent)
{
    Sint32 x1 = GET_STATE_X(state);
    sint32 y1 = GET_STATE_Y(state);

    for (Sint32 i = -1; i <= 1; ++i) {
        for (Sint32 j = -1; j <= 1; ++j) {
            if ((i == 0) && (j == 0)) {
                continue;
            }

            Sint32 adj_x = x1 + (i * GRID_SIZE);
            Sint32 adj_y = y1 + (j * GRID_SIZE);

            StateCost cost;
            cost.state = MAKE_STATE(adj_x, adj_y);
            cost.cost = 0;

            // TODO: Make doors impassable without a key
            // TODO: Make teleporters add another adjacent space on the other
            // of the teleporter

            // Any terrain in the way? This checks boundaries too.
            if (!myscreen->query_grid_passable(adj_x, adj_y, path_walker)) {
                continue;
            } else if (myscreen->level_data.myobmap->obmap_get_list(adj_x, adj_y).size() > 0) {
                // Any moving object in the way?
                cost.cost = 10;
            } else {
                // Nothing in the way, cost is 1 for adjacent, sqrt(2) for diagonal
                cost.cost = sqrtf((i * i) + (j * j));
            }

            // Smoothing heuristic using cross-product. This penalizes going away
            // from a straight line to the goal.
            Sint32 dx1 = adj_x - ALIGN_TO_GRID(path_walker->foe->xpos);
            Sint32 dy1 = adj_y - ALIGN_TO_GRID(path_walker->foe->ypos);
            Sint32 dx2 = path_walker->xpos - ALIGN_TO_GRID(path_walker->foe->xpos);
            Sint32 dy2 = path_walker->ypos - ALIGN_TO_GRID(path_walker->foe->ypos);
            float cross = (dx1 * dy2) - (dx2 * dy1);
            cost.cost += (fabs(cross) * 0.01f);

            adjacent->push_back(cost);
        }
    }
}

void Map::PrintStateInfo(void *state)
{
    Sint32 x1 = GET_STATE_X(state);
    Sint32 y1 = GET_STATE_Y(state);

    Log("(%d, %d)", x1, y1);
}
