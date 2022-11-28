#include <iostream>
#include <vector>
#include <iomanip>

int main(int argc, char *argv[])
{
  int desired_x;
  int desired_y;
  std::string word;

  while (std::cin >> word) {
    if (word == "row") {
      std::cin >> desired_y;
    } else if (word == "column") {
      std::cin >> desired_x;
    }
  }

  // std::cout << "DESIRED: " << desired_y << "," << desired_x << std::endl;

  int size = desired_x + desired_y;

  std::vector<std::vector<long>> grid;

  for (unsigned int i = 0; i < size; ++i) {
    grid.push_back(std::vector<long>(size, 0));
  }

  for (unsigned int i = 0; i < grid.size(); ++i) {
    grid[0][i] = i;
    grid[i][0] = i;
  }

  // int count = 1;
  int x = 1;
  int y = 1;
  long prev = -1;
  while (grid[desired_y][desired_x] == 0) {
    if ((x < grid.size()) && (y < grid.size())) {
      if (prev == -1) {
	grid[y][x] = 20151125;
      } else {
	grid[y][x] = (prev * 252533) % 33554393;
      }

      prev = grid[y][x];
    } else {
      prev = 0;
    }

    --y;
    ++x;
    // std::cout << x << ", " << y << std::endl;
    if (y == 0) {
      y = x;
      x = 1;
    }
    // std::cout << x << ", " << y << std::endl;

    // ++count;
  }

  // for (unsigned int i = 0; i < grid.size(); ++i) {
  //   for (unsigned int j = 0; j < grid[i].size(); ++j) {
  //     if (j != 0) {
  //       std::cout << " ";
  //     }

  //     std::cout << std::setw(8) << grid[i][j];
  //   }

  //   std::cout << std::endl;
  // }

  std::cout << grid[desired_y][desired_x] << std::endl;

  return 0;
}
