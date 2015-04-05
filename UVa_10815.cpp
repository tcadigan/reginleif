#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>

using namespace std;

int main(){
    string input;
    vector<string> dict;

    cin >> input;

    while(!cin.fail()){
        for(int i = 0; i < (int)input.length(); i++){
            input[i] = tolower(input[i]);
        }

        int prev = 0;
        for(int i = 0; i < (int)input.length(); i++){
            if(!isalpha(input[i])){
                string temp = input.substr(prev,i-prev);
                dict.push_back(temp);
                prev = i+1;
            }
        }
        string temp = input.substr(prev,input.length());

        dict.push_back(temp);

        cin >> input;
    }

    sort(dict.begin(), dict.end());

    string prev = "";
    string curr;
    for(unsigned int i = 0; i < dict.size(); i++){
        curr = dict[i];
        if(curr != prev){
            cout << curr << endl;
            prev = curr;
        }
    }

    return 0;
}
