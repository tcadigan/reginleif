#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

bool ispan(string x){
    if(x.find('0') != string::npos){
        return false;
    }
    for(char i = '1'; i <= '9'; ++i){
        if(x.find(i) != x.rfind(i) || x.find(i) == string::npos){
            return false;
        }
    }

    return true;
}

int main(){
    int num, max = 0;
    string res, temp;
    stringstream inp;

    for(int i = 1; i < 100000; ++i){
        res = "";
        for(int j = 1; res.length() < 9; ++j){
            inp << i*j;
            inp >> temp;
            res += temp;
            //cout << "res: " << res << endl;
            inp.clear();
        }
        if(ispan(res)){
            //cout << res << " at " << i << endl;
            num = atoi(res.data());
            if(num > max){
                max = num;
            }
        }
    }

    cout << max << endl;
    return 0;
}
