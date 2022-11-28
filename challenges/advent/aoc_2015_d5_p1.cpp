#include <iostream>

bool double_char(std::string const &str)
{
    char prev = str[0];
    char next;

    for (unsigned int i = 1; i < str.length(); ++i) {
        next = str[i];

        if (prev == next) {
            return true;
        }

        prev = next;
    }

    return false;
}

bool is_bad(std::string const &str)
{
    for (unsigned int i = 1; i < str.length(); ++i) {
        if (str[i] == 'b' && (str[i - 1] == 'a')) {
            return true;
        } else if (str[i] == 'd' && (str[i - 1] == 'c')) {
            return true;
        } else if (str[i] == 'q' && (str[i - 1] == 'p')) {
            return true;
        } else if (str[i] == 'y' && (str[i - 1] == 'x')) {
            return true;
        }
    }

    return false;
}

bool triple_vowel(std::string const &str)
{
    int count = 0;

    for (unsigned int i = 0; i < str.length(); ++i) {
        if (str[i] == 'a') {
            ++count;
        } else if (str[i] == 'e') {
            ++count;
        } else if (str[i] == 'i') {
            ++count;
        } else if (str[i] == 'o') {
            ++count;
        } else if (str[i] == 'u') {
            ++count;
        }

        if (count == 3) {
            return true;
        }
    }

    return false;
}

int main(int argc, char *argv[])
{
    std::string input;
    int count = 0;

    while (std::cin >> input) {
        if (double_char(input) && !is_bad(input) && triple_vowel(input)) {
            ++count;
        }
    }

    std::cout << "nice: " << count << std::endl;

    return 0;
}
