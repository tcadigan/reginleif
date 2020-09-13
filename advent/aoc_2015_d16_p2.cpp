#include <iostream>
#include <map>

int checker(std::map<std::string, int> const &ref, std::map<std::string, int> &pos)
{
  int val = 0;

  for (auto const &e : ref) {

    if (pos.find(e.first) != pos.end()) {
      if ((e.first == "cats") || (e.first == "trees")) {
	if (pos[e.first] > e.second) {
	  ++val;
	} else {
	  --val;
	}
      } else if ((e.first == "pomeranians") || (e.first == "goldfish")) {
        if ((pos[e.first] < e.second)) {
          ++val;
        } else {
          --val;
        }
      } else if (pos[e.first] == e.second) {
        ++val;
      } else if (pos[e.first] != e.second) {
	--val;
      }
    }
  }

  return val;
}

int main(int argc, char *argv[])
{
  std::map<int, std::map<std::string, int>> aunts;
  std::map<std::string, int> contents;

  contents["children"] = 3;
  contents["cats"] = 7;
  contents["samoyeds"] = 2;
  contents["pomeranians"] = 3;
  contents["akitas"] = 0;
  contents["vizslas"] = 0;
  contents["goldfish"] = 5;
  contents["trees"] = 3;
  contents["cars"] = 2;
  contents["perfumes"] = 1;

  int num;
  std::string key1;
  std::string key2;
  std::string key3;
  int val1;
  int val2;
  int val3;
  std::string junk;

  while (std::cin >> junk >> num >> junk >> key1 >> val1 >> junk >> key2 >> val2
	 >> junk >> key3 >> val3) {
    std::map<std::string, int> inputs;
    key1.pop_back();
    key2.pop_back();
    key3.pop_back();
    inputs[key1] = val1;
    inputs[key2] = val2;
    inputs[key3] = val3;
    aunts[num] = inputs;
  }

  int max = 0;
  int aunt = -1;
  for (auto &a : aunts) {
    int like = checker(contents, a.second);

    if (like > max) {
      max = like;
      aunt = a.first;
    }
  }

  std::cout << aunt << std::endl;

  return 0;
}
