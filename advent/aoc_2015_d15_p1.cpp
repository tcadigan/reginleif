#include <iostream>
#include <vector>

class Ingredient
{
public:
  std::string name;
  int capacity;
  int durability;
  int flavor;
  int texture;
  int calories;

  Ingredient(std::string name, int capacity, int durability,
	     int flavor, int texture, int calories)
    : name(name)
    , capacity(capacity)
    , durability(durability)
    , flavor(flavor)
    , texture(texture)
    , calories(calories)
  {}
};

int main(int argc, char *argv[])
{
  std::string name;
  std::string junk;
  int capacity;
  int durability;
  int flavor;
  int texture;
  int calories;
  std::vector<Ingredient> ingredients;
  std::vector<int> amounts;

  while (std::cin >> name >> junk >> capacity >> junk >> junk >> durability >>
         junk >> junk >> flavor >> junk >> junk >> texture >> junk >> junk >>
         calories) {
    name.pop_back();
    ingredients.push_back(Ingredient(name, capacity, durability,
                                     flavor, texture, calories));
    amounts.push_back(0);
  }

  int iterations = 1;
  for (auto const &a : amounts) {
    iterations *= 101;
  }

  int max = 0;
  for (unsigned int i = 0; i < iterations; ++i) {
    for (unsigned int j = 0; j < amounts.size(); ++j) {
      if (amounts[j] > 100) {
        ++amounts[j + 1];
        amounts[j] = 0;
      }
    }

    bool bad = false;
    int total = 0;
    for (unsigned int j = 0; j < amounts.size(); ++j) {
      total += amounts[j];

      if (total > 100) {
        bad = true;
        break;
      }
    }

    if (bad || (total < 100)) {
      ++amounts[0];
      continue;
    }

    int capacity_total = 0;
    int durability_total = 0;
    int flavor_total = 0;
    int texture_total = 0;
    for (unsigned int j = 0; j < ingredients.size(); ++j) {
      capacity_total += (ingredients[j].capacity * amounts[j]);
      durability_total += (ingredients[j].durability * amounts[j]);
      flavor_total += (ingredients[j].flavor * amounts[j]);
      texture_total += (ingredients[j].texture * amounts[j]);
    }

    int score = 0;

    if ((capacity_total >= 0)
	&& (durability_total >= 0)
	&& (flavor_total >= 0)
	&& (texture_total >= 0)) {
      score = capacity_total * durability_total * flavor_total * texture_total;
    }

    if (score > max) {
      // for (unsigned int j = 0; j < amounts.size(); ++j) {
      // 	if (j != 0) {
      // 	  std::cout << " ";
      // 	}

      // 	std::cout << amounts[j];
      // }
      // std::cout << " " << score;
      // std::cout << std::endl;

      max = score;
    }

    ++amounts[0];
  }

  std::cout << max << std::endl;

  return 0;
}
