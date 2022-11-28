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
    std::map<int, std::string> index;
    std::vector<int> nums;
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

    int idx = 0;
    for (auto const &loc : locations) {
        nums.push_back(idx);
        index[idx] = loc;
        ++idx;
    }

    sort(nums.begin(), nums.end());

    int max = 0;

    for (unsigned int i = 1; i < nums.size(); ++i) {
        for (auto const &e : edges) {
            if ((index[nums[i - 1]] == e.start) && (index[nums[i]] == e.dest)) {
                max += e.distance;

                break;
            }
        }
    }

    while (std::next_permutation(nums.begin(), nums.end())) {
        int total = 0;

        for (unsigned int i = 1; i < nums.size(); ++i) {
            for (auto const &e : edges) {
                if ((index[nums[i - 1]] == e.start) && (index[nums[i]] == e.dest)) {
                    total += e.distance;

                    break;
                }
            }

            if (total > max) {
                max = total;
            }
        }
    }

    std::cout << max << std::endl;

    return 0;
}
