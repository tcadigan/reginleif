#include <iostream>
#include <sstream>

using namespace std;

bool isAllOdd(int x){
    int digit;
    while(x != 0){
        digit = x % 10;
        x /= 10;
        if(digit % 2 != 1){
            return false;
        }
    }

    return true;
}

int main(){
    int count = 0;
    for(int i = 1; i < 100000001; ++i){
        if(i % 10 == 0){
            continue;
        }
        int copy = i;
        int reverse = 0;

        while(copy != 0){
            reverse = reverse * 10 + (copy % 10);
            copy /= 10;
        }

        int result = i + reverse;
        //cout << i << " " << reverse << " " << result << endl;
        if(isAllOdd(result)){
            ++count;
        }
    }
    
    cout << count << endl;
    return 0;
}
