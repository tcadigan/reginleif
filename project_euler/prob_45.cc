#include <iostream>
#include <set>
#include <iomanip>

using namespace std;

int main(){
    set<double> tnums;
    set<double> pnums;
    set<double> hnums;

    for(double n = 143; n < 60143; ++n){
        tnums.insert((n*(n+1))/2);
        pnums.insert((n*(3*n-1))/2);
        hnums.insert(n*(2*n-1));
    }

    set<double>::iterator it;
    for(it = tnums.begin(); it != tnums.end(); ++it){
        if(*it != 40755 && pnums.find(*it) != pnums.end() && hnums.find(*it) != hnums.end()){
            cout << setprecision(20) << *it << endl;
            break;
        }
    }


    return 0;
}
