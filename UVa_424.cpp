#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int main(){
    string input;
    vector<string> list;
    unsigned int length = 0;

    cin >> input;

    while(input != "0"){
        if(input.length() > length){
            length = input.length();
        }
        list.push_back(input);
        cin >> input;
    }

    for(unsigned int i = 0; i < list.size(); i++){
        while(list[i].length() < length){
            list[i] = "0" + list[i];
        }
    }

    int carry = 0;
    int sum = 0;
    string result = "";
    stringstream temp;

    for(int i = length-1; i >=0; i--){
        sum = carry;
        carry = 0;
        for(unsigned int j = 0; j < list.size(); j++){
            string test = list[j];
            char val = test[i];
            //cout << test << ", " << val << endl;
            sum += atoi(&val);
        }
        int res = sum % 10;
        carry = (sum - res)/10;
        temp << res;
        result = temp.str() + result;
        //cout << result << endl;
        temp.str(string());
        //cout << result << " carry: " << carry << endl;
    }

    if(carry){
        temp << carry;
        result = temp.str() + result;
    }

    cout << result << endl; 
    return 0;
}
