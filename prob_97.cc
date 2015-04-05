#include <iostream>

using namespace std;

int main(){
    long long int res = 28433;
    for(int i = 1; i <= 7830457; ++i){
        res  *= 2;
        res %= 10000000000;
    }

    ++res;
    res %= 10000000000;

    cout << res << endl;
}
