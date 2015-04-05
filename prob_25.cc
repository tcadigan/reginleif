#include <iostream>
#include <math.h>

using namespace std;

int main(){

    int result = 1;
    int num = 0;
    while(result < 1000){
        num++;
        double top = num * log10((1+sqrt(5))/2);
        double bottom = log10(sqrt(5));

        result = 1 + top - bottom;
    }

    cout << num << endl;

    return 0;
}
