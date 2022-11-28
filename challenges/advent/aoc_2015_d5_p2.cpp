#include <iostream>

bool distinct_pair(std::string const &str)
{
    for (unsigned int i = 0; i < str.length(); ++i) {
        for (unsigned int j = i + 2; j < str.length(); ++j) {
            if (str[i] == str[j]) {
                if (i != 0) {
                    if (str[i - 1] == str[j - 1]) {
                        return true;
                    }
                }

                if (j + 1 < str.length()) {
                    if (str[i + 1] == str[j + 1]) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool repeat_gap(std::string const &str)
{
    for (unsigned int i = 2; i < str.length(); ++i) {
        if (str[i] == str[i - 2]) {
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
        if (distinct_pair(input) && repeat_gap(input)) {
            ++count;
        }
    }

    std::cout << "nice: " << count << std::endl;

    return 0;
}
