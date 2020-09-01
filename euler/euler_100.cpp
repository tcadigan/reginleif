#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int main(){

    // (b/n)*(b-1/n-1)
    // (b^2-b)/(n^2-n) = 1/2
    // 2*b^2-2*b = n^2-n
    // 2*b^2-2*b-n^2+n = 0
    // 2*b^2-2*b-(b+r)^2+b+r = 0
    // 2*b^2-2*b-(b^2+2br+r^2)+b+r = 0
    // 2*b^2-2*b-b^2-2br-r^2+b+r= 0 
    // b^2-b-2br-r^2+r = 0

    unsigned long long i,j,result, red, blue;

    i = 0;
    j = 0;
    result = 0;
    red = 9228778026;
    blue = 22280241075;
    while(result < pow(10,12)){
        for(i = red * 58284272 / 10000000; i > red * 58284271 / 10000000; --i){
            for(j = blue * 58284272 / 10000000; j > blue * 58284271 / 10000000; --j){
                if(!(j*j-j-2*j*i-i*i+i)){
                    result = i + j;
                    red = i;
                    blue = j;
                    goto done;
                }
            }
        }
    done:
        cout << "total: " << result << " red: " << i << " blue: " << j << endl;
    }

    return 0;
}
