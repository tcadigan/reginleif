#include <iostream>
#include <set>
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
    int start = 1;
    int trbl = 2;
    int tlbr = 4;
    int tr, tl, bl, br;
    set<int> primes;

    tr = start + trbl;
    trbl += 2;
    bl = tr + trbl;
    trbl += 2;
    tl = start + tlbr;
    br = tl + tlbr;
    tlbr += 4;

    if(isprime(tr)){
        primes.insert(tr);
    }
    if(isprime(bl)){
        primes.insert(bl);
    }
    if(isprime(tl)){
        primes.insert(tl);
    }
    if(isprime(br)){
        primes.insert(br);
    }

    double ratio = (double) primes.size() / (4*1+1);
    if(ratio < .1){
        cout << 2*1+1 << endl;
        return 0;
    }

    for(int i = 2; i < 100000; i++){
        //cout << tr << " " << br << " " << bl << " " << tl << " ";
        tr = bl + trbl;
        trbl +=2;
        bl = tr + trbl;
        trbl += 2;
        tl = br + tlbr;
        br = tl + tlbr;
        tlbr += 4;

        if(isprime(tr)){
            primes.insert(tr);
        }
        if(isprime(bl)){
            primes.insert(bl);
        }
        if(isprime(tl)){
            primes.insert(tl);
        }
        if(isprime(br)){
            primes.insert(br);
        }

        ratio = (double) primes.size() / (4*i+1);
        //cout << ratio << endl;
        if(ratio < .1){
            cout << 2*i+1 << endl;
            return 0;
        }
    }
  
    cout << "not found yet" << endl;
    return 0;
}
