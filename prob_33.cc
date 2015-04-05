#include <iostream>
#include <math.h>

using namespace std;

int gcd(double a, double b){
    if(a == 0){
        return a;
    }
    while(b != 0){
        if(a > b){
            a -= b;
        }
        else{
            b -= a;
        }
    }
    return a;
}

int main(){
    double res1;
    double num = 1;
    double denom = 1;

    for(double i = 1; i < 101; ++i){
        for(double j = i+1; j < 100; ++j){
            res1 = i / j;

            int chopt = fmod(i,10);
            int chopb = (j - fmod(j,10)) / 10;

            double top = (i - fmod(i,10)) / 10;
            double bot = fmod(j,10);

            if(bot && chopt == chopb && top / bot == res1){
                //cout << i << " " << j << " " << top << " " << bot << endl;
                num *= i;
                denom *= j;
            }
        }
    }

    int g = gcd(num,denom);
    cout << denom /g << endl;

    return 0;
}
