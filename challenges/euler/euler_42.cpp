#include <iostream>
#include <vector>

using namespace std;

int main(){
    vector<int> tnums;

    for(int i = 0; i < 101; ++i){
        tnums.push_back((i*(i+1))/2);
    }

    string inp;
    int sum;
    int num = 0;
    while(cin >> inp){
        sum = 0;
        for(unsigned int i = 0; i < inp.length(); ++i){
            sum += inp[i] - 'A' + 1;
        }

        for(int i = 0; i < 101; ++i){
            if(sum == tnums[i]){
                num++;
                break;
            }
        }
    }

    cout << num << endl;

    return 0;
}
