#include <iostream>
#include <math.h>

using namespace std;

bool isprime(int n){
    if(n <= 0){
        return false;
    }
    for(int i = 2; i <= sqrt(n); i++){
        if(n % i == 0){
            return false;
        }
    }

    return true;
}

int main(){

    int maxn = 0;
    int prod = 0;

    for(int i = -999; i <= 999; i++){
        for(int j = -999; j <= 999;j++){
            int n = 0;
            while(isprime(n*n+i*n+j)){
                n++;
            }
            n--;
            if(n > maxn){
                maxn = n;
                prod = i * j;
                //cout << i << " " << j << " " << n << endl;
            }
        }
    }
      
    cout << prod << endl;

    return 0;
}
