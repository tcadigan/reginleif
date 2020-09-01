#include <iostream>
#include <set>
#include <map>
#include <math.h>

using namespace std;

int main(){
    set< pair<int, pair<int, int> > > seen;
    map<int,int> counts;

    pair<int, pair<int, int> > outer;
    pair<int, int> inner;

    for(int i = 1; i <= 500; ++i){
        for(int j = i; j <= 500; ++j){
            if(i + j > 1000){
                break;
            }
            for(int k = max(i,j); k <= 500; ++k){
                if(i + j + k > 1000){
                    break;
                }
                if(i*i + j*j == k*k){
                    if(i < j){
                        inner = make_pair(j,i);
                    }
                    else{
                        inner = make_pair(i,j);
                    }
                    outer = make_pair(k,inner);

                    if(seen.count(outer) == 0){
                        //cout << i << " " << j<< " " << k << " " << i+j+k << endl;
                        seen.insert(outer);
                        counts[i+j+k]++;
                    }
                }
            }
        }
    }

    map<int,int>::iterator it;
    int max = 0;
    int p = 0;
    for(it = counts.begin(); it != counts.end(); ++it){
        //cout << it->first << ": " << it->second << endl;
        if(it->second > max){
            max = it->second;
            p = it->first;
        }
    }

    cout << p << endl;

    return 0;
}
