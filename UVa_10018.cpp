#include <iostream>
#include <sstream>
using namespace std;

long long reverse(long long number){
    stringstream inp;
    long long num;
    string backwards = "";

    while(number != 0){
        backwards += (number % 10) + '0';
        number /= 10;
    }

    if(backwards == ""){
        backwards = "0";
    }

    inp << backwards;
    inp >> num;

    return num;
}

int main(){
    long long trials;

    cin >> trials;

    while(trials-- > 0){
        long long number;
        cin >> number;

        long result = 0;
        int iterations = 0;
        for(int i = 1; i < 1001; ++i){
            //cout << number << endl;
      
            number = number + reverse(number);
            if(number == reverse(number)){
                result = number;
                iterations = i;
                break;
            }
        }
    
        cout << iterations << " " << number << endl;
    }

    return 0;
}
