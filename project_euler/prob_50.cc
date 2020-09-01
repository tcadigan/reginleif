#include <iostream>
#include <vector>
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

int main(){
    vector<int> primes;
    primes.push_back(2);
    for(int i = 3; i < 1000000; ++i){
        if(isprime(i)){
            primes.push_back(i);
        }
    }

    int sum;
    for(int k = 550; k >= 2; --k){
        for(unsigned int i = 0; i < primes.size()-k; ++i){
            sum = 0;
            for(int j = 0; j < k; ++j){
                if(i+j < primes.size()){
                    sum += primes[i+j];
                }
            }
            if(isprime(sum) && sum <= 1000000){
                /*
                  cout << k << ": " << sum << endl;
                  for(int j = 0; j < k; ++j){
                  cout << primes[i+j] << " ";
                  }
                  cout << endl;
                */
                cout << sum << endl;
                return 0;
            }
        }
    }

    return 0;
}
