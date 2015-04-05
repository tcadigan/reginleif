#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

int main(){
    string a;
    vector<int> res;
    cin >> a;

    for(int i = 0; i < 200; i++){
        res.push_back(0);
    }

    while(!cin.eof()){
        int carry = 0;
        int i,j;
        for(i = a.size()-1, j = 0; i >= 0; i--,j++){
            char num1 = a[i];
            //cout << atoi(&num1) << " " << res[j] << endl;
            res[j] += atoi(&num1) + carry;
            if(res[j] >= 100){
                cout << "oops" << endl;
            }
            if(res[j] >= 10){
                carry = res[j] / 10;
                res[j] = res[j] % 10;
            }
            else{
                carry = 0;
            }
        }
        while(carry){
            res[j] += carry;
            if(res[j] >= 10){
                carry = res[j] / 10;
                res[j] = res[j] % 10;
            }
            else{
                carry = 0;
            }
            j++;
        }

        cin >> a;
    }
    /*
      for(int i = 0; i < 200; i++){
      cout << res[i];
      }
      cout << endl;
    */
    int count = 0;
    int flag = 0;
    for(int i = 199; i >= 0; i--){
        if(res[i] != 0){
            flag = 1;
        }
        if(flag){
            cout << res[i];
            count++;
            if(count == 10){
                break;
            }
        }
    }
    cout << endl;

    return 0;
}
