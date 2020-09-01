#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

int factorial(int x){
    if(x == 0){
        return 1;
    }
    else{
        return x * factorial(x-1);
    }
}

int main(){
    int result = 0;
    stringstream inp;
    string num;

    for(int i = 3; i < 100000; ++i){
        inp << i;
        inp >> num;
        int sum = 0;
        for(unsigned int j = 0; j < num.length(); ++j){
            char t = num[j];
            sum += factorial(atoi(&t));
        }
        if(sum == i){
            //cout << i << endl;
            result += sum;
        }

        inp.clear();
    }

    cout << result << endl;

    return 0;
}
