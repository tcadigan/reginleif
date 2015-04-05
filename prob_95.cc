#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <set>

using namespace std;

int pfact(int x){
    vector<int> factors;

    for(int i = 1; i < x; ++i){
        if(x % i == 0){
            factors.push_back(i);
        }
    }
    int sum = 0;

    for(unsigned int i = 0; i < factors.size(); ++i){
        sum += factors[i];
        //cout << factors[i] << " ";
    }
    //cout << endl;
    return sum;
}

int main(){
    map<int,int> sums;
    for(int i = 2; i < 1000001; ++i){
        if(!(i % 50000)){
            cout << i << endl;
        }
        sums[i] = pfact(i);
    }
    cout << "finished gen" << endl;
    map<int,int>::iterator it;
    int temp, length, max = 0, resval = 0;;
    for(it = sums.begin(); it != sums.end(); ++it){
        //cout << "at " << it->first << endl;
        temp = it->second;
        length = 0;
        set<int> seen;
        seen.insert(it->first);
        while(temp != 1 && temp <= 1000000){
            //cout << temp << " ";
            if(seen.count(temp) == 0){
                seen.insert(temp);
                ++length;
                if(sums.find(temp) != sums.end()){
                    temp = sums.find(temp)->second;
                }
                else{
                    break;
                }
            }
            else{
                if(temp == it->first){
                    if(length > max){
                        max = length;
                        resval = it->first;
                        cout << max << " at " << resval << endl;
                    }
                }
                break;
            }
        }
        //cout << endl;
        //cout << it->first << ": " << it->second << endl;
    }

    cout << "max: " << resval << endl;
    return 0;
}
