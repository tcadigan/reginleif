#include <iostream>

int main(int argc, char* argv[])
{
    std::string input;
    int str_data = 0;
    int code = 0;

    while (std::getline(std::cin, input)) {
        int skip = 0;
        for (unsigned int i = 0; i < input.length(); ++i) {
            if (skip == 0) {
                if (input[i] == '\\' && i + 1 < input.length()
                    && (input[i + 1] == '\\' || input[i + 1] == '\"')) {
                    skip = 1;
                    ++str_data;
                } else if (input[i] == '\\' && i + 3 < input.length() && input[i + 1] == 'x') {
                    skip = 3;
                    ++str_data;
                } else if (i != 0 && i != input.length() - 1) {
                    ++str_data;
                }
            } else {
                --skip;
            }

            ++code;

        }
    }

    std::cout << "code: " << code
              << " str_data: " << str_data
              << " delta: " << code - str_data << std::endl;

    return 0;
}
