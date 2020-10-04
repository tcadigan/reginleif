#include <regex>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

int main(int argc, char* argv[])
{
  std::vector<std::pair<std::string, std::string>> rules;
  std::string line;
  std::string input;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    } else if (line.find("=>") != std::string::npos) {
      std::stringstream input;
      input << line;

      std::string regex;
      std::string junk;
      std::string replacement;
      input >> regex >> junk >> replacement;
      rules.push_back(std::make_pair(regex, replacement));
    } else {
      input = line;
    }
  }

  std::set<std::string> contents;

  for (unsigned int i = 0; i < rules.size(); ++i) {
    std::string orig = input;
    std::regex regex(rules[i].first);
    std::string replacement(rules[i].second);

    // std::cout << "Checking: " << rules[i].first << " -> " << rules[i].second << std::endl;

    std::smatch sm;
    std::string cur_prefix;
    while (std::regex_search(orig, sm, regex)) {
      // std::cout << "(" << sm.prefix() << ") " << sm.str() << " (" << sm.suffix()
      //           << ")" << std::endl;
      cur_prefix += sm.prefix();
      std::string word(cur_prefix);
      word += replacement;
      word += sm.suffix();
      // std::cout << "Word: " << word << std::endl;
      contents.insert(word);

      cur_prefix += sm.str();
      // std::cout << "(" << cur_prefix << ") (" << sm.suffix() << ")"
      //           << std::endl;
      orig = sm.suffix();
    }
  }

  // for (auto const &word : contents) {
  //   std::cout << word << std::endl;
  // }

  std::cout << contents.size() << std::endl;
  return 0;
}
