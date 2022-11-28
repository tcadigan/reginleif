#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <cctype>
#include <algorithm>
using namespace std;

bool same(vector<int> &first, vector<int> &second){
    bool result = true;

    for(int i = 0; i < 26; ++i){
        if(first[i] != second[i]){
            result = false;
            break;
        }
    }

    return result;
}

int main(){
    string word;

    map< string, vector<int> > mapping;
    vector<string> finals;

    while(cin >> word){
        if(word == "#"){
            break;
        }

        vector<int> freq;
        freq.resize(26,0);
        for(unsigned int i = 0; i < word.size(); ++i){
            char temp = word[i];
            freq[tolower(temp) % 'a'] += 1;
        }

        mapping[word] = freq;
    }

    map<string, vector<int> >::iterator it2;
    map<string, vector<int> >::iterator it1;
  
    for(it1 = mapping.begin(); it1 != mapping.end(); ++it1){
        bool distinct = true;
        for(it2 = mapping.begin(); it2 != mapping.end(); ++it2){
            if(it1 != it2){
                //cout << it1->first << " " << it2->first << " " << same(it1->second, it2->second) << endl;
                if(same(it1->second, it2->second)){
                    distinct = false;
                    break;
                }
            }
        }
        if(distinct){
            finals.push_back(it1->first);
        }
    }

    sort(finals.begin(), finals.end());

    for(unsigned int i = 0; i < finals.size(); i++){
        cout << finals[i] << endl;
    }

    return 0;
}
