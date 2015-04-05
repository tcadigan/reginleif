#include <iostream>
#include <math.h>

#define NUM 600851475143

using namespace std;

int main(){
    double val = NUM;
    double largest = 1;
    for(double i = 2; i < sqrt(NUM); i++){
        if(fmod(val,i) == 0){
            val = val / i;
            //cout << i << " ";
            if(i > largest){
                largest = i;
            }
            i--;
        }
    }
    if(val > largest){
        largest = val;
    }
    cout << largest << endl;

    return 0;
}
