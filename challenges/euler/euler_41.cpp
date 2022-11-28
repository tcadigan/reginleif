#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iomanip>

using namespace std;

bool isprime(double x){
    for(double i = 2; i < sqrt(x); ++i){
        if(fmod(x,i) == 0){
            return false;
        }
    }
    return true;
}

int main(){
    vector<int> nums;
    double max = 0;

    for(int j = 2; j <= 9; ++j){
        for(int i = 1; i <= j; ++i){
            nums.push_back(i);
        }
    
        do{
            double sum = nums[0];
            for(int i = 1; i < j;++i){
                sum *= 10;
                sum += nums[i];
            }
            //cout << sum << endl;
            if(isprime(sum) && sum > max){
                max = sum;
            }
      
        }while(next_permutation(nums.begin(), nums.end()));

        nums.clear();
    }
    
    cout << setprecision(10) << max << endl;

    return 0;
}
