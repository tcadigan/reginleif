#include <iostream>
#include <vector>

using namespace std;

bool check(int num_regions, int m) {
    vector<int> regions;

    for(int i = 0; i < num_regions; ++i) {
        regions.push_back(i);
    }

    int pos = 0;

    regions.erase(regions.begin() + pos);

    while(regions.size() > 1) {
        pos = (pos + m) % regions.size();

        if(regions.at(pos) == 13) {
            return false;
        }

        regions.erase(regions.begin() + pos);
        --pos;
    }

    return true;
}

int main(int argc, char *argv[])
{
    while(!cin.eof()) {
        int input;

        cin >> input;

        if(input == 0) {
            break;
        }
        else {
            int i = 1;
            bool result = check(input, i);

            while(!result) {
                ++i;
                result = check(input, i);
            }

            cout << i << endl;
        }
    }
    
    return 0;
}
