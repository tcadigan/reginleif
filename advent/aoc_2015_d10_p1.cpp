#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
    std::string input = "1113122113";

    for (int i = 0; i < 40; ++i) {
        std::stringstream buf;
        char prev = '\0';
        int inc = 0;
        for (auto const &c : input) {
            // std::cout << "c " << c << std::endl;
            if (c == prev) {
                ++inc;
            } else {
                if (inc != 0) {
                    // std::cout << "putting: " << inc << " " << prev << std::endl;
                    buf << inc << prev;
                    inc = 0;
                }

                prev = c;
                ++inc;
            }
        }

        // std::cout << "putting2: " << inc << " " << prev << std::endl;
        buf << inc << prev;

        input = buf.str();

        // std::cout << i << " " << input << " (" << input.size() << ")" << std::endl;
    }

    // std::cout << input << " " << input.size() << std::endl;
    std::cout << input.size() << std::endl;

    return 0;
}
