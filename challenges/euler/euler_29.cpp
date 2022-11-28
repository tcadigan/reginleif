#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include <set>
#include <sstream>
#include <iomanip>

using namespace std;

vector<int> primes;
int size = 100;

void genprimes(){
    for(int i = 2; i <= size; i++){
        bool isprime = true;
        for(int j = 2; j <= sqrt(i); j++){
            if(i % j == 0){
                isprime = false;
                break;
            }
        }
        if(isprime){
            primes.push_back(i);
        }
    }
}

int main(){
    genprimes();

    vector<int>::iterator it;
    set<string> results;

    /*
      for(it = primes.begin(); it != primes.end(); it++){
      cout << *it << endl;
      }
    */

    int factors[primes.size()];
  
    for(int i = 2; i <= size; i++){
        int num = i;
        memset(factors, 0, sizeof(factors));

        int j;
        for(it = primes.begin(), j = 0; it != primes.end(); it++, j++){
            while(num % *it == 0){
                factors[j]++;
                num /= *it;
            }
            if(num == 1){
                break;
            }
        }

        /*
          cout << i << ": ";
          for(int j = 0; (unsigned) j < primes.size(); j++){
          cout << factors[j];
          }
          cout << endl;
        */

        stringstream inp;
        string x;
        for(j=2; j <= size; j++){
            for(int k = 0; (unsigned)k < primes.size(); k++){
                inp << setw(3) << setfill('0') << factors[k] * j;
            }
            inp >> x;
            results.insert(x);
            inp.clear();
        } 
    }

    cout << results.size() << endl;

    return 0;
}
