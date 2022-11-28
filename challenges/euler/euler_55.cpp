#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

vector<int> add(vector<int> a, vector<int> b){
    vector<int> result;

    int carry = 0;
    /*
      for(unsigned int i = 0; i < a.size(); ++i){
      cout << a[i];
      }
      cout << endl;
      for(unsigned int i = 0; i < b.size(); ++i){
      cout << b[i];
      }
      cout << endl;
    */
    for(int i = a.size()-1; i >= 0; --i){
        int res = a[i] + b[i] + carry;
        carry = (res - (res % 10)) / 10;
        result.push_back(res % 10);
    }
    if(carry){
        result.push_back(carry);
    }

    reverse(result.begin(), result.end());
  
    return result;
}

bool ispal(vector<int> x){
    for(unsigned int i = 0, j = x.size()-1; i < j; ++i,--j){
        if(x[i] != x[j]){
            return false;
        }
    }

    return true;
}

bool islychrel(vector<int> x){
    for(int i = 0; i < 50; ++i){
        /*
          cout << i << ": ";
          for(unsigned int i = 0; i < x.size(); ++i){
          cout << x[i];
          }
          cout << endl;
        */
        vector<int> b;
        for(unsigned int j = 0; j < x.size(); ++j){
            b.push_back(x[j]);
        }

        reverse(b.begin(), b.end());
        /*
          for(unsigned int i = 0; i < b.size(); ++i){
          cout << b[i];
          }
          cout << endl;
        */
        x = add(x,b);
        /*
          for(unsigned int i = 0; i < x.size(); ++i){
          cout << x[i];
          }
          cout << endl;
        */
        if(ispal(x)){
            return true;
        }
    }

    return false;
}


int main(){
    stringstream inp;
    int total = 0;

    for(int i = 0; i < 10000; ++i){
        inp << i;
        string num;
        inp >> num;

        vector<int> lnum;
        for(unsigned int j = 0; j < num.length(); ++j){
            lnum.push_back(num[j]-'0');
        }

        if(!islychrel(lnum)){
            total++;
        }

        inp.clear();
    }

    cout << total << endl;

    return 0;
}
