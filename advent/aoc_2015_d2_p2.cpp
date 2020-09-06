#include <iostream>

int main(int argc, char* argv[])
{
    int length;
    int width;
    int height;
    char delim;
    int total = 0;

    while (std::cin >> length >> delim >> width >> delim >> height) {
        int extra = 2 * length + 2 * width;

        if ((2 * length + 2 * height) < extra) {
            extra = 2 * length + 2 * height;
        }

        if ((2 * width + 2 * height) < extra) {
            extra = 2* width + 2 * height;
        }

        total += extra + (length * width * height);
    }

    std::cout << "total: " << total << std::endl;
    return 0;
}
