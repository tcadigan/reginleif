#include<iostream>
#include<vector>
#include <math.h>

using namespace std;

bool isprime(int x){
    if(x % 2 == 0){
        return false;
    }

    for(int i = 3; i <= sqrt(x); i += 2){
        if(x % i == 0){
            return false;
        }
    }

    return true;
}

vector<int> primes;

int count(int n, int m){
    if(n == 0){
        return 1;
    }
    if(n < 0 || (m < 0 && n >= 1)){
        return 0;
    }

    return count(n,m-1) + count(n-primes[m],m);

}

int main(){
    primes.push_back(2);

    for(int i = 3; i < 100; i += 2){
        if(isprime(i)){
            primes.push_back(i);
        }
    }

    for(int i = 0; i <= 100; ++i){
        if(count(i,primes.size()-1) > 5000){
            cout << i << endl;
            break;
        }
    }

    return 0;
}
