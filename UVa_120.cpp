#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

int find(vector<int> &stack, int key){
    unsigned int i = 0;
    for(i = 0; i < stack.size()-1; ++i){
        if(stack[i] == key){
            break;
        }
    }

    return i;
}

int main(){
    string line;

    while(getline(cin, line)){
        stringstream inp;
        inp << line;
    
        int num;
        vector<int> stack;
        vector<int> seq;
        bool first = true;
        while(inp >> num){
            stack.push_back(num);
            seq.push_back(num);
            if(!first){
                cout << " ";
            }
            cout << num;
            if(first){
                first = false;
            }
        }
        cout << endl;
    
        sort(seq.begin(), seq.end()); 
        reverse(seq.begin(), seq.end());
        reverse(stack.begin(), stack.end());

        unsigned int loc;
        first = true;
        for(unsigned int i = 0; i < seq.size(); ++i){

            int sorted = true;
            for(unsigned int j = 0; j < seq.size(); ++j){
                if(seq[j] != stack[j]){
                    sorted = false;
                    break;
                }
            }

            if(sorted){
                if(first){
                    cout << 0 << endl;
                }
                else{
                    cout << " " << 0 << endl;
                }
                break;
            }

            loc = find(stack, seq[i]);
            if(loc != stack.size()-1){
                reverse(stack.begin()+loc, stack.end());
                if(first){
                    cout << loc + 1;
                    first = false;
                }
                else{
                    cout << " " << loc+1 ;
                }
            }
            reverse(stack.begin()+i, stack.end());

            if(first){
                cout << i+1;
                first = false;
            }
            else{
                cout << " " << i+1;
            }

            // for(unsigned int i = 0; i < stack.size(); ++i){
            // 	cout << " " << stack[i];
            //  }
            //  cout << endl;
        }


        inp.clear();

    }

    return 0;
}
