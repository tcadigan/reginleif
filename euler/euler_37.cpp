#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

bool isprime(int x){
    if(x == 1){
        return false;
    }
    for(int i = 2; i <= sqrt(x); ++i){
        if(x % i == 0){
            return false;
        }
    }
    return true;
}

bool sright(string x){
    bool result = true;
    string temp = x;
    for(unsigned int i = 0; i < x.length(); ++i){
        result = isprime(atoi(temp.data()));

        if(!result){
            break;
        }
    
        string s = temp.substr(1,temp.length());
        temp = s;
    }
    return result;
}

bool sleft(string x){
    bool result = true;
    string temp = x;
    for(unsigned int i = 0; i < x.length(); ++i){
        result = isprime(atoi(temp.data()));

        if(!result){
            break;
        }
    
        string s = temp.substr(0,temp.length()-1);
        temp = s;
    }
    return result;
}

int main(){
    stringstream inp;
    string number;
    int sum = 0;

    for(int i = 11; i < 1000001; i += 2){
        inp << i;
        inp >> number;
        if(sright(number) && sleft(number)){
            //cout << i << endl;
            sum += i;
        }

        inp.clear();
    }

    cout << sum << endl;

    return 0;
}
