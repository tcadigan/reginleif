#include <iostream>

using namespace std;

int main(){
    for(int i = 1; i <= 500; i++){
        int sq1 = i*i;
        for(int j = 1; j <= 500; j++){
            int sq2 = j*j;
            for(int k = 1; k <= 500; k++){
                if(sq1 + sq2 == k*k && i + j + k == 1000){
                    cout << i*j*k << endl;
                    goto end;
                }
            }
        }
    }


 end:
    return 0;
}
