#include <iostream>
#include <string>
#include <set>
#include <cstdlib>

using namespace std;

string left(string s){
    string result = "";
    result += s[3];
    result += s[1];
    result += s[0];
    result += s[5];
    result += s[4];
    result += s[2]; 
    return result;
}

string down(string s){
    string result = "";
    result += s[4];
    result += s[0];
    result += s[2];
    result += s[3];
    result += s[5];
    result += s[1];
    return result;
}

string around(string s){
    string result = "";
    result += s[0];
    result += s[3];
    result += s[1];
    result += s[4];
    result += s[2];
    result += s[5];
    return result;
}

set<string> precompute(){
    string start = "123456";
    string working;

    set<string> possible;
    int num;
    for(int i = 0; i < 4; ++i){
        working = start;
        num = i;
        while(num){
            working = left(working);
            --num;
        }
    
        for(int j = 0; j < 4; ++j){
            num = j;
            while(num){
                working = down(working);
                --num;
            }
	
            for(int k = 0; k < 4; ++k){
                num = k;
                while(num){
                    working = around(working);
                    --num;
                }
	
                if(possible.find(working) == possible.end()){
                    possible.insert(working);
                }
            }
        }
    }
  
    return possible;
}

int main(){
    string input;
    set<string>  pre = precompute();
    set<string>::iterator it;

    while(cin >> input){
        string start = input.substr(0, 6);
        string target = input.substr(6, input.size());

        string working;
        bool found = false;
        string perm;
        char temp;
        for(it = pre.begin(); it != pre.end(); ++it){
            working = "";
            perm = *it;
            for(unsigned int i = 0; i < perm.size(); ++i){
                temp = perm[i]-1;
                working += start[atoi(&temp)];
            }

            if(working == target){
                found = true;
                break;
            }
        }
   
        if(found){
            cout << "TRUE" << endl;
        }
        else{
            cout << "FALSE" << endl;
        }
    }

    return 0;
}
