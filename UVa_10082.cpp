#include<iostream>
#include<map>

using namespace std;

void init(map<char,char> *convert){
    (*convert)['2'] = '1';
    (*convert)['3'] = '2';
    (*convert)['4'] = '3';
    (*convert)['5'] = '4';
    (*convert)['6'] = '5';
    (*convert)['7'] = '6';
    (*convert)['8'] = '7';
    (*convert)['9'] = '8';
    (*convert)['0'] = '9';
    (*convert)['-'] = '0';
    (*convert)['='] = '-';

    (*convert)['W'] = 'Q';
    (*convert)['E'] = 'W';
    (*convert)['R'] = 'E';
    (*convert)['T'] = 'R';
    (*convert)['Y'] = 'T';
    (*convert)['U'] = 'Y';
    (*convert)['I'] = 'U';
    (*convert)['O'] = 'I';
    (*convert)['P'] = 'O';
    (*convert)['['] = 'P';
    (*convert)[']'] = '[';
    (*convert)['\\'] = ']';

    (*convert)['S'] = 'A';
    (*convert)['D'] = 'S';
    (*convert)['F'] = 'D';
    (*convert)['G'] = 'F';
    (*convert)['H'] = 'G';
    (*convert)['J'] = 'H';
    (*convert)['K'] = 'J';
    (*convert)['L'] = 'K';
    (*convert)[';'] = 'L';
    (*convert)['\''] = ';';

    (*convert)['X'] = 'Z';
    (*convert)['C'] = 'X';
    (*convert)['V'] = 'C';
    (*convert)['B'] = 'V';
    (*convert)['N'] = 'B';
    (*convert)['M'] = 'N';
    (*convert)[','] = 'M';
    (*convert)['.'] = ',';
    (*convert)['/'] = '.';

    (*convert)[' '] = ' ';
}

int main(){
    map<char, char> convert;
    string input;

    init(&convert);

    while(getline(cin, input)){    
        for(unsigned int x = 0; x < input.size(); ++x){
            cout << convert[input[x]];
        }
        cout << endl;
    }
    return 0;
}
