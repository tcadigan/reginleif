#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

map<string,string> table;

bool isless(string a, string b){
    if(a[0] == '-'){
        return true;
    }
    if(a.length() > b.length()){
        return false;
    }
    if(a.length() < b.length()){
        return true;
    }
    return a < b;
}

bool isdiv(string a){
    for(int i = 1; i <= 6; ++i){
        if(a[a.length()-i]-'0' != 0){
            return false;
        }
    }

    return true;
}

string add(string a, string b){
    stringstream inp;

    //cout << "begin add: " << a << " " << b << endl;
    int carry = 0;
    for(unsigned int i = 0; i < max(a.length(),b.length()); ++i){
        unsigned int x = 0;
        if(i < a.length()){
            x = a[a.length()-i-1]-'0';
        }

        unsigned int y = 0;
        if(i < b.length()){
            y = b[b.length()-i-1]-'0';
        }

        //cout << x  << " " << y << endl;
        inp << (x+y+carry) % 10;

        carry = (x+y+carry) / 10;
    }
    if(carry){
        inp << carry;
    }

    string res;
    inp >> res;

    reverse(res.begin(), res.end());

    return res;
}

string sub(string a, string b){
    string res;
  
    if(isless(a,b)){
        res = "-1";
        return res;
    }

    vector<int> top;
    stringstream inp;

    //cout << "begin: " << a << " " << b << endl;
    for(unsigned int i = 0; i < a.length(); ++i){
        top.push_back(a[i]-'0');
    }

    for(unsigned int i = 0; i < top.size(); ++i){
        int x = top[top.size()-i-1];
        int y = 0;
        if(i < b.length()){
            y = b[b.length()-i-1]-'0';
        }

        if(y > x){
            top[top.size()-i-2]--;
            x += 10;
        }

        inp << x-y;
    }

    inp >> res;

    reverse(res.begin(),res.end());
    //cout << "result: " << res << endl;
    unsigned int pos;
    for(pos = 0; res[pos]-'0' == 0 && pos < res.length()-1;++pos){}

    res = res.substr(pos);

    return res;
}

int main(){
    vector<string> pents;
    stringstream inp;
    string first;

    for(int i = 1; i < 1001; ++i){
        inp << (3*i*i-i)/2 << " ";
        i *= -1;
        inp << (3*i*i-i)/2;
        i *= -1;
        inp >> first;
        pents.push_back(first);
        inp >> first;
        pents.push_back(first);
        inp.clear();
    }

    table["0"] = "1";
    table["1"] = "1";
    //cout << "1: 1" << endl;
    string zero = "0";

    for(int i = 2; i < 100001; ++i){
        if(i % 10000 == 0){
            cout << i << endl;
        }
        inp << i;
        string stri;
        inp >> stri;
        inp.clear();

        int sign = 0;
        string sum = zero;
        for(unsigned int j = 0; j < pents.size(); ++j){
            //cout << "starting: " << stri << " " << pents[j] << endl;
            string val = sub(stri,pents[j]);
            //cout << "after sub: " << val << endl;
            if(isless(val,zero)){ 
                break;
            }
            string tableval = table.find(val)->second;
            //cout << "after lookup: " << tableval << endl;
            if(sign < 2){
                sum = add(sum,tableval);
                //cout << "after add: " << sum << endl;
            }
            else{
                sum = sub(sum,tableval);
                //cout << "after sub: " << sum << endl;
            }
            sign = (sign + 1) % 4;
        }

        if(isdiv(sum)){
            cout << stri << ": " << sum << endl;
            break;
        }
        //cout << stri << ": " << sum << endl;
        table[stri] = sum;
    }

    return 0;
}
