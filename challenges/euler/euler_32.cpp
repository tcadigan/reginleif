#include <iostream>
#include <set>
#include <sstream>

using namespace std;

int main(){
    stringstream inp;
    string product;
    set<int> result;

    for(int i = 1; i < 10000; ++i){
        for(int j = 1; j < 10000; ++j){
            int res = i * j;
      
            inp << i << "x" << j << "=" << res;

            inp >> product;
      

            if(product.length() > 11){
                inp.clear();
                break;
            }
            else{
                bool fail = false;
                if(product.find('0') == string::npos){
                    for(char k = '1'; k <= '9'; ++k){
                        //cout << k << ": " << product.find(k) << " " << product.rfind(k) << endl;
                        if(product.find(k) != product.rfind(k) || product.find(k) == string::npos){
                            fail = true;
                            break;
                        }
                    }
                }
                else{
                    fail = true;
                }
                if(!fail){
                    //cout << product << endl;
                    result.insert(res);
                }
                inp.clear();
            }
        }
    }

    set<int>::iterator it;
    int sum = 0;
    for(it = result.begin(); it != result.end(); ++it){
        //cout << *it << endl;
        sum += *it;
    }

    cout << sum;

    return 0;
}
