#include <climits>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <sstream>
#include <vector>

class VirtualMachine {
public:
  VirtualMachine(bool disassemble);
  void loadProgram(std::filesystem::path path);
  void execute();

private:
  std::string decodeLocation(unsigned short location);
  void debugDumpBinary();
  void debugLoadBinary();
  void dumpRegisters();
  void dumpMemory();
  void setRegister(unsigned short a, unsigned short val);
  unsigned short translateAddress(unsigned short address);

  bool disassemble;
  short ip;
  short reg0;
  short reg1;
  short reg2;
  short reg3;
  short reg4;
  short reg5;
  short reg6;
  short reg7;
  std::vector<unsigned short> memory;
  std::stack<unsigned short> stack;
  std::stringstream internal_buf;
  std::vector<unsigned short> breakpoints;
};

VirtualMachine::VirtualMachine(bool disassemble)
  : disassemble(disassemble)
  , ip(0)
  , reg0()
  , reg1()
  , reg2()
  , reg3(0)
  , reg4()
  , reg5()
  , reg6(0)
  , reg7(0)
  , memory(std::vector<unsigned short>(SHRT_MAX, 0))
  , stack(std::stack<unsigned short>())
{
}

void VirtualMachine::loadProgram(std::filesystem::path path)
{
  std::ifstream input(path, std::ios::in | std::ios::binary);

  unsigned short val;
  input.read(reinterpret_cast<char *>(&val), sizeof(val));

  while (input) {
    memory[ip] = val;
    ++ip;
    input.read(reinterpret_cast<char *>(&val), sizeof(val));
  }

  input.close();

  ip = 0;
}

