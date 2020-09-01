#include <iostream>
#include <cstdlib>
#include <string>
#include <limits.h>

using namespace std;

int main(int argc, char* argv[]){
    string first;
    string second;
    string third;

    cin >> first >> second >> third;
  
    while(!cin.eof()){
        cout << first << " " << second << " " << third << endl;
        double firsti = -1;
        double thirdi = -1;

        firsti = atof(first.c_str());
        if(firsti > INT_MAX){
            cout << "first number too big" << endl;
        }
        thirdi = atof(third.c_str());
        if(thirdi > INT_MAX){
            cout << "second number too big" << endl;
        }

        if(second.compare("+") == 0){
            if(firsti + thirdi > INT_MAX){
                cout << "result too big" << endl;
            }
        }
        else{
            if(firsti * thirdi > INT_MAX){
                cout << "result too big" << endl;
            }
        }
        cin >> first >> second >> third;
    }

    return 0;
}
    
