#include <sstream>
#include <iostream>

using namespace std;

int main(){
    long first, second;

    cin >> first >> second;

    while(1){
        if(first == 0 && second == 0){
            break;
        }

        stringstream inp;
        inp << first << " " << second;
        string firstword, secondword;
        inp >> firstword >> secondword;
        inp.clear();

        int length = max(firstword.length(),secondword.length());

        int carrynum = 0;
        int carry = 0;

        for(int i = 0; i < length; ++i){
            //cout << "debug: " << first % 10 + second % 10 + carry << endl;
            if((first % 10 + second % 10 + carry)>= 10){
                carrynum++;
            }
            carry = (first % 10 + second % 10 + carry) / 10;
            first /= 10;
            second /= 10;
        }
    
        if(carrynum == 0){

            cout << "No carry operation." << endl;
        }
        else if(carrynum > 1){
            cout << carrynum << " carry operations." << endl;
        }
        else{
            cout << carrynum << " carry operation." << endl;
        }

        cin >> first >> second;
    }

    return 0;
}
