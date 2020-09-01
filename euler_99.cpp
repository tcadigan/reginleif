#include <iostream>
#include <cmath>

using namespace std;

int main(){
    int base;
    char comma;
    int power;

    int line = 1;
    long max = 0;
    int max_line;
    while(cin >> base >> comma >> power){
        long val = power * log10(base) + 1;
        if(val >= max){
            max = val;
            max_line = line;
        }

        line++;
    }
    cout << max_line << " with " << max << " digits" << endl;
    return 0;
}
