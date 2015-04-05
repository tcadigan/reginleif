#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <cmath>

using namespace std;

string genkey(string s){
    int letters[26];
  
    memset(letters, 0, sizeof(letters));

    for(unsigned int i = 0; i < s.size(); ++i){
        letters[s[i]-'A']++;
    }

    stringstream inp;
    string result;
    for(int i = 0; i < 26; ++i){
        inp << letters[i];
    }
    inp >> result;
    return result;
}

set<char> uniqueLetters(string s){
    set<char> unique;

    for(unsigned int i = 0; i < s.size(); ++i){
        unique.insert(s[i]);
    }

    return unique;
}

long long createNumber(map<char, int> mapper, string x){
    long long result = 0;
    for(unsigned int i = 0; i < x.size(); ++i){
        result = result * 10 + mapper[x[i]];
    }

    return result;
}
  
bool isSquare(double x){
    return (int)sqrt(x) * (int)sqrt(x) == x;
}

void genSquares(map<int, vector<string> > &mapper, int n){
    stringstream inp;

    string number;
    set<char> uniques;
    for(long long i = 6*pow(10,(n-1)); i < 8 * pow(10,n-1); ++i){
        inp << i << " ";
        inp >> number;
        uniques = uniqueLetters(number);
        if(uniques.size() == number.size()){
            if(mapper.find(number.size()) == mapper.end()){
                vector<string> t;
                mapper[number.size()] = t;
            }
            mapper[number.size()].push_back(number);
        }
    }
}

int main(){
    string input;
    stringstream inp;

    cin >> input;
    int start = 0;
    int end = 0;
    string temp;

    int offset = 0;
    while(start != (signed)string::npos && end != (signed)string::npos){
        start = input.find("\"", end+offset);
        end = input.find("\"", start+1);

        temp = input.substr(start+1, end-start-1);

        inp << temp << " ";
    
        offset = 1;
    }

    map<string, vector<string> > dict;
    map<string, vector<string> >::iterator dit;
    vector<string>::iterator vit;
    vector<string>::iterator vit2;
  
    string key;
    while(inp >> temp){
        key = genkey(temp);
        if(dict.find(key) == dict.end()){
            vector<string> list;
            list.push_back(temp);
            dict[key] = list;
        }
        else{
            dict[key].push_back(temp);
        }
    }

    vector<pair<string, string> > anagrams;

    for(dit = dict.begin(); dit != dict.end(); ++dit){
        if(dit->second.size() > 1){
            for(vit = dit->second.begin(); vit != dit->second.end(); ++vit){
                for(vit2 = vit+1; vit2 != dit->second.end(); ++vit2){
                    pair<string, string> p(*vit, *vit2);
                    anagrams.push_back(p);
                }
            }
        }
    }

    // for(it = anagrams.begin(); it != anagrams.end(); ++it){
    //   cout << it->first << " " << it->second << endl;
    // }

    vector<string>::iterator nlit;
    set<char>::iterator ulit;

    int max = 0;

    vector<pair<string, string> >::iterator it;
    for(it = anagrams.begin(); it != anagrams.end(); ++it){  
        cout << it->first << " " << it->second << endl;
        set<char> uniques = uniqueLetters(it->first);

        map<int, vector<string> > squares;
        map<int, vector<string> >::iterator sqit;
        genSquares(squares, uniques.size());

        vector<string> numberList = squares[uniques.size()];  

        // for(sqit = squares.begin(); sqit != squares.end(); sqit++){
        //   cout << sqit->first << ": ";
        //   for(vit = sqit->second.begin(); vit != sqit->second.end(); ++vit){
        //     cout << *vit << " ";
        //   }
        //   cout << endl;
        // }

        for(nlit = numberList.begin(); nlit != numberList.end(); nlit++){
            int i = 0;
            map<char, int> mapper;
            for(ulit = uniques.begin(); ulit != uniques.end(); ++ulit){
                mapper[*ulit] = (*nlit)[i]-'0';
                ++i;
            }

            if(mapper[(it->first)[0]] == 0 || mapper[(it->second)[0]] == 0){
                continue;
            }

            // map<char, int>::iterator mapit;
            // for(mapit = mapper.begin(); mapit != mapper.end(); ++mapit){
            // 	cout << mapit->first << " " << mapit->second << endl;
            // }

            long long first = createNumber(mapper, it->first);
            long long second = createNumber(mapper, it->second);
      
            // if(it->first == "RACE" || it->first == "CARE"){
            // 	cout << it->first << " " << it->second << endl;
            // 	cout << first << " " << second << endl;
            // }

            if(isSquare(first) && isSquare(second)){
                int larger = (first > second) ? first : second;
                if(larger > max){
                    max = larger;
                    cout << max << endl;
                }
            }
        }
    }
    // take it->first unique chars number
    // map chars in it->first to number
    // apply mapping to it->second
    // get number of it->second
    // if good, record. else skip.

    cout << "result: " << max << endl;

    return 0;
}

