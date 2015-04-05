#include <iostream>
#include <sstream>

using namespace std;

int val(char x){
    int val;
    switch(x){
    case 'M':
        val = 1000;
        break;
    case 'D':
        val = 500;
        break;
    case 'C':
        val = 100;
        break;
    case 'L':
        val = 50;
        break;
    case 'X':
        val = 10;
        break;
    case 'V':
        val = 5;
        break;
    default:
        val = 1;
    }

    return val;
}

int main(){
    string num;
    int tally = 0;

    while(cin >> num){
        int total = 0;
        for(int i = num.size()-1; i >= 0; --i){
            int curr = val(num[i]);
            if(i != 0){
                int next = val(num[i-1]);
 
                if(next < curr){
                    total += (curr-next);
                    --i;
                }
                else{
                    total += curr;
                }
            }
            else{
                total += curr;
            }
        }

        cout << num << " = " << total << " = ";

        stringstream inp;
        while(total >= 1000){
            inp << "M";
            total -= 1000;
        }

        while(total >= 900){
            inp << "CM";
            total -= 900;
        }

        while(total >= 500){
            inp << "D";
            total -= 500;
        }

        while(total >= 400){
            inp << "CD";
            total -= 400;
        }

        while(total >= 100){
            inp << "C";
            total -= 100;
        }

        while(total >= 90){
            inp << "XC";
            total -= 90;
        }

        while(total >= 50){
            inp << "L";
            total -= 50;
        }

        while(total >= 40){
            inp << "XL";
            total -= 40;
        }

        while(total >= 10){
            inp << "X";
            total -= 10;
        }

        while(total >= 9){
            inp << "IX";
            total -= 9;
        }

        while(total >= 5){
            inp << "V";
            total -= 5;
        }

        while(total >= 4){
            inp << "IV";
            total -= 4;
        }

        while(total >= 1){
            inp << "I";
            total -= 1;
        }

        string mini;
        inp >> mini;

        inp.clear();

        if(num.length()-mini.length() > 0){
            tally += (num.length()-mini.length());
        }

        cout << mini << endl;
    }

    cout << tally << endl;

    return 0;
}
