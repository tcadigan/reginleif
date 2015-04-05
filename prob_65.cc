#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

string multiply(string x, string y){
    vector<int> res;
    res.resize(2*max(x.length(),y.length()),0);

    int offset = 0;
    for(unsigned int i = 0; i < x.length(); ++i){
        char a[2];
        a[0] = x[x.length()-i-1];
        a[1] = '\0';
        for(unsigned int j = 0; j < y.length(); ++j){
            char b[2];
            b[0] = y[y.length()-j-1];
            b[1] = '\0';
            res[res.size()-1-offset-j] += atoi(a) * atoi(b);
        }
        /*
          for(unsigned int i = 0; i < res.size(); ++i){
          cout << res[i] << " ";
          }
          cout << endl;
        */
        offset++;
    }

    int carry = 0;
    for(unsigned int i = res.size()-1; i >= 1; --i){
        res[i] += carry;
        carry = (res[i] - (res[i] % 10)) / 10;
        res[i] = res[i] % 10;
    }
    if(carry){
        res[0] = carry;
    }

    int pos = 0;
    for(unsigned int i = 0; i < res.size(); ++i){
        if(res[i] != 0){
            pos = i;
            break;
        }
    }

    stringstream inp;
    for(unsigned int i = pos; i < res.size(); ++i){
        inp << res[i];
    }

    string result;
    inp >> result;

    return result;
}

string add(string x, string y){
    vector<int> res;
    res.resize(max(x.length(),y.length())*2,0);

    int carry = 0;
    for(int i = res.size()-1; i >= 1; --i){
        char first[2];
        first[0] = '0';
        first[1] = '\0';
        if((signed)x.length()-((signed)res.size()-i) >= 0){
            first[0] = x[x.length()-(res.size()-i)];
        }
        char second[2];
        second[0] = '0';
        second[1] = '\0';
        if((signed)y.length()-((signed)res.size()-i) >= 0){
            second[0] = y[y.length()-(res.size()-i)];
        }
    
        res[i] = atoi(first) + atoi(second) + carry;
        carry = (res[i] - (res[i] % 10)) / 10;
        res[i] = res[i] % 10;
    }

    if(carry){
        res[0] = carry;
    }

    int pos = 0;
    for(unsigned int i = 0; i < res.size(); ++i){
        if(res[i] != 0){
            pos = i;
            break;
        }
    }

    stringstream inp;
    for(unsigned int i = pos; i < res.size(); ++i){
        inp << res[i];
    }

    string result;
    inp >> result;
    return result;
}

int main(){
    /*
      string a = "13143215626008506113755473356836080013124";
      string b = "12773054640897453134174782992085058765091";
      cout << "12345 * 987 = " << multiply(a,b) << endl;
      cout << "12345 + 987 = " << add(a,b) << endl;
    */
    vector<string> efrac;
    efrac.push_back("2");

    int k = 1;
    stringstream inp;
    string temp;
    for(int i = 1; i < 100; ++i){
        if(i % 3 == 2){
            inp << 2*k;
            inp >> temp;
            efrac.push_back(temp);
            ++k;
        }
        else{
            inp << 1;
            inp >> temp;
            efrac.push_back(temp);
        }
        inp.clear();
    }

    /*  
        for(unsigned int i = 0; i < efrac.size(); ++i){
        cout << efrac[i] << " ";
        }
        cout << endl;
    */

    string num = "1";
    string denom = efrac[efrac.size()-1];
    //cout << num << " " << denom << endl;

    string swap;
    string oldnum;
    for(unsigned int i = 1; i < efrac.size(); ++i){
        oldnum = num;
        num = multiply(efrac[efrac.size()-i-1],denom);
        num = add(num,oldnum);
        if(i != efrac.size() -1){
            //cout << num << " " << denom << endl;
            swap = denom;
            denom = num;
            num = swap;
            //cout << num << "+" << efrac[efrac.size()-i-2] << "*" << denom << endl;
        }
    }

    //cout << num << " " << denom << endl;

    int sum = 0;
    for(unsigned int i = 0; i < num.length(); ++i){
        char x[2];
        x[0] = num[i];
        x[1] = '\0';
        sum += atoi(x);
    }
    cout << sum << endl;
  
    return 0;
}
