#include<iostream>
#include<vector>
#include<sstream>
#include<map>
#include<algorithm>

using namespace std;

int main(){
    map<vector<int>,vector<unsigned long long> > table;
    map<vector<int>,vector<unsigned long long> >::iterator it;
 
    
    stringstream inp;
    string temp;
    for(unsigned long long i = 0; i < 100000; ++i){
        inp << i*i*i;
        inp >> temp;
        inp.clear();
        //cout <<  i << " " << temp << " ";
        vector<int> numfreq;
        numfreq.resize(10,0);
        for(unsigned int j = 0; j < temp.length(); ++j){
            numfreq[temp[j]-'0']++;
        }

        /*
          for(unsigned int j = 0; j < numfreq.size(); ++j){
          cout << numfreq[j] << " ";
          }
        */

        it = table.find(numfreq);
        if(it == table.end()){
            vector<unsigned long long> val;
            val.push_back(i);
            table[numfreq] = val;
        }
        else{
            it->second.push_back(i);
            //cout << it->second.size();
        }

        //cout << endl;
    }

    vector<unsigned long long> first;
    for(it = table.begin(); it != table.end(); ++it){
        if(it->second.size() == 5){
            sort(it->second.begin(), it->second.end());
            /*
              for(unsigned int i = 0; i < it->second.size(); ++i){
              cout << it->second.at(i) << " ";
              }
              cout << endl;*/
            //cout << it->second.at(0) << endl;
            first.push_back(it->second.at(0));
        }
    }

    sort(first.begin(), first.end());
  
    cout << first[0] * first[0] * first[0] << endl;

    return 0;
}
