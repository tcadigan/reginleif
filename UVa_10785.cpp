#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

int main(){
    string cons = "JSBKTCLDMVNWFXGPYHQZR";
    string vows = "AUEOI";

    int inputs;
    int cases = 1;

    cin >> inputs;
    while(inputs > 0){
        cout << "Case " << cases << ": ";

        int length;
        cin >> length;

        vector<pair<char, int> >vvector;
        vector<pair<char, int> >cvector;

        int vamount = ceil(length / 2.0);
        string vallowed = vows.substr(0, ceil(vamount / 21.0));
        for(unsigned int i = 0; i < vallowed.size(); ++i){
            if(vamount >= 21){
                pair<char, int> p(vallowed[i], 21);
                vvector.push_back(p);
                vamount -= 21;
            }
            else{
                pair<char, int> p(vallowed[i], vamount);
                vvector.push_back(p);
            }
        }
        sort(vvector.begin(), vvector.end());

        int camount = floor(length / 2.0);
        string callowed = cons.substr(0, ceil(camount / 5.0));
        for(unsigned int i = 0; i < callowed.size(); ++i){
            if(camount >= 5){
                pair<char, int> p(callowed[i], 5);
                cvector.push_back(p);
                camount -= 5;
            }
            else{
                pair<char, int> p(callowed[i], camount);
                cvector.push_back(p);
            }
        }
        sort(cvector.begin(), cvector.end());

        int vloc = 0;
        int cloc = 0;

        for(int i = 0; i < length; ++i){
            if(i % 2 == 0 && vloc < 5){
                cout << vvector[vloc].first;
                vvector[vloc].second -= 1;
                if(vvector[vloc].second == 0){
                    vloc++;
                }
            }
            else{
                cout << cvector[cloc].first;
                cvector[cloc].second -= 1;
                if(cvector[cloc].second == 0){
                    cloc++;
                }
            }
        }
        cout << endl;

        ++cases;
        --inputs;
    }
  
    return 0;
}
