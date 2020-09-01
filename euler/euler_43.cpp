#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

int main(){
    vector<int> nums;
    for(int i = 0; i < 10; ++i){
        nums.push_back(i);
    }

    double sum = 0;
    int primes[] = {2,3,5,7,11,13,17};
    bool flag;
    do{    
        flag = true;
        for(int i = 1,j = 0; i <= 7; ++i,++j){
            if(((nums[i]*100+nums[i+1]*10+nums[i+2]) % primes[j]) != 0){
                flag = false;
                break;
            }
        }
        if(flag){
            double temp = nums[0];
            for(int i = 1; i <10; ++i){
                temp *= 10;
                temp += nums[i];
            }
            //cout << temp << endl;
            sum += temp;
        }
    }while(next_permutation(nums.begin(), nums.end()));

    cout << setprecision(15) << sum << endl;
    return 0;
}
