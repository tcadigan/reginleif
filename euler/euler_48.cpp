#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

int main(){
    double sum = 0;

    for(int i = 1; i <= 1000; ++i){
        double temp = 1;
        for(int j = 1; j <= i; ++j){
            temp *= i;
            temp = fmod(temp,10000000000);
        }
        sum += temp;
        sum = fmod(sum,10000000000);
    }

    cout << setw(10) << setfill('0') << setprecision(10) << sum << endl;
}
