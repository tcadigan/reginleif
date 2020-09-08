#include <iostream>
#include <sstream>

char symbol;
int total = 0;
bool found_red = false;
std::stringstream buf;

void dictionaryContent2();
void arrayContent2();
void value(bool);

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

void plainString(bool check_red) {
  std::stringstream temp;
  temp << symbol;

  while (character()) {
    temp << symbol;
  }

  std::string word(temp.str());
  word.pop_back();

  // std::cout << "Word: \"" << word << "\"" << std::endl;

  if (check_red && (word == "red")) {
    found_red = true;
  }
}

void quotedString(bool check_red) {
  plainString(check_red);
  expect('"');
}

void dictionaryContent() {
  expect('"');
  quotedString(false);
  expect(':');
  value(true);

  if (accept(',')) {
    dictionaryContent();
  }
}

void dictionary() {
  bool prior_found_red = found_red;
  int prior_total = total;

  dictionaryContent();
  expect('}');

  if (found_red) {
    total = prior_total;
  }

  found_red = prior_found_red;
}

void arrayContent() {
  value(false);

  if (accept(',')) {
    arrayContent();
  }
}

void array() {
  arrayContent();
  expect(']');
}

void value(bool check_red) {
  if (accept('"')) {
    quotedString(check_red);
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
