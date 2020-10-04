#include <iostream>
#include <vector>

std::vector<std::vector<bool>> lights;
std::vector<std::vector<bool>> temp;

bool check_light(int x, int y)
{
  int count = 0;

  // std::cout << "At " << x << "," << y << std::endl;
  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      // std::cout << "Checking " << x + i << "," << y + j << std::endl;
      if ((x + i < 0) || (x + i >= lights.size())) {
	continue;
      }

      if ((y + j < 0) || (y + j >= lights[x].size())) {
	continue;
      }

      if ((i == 0) && (j == 0)) {
        continue;
      }

      if (lights[x + i][y + j]) {
	++count;
      }
    }
  }

  if (lights[x][y]) {
    if ((count != 2) && (count != 3)) {
      return false;
    } else {
      return true;
    }
  } else {
    if (count == 3) {
      return true;
    } else {
      return false;
    }
  }
}

void print_lights()
{
  for (unsigned int i = 0; i < lights.size(); ++i) {
    for (unsigned int j = 0; j < lights[i].size(); ++j) {
      if (lights[i][j]) {
	std::cout << "#";
      } else {
        std::cout << ".";
      }
    }

    std::cout << std::endl;
  }
}

int main(int argc, char *argv[])
{
  int dimension = 100;
  for (int i = 0; i < dimension; ++i) {
    std::vector<bool> row(dimension, false);
    lights.push_back(row);
    temp.push_back(row);
  }

  char c;
  int row = 0;
  int column = 0;

  while (std::cin >> c) {
    if (c == '#') {
      lights[row][column] = true;
    } else {
      lights[row][column] = false;
    }

    ++column;

    if (column % dimension == 0) {
      ++row;
      column = 0;
    }
  }

  std::cout << "Step 0:" << std::endl;
  print_lights();

  int steps = 100;

  for (int i = 0; i < steps; ++i) {
    for (unsigned int x = 0; x < lights.size(); ++x) {
      for (unsigned int y = 0; y < lights[x].size(); ++y) {
        temp[x][y] = check_light(x, y);
      }
    }

    lights = temp;
    std::cout << std::endl << "Step " << i + 1 << ":" << std::endl;
    print_lights();
  }

  int count = 0;
  for (unsigned int x = 0; x < lights.size(); ++x) {
    for (unsigned int y = 0; y < lights[x].size(); ++y) {
      if (lights[x][y]) {
	++count;
      }
    }
  }

  std::cout << count << std::endl;

  return 0;
}
