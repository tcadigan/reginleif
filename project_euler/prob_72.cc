#include <iostream>
#include <set>
#include <map>
#include <math.h>
#include <iomanip>
using namespace std;

map<int, set<int> > divlist;

set<int> fact(int x){
    int a = x;

    set<int> factors;

    while(a % 2 == 0){
        a /= 2;
        if(divlist.find(a) != divlist.end()){
            factors = divlist.find(a)->second;
            a = 1;
        }
        factors.insert(2);
    }

    for(int i = 3; a != 1 && i <= sqrt(x); i += 2){
        while(a % i == 0){
            a /= i;
            if(divlist.find(a) != divlist.end()){
                factors = divlist.find(a)->second;
                a = 1;
            }
            factors.insert(i);
        }
    }

    if(a != 1){
        factors.insert(a);
    }

    return factors;
}

int main(){
    double sum = 0;
    set<int> res;
    set<int>::iterator it;
    double phi;
    for(int i = 2; i <= 1000000; ++i){
        res = fact(i);
        divlist[i] = res;

        phi = i;
        for(it = res.begin(); it != res.end(); ++it){
            phi *= (1-(1/(double)*it));
        }
    
        sum += phi;
    }

    cout << setprecision(20) << sum << endl;

    return 0;
}
