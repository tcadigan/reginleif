#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main(){
    vector<int> list;

    for(int i = 0; i < 10; i++){
        list.push_back(i);
    }

    int permnum = 1;

    while(next_permutation(list.begin(), list.end())){

        permnum++;

        if(permnum == 1000000){
            break;
        } 
    }  

    for(int i = 0; (unsigned)i < list.size(); i++){
        cout << list.at(i);
    }

    cout << endl;

    return 0;
}
