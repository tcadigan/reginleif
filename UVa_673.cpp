#include <iostream>
#include <stack>

using namespace std;

int main(int argc, char* argv[]){
    int num;

    cin >> num;
    string input;
    getline(cin, input);

  
    while(num > 0){
        stack<char> stk;
    
        getline(cin, input);
        //cout << "\"" << input << "\"" << endl;
        bool fail = false;
        for(unsigned int i = 0; i < input.length(); ++i){
            char x = input[i];
            //cout << i << ": " << x << endl;
            switch(x){
            case '(':
            case '[':
                stk.push(x);
                break;
            case ')':
                if(!stk.empty() && stk.top() == '('){
                    stk.pop();
                }
                else{
                    fail = true;
                }
                break;
            case ']':
                if(!stk.empty() && stk.top() == '['){
                    stk.pop();
                }
                else{
                    fail = true;
                }
                break;
            }
            if(fail){
                break;
            }
        }

        if(fail || !stk.empty()){
            cout << "No" << endl;
        }
        else{
            cout << "Yes" << endl;
        }

        --num;
    }


    return 0;
}
