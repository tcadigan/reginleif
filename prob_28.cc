#include <iostream>

using namespace std;

int main(){

    int sum = 1;
    int factor = 2;
    int pval = 1;
    int nval;
    for(int i = 1; i <= 1000; i++){
        nval = pval+factor;
        /*    if( i % 2 == 1){
              cout << "br ";
              }
              else{
              cout << "tl ";
              }
              cout << nval << endl;
        */
        sum += nval;

        factor += 2;
        pval = nval;
    }
    
    pval = 1;
    factor = 4;
    for(int i = 1; i <= 500; i++){
        nval = pval+factor;
        //cout <<  "bl " << nval << endl;

        sum += nval;
        nval = nval+factor;
        //cout << "tr " << nval << endl;

        sum += nval;

        factor += 4;
        pval = nval;
    }

    cout << sum << endl;

    return 0;
}
