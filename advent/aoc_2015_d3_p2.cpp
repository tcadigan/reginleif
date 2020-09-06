#include <iostream>
#include <map>

int main(int argc, char* argv[])
{
    std::map<std::pair<int, int>, int> visit;
    int s_x = 0;
    int s_y = 0;
    int r_x = 0;
    int r_y = 0;
    bool s_turn = true;
    char direction;

    visit[std::make_pair(s_x, s_y)] = 1;
    ++visit[std::make_pair(r_x, r_y)];

    int x = s_x;
    int y = s_y;

    while (std::cin >> direction) {
        if (direction == '<') {
            --x;
        } else if (direction == '>') {
            ++x;
        } else if (direction == '^') {
            ++y;
        } else if (direction == 'v') {
            --y;
        }

        if (visit.find(std::make_pair(x, y)) == visit.end()) {
            visit[std::make_pair(x, y)] = 1;
        } else {
            ++visit[std::make_pair(x, y)];
        }

        if (s_turn) {
            s_x = x;
            s_y = y;
            x = r_x;
            y = r_y;
        } else {
            r_x = x;
            r_y = y;
            x = s_x;
            y = s_y;
        }

        s_turn = !s_turn;
    }

    std::cout << "Visited: " << visit.size() << std::endl;

    return 0;
}
