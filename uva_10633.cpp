#include <iostream>
#include <math.h>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]){
    unsigned long long number;

    cin >> number;

    while(number != 0){
        unsigned long long x = (number * 10) / 9;

        int first = 1;

        for(unsigned long long i = x-10; i < x+10; i++){
            if(i - i /10 == number){
                if(first){
                    cout << i;
                    first = 0;
                }
                else{
                    cout << " " << i;
                }
            }
        }
        cout << endl;

    
  
        cin >> number;
    }

    return 0;
}
