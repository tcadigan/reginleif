#include <iostream>
#include <iomanip>

using namespace std;

double fib(){
    double sum = 0;
    int prev = 0;
    int curr = 1;
    int next;
 top:
    next = prev + curr;
    if(next > 4000000){
        goto end;
    }
    if(next % 2 == 0){
        sum += next;
    }
    prev = curr;
    curr = next;
    goto top;

 end:
    return sum;
}

int main(){
    cout << setprecision(30) << fib() << endl;
    return 0;
}
