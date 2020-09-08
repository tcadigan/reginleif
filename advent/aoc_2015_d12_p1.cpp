#include <iostream>
#include <sstream>

char symbol;
int total = 0;
std::stringstream buf;

void dictionaryContent2();
void arrayContent2();
void value();

void nextsymbol() { std::cin >> symbol; }

bool accept(char sym) {
  if (sym == symbol) {
    nextsymbol();
    return true;
  }

  return false;
}

bool expect(char sym) {
  if (accept(sym)) {
    return true;
  }

  std::cout << "Unexpected symbol: \'" << sym << "\'" << std::endl;

  return false;
}

bool character() {
  return accept('a') || accept('b') || accept('c') || accept('d') ||
         accept('e') || accept('f') || accept('g') || accept('h') ||
         accept('i') || accept('j') || accept('k') || accept('l') ||
         accept('m') || accept('n') || accept('o') || accept('p') ||
         accept('q') || accept('r') || accept('s') || accept('t') ||
         accept('u') || accept('v') || accept('w') || accept('x') ||
         accept('y') || accept('z');
}

bool number(bool allow_zero) {
  if (allow_zero) {
    if (accept('0')) {
      return true;
    }
  }

  return accept('1') || accept('2') || accept('3') || accept('4') ||
         accept('5') || accept('6') || accept('7') || accept('8') ||
         accept('9');
}

void numeral(bool is_negative) {
  std::stringstream temp;
  temp << symbol;

  number(!is_negative);
  temp << symbol;

  while (number(true)) {
    temp << symbol;
  }

  std::string word(temp.str());
  word.pop_back();
  temp << word;

  int num;
  temp >> num;

  if (is_negative) {
    num *= -1;
  }

  // std::cout << "Number: " << num << std::endl;
  total += num;
}

void plainString() {
  std::stringstream temp;
  temp << symbol;

  while (character()) {
    temp << symbol;
  }

  std::string word(temp.str());
  word.pop_back();

  // std::cout << "Word: \"" << word << "\"" << std::endl;
}

void quotedString() {
  plainString();
  expect('"');
}

void dictionaryContent() {
  expect('"');
  quotedString();
  expect(':');
  value();

  if (accept(',')) {
    dictionaryContent();
  }
}

void dictionary() {
  dictionaryContent();
  expect('}');
}

void arrayContent() {
  value();

  if (accept(',')) {
    arrayContent();
  }
}

void array() {
  arrayContent();
  expect(']');
}

void value() {
  if (accept('"')) {
    quotedString();
  } else if (accept('{')) {
    dictionary();
  } else if (accept('[')) {
    array();
  } else if (accept('-')) {
    numeral(true);
  } else {
    numeral(false);
  }
}

void document() {
  if (accept('{')) {
    dictionary();
  } else if (accept('[')) {
    array();
  }
}

int main(int argc, char *argv[]) {
  nextsymbol();
  document();

  std::cout << "Total: " << total << std::endl;
  return 0;
}
