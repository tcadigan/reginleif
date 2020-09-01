#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

bool isprime(int x){
    for(int i = 2; i <= sqrt(x); ++i){
        if(x % i == 0){
            return false;
        }
    }
    return true;
}

int main(){
    vector<int> tsnums;

    for(int i = 1; i < 1001; ++i){
        tsnums.push_back(2*i*i);
    }

    vector<int> primes;
    vector<int>::iterator it;

    bool result;
    for(int i = 3; i < 10003; i += 2){
        result = false;
        if(isprime(i)){
            primes.push_back(i);
            result = true;
        }
        else{
            for(int j = 0; j < 1001; j++){
                for(it = primes.begin(); it != primes.end(); ++it){
                    //cout << i << " " << tsnums[j] << " " << *it << endl;
                    if(tsnums[j] + *it == i){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
            if(!result){
                cout << i << endl;
                break;
            }
        }
    }

    return 0;
}
