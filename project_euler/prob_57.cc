#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> add(vector<int> x, vector<int> y){
    vector<int> result;

    unsigned int largest = max(x.size(),y.size());
    x.insert(x.begin(),largest-x.size(),0);
    y.insert(y.begin(),largest-y.size(),0);

    int carry = 0;
    int res;
    for(int i = x.size()-1; i >= 0; --i){
        res = x[i] + y[i] + carry;
        carry = (res - (res % 10)) / 10;
        result.push_back(res % 10);
    }

    if(carry){
        result.push_back(carry);
    }

    reverse(result.begin(), result.end());

    return result;
}

int main(){
    string tops;
    string bottoms;
    stringstream inp;
    int count = 0;

    vector<int> top;
    vector<int> bottom;
    top.push_back(3);
    bottom.push_back(2);

    if(top.size() > bottom.size()){
        count++;
    }
    inp.clear();

    vector<int> oldtop;
    for(int i = 2; i <= 1000; ++i){
        oldtop = top;
        top = add(top,bottom);
        top = add(top,bottom);
        bottom = add(oldtop,bottom);
    
        if(top.size() > bottom.size()){
            /*
              cout << i << " ";
              for(unsigned int i = 0; i < top.size(); ++i){
              cout << top[i];
              }
              cout << " / ";
              for(unsigned int i = 0; i < bottom.size(); ++i){
              cout << bottom[i];
              }
              cout << endl;
            */
            count++;
        }
        inp.clear();
    }

    cout << count << endl;
}
