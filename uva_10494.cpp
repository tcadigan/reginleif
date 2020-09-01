#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(){
    vector<int> res;
    long long n2, curr;

    string n1, op;
    while(cin >> n1 >> op >> n2){
        curr = 0;
        for(unsigned int i = 0; i < n1.length(); ++i){
            curr = curr * 10 + (n1[i] - '0');
            res.push_back(curr / n2 + '0');
            curr %= n2;
        }
    
        if(op == "%"){
            cout << curr << endl;
        }
        else{
            if(res.size() == 1){
                cout << res[0] - '0';
            }
            else{
                unsigned int j;
                for(j = 0; j < res.size(); ++j){
                    if(res[j] != '0'){
                        break;
                    }
                }
       
                while(j < res.size()){
                    cout << res[j]-'0';
                    ++j;
                }
            }
            cout << endl;
        }
    
        res.clear();
    }
 
    return 0;
}
