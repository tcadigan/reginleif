#include <iostream>
#include <set>
#include <cstdlib>

using namespace std;

int main(){
    set<int> nums;
    set<int>::iterator it;
    set<int>::iterator it2;

    for(int i = 1; i < 3000; ++i){
        nums.insert((i*(3*i-1))/2);
    }

    int min = 1000000000;
    for(it = nums.begin(); it != nums.end(); ++it){
        for(it2 = it; it2 != nums.end(); ++it2){
            if(nums.find(*it+*it2) != nums.end()){
                if(nums.find(*it2-*it) != nums.end()){
                    //cout << *it2 << " " << *it << " " << *it2-*it << " " << min << endl;
                    if(abs(*it2-*it) < min){
                        min = abs(*it2-*it);
                    }
                }
            }
        }
    }

    cout << min << endl;
    return 0;
}
