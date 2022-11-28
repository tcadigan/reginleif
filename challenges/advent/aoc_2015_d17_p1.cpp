#include <iostream>
#include <vector>

bool check(std::vector<int> const &ref, std::vector<int> &vals, int total)
{
  int cur = 0;
  for (unsigned int i = 0; i < vals.size(); ++i) {
    if (ref[i] == 1) {
      cur += vals[i];
    }

    if (cur > total) {
      return false;
    }
  }

  if (cur != total) {
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  std::vector<int> vals;
  std::vector<int> seq;
  int num;

  while (std::cin >> num) {
    vals.push_back(num);
    seq.push_back(0);
  }

  int iterations = 1;
  for (unsigned int i = 0; i < seq.size(); ++i) {
    iterations *= 2;
  }

  int count = 0;
  int total = 150;
  for (unsigned int i = 0; i < iterations; ++i) {

    // for (unsigned int j = 0; j < seq.size(); ++j) {
    //   if (j != 0) {
    // 	std::cout << " ";
    //   }

    //   std::cout << seq[j];
    // }

    // std::cout << std::endl;

    if (check(seq, vals, total)) {
        ++count;
    }

    ++seq[0];

    for (unsigned int j = 0; j < seq.size(); ++j) {
      if (seq[j] > 1) {
	++seq[j + 1];
	seq[j] = 0;
      } else {
        break;
      }
    }
  }

  std::cout << count << std::endl;

  return 0;
}
