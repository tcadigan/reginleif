#include <iostream>
#include <iomanip>

using namespace std;

int main(){
    double sum_square = 0;
    double square_sum = 0;
    for(int i =1 ; i <= 100; i++){
        sum_square = sum_square + i * i;
        square_sum = square_sum + i;
    }

    square_sum = square_sum*square_sum;

    cout << setprecision(20) << square_sum -sum_square << endl;

    return 0;
}
