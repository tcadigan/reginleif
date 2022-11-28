#include <iostream>
#include <vector>

void add_bit(std::vector<int> &bits)
{
  int index = 0;
  bits[index] += 1;

  while (bits[index] > 1) {
    bits[index] %= 2;
    bits[index + 1] += 1;
    ++index;
  }
}

int count_ones(std::vector<int> const &bits) {
  int count;
  for (unsigned int i = 0; i < bits.size(); ++i) {
    if (bits[i] == 1) {
      ++count;
    }
  }

  return count;
}

int main(int argc, char *argv[]) {
  std::vector<int> numbers;
  std::vector<int> bits;
  int num;

  int sum = 0;
  while (std::cin >> num) {
    numbers.push_back(num);
    bits.push_back(0);
    sum += num;
  }

  bits.push_back(0);

  int count = 0;
  int min_size = numbers.size();
  long min_quantum = 1000 * numbers.size();

  while (bits[bits.size() - 1] == 0) {
    if (count % 1000000 == 0) {
      // std::cout << "COMPUTED: " << count << std::endl;
    }

    if (count_ones(bits) > min_size) {
      add_bit(bits);
      ++count;
      continue;
    }

    int subset_sum = 0;
    long quantum = 1;
    std::vector<int> subset;
    for (unsigned int i = 0; i < (bits.size() - 1); ++i) {

      if (bits[i]) {
        subset_sum += numbers[i];
	quantum *= numbers[i];

        if (subset_sum > (sum / 3)) {
          break;
        } else {
	  subset.push_back(numbers[i]);
        }
      }
    }

    if (subset_sum == (sum / 3)) {
      if (subset.size() < min_size) {
	std::cout << "FEWER" << std::endl;

	min_size = subset.size();
	min_quantum = quantum;

	std::cout << "SUBSET:";
	for (auto const &num : subset) {
	  std::cout << " " << num;
	}
	std::cout << " SIZE: " << min_size << " QUANTUM: " << min_quantum << std::endl;

      } else if ((subset.size() == min_size) && (quantum < min_quantum)) {
	std::cout << "SMALLER" << std::endl;

	min_quantum = quantum;

	std::cout << "SUBSET:";
	for (auto const &num : subset) {
	  std::cout << " " << num;
	}
	std::cout << " SIZE: " << min_size << " QUANTUM: " << min_quantum << std::endl;
      }
    }

    add_bit(bits);
    ++count;
  }

  std::cout << min_quantum << std::endl;

  return 0;
}
