#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;

string add(string a, string b){
    string res;
    stringstream inp;
    int carry = 0;

    //cout << a << " " << b << endl;;
    for(unsigned int i = 0; i < max(a.length(),b.length()); ++i){
        int x = 0;
        if(i <= a.length()-1){
            x = a[a.length()-1-i]-'0';
        }

        int y = 0;
        if(i <= b.length()-1){
            y = b[b.length()-1-i]-'0';
        }

        inp << (x+y+carry) % 10;

        carry = (x+y+carry) / 10;
    }
    if(carry){
        inp << carry;
    }

    inp >> res;
    reverse(res.begin(),res.end());
  
    //cout << res << endl;

    return res;
}

int main(){
    string one = "1";
    string eightnine = "89";

    stringstream inp;
    string curr, next, orig;
    int total = 0;

    string sq[] = {"0","1","4","9","16","25","36","49","64","81"};
    map<string, int> seen;

    for(int i = 10000000; i > 0; --i){
        if(i % 100000 == 0){
            cout << i << endl;
        }
        inp << i;
        inp >> curr;
        inp.clear();
        orig = curr;

        while(1){
            if(curr.compare(one) == 0){
                seen[orig] = 0;
                break;
            }
            if(curr.compare(eightnine) == 0){
                //cout << i << endl;
                seen[orig] = 1;
                ++total;
                break;
            }

            next = "0";
            for(unsigned int j = 0; j < curr.length(); ++j){
                next = add(next,sq[curr[j]-'0']);
            }
            curr = next;

            if(seen.find(curr) != seen.end()){
                total += seen.find(curr)->second;
                break;
            }

            //cout << curr << endl;
        }
    }
  
    cout << total << endl;
  
    return 0;
}
