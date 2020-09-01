#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

pair<string,vector<int> > distx(string a, string b){
    vector<int> pos;
    pair<string,vector<int> > result;
  
    if(a.length() != b.length()){
        pos.push_back(-1);
        result = make_pair(a,pos);
        return result;
    }

    for(unsigned int i = 0; i < a.length(); ++i){
        if(a[i] != b[i]){
            pos.push_back(i);
        }
    }

    if(pos.size() > 1){
        bool flag = true;
        for(unsigned int i = 0; i < pos.size()-1; ++i){
            if(b[pos[i]] != b[pos[i+1]]){
                flag = false;
                break;
            }
        }
    
        if(!flag){
            pos.clear();
            pos.push_back(-1);
        }
    }
    
    result = make_pair(a,pos);
    return result;
}

bool isprime(int x){
    if(x % 2 == 0){
        return false;
    }
    for(int i = 3; i <= sqrt(x); i += 2){
        if(x % i == 0){
            return false;
        }
    }

    return true;
}

int main(){

    vector<string> primes;
    stringstream inp;
    string temp;

    for(int i = 100000; i < 1000001; i++){
        if(isprime(i)){
            inp << i;
            inp >> temp;
            primes.push_back(temp);
            inp.clear();
        }
    }

    map<pair<string,vector<int> >,set<string> > results;
 
    for(unsigned int i = 0; i < primes.size(); ++i){
        if(i % 100 == 0){
            cout << "at " << primes[i] << endl;
        }
        pair<string, vector<int> > res;
        for(unsigned int j = i+1; j < primes.size(); ++j){
            res = distx(primes[i],primes[j]);
            /*
              cout << res.first << " at {";
              for(unsigned int k = 0; k < res.second.size(); ++k){
              cout << res.second.at(k);
              if(k != res.second.size()-1){
              cout << " ";
              }
              }
              cout << "} with " << primes[j] << endl;
            */
            if(results.find(res) == results.end()){
                set<string> rset;
                rset.insert(primes[j]);
                results[res] = rset;
            }
            else{
                if(res.second.at(0) != -1){
                    results.find(res)->second.insert(primes[j]);
                    if(res.second.at(0) != -1 && results.find(res)->second.size() == 7){
                        cout << primes[i] << endl;
                        return 0;
                    }
                }
            }
        }
    }

  
    unsigned int max = 0;
    map<pair<string,vector<int> >,set<string> >::iterator it;
    set<string>::iterator it2;
    for(it = results.begin(); it != results.end(); ++it){
        if((*it).second.size() > max && (*it).first.second.at(0) != -1){
            max = (*it).second.size();
        }
    }

    for(it = results.begin(); it != results.end(); ++it){
        if((*it).second.size() == max && (*it).first.second.at(0) != -1){
            cout << (*it).first.first << " at {";
            for(unsigned int k = 0; k < (*it).first.second.size(); ++k){
                cout << (*it).first.second.at(k);
                if(k != (*it).first.second.size()-1){
                    cout << " ";
                }
            }
            cout << "} with {";
            for(it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2){
                cout << (*it2) << " ";
            }
            cout << "}" << endl;
            max = (*it).second.size();
        }
    }
  
    /*
      map<pair<string,vector<int> >,set<string> >::iterator it;
      set<string>::iterator it2;
      for(it = results.begin(); it != results.end(); ++it){
      cout << (*it).first.first << " at {";
      for(unsigned int k = 0; k < (*it).first.second.size(); ++k){
      cout << (*it).first.second.at(k);
      if(k != (*it).first.second.size()-1){
	  cout << " ";
      }
      }
      cout << "} with {";
      for(it2 = it->second.begin(); it2 != it->second.end(); ++it2){
      cout << *it2 << " ";
      }
      cout << "}" <<endl;
      }
    */
    return 0;
}
