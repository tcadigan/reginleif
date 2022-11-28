#include <iostream>
#include <stdlib.h>

using namespace std;

int main(){
    char a;
    int arr[5] = {0};

    int sum = 1;

    for(int i = 0; i < 5; i++){
        cin >> a;
        arr[i] = atoi(&a);
        sum *= arr[i];
    }

    while(1){
        cin >> a;
        if(cin.eof()){
            break;
        }
        int newsum = 1;
        for(int i = 0; i < 4; i++){
            arr[i] = arr[i+1];
            newsum *= arr[i];
        }
        arr[4] = atoi(&a);
        newsum *= arr[4];
    
        if(newsum > sum){
            sum = newsum;
        }
    }

    cout << sum << endl;

    return 0;
}
