#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <prefix> <start> <end>" << std::endl;
        return 1;
    }

    std::string prefix(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);

    std::cout << "Running with \"" << prefix << "\" from "
              << start << " to " << end << std::endl;

    std::string cmd = "md5sum test.txt";

    for (int i = start; i <= end; ++i) {
        std::ofstream output("test.txt");

        output << prefix << i;
        output.close();

        FILE *fd = popen(cmd.c_str(), "r");

        while (!feof(fd)) {
            char buffer[200];
            if (fgets(buffer, 200, fd) == NULL) {
                break;
            }

            std::string str(buffer);
            str.erase(str.length() - 1);
            std::size_t pos = str.find_first_not_of("0");

            if (pos >= 5) {
                std::cout << i << " " << prefix << i << " "
                          << str << " " << pos << std::endl;
                break;
            }
        }

        pclose(fd);

        if (i % 10000 == 0) {
            std::cout << "Checked through " << i << std::endl;
        }
    }

    return 0;
}
