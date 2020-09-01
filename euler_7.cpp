#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

bool isp(int num){
    for(int i = 2 ; i <= sqrt(num); i++){
        if(num % i == 0){
            return false;
        }
    }

    return true;
}

int main(){
    int num = 1;
    int i;
    for(i = 3; ; i+= 2){
        if(isp(i)){
            num++;
            if(num == 10001){
                cout << setprecision(20) << i << endl;
                break;
            }
        }
    }
 

    return 0;
}
