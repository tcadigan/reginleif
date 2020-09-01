#include <iostream>
#include <set>
#include <map>
#include <math.h>
#include <sstream>
#include<string.h>
#include<iomanip>

using namespace std;

map< int,set<int> > divlist;

set<int> factor(int x){
    int a = x;

    set<int> factors;

    while(a != 1 && a % 2 == 0){
        a /= 2;

        if(divlist.find(a) != divlist.end()){
            factors = divlist.find(a)->second;
            a = 1;
        }
    
        factors.insert(2);
    }

    for(int i = 3; a != 1 && i <= sqrt(x); i += 2){
        while(a != 1 && a % i == 0){
            a /= i;
      
            if(divlist.find(a) != divlist.end()){
                factors=divlist.find(a)->second;
                a = 1;
            }

            factors.insert(i);
        }
    }

    if(a != 1){
        factors.insert(a);
    }

    return factors;
}

int main(){
    set<int> divs;
    set<int>::iterator it;
    stringstream inp;
    double phi;
    string a,b;
    int tally[10];
    bool flag;
    double ratio = 10000000;
    int answer = 0;

    inp << setprecision(20);
    cout << setprecision(20);

    for(int i = 2; i < 10000000; ++i){
        if(i % 100000 == 0){
            cout << i << endl;
        }

        divs = factor(i);
        if( i < 1000001){
            divlist[i] = divs;
        }

        phi = i;
        for(it = divs.begin(); it != divs.end(); ++it){
            phi *= (1-(1 / (double)*it));
        }

        inp << i << " " << phi;
        inp >> b >> a;
        inp.clear();

        if(a.length() != b.length()){
            continue;
        }

        memset(tally,0,sizeof(int)*10);
        for(unsigned int j = 0; j < a.length(); ++j){
            tally[a[j]-'0']++;
            tally[b[j]-'0']--;
        }

        flag = true;
        for(int j = 0; j < 10; ++j){
            if(tally[j] != 0){
                flag = false;
            }
        }

        if(flag && i/phi < ratio){
            ratio = i/phi;
            answer = i;
            //cout << "new min " << ratio << " " << answer << endl;
        }
    }

    cout << answer << endl;

    return 0;
}
