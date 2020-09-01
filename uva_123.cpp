#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <cctype>
#include <string>
#include <sstream>

using namespace std;

string lowerWord(string x){
    string result = "";
    char temp;
    for(unsigned int i = 0; i < x.length(); ++i){
        temp = x[i];
        result += tolower(temp);
    }
    return result;
}

string upperWord(string x){
    string result = "";
    char temp;
    for(unsigned int i = 0; i < x.length(); ++i){
        temp = x[i];
        result += toupper(temp);
    }

    return result;
}

bool sameVector(vector<string> &one, vector<string> &two){
    if(one.size() != two.size()){
        return false;
    }
  
    for(unsigned int i = 0; i < one.size(); ++i){
        if(one[i] != two[i]){
            return false;
        }
    }

    return true;
}

void prettyPrint(vector<string> &words, int count, string key){
    for(unsigned int i = 0; i < words.size(); ++i){
        if(i != 0){
            cout << " ";
        }

        if(words[i] == lowerWord(key)){
            --count;
            if(count == 0){
                cout << key;
            }
            else{
                cout << words[i];
            }
        }
        else{
            cout << words[i];
        }
    }
    cout << endl;
}

void printDictionary(map<string, vector< vector<string> > > dict){
    map<string, vector< vector<string> > >::iterator it;
    vector<vector<string> >::iterator it2;
    for(it = dict.begin(); it != dict.end(); ++it){
        vector<string> prev;
        int count;
        for(it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            if(sameVector(*it2, prev)){
                count++;
            }
            else{
                count = 1;
            }
            prettyPrint(*it2, count, it->first);
            prev = *it2;
        }
    }
}

int main(){
    string line;
    set<string> skip;
    map<string, vector< vector<string> > > dict;

    while(cin >> line){
        if(line == "::"){
            break;
        }

        skip.insert(line);
    }

    stringstream inp;
    while(getline(cin, line)){
        inp << line;
    
        vector<string> wordList;
        string word;
        while(inp >> word){
            wordList.push_back(lowerWord(word));
        }

        for(unsigned int i = 0; i < wordList.size(); ++i){
            word = wordList[i];
            if(skip.find(lowerWord(word)) == skip.end()){
                word = upperWord(word);
                if(dict.find(word) == dict.end()){
                    vector<vector<string> > element;
                    dict[word] = element;
                }
                dict[word].push_back(wordList);
            }
        }
        inp.clear();
    }

    printDictionary(dict);
  
    return 0;
}
