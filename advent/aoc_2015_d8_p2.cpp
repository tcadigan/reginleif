#include <iostream>

int main(int argc, char* argv[])
{
    std::string input;
    // keep
    int code = 0;

    int str_data = 0;

    while (std::getline(std::cin, input)) {
        int skip = 0;
        for (unsigned int i = 0; i < input.length(); ++i) {
            ++code;

            if (skip == 0) {
                if (i == 0 || i == input.length() - 1) {
                    str_data += 3;
                } else if (input[i] == '\\' && i + 3 < input.length() && input[i + 1] == 'x') {
                    skip = 3;
                    str_data += 5;
                } else if (input[i] == '\\' && i + 1 < input.length()
                           && (input[i + 1] == '\\' || input[i + 1] == '\"')) {
                    skip = 1;
                    str_data += 4;
                } else {
                    ++str_data;
                }
            } else {
                --skip;
            }
        }
    }

    std::cout << "str_data: " << str_data
              << " code: " << code
              << " delta: " << str_data - code << std::endl;

    return 0;
}
