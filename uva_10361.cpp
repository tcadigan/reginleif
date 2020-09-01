#include <iostream>
#include <string>

using namespace std;

int main(){
    int tests;
    string junk;

    cin >> tests;

    getline(cin, junk);

    while(tests > 0){
        string input;
        string input_2;

        getline(cin, input);
        getline(cin, input_2);

        //cout << input << ", " << input_2 << endl;

        int first = input.find('<',0);
        int second = input.find('>',0);
        int third = input.find('<',first+1);
        int fourth = input.find('>',second+1);

        //cout << first << ", " << second << ", " << third << ", " << fourth << endl;

        string s_1 = input.substr(0,first);
        string s_2 = input.substr(first+1,second-first-1);
        string s_3 = input.substr(second+1,third-second-1);
        string s_4 = input.substr(third+1,fourth-third-1);
        string s_5 = input.substr(fourth+1,input.length());

        //cout << s_1 << ", " << s_2 << ", " << s_3 << ", " << s_4 << ", " << s_5 << endl;

        int loc = input_2.find("...",0);

        string new_input_2 = input_2.substr(0,loc);
        string new_input = s_1 + s_2 + s_3 + s_4 + s_5;

        new_input_2 += s_4 + s_3 + s_2 + s_5;

        cout << new_input << endl << new_input_2 << endl;

        tests--;
    }
    return 0;
}
