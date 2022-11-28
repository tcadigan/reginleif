#include <regex>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

int main(int argc, char* argv[])
{
  std::vector<std::pair<std::string, std::string>> rules;
  std::string line;
  std::string target;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    } else if (line.find("=>") == std::string::npos) {
      target = line;
    }
  }

  std::string orig = target;
  std::regex regex("Rn|Ar");
  std::smatch sm;
  int parens = 0;

  while (std::regex_search(orig, sm, regex)) {
    ++parens;
    orig = sm.suffix();
  }

  // std::cout << "Rn or Ar: " << parens << std::endl;

  orig = target;
  regex = "Y";
  int delim = 0;

  while (std::regex_search(orig, sm, regex)) {
    ++delim;
    orig = sm.suffix();
  }

  // std::cout << "Y: " << delim << std::endl;

  orig = target;
  regex = "[A-Z]";
  int count = 0;

  while (std::regex_search(orig, sm, regex)) {
    ++count;
    orig = sm.suffix();
  }

  std::cout << count - parens - (2 * delim) - 1 << std::endl;

  return 0;
}
