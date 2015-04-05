#include<iostream>
#include<math.h>
#include<vector>

using namespace std;

vector<pair<int, int> > factor(int x){
    vector<pair<int,int> > factors;
    pair <int, int> powpair;

    int count = 0;
    while(x % 2 == 0){
        ++count;
        x /= 2;
    }
  
    if(count){
        powpair = make_pair(2,count);
        factors.push_back(powpair);
    }
    for(int i = 3; x != 1; i += 2){
        count = 0;

        while(x % i == 0){
            ++count;
            x /= i;
        }

        if(count){
            powpair = make_pair(i,count);
            factors.push_back(powpair);
        }
    }

    return factors;
}

int main(){
    double ans = 0;
    int ansval = 0;

    vector<pair<int, int> > fact;
    for(int i = 2; i <= 1000000; ++i){
        fact = factor(i);

        int totient = 1;
        for(unsigned int j = 0; j < fact.size(); ++j){
            totient *= (fact[j].first-1)*pow(fact[j].first,fact[j].second-1);
        }
        //cout << i << ": " << totient << endl;
        if((double)i/totient > ans){
            ans = (double)i/totient;
            ansval = i;
        }
    }

    cout << ansval << endl;

}
