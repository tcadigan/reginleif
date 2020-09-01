#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(){
    long number1, number2, result;

    while(scanf("%ld %ld", &number1, &number2) == 2){
        result = number2 - number1;
        if(result < 0){
            result = number1 - number2;
        }
        printf("%ld\n", result);
    }
    return 0;
}
