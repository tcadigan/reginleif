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

#include "base.hpp"
#include "video_screen.hpp"
#include "walker.hpp"

#define POS_INFINITY 1000000

#define MAP_WIDTH 400
// Should not really be duplicating this from screen.cpp
#define GRID_SIZE 16

#define MAKE_STATE(x, y) ((((y) / GRID_SIZE) * MAP_WIDTH) + ((x) / GRID_SIZE))
#define GET_STATE_X(state) (((state) % MAP_WIDTH) * GRID_SIZE)
#define GET_STATE_Y(state) (((state) / MAP_WIDTH) * GRID_SIZE)
#define ALIGN_TO_GRID(x) (((x) / GRID_SIZE) * GRID_SIZE)

class StateScore {
public:
    StateScore(Sint32 state, float fscore);

    friend bool operator<(StateScore const &lhs, StateScore const &rhs) {
        // On score first
        if (lhs.fscore < rhs.fscore) {
            return true;
        } else if (lhs.fscore == rhs.fscore) {
            // On values second
            if (GET_STATE_X(lhs.state) < GET_STATE_X(lhs.state)) {
                return true;
            } else if (GET_STATE_X(lhs.state) == GET_STATE_X(rhs.state)) {
                return GET_STATE_Y(lhs.state) < GET_STATE_Y(lhs.state);
            }
        }

        return false;
    }

    Sint32 getState() const;

private:
    Sint32 state;
    float fscore;
};

StateScore::StateScore(Sint32 state, float fscore)
    : state(state)
    , fscore(fscore)
{
}

Sint32 StateScore::getState() const
{
    return state;
}

// i.e. taxicab_distance
float Map::LeastCostEstimate(Sint32 stateStart, Sint32 stateEnd)
{
    Sint32 x1 = GET_STATE_X(stateStart);
    Sint32 y1 = GET_STATE_Y(stateStart);
    Sint32 x2 = GET_STATE_X(stateEnd);
    Sint32 y2 = GET_STATE_Y(stateEnd);

    return sqrtf(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

std::vector<Sint32> Map::Neighbors(Sint32 state)
{
    std::vector<Sint32> results;
    Sint32 x1 = GET_STATE_X(state);
    Sint32 y1 = GET_STATE_Y(state);

    for (Sint32 i = -1; i <= 1; ++i) {
        for (Sint32 j = -1; j <= 1; ++j) {
            if ((i == 0) && (j == 0)) {
                continue;
            }

            Sint32 adj_x = x1 + (i * GRID_SIZE);
            Sint32 adj_y = y1 + (j * GRID_SIZE);

            // Don't roll off the row
            if ((i == -1) && ((adj_x % GRID_SIZE) == (GRID_SIZE - 1))) {
                continue;
            } else if ((i == 1) && ((adj_x % GRID_SIZE) == 0)) {
                continue;
            }

            // Don't roll off the column (not needed?)
            if ((j == -1) && ((adj_y % GRID_SIZE) == (GRID_SIZE - 1))) {
                continue;
            } else if ((j == 1) && ((adj_y % GRID_SIZE) == 0)) {
                continue;
            }

            results.push_back(MAKE_STATE(adj_x, adj_y));
        }
    }

    return results;
}

float Map::AdjacentCost(Sint32 start, Sint32 adjacent)
{
    Sint32 x1 = GET_STATE_X(adjacent);
    Sint32 y1 = GET_STATE_Y(adjacent);

    float cost;

    // TODO: Make doors impassable without a key
    // TODO: Make teleporters add another adjacent space on the other
    // of the teleporter

    // Any terrain in the way? This checks boundaries too.
    if (!myscreen->query_grid_passable(x1, y1, path_walker)) {
        cost = POS_INFINITY;
    } else if (myscreen->level_data.myobmap->obmap_get_list(x1, y1).size() > 0) {
        // Any moving object in the way?
        cost = 10;
    } else {
        // Nothing in the way, cost is 1 for adjacent, sqrt(2) for diagonal
        Sint32 x0 = GET_STATE_X(start);
        Sint32 y0 = GET_STATE_Y(start);

        if ((abs(x0 - x1) == 1) && (abs(y0 - y1) == 1)) {
            cost = sqrtf(2);
        } else {
            cost = 1;
        }
    }

    // Smoothing heuristic using cross-product. This penalizes going away from
    // a straight line to the goal.
    Sint32 dx1 = x1 - ALIGN_TO_GRID(path_walker->foe->xpos);
    Sint32 dy1 = y1 - ALIGN_TO_GRID(path_walker->foe->ypos);
    Sint32 dx2 = path_walker->xpos - ALIGN_TO_GRID(path_walker->foe->xpos);
    Sint32 dy2 = path_walker->ypos - ALIGN_TO_GRID(path_walker->foe->ypos);
    float cross = (dx1 * dy2) - (dx2 * dy1);
    cost += (fabs(cross * 0.01f));

    return cost;
}

std::vector<Sint32> Map::ReconstructPath(std::map<Sint32, Sint32> cameFrom, Sint32 current)
{
    std::vector<Sint32> total_path;
    total_path.push_back(current);

    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom.find(current)->second;
        total_path.push_back(current);
    }

    return total_path;
}

std::vector<Sint32> Map::solve(Sint32 start, Sint32 goal)
{
    std::set<StateScore> openSet;
    openSet.insert(StateScore(start, LeastCostEstimate(start, goal)));

    std::map<Sint32, Sint32> cameFrom;
    std::map<Sint32, float> gScore;
    gScore[start] = 0;

    std::map<Sint32, float> fScore;
    fScore[start] = LeastCostEstimate(start, goal);

    while (!openSet.empty()) {
        Sint32 current = openSet.begin()->getState();

        if (current == goal) {
            return ReconstructPath(cameFrom, current);
        }

        openSet.erase(openSet.begin());

        for (auto &n : Neighbors(current)) {
            float tentative_gscore = gScore[current] + AdjacentCost(current, n);

            if (gScore.find(n) == gScore.end()) {
                gScore[n] = POS_INFINITY;
            }

            if (tentative_gscore < gScore[n]) {
                cameFrom[n] = current;
                gScore[n] = tentative_gscore;

                fScore[n] = gScore[n] + LeastCostEstimate(n, goal);

                StateScore score_state(n, fScore[n]);

                if (openSet.find(score_state) == openSet.end()) {
                    openSet.insert(score_state);
                }
            }
        }
    }

    return std::vector<Sint32>();
}
