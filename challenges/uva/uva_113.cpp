#include <iostream>
#include <cmath>

using namespace std;

int main(){
    double n;
    double p;

    while(cin >> n >> p){
        int begin = 1;
        int end = 1000000000;
        int mid;
        while(begin <= end){
            mid = (begin+end)/2;

            double val = pow(mid, n);

            if(val == p){
                cout << mid << endl;
                break;
            }
            else if(val > p){
                end = mid - 1;
            }
            else{
                begin = mid + 1;
            }
        }
    }

    return 0;
}
