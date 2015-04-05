#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

int main(){
    stringstream inp; 
    string val;
    int tsum = 0;
    for(int i = 2; i < 1000000; i++){
        inp << i;
        inp >> val;
        int isum = 0;
        char temp;
        for(int j = 0; (unsigned)j < val.length(); j++){
            temp = val[j];
            isum += pow(atoi(&temp),5);
        }
        if(isum == i){
            //cout << i << endl;
            tsum += i;
        }
        inp.clear();
    }

    cout << tsum << endl;

    return 0;
}
