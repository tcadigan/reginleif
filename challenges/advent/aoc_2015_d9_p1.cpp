#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

class Edge {
public:
  std::string start;
  std::string dest;
  int distance;

  Edge(std::string start, std::string dest, int distance)
      : start(start), dest(dest), distance(distance) {}
};

int main(int argc, char *argv[]) {
    std::set<std::string> locations;
    std::set<std::string> visited;
    std::vector<Edge> edges;
    std::string start;
    std::string to;
    std::string end;
    std::string eq;
    int distance;

    while (std::cin >> start >> to >> end >> eq >> distance) {
        if (locations.find(start) == locations.end()) {
            locations.insert(start);
        }

        if (locations.find(end) == locations.end()) {
            locations.insert(end);
        }

        edges.push_back(Edge(start, end, distance));
        edges.push_back(Edge(end, start, distance));
    }

    int min = 1000000;

    for (auto const & loc : locations) {
        std::string start = loc;
        visited.insert(start);
        int total = 0;

        while (visited.size() != locations.size()) {
            std::string dest;
            int dist = 1000000;
            for (auto const &e : edges) {
                if ((e.start == start) &&
                    (visited.find(e.dest) == visited.end()) &&
                    (e.distance < dist)) {
                    dest = e.dest;
                    dist = e.distance;
                }
            }

            visited.insert(dest);
            total += dist;
            start = dest;
        }

        if (total < min) {
          min = total;
        }

        visited.clear();
    }

    std::cout << min << std::endl;

    return 0;
}
