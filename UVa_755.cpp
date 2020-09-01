#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

int main(){
    map<char, int> dict;
    dict['0'] = 0;
    dict['1'] = 1;

    dict['A'] = 2;
    dict['B'] = 2;
    dict['C'] = 2;
    dict['2'] = 2;

    dict['D'] = 3;
    dict['E'] = 3;
    dict['F'] = 3;
    dict['3'] = 3;

    dict['G'] = 4;
    dict['H'] = 4;
    dict['I'] = 4;
    dict['4'] = 4;

    dict['J'] = 5;
    dict['K'] = 5;
    dict['L'] = 5;
    dict['5'] = 5;

    dict['M'] = 6;
    dict['N'] = 6;
    dict['O'] = 6;
    dict['6'] = 6;

    dict['P'] = 7;
    dict['R'] = 7;
    dict['S'] = 7;
    dict['7'] = 7;

    dict['T'] = 8;
    dict['U'] = 8;
    dict['V'] = 8;
    dict['8'] = 8;

    dict['W'] = 9;
    dict['X'] = 9;
    dict['Y'] = 9;
    dict['9'] = 9;
  
    int datasets;
    cin >> datasets;
    while(datasets > 0){
        int numNumbers;
        vector<int> book;

        cin >> numNumbers;
    
        while(numNumbers > 0){
            string number;
            cin >> number;
            int value = 0;
      
            for(unsigned int i = 0; i < number.size(); ++i){
                if(number[i] == '-'){
                    continue;
                }
                value = value*10 + dict[number[i]];
            }

            book.push_back(value);

            --numNumbers;
        }

        sort(book.begin(), book.end());

        vector<pair<int, int> > results;
    
        int previous = -1;
        bool singular = true;
        for(unsigned int i = 0; i < book.size(); ++i){
            if(book[i] != previous){
                pair<int, int> p(book[i], 1);
                results.push_back(p);
                previous = book[i];
            }
            else{
                results.back().second++;
                singular = false;
            }
        }

        if(singular){
            cout << "No duplicates." << endl;
        }
        else{
            for(unsigned int i = 0; i < results.size(); ++i){
                if(results[i].second > 1){
                    cout << setw(3) << setfill('0') << results[i].first / 10000;
                    cout << "-" << setw(4) << setfill('0') << results[i].first % 10000;
                    cout << " " << results[i].second << endl;
                }
            }
        }

        --datasets;
        if(datasets != 0){
            cout << endl;
        }
    }

    return 0;
}
