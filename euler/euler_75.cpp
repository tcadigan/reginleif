#include<iostream>
#include<map>
#include<math.h>

using namespace std;

bool rprime(long long x, long long y){
    long long temp;
    while(y){
        temp = y;
        y = x % y;
        x = temp;
    }

    return x == 1;
}

int main(){
    map<int,int> wire;

    // need to compute fundamentals,
    for(long long i = 1; i < 100000; ++i){
        if(i % 1500 == 0){
            cout << i << endl;
        }
        for(long long j = 1; j < i; ++j){

            int r1 = i % 2;
            int r2 = j % 2;
            if((r1 && r2) || (!r1 && !r2) || !rprime(i,j)){
                continue;
            }

            long long a = i*i-j*j;
            long long b = 2*i*j;
            long long c = i*i+j*j;

            if(a+b+c > 1500000){
                continue;
            }

            if(c > a && c > b){
                //cout << a << " " << b << " " << c << endl;
                int mult = 1;
                while(mult*(a+b+c) <= 1500000){
                    wire[mult*(a+b+c)]++;
                    ++mult;
                }
            }
        }
    }

    map<int,int>::iterator it;
    int count = 0;
    for(it = wire.begin(); it != wire.end(); ++it){
        //cout << it->first << " "<< it->second << endl;
        if(it->second == 1){
            count++;
        }
    }
  
    cout << count << endl;

    return 0;
}
