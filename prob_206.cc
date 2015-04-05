#include <iostream>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

int main(){
    stringstream inp;
    long long number;
    double result;
    for(int a = 9; a < 10; ++a){
        for(int b = 0; b < 10; ++b){
            cout << a << " " << b << endl;
            for(int c = 0; c < 10; ++c){
                for(int d = 0; d < 10; ++d){
                    for(int e = 0; e < 10; ++e){
                        for(int f = 0; f < 10; ++f){
                            for(int g = 0; g < 10; ++g){
                                for(int h = 0; h < 10; ++h){
                                    for(int i = 0; i < 10; ++i){
                                        inp << 1 << a << 2 << b << 3 << c << 4 << d << 5 << e << 6 << f << 7 << g << 8 << h << 9 << i << 0; 
                                        inp >> number;
                                        inp.clear();
                                        result = sqrt(number);
                                        if(result - floor(result) == 0){
                                            cout << setprecision(15) << result << endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
