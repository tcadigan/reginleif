#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

bool prime(int a){
    for(int i = 2; i <= sqrt(a); i++){
        if(a % i == 0){
            return false;
        }
    }

    return true;
  
}

int main(){
    double sum = 2;
    for(int i = 3; i <= 2000000; i += 2){
        if(prime(i)){
            //cout << i << endl;
            sum += i;
        }
    }

    cout << setprecision(30) << sum << endl;

    return 0;
}
