#include<iostream>
#include<set>
#include<sstream>
#include<algorithm>

using namespace std;

int main(){
    set<string> codes;
    string num;

    while(cin >> num){
        codes.insert(num);
    }
  
    set<string>::iterator it;
    /*
      for(it = codes.begin(); it != codes.end(); ++it){
      cout << *it << endl;
      }
    */
    stringstream inp;
    string pin;
    set<string> seen;
    for(int i = 100; i < 100000001; ++i){
        if(i % 100000 == 0){
            cout << i << endl;
        }
        inp << i;
        inp >> pin;
        inp.clear();

        sort(pin.begin(),pin.end());

        if(seen.count(pin) != 0){
            continue;
        }

        seen.insert(pin);
        //cout << "current pin: " << pin << endl;
        do{
            bool found;
            for(it = codes.begin(); it != codes.end(); ++it){
                string code = *it;
                string curr = pin;
                found = false;
                if(curr.find(code[0]) != string::npos){
                    curr = curr.substr(curr.find(code[0]));
                    if(curr.find(code[1]) != string::npos){
                        curr = curr.substr(curr.find(code[1]));
                        if(curr.find(code[2]) != string::npos){
                            found = true;
                        }
                    }
                }
                if(!found){
                    break;
                }
            }
            if(found){
                cout << pin << endl;
                return 0;
            }
        }while(next_permutation(pin.begin(),pin.end()));
    }
    return 0;
}
