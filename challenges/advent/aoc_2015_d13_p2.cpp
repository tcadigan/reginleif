#include <set>
#include <vector>
#include <iostream>

class Edge {
public:
  Edge(std::string const &from, std::string const &to, int weight)
    : from(from)
    , to(to)
    , weight(weight)
  {}

  std::string from;
  std::string to;
  int weight;
};

int main(int argc, char *argv[]) {
  std::string from;
  std::string to;
  std::string operation;
  int weight;
  std::string junk;
  std::set<std::string> names;
  std::vector<std::string> pos;
  std::vector<Edge> edges;

  while (std::cin >> from >> junk >> operation >> weight >> junk >> junk >>
         junk >> junk >> junk >> junk >> to) {
    to.pop_back();

    if (names.find(from) == names.end()) {
      names.insert(from);
      pos.push_back(from);
    }

    if (names.find(to) == names.end()) {
      names.insert(to);
      pos.push_back(to);
    }

    if (operation == "lose") {
      weight *= -1;
    }

    edges.push_back(Edge(from, to, weight));
  }

  pos.push_back("self");

  for (auto const &p : pos) {
    if (p != "self") {
      edges.push_back(Edge("self", p, 0));
      edges.push_back(Edge(p, "self", 0));
    }
  }

  sort(pos.begin(), pos.end());
  int max = 0;

  while (std::next_permutation(pos.begin(), pos.end())) {
    int total = 0;

    // bool first = true;
    // for (auto const &p : pos) {
    //   if (!first) {
    // 	std::cout << " ";
    //   }

    //   std::cout << p;
    //   first = false;
    // }

    // std::cout << std::endl;

    for (unsigned int i = 0; i < pos.size(); ++i) {
      int prior;
      int next;

      if (i == 0) {
	prior = pos.size() - 1;
      } else {
        prior = i - 1;
      }

      if ((i + 1) == pos.size()) {
	next = 0;
      } else {
        next = i + 1;
      }

      // std::cout << pos[prior] << " " << pos[i] << " " << pos[next] << std::endl;;

      for (auto const &e : edges) {
        if (((e.from == pos[i]) && (e.to == pos[next])) ||
            ((e.from == pos[i]) && (e.to == pos[prior]))) {
          total += e.weight;
        }
      }
    }

    // std::cout << "total: " << total << std::endl;

    if (total > max) {
      max = total;
    }
  }

  std::cout << max << std::endl;

  return 0;
}
