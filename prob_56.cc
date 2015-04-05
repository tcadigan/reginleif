#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

vector<int> multiply(vector<int> x, vector<int> y){
    vector<int> result;
    result.resize(201,0);

    int offset = 0;
    for(unsigned int i = 0; i  < x.size(); ++i){
        for(unsigned int j = 0; j < y.size(); ++j){
            //cout << x.size()-i-1 << " " << y.size()-j-1 << " ";
            //cout << result.size()-j-offset-1 << endl;
            result[result.size()-j-offset-1] += y[y.size()-1-j] * x[x.size()-1-i];
        }
        ++offset;
    }

    int carry = 0;
    for(unsigned int i = result.size()-1; i >= 1; --i){
        int val = result[i] + carry;
        carry = (val - (val % 10)) / 10;
        result[i] = val % 10;
    }

    return result;
}

int digitsum(vector<int> x){
    int sum = 0;
    for(unsigned int i = 0; i < x.size(); ++i){
        sum += x[i];
    }

    return sum;
}

int main(){
    stringstream inp;
    int max = 0;
  
    for(int i = 1; i < 101; ++i){
        vector<int> num;
    
        inp << i;
        string snum;
        inp >> snum;
        for(unsigned int j = 0; j < snum.length(); ++j){
            num.push_back(snum[j]-'0');
        }
        int sum = digitsum(num);
        if(sum > max){
            max = sum;
        }
    
        vector<int> orig;
        for(unsigned int j = 0; j < num.size(); ++j){
            orig.push_back(num[j]);
        }

        for(int j = 2; j < 101; ++j){
            num = multiply(orig,num);

            sum = digitsum(num);
            if(sum > max){
                max = sum;
            }
            /*
              cout << i << "," << j << " ";
              for(unsigned int k = 0; k < num.size(); ++k){
              cout << num[k];
              }
              cout << endl;
            */
        }

        inp.clear();
    }
    /*
      vector<int> a;
      a.push_back(1);
      a.push_back(2);
      a.push_back(3);

      vector<int> b;
      b.push_back(1);
      b.push_back(5);

      a = multiply(b,a);

      for(unsigned int i = 0; i < a.size(); ++i){
      cout << a[i];
      }
      cout << endl;
    */
    cout << max << endl;
}
