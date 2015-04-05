#include <iostream>
#include <set>

using namespace std;

int main(){
    set<double> fracs;

    for(int d = 2; d <= 12000; ++d){
        if(d % 1000 == 0){
            cout << d << endl;
        }
        for(int n = 1; n < d; ++n){
            if(n/(double)d >= 1/2.0){
                break;
            }
            if(n/(double)d <= 1/3.0){
                continue;
            }
            fracs.insert(n/(double)d);
        }
    }

    cout << fracs.size() << endl;

    return 0;
}
