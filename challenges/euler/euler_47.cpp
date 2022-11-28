#include <iostream>
#include <set>
#include <math.h>
#include <vector>

using namespace std;

vector<int> primes;

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

void genprimes(int x){
    int count = 0;
    primes.push_back(2);
    count++;
    for(int i = 3; count <= x; i += 2){
        if(isprime(i)){
            primes.push_back(i);
            count++;
        }
    }
}

set<double> factor(double x){
    set<double> factors;
    vector<int>::iterator it;

    for(it = primes.begin(); it != primes.end() && x != 1; ++it){
        if(fmod(x,*it) == 0){
            factors.insert(*it);
            x = x / *it;
            --it;
        }
    }

    int i = *(primes.end()-1)+2;
    while(x != 1){
        if(fmod(x,i) == 0){
            factors.insert(i);
            x = x/i;
            i-=2;
        }
        i+=2;
    }

    return factors;
}

int main(){
    set<double> result,result2,result3,result4;
    set<double>::iterator it;

    genprimes(100000);
    //cout << "gen primes done" << endl;

    double i = 2;
    result = factor(i);
    result2 = factor(i+1);
    result3 = factor(i+2);
    result4 = factor(i+3);

    while(result.size() != 4 || result2.size() != 4 || result3.size() != 4 || result4.size() != 4){
        ++i;
        result = result2;
        result2 = result3;
        result3 = result4;
        result4 = factor(i+3);

        /*
          if(fmod(i,10000) == 0){
          cout << i << endl;
          }*/
    }

    cout << i << endl;

    return 0;
}
