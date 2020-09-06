#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    std::vector<std::vector<int>> lights;

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 1000; ++j) {
            if (j == 0) {
                lights.push_back(std::vector<int>());
            }

            lights[i].push_back(0);
        }
    }

    char first;
    char second;

    while (std::cin >> first >> second) {
        std::string rest;
        std::string cmd;
        int x1;
        char delim;
        int y1;
        std::string through;
        int x2;
        int y2;

        if (second == 'o') {
            std::cin >> rest >> x1 >> delim >> y1 >> through >> x2 >> delim >> y2;
        } else {
            std::cin >> rest >> cmd >> x1 >> delim >> y1 >> through >> x2 >> delim >> y2;
        }

        for (int i = x1 < x2 ? x1 : x2; i <= (x1 > x2 ? x1 : x2); ++i) {
            for (int j = y1 < y2 ? y1 : y2; j <= (y1 > y2 ? y1 : y2); ++j) {
                if (!cmd.empty() && cmd == "off") {
                    if (lights[i][j] > 0) {
                        --lights[i][j];
                    }
                } else if (!cmd.empty() && cmd == "on") {
                    ++lights[i][j];
                } else {
                    lights[i][j] += 2;
                }
            }
        }
    }

    int count = 0;
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 1000; ++j) {
            count += lights[i][j];
        }
    }

    std::cout << "brightness: " << count << std::endl;
}
