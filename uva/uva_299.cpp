#include <iostream>
#include <vector>

using namespace std;

bool sorted(vector<unsigned int> t){
    for(unsigned int x = 0; x < t.size(); ++x){
        if(t[x] != x+1){
            return false;
        }
    }

    return true;
}

int main(){
    int cases;
    cin >> cases;

    while(cases > 0){
        vector<unsigned int> train;

        int length;
        cin >> length;

        int temp;
        while(length){
            cin >> temp;
            train.push_back(temp);
            --length;
        }
    
        int result = 0;
        while(!sorted(train)){
            for(unsigned int current = 0; current < train.size()-1; ++current){
                if(train[current] > train[current+1]){
                    temp = train[current];
                    train[current] = train[current+1];
                    train[current+1] = temp;
                    ++result;
                }
            }
        }

        cout << "Optimal train swapping takes " << result << " swaps." << endl;

        --cases;
    }

    return 0;
}
