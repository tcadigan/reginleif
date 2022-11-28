#include <iostream>
#include <vector>

char to_char(int num) {
  if ((num > 26) || (num < 0)) {
    return '-';
  }

  return ('a' + num);
}

int from_char(char character) {
  if ((character < 'a') || (character > 'z')) {
    return -1;
  }

  return (character - 'a');
}

void increment(std::vector<int> &nums) {
  int index = nums.size() - 1;
  ++nums[index];

  for (unsigned int i = 0; i < nums.size(); ++i) {
    int idx = index - i;

    if (idx < 0) {
      idx += nums.size();
    }

    if (nums[idx] == 26) {
      nums[idx] %= 26;

      if (idx == 0) {
        ++nums[nums.size() - 1];
      } else {
        ++nums[idx - 1];
      }
    } else {
      break;
    }
  }
}

void big_increment(std::vector<int> &nums) {
  for (unsigned int i = 0; i < nums.size(); ++i) {
    if ((from_char('o') == nums[i]) || (from_char('i') == nums[i]) ||
        (from_char('l') == nums[i])) {
      ++nums[i];

      for (unsigned int j = i + 1; j < nums.size(); ++j) {
        nums[j] = 0;
      }
    }
  }
}

bool consecutive(std::vector<int> const &nums) {
  for (unsigned int i = 2; i < nums.size(); ++i) {
    if (((nums[i - 2] + 2) == nums[i]) && ((nums[i - 1] + 1) == nums[i])) {
      return true;
    }
  }

  return false;
}

bool legal(std::vector<int> const &nums) {
  for (auto const &n : nums) {
    if ((from_char('o') == n) || (from_char('i') == n) ||
        (from_char('l') == n)) {
      return false;
    }
  }

  return true;
}

bool pairs(std::vector<int> const &nums) {
  int prior = -1;
  for (unsigned int i = 1; i < nums.size(); ++i) {
    if (nums[i - 1] == nums[i]) {
      if (prior == -1) {
        prior = nums[i];
        ++i;
      } else if (nums[i] != prior) {
        return true;
      } else {
        prior = nums[i];
      }
    }
  }

  return false;
}

int main(int argc, char *argv[]) {
  std::string password = "hxbxxyzz";

  std::vector<int> nums;

  for (auto const &c : password) {
    nums.push_back(from_char(c));
  }

  bool valid = false;
  increment(nums);

  while (!valid) {
    // std::cout << "Trying: ";
    // for (auto const &n : nums) {
    //   std::cout << to_char(n);
    // }

    // std::cout << std::endl;

    if (!consecutive(nums)) {
      increment(nums);
      continue;
    }

    if (!legal(nums)) {
      big_increment(nums);
      continue;
    }

    if (!pairs(nums)) {
      increment(nums);
      continue;
    }

    valid = true;
  }

  for (auto const &n : nums) {
    std::cout << to_char(n);
  }

  std::cout << std::endl;

  return 0;
}
