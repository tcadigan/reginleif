#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

int main(){
    for(double i = 21; ;i++){
        if(fmod(i,20) == 0 && fmod(i,19) == 0 &&
           fmod(i,18) == 0 && fmod(i,17) == 0 && 
           fmod(i,16) == 0 && fmod(i,15) == 0 &&
           fmod(i,15) == 0 && fmod(i,14) == 0 &&
           fmod(i,14) == 0 && fmod(i,13) == 0 &&
           fmod(i,12) == 0 && fmod(i,12) == 0 &&
           fmod(i,11) == 0){
            cout << setprecision(20) << i << endl;
            break;
        }
    }

    return 0;
}