void VirtualMachine::execute()
{
  while (static_cast<unsigned long>(ip) < memory.size()) {
    for (auto const &bp : breakpoints) {
      if (ip == bp) {
	dumpRegisters();
	return;
      }
    }

    switch(memory[ip]) {
    case 0:
      if (disassemble) {
	std::cerr << ip << ":\tHALT" << std::endl;
      }

      if (disassemble) {
	++ip;

	break;
      } else {

	return;
      }
    case 1:
      if (disassemble) {
	std::cerr << ip << ":\tSET "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
      } else {
	setRegister(memory[ip + 1], translateAddress(memory[ip + 2]));
      }

      ip += 3;

      break;
    case 2:
      if (disassemble) {
	std::cerr << ip << ":\tPUSH "
		  << decodeLocation(memory[ip + 1]) << std::endl;
      } else {
	stack.push(translateAddress(memory[ip + 1]));
      }

      ip += 2;

      break;
    case 3:
      if (disassemble) {
	std::cerr << ip << ":\tPOP "
		  << decodeLocation(memory[ip + 1]) << std::endl;
      } else {
	if (stack.empty()) {
	  std::cout << "EMPTY STACK!" << std::endl;

	  exit(1);
	}

	setRegister(memory[ip + 1], stack.top());
	stack.pop();
      }

      ip += 2;

      break;
    case 4:
      if (disassemble) {
	std::cerr << ip << ":\tEQ "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	if (translateAddress(memory[ip + 2]) == translateAddress(memory[ip + 3])) {
	  setRegister(memory[ip + 1], 1);
	} else {
	  setRegister(memory[ip + 1], 0);
	}
      }

      ip += 4;

      break;
    case 5:
      if (disassemble) {
	std::cerr << ip << ":\tGT "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	if (translateAddress(memory[ip + 2]) > translateAddress(memory[ip + 3])) {
	  setRegister(memory[ip + 1], 1);
	} else {
	  setRegister(memory[ip + 1], 0);
	}
      }

      ip += 4;

      break;
    case 6:
      if (disassemble) {
	std::cerr << ip << ":\tJMP "
		  << decodeLocation(memory[ip + 1]) << std::endl;

	ip += 2;
      } else {
	ip = translateAddress(memory[ip + 1]);
      }

      break;
    case 7:
      if (disassemble) {
	std::cerr << ip << ":\tJT "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;

	ip += 3;
      } else {
	if (translateAddress(memory[ip + 1]) != 0) {
	  ip = translateAddress(memory[ip + 2]);
	} else {
	  ip = ip + 3;
	}
      }

      break;
    case 8:
      if (disassemble) {
	std::cerr << ip << ":\tJF "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
	ip += 3;
      } else {
	if (translateAddress(memory[ip + 1]) == 0) {
	  ip = translateAddress(memory[ip + 2]);
	} else {
	  ip = ip + 3;
	}
      }

      break;
    case 9:
      if (disassemble) {
	std::cerr << ip << ":\tADD "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    (translateAddress(memory[ip + 2])
		     + translateAddress(memory[ip + 3])) % 32768);
      }

      ip += 4;

      break;
    case 10:
      if (disassemble) {
	std::cerr << ip << ":\tMULT "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    (translateAddress(memory[ip + 2])
		     * translateAddress(memory[ip + 3])) % 32768);
      }

      ip += 4;

      break;
    case 11:
      if (disassemble) {
	std::cerr << ip << ":\tMOD "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    (translateAddress(memory[ip + 2])
		     % translateAddress(memory[ip + 3])));
      }

      ip += 4;

      break;
    case 12:
      if (disassemble) {
	std::cerr << ip << ":\tAND "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 3]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    (translateAddress(memory[ip + 2])
		     & translateAddress(memory[ip + 3])));
      }

      ip += 4;

      break;
    case 13:
      if (disassemble) {
	std::cerr << ip << ":\tOR "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    (translateAddress(memory[ip + 2])
		     | translateAddress(memory[ip + 3])));
      }

      ip += 4;

      break;
    case 14:
      if (disassemble) {
	std::cerr << ip << ":\tNOT "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    ~translateAddress(memory[ip + 2]) & 0x7FFF);
      }

      ip += 3;

      break;
    case 15:
      if (disassemble) {
	std::cerr << ip << ":\tRMEM "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
      } else {
	setRegister(memory[ip + 1],
		    memory[translateAddress(memory[ip + 2])]);
      }

      ip += 3;

      break;
    case 16:
      if (disassemble) {
	std::cerr << ip << ":\tWMEM "
		  << decodeLocation(memory[ip + 1]) << " "
		  << decodeLocation(memory[ip + 2]) << std::endl;
      } else {
	memory[translateAddress(memory[ip + 1])] = translateAddress(memory[ip + 2]);
      }

      ip += 3;

      break;
    case 17:
      if (disassemble) {
	std::cerr << ip << ":\tCALL "
		  << decodeLocation(memory[ip + 1]) << std::endl;

      	ip += 2;
      } else {
	stack.push(ip + 2);
	ip = translateAddress(memory[ip + 1]);
      }

      break;
    case 18:
      if (disassemble) {
	std::cerr << ip << ":\tRET" << std::endl;

      	++ip;
      } else {
	if (stack.empty()) {
	  std::cout << "EMPTY CALL STACK!" << std::endl;

	  exit(1);
	}

	ip = stack.top();
	stack.pop();
      }

      break;
    case 19:
      if (disassemble) {
	std::cerr << ip << ":\tOUT "
		  << decodeLocation(memory[ip + 1]) << std::endl;
      } else {
	std::cout << static_cast<char>(translateAddress(memory[ip + 1]));
      }

      ip += 2;

      break;
    case 20:
      if (disassemble) {
	std::cerr << ip << ":\tIN "
		  << decodeLocation(memory[ip + 1]) << std::endl;
      } else {
	char input;
	std::cin.get(input);

	if (input == '\n') {
	  std::string command;
	  internal_buf >> command;

	  if (command == "save") {
	    debugDumpBinary();
	  } else if (command == "load") {
	    debugLoadBinary();
	  } else if (command == "break") {
	    unsigned short line;
	    internal_buf >> line;

	    breakpoints.push_back(line);
	  } else if (command == "dump") {
	    std::string which;
	    internal_buf >> which;

	    if (which == "registers") {
	      dumpRegisters();
	    }
	  } else if (command == "poke") {
	    std::string reg;
	    internal_buf >> reg;

	    if (reg == "REG0") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32768, val);
	    } else if (reg == "REG1") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32769, val);
	    } else if (reg == "REG2") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32770, val);
	    } else if (reg == "REG3") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32771, val);
	    } else if (reg == "REG4") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32772, val);
	    } else if (reg == "REG5") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32773, val);
	    } else if (reg == "REG6") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32774, val);
	    } else if (reg == "REG7") {
	      unsigned short val;
	      internal_buf >> val;
	      setRegister(32775, val);
	    } else if (reg == "IP") {
	      unsigned short val;
	      internal_buf >> val;
	      ip = val;
	    } else if (reg == "MEM") {
	      unsigned short addr;
	      unsigned short ins;
	      unsigned short val;
	      internal_buf >> addr >> ins >> val;

	      memory[addr] = ins;
	      memory[addr + 1] = val;
	    }
	  }

	  internal_buf.str("");
	  internal_buf.clear();
	} else {
	  internal_buf << input;
	}

	setRegister(memory[ip + 1], input);
      }

      ip += 2;

      break;
    case 21:
      if (disassemble) {
	std::cerr << ip << ":\tNOOP" << std::endl;
      }

      ++ip;

      break;
    default:
      if (disassemble) {
	std::cerr << ip << ":\tINV" << std::endl;
	++ip;
      } else {
	std::cout << "UNKNOWN OPCODE: "
		  << memory[ip] << "(" << ip << ")" << std::endl;

	exit(1);
      }
    }
  }
}

void VirtualMachine::dumpRegisters()
{
  std::cout << "===== DEBUG =====" << std::endl;

  for (unsigned short i = 32768; i <= 32775; ++i) {
    std::cout << decodeLocation(i) << ": " << translateAddress(i) << std::endl;
  }

  std::cout << "=================" << std::endl;
}

