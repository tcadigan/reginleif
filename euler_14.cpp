#include <iostream>
#include <math.h>
#include <string.h>
using namespace std;

int table[500000];

unsigned long long collatz(unsigned long a, int count){
    if(a < 500000 && table[(int)a] != 0){
        return count+table[(int)a];
    }
    if(a == 1){
        return count;
    }
    if((a&1) == 1){
        return collatz(3*a+1,count+1);
    }
    else{
        return collatz(a/2,count+1);
    }
}

int main(){  
    memset(table, 0, 500000);
    unsigned long max = 1;
    unsigned long val = 1;
    for(unsigned long i = 2; i <= 1000000; i++){
        unsigned long res = collatz(i,0);
        if(i < 500000 && table[(int)i] == 0){
            table[(int)i] = res;
        }
        if(res > max){
            max = res;
            val = i;
        }
    }

    cout << val << endl;

    return 0;
}
