#include <iostream>

int main(int argc, char* argv[])
{
    int length;
    int width;
    int height;
    char delim;
    int total = 0;

    while (std::cin >> length >> delim >> width >> delim >> height) {
        int extra = length * width;

        if ((length * height) < extra) {
            extra = length * height;
        }

        if ((width * height) < extra) {
            extra = width * height;
        }

        total += extra + (2 * length * width) + (2 * width * height) + (2 * height * length);
    }

    std::cout << "total: " << total << std::endl;
    return 0;
}
