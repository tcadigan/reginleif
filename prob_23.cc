#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

int dsum(int x){
    int dsum = 0;

    for(int i = 1; i < x; i++){
        if(x % i == 0){
            dsum += i;
        }
    }

    return dsum;

}

int main(){
    vector<int> anum;
    set<int> snum;

    for(int i = 1; i < 28124; i++){
        if(dsum(i) > i){
            anum.push_back(i);
        }
    }

    for(int i = 0; (unsigned) i < anum.size(); i++){
        for(int j = i; (unsigned) j < anum.size(); j++){
            if(anum[i] + anum[j] < 28124){
                snum.insert(anum[i] + anum[j]);
            }
        }
    }

    unsigned long sum = 0;
    for(int i = 1; i < 28124; i++){
        if(snum.count(i) == 0){
            sum += i;
        }
    }

    cout << sum << endl;

    return 0;
}
