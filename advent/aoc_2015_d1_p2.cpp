#include <iostream>

int main(int argc, char* arg[])
{
    char input;
    int floor = 0;
    int idx = 1;

    while (std::cin >> input) {
        if (input == '(') {
            ++floor;
        } else {
            --floor;
        }

        if (floor < 0) {
            std::cout << "Entering basement" << std::endl;
            break;
        }

        ++idx;
    }

    std::cout << "index: " << idx << std::endl;

    return 0;
}
