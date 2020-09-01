#include <iostream>
#include <iomanip>
#include <string>
#include <ctype.h>
#include <sstream>

using namespace std;

int main(){
    int tests;
    string junk;
    int number = 1;

    cin >> tests;
    getline(cin, junk);

    while(tests > 0){
        string input;
        double p_val = -1;
        double u_val = -1;
        double i_val = -1; 
        int offset = 1;
        double mult = 1;
        stringstream value;
        stringstream value_2;

        getline(cin, input);

        int first = input.find('=',0);
        int second = input.find('=',first+1);

        switch(input[first-1]){
        case 'P':
            while(isdigit(input[first+offset]) || input[first+offset] == '.'){
                offset++;
            }
            switch(input[first+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
            value << input.substr(first+1,offset-1);
            value >> p_val;
            p_val *= mult;
            break;
        case 'U':
            while(isdigit(input[first+offset]) || input[first+offset] == '.'){
                offset++;
            }
            switch(input[first+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
            value << input.substr(first+1,offset-1);
            value >> u_val;
            u_val *= mult;
            break;
        case 'I':
            while(isdigit(input[first+offset]) || input[first+offset] == '.'){
                offset++;
            }
            switch(input[first+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
            value << input.substr(first+1,offset-1);
            value >> i_val;
            i_val *= mult;
            break;
        }

        offset = 1;
        mult = 1;

        switch(input[second-1]){
        case 'P':
            while(isdigit(input[second+offset]) || input[second+offset] == '.'){
                offset++;
            }
            switch(input[second+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
            value_2 << input.substr(second+1,offset-1);
            value_2 >> p_val;
            p_val *= mult;
            break;
        case 'U':
            while(isdigit(input[second+offset]) || input[second+offset] == '.'){
                offset++;
            }
            switch(input[second+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
            value_2 << input.substr(second+1,offset-1);
            value_2 >> u_val;
            u_val *= mult;
            break;
        case 'I':
            while(isdigit(input[second+offset]) || input[second+offset] == '.'){
                offset++;
            }
            switch(input[second+offset]){
            case 'm':
                mult = 0.001;
                break;
            case 'k':
                mult = 1000;
                break;
            case 'M':
                mult = 1000000;
                break;
            }
 
            value_2 << input.substr(second+1,offset-1);
            value_2 >> i_val;
            i_val *= mult;
            break;
        }

        cout.setf(ios::fixed);
        //cout << setprecision(2) << "P=" << p_val << " I=" << i_val << " U=" << u_val << endl;

        cout << "Problem #" << number << endl;
        if(p_val == -1){
            p_val = u_val * i_val;
            cout << setprecision(2) << "P=" << p_val << "W" << endl;
        }
        else if(u_val == -1){
            u_val = p_val / i_val;
            cout << setprecision(2) << "U=" << u_val << "V" << endl;
        }
        else{
            i_val = p_val / u_val;
            cout << setprecision(2) << "I=" << i_val << "A" << endl;
        }

        cout << endl;

        number++;
        tests--;
    }


    return 0;
}