void VirtualMachine::dumpMemory()
{
  unsigned int num_cols = 32;

  for (unsigned int rows = 0; rows <= (memory.size() / num_cols) + 1; ++rows) {
    for (unsigned int cols = 0; cols <= num_cols; ++cols) {
      if (rows == 0) {
	if (cols == 0) {
	  std::cout << std::setw(6) << " ";
	} else {
	  std::cout << " " << std::setw(5) << (cols - 1);
	}
      } else if (cols == 0) {
	std::cout << std::setw(5) << (rows - 1) << ":";
      } else {
	std::cout << " " << std::setw(5) << memory[((rows - 1 ) * num_cols) + (cols - 1)];
      }
    }

    std::cout << std::endl;
  }
}

std::string VirtualMachine::decodeLocation(unsigned short location)
{
  if (location <= 32767) {
    return std::to_string(location);
  } else if (location <= 32775) {
    return "REG" + std::to_string(location - 32768);
  } {
    return "INV";
  }
}

void VirtualMachine::debugDumpBinary()
{
  std::ofstream output("save.bin", std::ios::out | std::ios::binary);

  output.write(reinterpret_cast<char *>(&ip), sizeof ip);

  for (unsigned short i = 32768; i <= 32775; ++i) {
    unsigned short val = translateAddress(i);
    output.write(reinterpret_cast<char *>(&val), sizeof val);
  }

  std::stack<unsigned short> temp;

  while (!stack.empty()) {
    temp.push(stack.top());
    stack.pop();
  }

  unsigned short size = temp.size();
  output.write(reinterpret_cast<char *>(&size), sizeof size);

  while (!temp.empty()) {
    unsigned short val = temp.top();
    output.write(reinterpret_cast<char *>(&val), sizeof val);
    stack.push(temp.top());
    temp.pop();
  }

  for (auto mem : memory) {
    output.write(reinterpret_cast<char *>(&mem), sizeof(mem));
  }

  output.close();
}

void VirtualMachine::debugLoadBinary()
{
  std::ifstream input("save.bin", std::ios::in | std::ios::binary);

  input.read(reinterpret_cast<char *>(&ip), sizeof ip);

  for (unsigned short i = 32768; i <= 32775; ++i) {
    unsigned short val;
    input.read(reinterpret_cast<char *>(&val), sizeof val);

    setRegister(i, val);
  }

  unsigned short size;
  input.read(reinterpret_cast<char *>(&size), sizeof size);

  unsigned short val;

  while (size) {
    input.read(reinterpret_cast<char *>(&val), sizeof val);
    --size;
    stack.push(val);
  }

  input.read(reinterpret_cast<char *>(&val), sizeof(val));

  unsigned short idx = 0;
  while (input) {
    memory[idx] = val;
    ++idx;
    input.read(reinterpret_cast<char *>(&val), sizeof(val));
  }

  input.close();
}

void VirtualMachine::setRegister(unsigned short a, unsigned short val)
{
  switch (a - 32768) {
  case 0:
    reg0 = val;

    break;
  case 1:
    reg1 = val;

    break;
  case 2:
    reg2 = val;

    break;
  case 3:
    reg3 = val;

    break;
  case 4:
    reg4 = val;

    break;
  case 5:
    reg5 = val;

    break;
  case 6:
    reg6 = val;

    break;
  case 7:
    reg7 = val;

    break;
  default:
    std::cout << "UNKNOWN REGISTER: " << (a - 32768) << std::endl;

    exit(1);
  }
}

unsigned short VirtualMachine::translateAddress(unsigned short address)
{
  if (address < 32768) {
    return address;
  }

  switch(address - 32768) {
  case 0:

    return reg0;
  case 1:

    return reg1;
  case 2:

    return reg2;
  case 3:

    return reg3;
  case 4:

    return reg4;
  case 5:

    return reg5;
  case 6:

    return reg6;
  case 7:

    return reg7;
  default:
    std::cout << "UNKNOWN REGISTER: " << (address - 32768) << std::endl;

    exit(1);
  }
}

int main(int argc, char *argv[])
{
  bool disassemble = false;
  std::filesystem::path path;

  if (argc <= 1) {
    std::cout << "USAGE: " << argv[0] << " [-d] <binary_file>" << std::endl;

    return 1;
  } else if (argc == 2) {
    if (strncmp(argv[1], "-d", strlen("-d")) == 0) {
      std::cout << "USAGE: " << argv[0] << " [-d] <binary_file>" << std::endl;

      return 1;
    } else {
      path = std::filesystem::path(argv[1]);
    }
  } else if (argc == 3) {
    disassemble = true;

    if (strncmp(argv[1], "-d", strlen("-d")) == 0) {
      path = std::filesystem::path(argv[2]);
    } else if (strncmp(argv[2], "-d", strlen("-d")) == 0) {
      path = std::filesystem::path(argv[1]);
    } else {
      std::cout << "USAGE: " << argv[0] << " [-d] <binary_file>" << std::endl;

      return 1;
    }
  } else {
      std::cout << "USAGE: " << argv[0] << " [-d] <binary_file>" << std::endl;

      return 1;
  }

  VirtualMachine machine(disassemble);
  machine.loadProgram(path);
  machine.execute();

  return 0;
}
