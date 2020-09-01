#include <iostream>

using namespace std;

int p(int k, int n){
    if(k > n){
        return 0;
    }
    if(k == n){
        return 1;
    }
    return p(k+1,n) + p(k,n-k);
}

int main(){

    cout << p(1,100)-1 << endl;

    return 0;
}
