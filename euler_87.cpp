#include <iostream>
#include <cmath>
#include <vector>
#include <set>

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

    for(int i = 3; i < 7072; i += 2){
        if(isprime(i)){
            primes.push_back(i);
        }
    }

    vector<int>::iterator it1;
    vector<int>::iterator it2;
    vector<int>::iterator it3;

    set<int> seen;

    int total = 0;
    for(it1 = primes.begin(); it1 != primes.end(); ++it1){
        for(it2 = primes.begin(); it2 != primes.end(); ++it2){
            if((*it1) * (*it1) + (*it2) * (*it2) * (*it2) >= 50000000){
                break;
            }
            for(it3 = primes.begin(); it3 != primes.end(); ++it3){
                total = (*it1) * (*it1) + (*it2) * (*it2) * (*it2) + (*it3) * (*it3) * (*it3) * (*it3);
                if(total >= 50000000){
                    break;
                }
                if(seen.count(total) == 0){
                    seen.insert(total);
                }
            }
        }
    }

    cout << seen.size() << endl;

    return 0;
}
