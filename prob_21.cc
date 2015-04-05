#include <iostream>
#include <set>

using namespace std;

int main(){
    set<int> s;
    set<int>::iterator it;

    for(int i = 1; i < 10000; i++){
        int sum = 0;
        for(int j = 1; j < i; j++){
            if(i % j == 0){
                sum += j;
            }
        }
        int val = 0;
        for(int j = 1; j < sum; j++){
            if(sum % j == 0){
                val += j;
            }
        }

        //cout << i << " to " << sum << " to " << val << endl;

        if(val == i && val != sum){
            s.insert(sum);
            s.insert(i);
        }
    }

    int total = 0;
    for(it = s.begin(); it != s.end(); it++){
        total += *it;
    }

    cout << total << endl;

}
