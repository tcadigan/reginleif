#include <iostream>
#include <math.h>

using namespace std;

int main(){
    int d = 0;
    int max = 0;

    for(int i = 2; i < 1000; i++){
    
        int num = i;
        int alpha = 0;
        while(num % 2 == 0){
            num /= 2;
            alpha++;
        }

        int beta = 0;
        while(num % 5 == 0){
            num /= 5;
            beta++;
        }

        int x = 0;
        if(num != 1){
            x = 1;
            int sum = 10 % num;
            while(sum != 1){
                sum *= 10;
                x++;
                sum %= num;
            }
        }

        //cout << i  << " length " << x << endl;

        if(x > max){
            max = x;
            d = i;
        }
    }

    cout << d << endl;

    return 0;
}
