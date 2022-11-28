#include <iostream>
#include <set>

int main(int argc, char *argv[])
{
  int target = 34000000;
  int total = 0;
  int house = 0;
  while (total < target) {
    ++house;

    std::set<int> vals;
    int count = 1;

    while ((count * count) <= house) {
      if (house % count == 0) {
	vals.insert(count);
	vals.insert(house / count);
      }

      ++count;
    }

    total = 0;
    for (auto const &num : vals) {
      total += (10 * num);
      // std::cout << num << std::endl;
    }

    if ((house != 0) && (house % 10000 == 0)) {
      std::cout << "Tested " << house << " houses" << std::endl;
    }
  }

  std::cout << "House " << house << " gets " << total << " presents" << std::endl;

  return 0;
}
