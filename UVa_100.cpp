#include <iostream>
#include <string.h>

using namespace std;

int list[50000];

int collatz(unsigned int i, int count){
    if(i < 50000 && list[i] != 0){
        return list[i]+count;
    }
    if(i == 1){
        count++;
        return count;
    }
    if(i % 2 == 1){
        return collatz(3*i+1,count+1);
    }
    else{
        return collatz(i/2,count+1);
    }
}

int main(){
    int i, j;

    memset(list,0,50000*sizeof(int));

    while(cin >> i >> j){
        int max = 0;
        int val;

        bool flag = false;
        if(j < i){
            int temp = i;
            i = j;
            j = temp;
            flag = true;
        }

        for(int a = i; a <= j; ++a){
            val = collatz(a,0);
            if(a < 50000 && list[a] == 0){
                list[a] = val;
            }
            if(val > max){
                max = val;
            }
        }
        if(!flag){
            cout << i << " " << j;
        }
        else{
            cout << j << " " << i;
        }
        cout << " " << max << endl;
    }
    return 0;
}
