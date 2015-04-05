#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>
#include <algorithm>

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

bool ispermutation(int x, int y){
    stringstream inp;
    inp << x << " " << y;
    string a, b;
    inp >> a >> b;
  
    sort(a.begin(),a.end());

    do{
        if(b.compare(a) == 0){
            return true;
        }
    }while(next_permutation(a.begin(),a.end()));

    return false;
}

int main(){
    vector<int> primes;

    for(int i = 1000; i < 10000; ++i){
        if(isprime(i)){
            primes.push_back(i);
        }
    }

    vector<int>::iterator it;
    vector<int>::iterator it2;

    for(it = primes.begin(); it != primes.end(); ++it){
        for(it2 = it+1; it2 != primes.end();++it2){
            if(ispermutation(*it,*it2)){
                if(isprime((*it2)+(*it2-*it)) && ispermutation(*it,(*it2)+(*it2-*it))){
                    if(*it != 1487){
                        cout << *it << *it2 << *it2+(*it2-*it) << endl;
                    }
                }
            }
        }	
    }
  
    return 0;
}
