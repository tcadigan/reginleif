#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

bool prime(int x){
    //cout << "prime: " << x << endl;
    for(int i = 2; i <= sqrt(x); i++){
        if(x % i == 0){
            return false;
        }
    }
    return true;
}

int main(){
    stringstream inp;
    int total = 1;
    for(int i = 3; i < 1000001; i += 2){
        inp << i;
        string num;
        inp >> num;

        bool circular = true;
        for(unsigned int j = 0; j < num.length(); ++j){
            circular = prime(atoi(num.data()));
            if(!circular){
                break;
            }
            string nnum = "";
            for(unsigned int k = 1; k < num.length(); ++k){
                nnum += num[k];
            }
            nnum += num[0];
            num = nnum;
        }
        if(circular){
            //cout << i << endl;
            total++;
        }
        inp.clear();
    }
  
    cout << total << endl;

    return 0;
}
