#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

int main(){
    vector<int> num;
    vector<int>::iterator it;
    vector<int>::iterator bit;
    stringstream inp;

    int sum = 0;
    for(int i = 1; i < 1000001; ++i){
        inp << i;

        string number;
        inp >> number;

        int j,k;
        for(j = 0, k = number.length()-1; number[j] == number[k] && j < k; ++j, --k){}

        if(j >= k){
            int temp = i;
            while(temp != 0){
                num.push_back(temp % 2);
                temp /= 2;
            }
      
            reverse(num.begin(),num.end());
      
            /*cout << i << ": ";
              for(it = num.begin(); it != num.end(); ++it){
              cout << *it;
              }
              cout << endl;*/
      
            for(it = num.begin(), bit = num.end()-1; *it == *bit && it < bit; ++it, --bit){}
            if(it >= bit){
                sum += i;
            }
            num.clear();
        }
    
        inp.clear();
    }

    cout << sum << endl;
    return 0;
}
