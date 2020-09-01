#include <iostream>

using namespace std;

int main(int argc, char * argv[]){
    int tests;
  
    cin >> tests;
  
    int case_num = 1;

    while(case_num <= tests){
        int a;
        int b;

        cin >> a >> b;

        if(a % 2 == 0){
            ++a;
        }

        int total = 0;
        for(int i = a; i <= b; i += 2){
            total += i;
        }

        cout << "Case " << case_num << ": " << total << endl;

        ++case_num;
    }

    return 0;
}
