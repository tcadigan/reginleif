#include<iostream>
#include<set>
#include<math.h>
#include<vector>

using namespace std;

int main(){
    vector<int> factorials;
    int val;

    for(int i = 0; i <= 9; ++i){
        if(i == 0 || i == 1){
            factorials.push_back(1);
        }
        else{
            val = i;
            for(int j = i-1; j > 1; --j){
                val *= j;
            }
            factorials.push_back(val);
        }
    }

    int total = 0;
    for(int i = 1; i < 1000000; ++i){
        set<int> cycle;
        cycle.insert(i);
        int x = i;
        int sum;
        int count = 1;
        while(1){
            sum = 0;
            while(x != 0){
                sum += factorials[x%10];
                x /= 10;
            }
            if(cycle.count(sum) != 0){
                break;
            }
            //cout << "sum: " << sum <<  endl;
            cycle.insert(sum);
            ++count;
            x = sum;
        }
        //cout << i << " " << count << endl;
        if(count == 60){
            ++total;
        }
    }

    cout << total << endl;

    return 0;
}
