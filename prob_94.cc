#include <iostream>
#include <cmath>
using namespace std;

unsigned long long newton(unsigned long long inp){
    unsigned long long x = 1;
    for(int i = 1; i < 100; ++i){
        x = .5*(x+(inp/x));
    }

    return x;
}


int main(){
    unsigned long long perimeter=0,a;
    long double temp;
    for(unsigned long long i = 2; i < 350000001; ++i){
        //for(unsigned long long i = 329874525; i < 329874526; ++i){
        a = i-1;
        if(2*i+a <= 1000000000){
            //temp = sqrt(4*i*i-a*a);
            //cout << temp << " " << 4*i*i-a*a << " " << newton(4*i*i-a*a) << endl;
            //if(ceil(temp) - floor(temp) == 0){
            temp = newton(4*i*i-a*a);
            if(temp*temp == 4*i*i-a*a){
                //cout << "here" << endl;
                temp = 0.25 * a* temp;
                if(temp - floor(temp) == 0){
                    perimeter += (2*i+a);
                    cout << "from minus: (" << i << "," << i << "," << a << ") => " << perimeter << endl;
                }
            }
        }
        a = i+1;
        if(2*i+a <= 1000000000){
            //temp = sqrt(4*i*i-a*a);
            //cout << temp << " " << 4*i*i-a*a << endl;
            //if(ceil(temp) - floor(temp) == 0){
            temp = newton(4*i*i-a*a);
            if(temp*temp == 4*i*i-a*a){
                //cout << "here" << endl;
                temp = 0.25 * a * temp;
                if(temp - floor(temp) == 0){
                    perimeter += (2*i+a);
                    cout << "from plus: (" << i << "," << i << "," << a << ") => " << perimeter << endl;
                }
            }
        }
    }

    cout << perimeter << endl;

    return 0;
}
