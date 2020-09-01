#include<iostream>
#include<cstdlib>
#include<cmath>
#include<set>

using namespace std;

int main(){

    int min = 2000000;
    int minval = 0;

    set<pair<int,int> > seen;
    pair<int,int> p;
    for(int a = 1; a < 101; ++a){
        if (a % 10 == 0){
            cout << a << endl;
        }

        for(int b = 1; b < 101; ++b){
            int sqh = a;
            int sqw = b;

            if(a < b){
                p = make_pair(a, b);
            }
            else{
                p = make_pair(b,a);
            }

            if(seen.count(p) != 0){
                continue;
            }

            seen.insert(p);

            int total = 0;
            for(int h = 1; h <= sqh; ++h){
                for(int l = 1; l <= sqw; ++l){
	  
                    for(int i = 0; i < sqh; ++i){
                        for(int j = 0; j < sqw; ++j){
                            if( j + l <= sqw && i + h <= sqh){
                                ++total;
                            }
                        }
                    }
                }
            }
     
            if(abs(2000000-total) < min){
                min = abs(2000000-total);
                minval = a * b;
            }
        }
    }
    cout << min << " " << minval << endl;

    return 0;
}
