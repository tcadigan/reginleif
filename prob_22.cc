#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
    vector<string> list;
    vector<string>::iterator it;

    string inp;

    cin >> inp;

    while(!cin.eof()){
        list.push_back(inp);
        cin >> inp;
    }
  
    sort(list.begin(), list.end());

    int i;
    unsigned long tsum = 0;
    for(it = list.begin(), i = 1; it != list.end(); it++, i++){
        int wsum = 0;
        for(int j = 0; (unsigned) j < (*it).size(); j++){
            wsum += (*it).at(j) - 'A' + 1;
        }
        tsum += (wsum * i);
    }

    cout << tsum << endl;

    return 0;
}
