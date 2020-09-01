#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char *arg[])
{
    while(!cin.eof()) {
        string input;

        cin >> input;
        
        if(input == "#") {
            break;
        }
        else {
            if(next_permutation(input.begin(), input.end())) {
                cout << input << endl;
            }
            else {
                cout << "No Successor" << endl;
            }
        }
    }

    return 0;
}
