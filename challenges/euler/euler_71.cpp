#include <iostream>
#include <iomanip>
using namespace std;

int main(){
    pair<int,int> best;
    best.first = 0;
    best.second = 1;

    cout << setprecision(20) << endl;
    for(int d = 1000000; d > 1; --d){
        if(d % 10000 == 0){
            cout << d << endl;
        }
        for(int n = 1; n < d; ++n){
            double res = n/(double)d;
            if(res >= 3/7.0){
                break;
            }
            if(res > best.first/(double)best.second){
                best.first = n;
                best.second = d;
                //cout << best.first << " " << best.second <<  " " << best.first/(double)best.second << endl;
            }
        }
    }

    int a = best.first;
    int b = best.second;
    int temp;
    while(b != 0){
        temp = b;
        b = a % b;
        a = temp;
    }

    cout << best.first/a << endl;

    return 0;
}
