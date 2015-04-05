#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

int numd(double a){
    int num = 1;
    for(double i = 0; i <= sqrt(a); i++){
        if(fmod(a, i) == 0){
            num++;
        }
    }

    return 2*num;
}

int main(){
    double j = 10;
    int max = 0;
    for(double i = 55; ; i += j){
        j++;
        int res = numd(i);
        if(res > 500){
            cout << setprecision(500) << i << endl;
            break;
        }
        if(res > max){
            max = res;
            cout << "largest is: " << max << endl;
        }
        //cout << i << " " << numd(i) << endl;
    }

    return 0;
}
