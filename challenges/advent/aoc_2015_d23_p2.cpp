#include <iostream>
#include <vector>
#include <sstream>

class Computer {
public:
  Computer()
    : a(1)
    , b(0)
    , ip(0)
  {}

  void load_instructions(std::vector<std::string> const &ins)
  {
    instructions = ins;
  }

  int process_instruction() {
    std::string cmd = instructions[ip].substr(0, instructions[ip].find(' '));
    std::string rest = instructions[ip].substr(instructions[ip].find(' ') + 1);

    std::cout << "CMD: " << cmd << " ";

    if (cmd == "hlf") {
      std::cout << "reg: " << rest << std::endl;
      if (rest == "a") {
	a /= 2;
      } else if (rest == "b") {
	b /= 2;
      }

      return 1;
    } else if (cmd == "tpl") {
      std::cout << "reg: " << rest << std::endl;

      if (rest == "a") {
	a *= 3;
      } else if (rest == "b") {
        b *= 3;
      }

      return 1;
    } else if (cmd == "inc") {
      std::cout << "reg: " << rest << std::endl;

      if (rest == "a") {
	++a;
      } else if (rest == "b") {
	++b;
      }

      return 1;
    } else if (cmd == "jmp") {
      std::stringstream buf(rest);
      char sign;
      int offset;

      buf >> sign >> offset;

      std::cout << " offset: " << sign << offset << std::endl;

      if (sign == '-') {
        offset = -offset;
      }

      return offset;
    } else if (cmd == "jie") {
      std::string reg = rest.substr(0, rest.find(','));
      rest = rest.substr(rest.find(',') + 2);
      std::stringstream buf(rest);
      char sign;
      int offset;
      buf >> sign >> offset;

      std::cout << "reg: " << reg << " offset: " << sign << offset << std::endl;

      if (sign == '-') {
	offset = -offset;
      }

      if ((reg == "a") && (a % 2 == 0)) {
	return offset;
      } else if ((reg == "b") && (b % 2 == 0)) {
        return offset;
      } else {
	return 1;
      }
    } else if (cmd == "jio") {
      std::string reg = rest.substr(0, rest.find(','));
      rest = rest.substr(rest.find(',') + 2);
      std::stringstream buf(rest);
      char sign;
      int offset;
      buf >> sign >> offset;

      std::cout << "reg: " << reg << " offset: " << sign << offset << std::endl;

      if (sign == '-') {
	offset = -offset;
      }

      if ((reg == "a") && (a == 1)) {
	return offset;
      } else if ((reg == "b") && (b == 1)) {
	return offset;
      } else {
	return 1;
      }
    }

    std::cout << "UNKNOWN CMD" << std::endl;
    return instructions.size() - ip;
  }

  void run() {
    while ((ip >= 0) && (ip < instructions.size())) {
      ip += process_instruction();
    }
  }

  void dump() const {
    std::cout << "===== DUMPING =====" << std::endl
	      << "ip: " << ip << std::endl
	      << "a: " << a << std::endl
	      << "b: " << b << std::endl
	      << "instructions:" << std::endl;

    for (auto const &ins : instructions) {
      std::cout << "\t" << ins << std::endl;
    }
    std::cout << "===================" << std::endl;
  }

private:
  unsigned int a;
  unsigned int b;
  unsigned int ip;
  std::vector<std::string> instructions;
};

int main(int argc, char *argv[])
{
  std::vector<std::string> instructions;
  std::string line;

  while (getline(std::cin, line)) {
    instructions.push_back(line);
  }

  instructions.shrink_to_fit();

  Computer computer;
  computer.load_instructions(instructions);
  computer.run();
  computer.dump();

  return 0;
}
