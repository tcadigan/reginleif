#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

int main(){
    int num;
  
    while(cin >> num){
        for(int i = 0; i < 60; ++i){
            cout << "-";
        }
        cout << endl;

        string file;
        vector<string> list;

        unsigned int longest = 0;
        for(int i = 0; i < num; ++i){
            cin >> file;
            list.push_back(file);
            if(file.size() > longest){
                longest = file.size();
            }
        }

        int columns = 0;
        int width = 60;
        width -= longest;
        ++columns;

        columns += (width/(longest+2));

        int rows = ceil(list.size() / (double)columns);

        sort(list.begin(), list.end());

        for(int i = 0; i < rows; ++i){
            int j = 0;
            for(j = 0; j < columns-1; ++j){
                cout << left << setw(longest+2) << list[j*rows+i];
            }
            if((unsigned)(j*rows+i) < list.size()){
                cout << left << setw(longest) << list[j*rows+i] << endl;
            }
            else{
                cout << endl;
            }
        }
    }

    return 0;
}
