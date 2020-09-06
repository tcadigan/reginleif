#include <iostream>
#include <map>

int main(int argc, char* argv[])
{
    std::map<std::pair<int, int>, int> visit;
int x = 0;
    int y = 0;
    char direction;

    visit[std::make_pair(x, y)] = 1;

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
            ++visit[std::make_pair(x,y)];
        }
    }

    std::cout << "Visited: " << visit.size() << std::endl;

    return 0;
}
