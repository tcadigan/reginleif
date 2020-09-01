#include <iostream>

using namespace std;



int main(){
    int result = 0;
    for(int i = 1; i < 101; ++i){
        for(int j = 0; j <= i; ++j){
            double top = 1;
            for(int k = i-j+1; k <= i; ++k){
                top *= k;
            }

            double bottom = 1;
            for(int k = 2; k <= j; ++k){
                bottom *= k;
            }

            if(top / bottom > 1000000){
                result += i-2*j+1;
                break;
            }
        }
    }

    cout << result << endl;

    return 0;
}
