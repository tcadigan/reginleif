#include <iostream>
#include <map>
#include <string>

using namespace std;

int main(){
    map<string, int> conquests;
    int n;

    cin >> n;

    string country;
    while(n-- > 0){
        cin >> country;

        string girl;
        getline(cin, girl);

        conquests[country] += 1;
    }

    map<string,int>::iterator it;
    
    for(it = conquests.begin(); it != conquests.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }

    return 0;
}
