#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
    vector<int> queries;
    vector<int> marbles;

    int numMarbles, numQueries;

    cin >> numMarbles >> numQueries;

    int tests = 0;
    while(1){
        if(numMarbles == 0 && numQueries == 0){
            break;
        }

        tests += 1;

        //cout << "marbles" << endl;
        for(int i = 0; i < numMarbles; ++i){
            int x;
            cin >> x;
            marbles.push_back(x);

            //cout << x << endl;
        }

        //cout << "queries" << endl;
        for(int i = 0; i < numQueries; ++i){
            int x;
            cin >> x;

            queries.push_back(x);
            //cout << x << endl;
        }

        sort(marbles.begin(),marbles.end());

        cout << "CASE# " << tests << ":" << endl;

        vector<int>::iterator it;
        bool flag;
        for(it = queries.begin(); it != queries.end(); ++it){
            flag = false;
            for(unsigned int i = 1; i-1 < marbles.size(); ++i){
                if(*it == marbles.at(i-1)){
                    cout << *it << " found at " << i << endl;
                    flag = true;
                    break;
                }
            }
            if(!flag){
                cout << *it << " not found" << endl;
            }
        }
    
        marbles.clear();
        queries.clear();

        cin >> numMarbles >> numQueries;
    }

    return 0;
}
