#include <iostream>

int main(int argc, char* arg[])
{
    char input;
    int floor = 0;

    while (std::cin >> input) {
        if (input == '(') {
            ++floor;
        } else {
            --floor;
        }
    }

    std::cout << "Final floor: " << floor << std::endl;

    return 0;
}
