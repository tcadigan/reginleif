#include<iostream>
#include<vector>
#include<set>
#include<algorithm>

using namespace std;

int main(){
    set<unsigned long> nums;
    set<unsigned long>::iterator it;
    nums.insert(1);
    nums.insert(2);
    nums.insert(3);
    nums.insert(5);

    while(nums.size() < 1800){ 
        for(it = nums.begin(); it != nums.end(); ++it){
            nums.insert(*it*2);
            nums.insert(*it*3);
            nums.insert(*it*5);
            if(nums.size() > 1800){
                break;
            }
        }
    }

    vector<unsigned long> complete;
    for(it = nums.begin(); it != nums.end(); ++it){
        complete.push_back(*it);
        //cout << *it << endl; 
    }

    sort(complete.begin(), complete.end());

    cout << "The 1500'th ugly number is " << complete.at(1499) << "." << endl;

    return 0;
}
