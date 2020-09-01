#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

set<vector<int> > seen;

vector<int> factor(int a){
    vector<int> facts;

    while(a % 2 == 0){
        facts.push_back(2);
        a /= 2;
    }

    for(int i = 3; a != 1 && i <= sqrt(a); i += 2){
        while(a % i == 0){
            facts.push_back(i);
            a /= i;
        }
    }

    if(a != 1){
        facts.push_back(a);
    }

    return facts;
}

void printv(vector<int> x){
    for(unsigned int i = 0; i < x.size(); ++i){
        cout << x[i] << " ";
    }
    cout << endl;
}

int sumv(vector<int> *x){
    int total = 0;
    for(unsigned int i = 0; i < (*x).size(); ++i){
        total += (*x).at(i);
    }

    return total;
}

int prodv(vector<int> *x){
    int total = 1;
    for(unsigned int i = 0; i < (*x).size(); ++i){
        total *= (*x).at(i);
    }

    return total;
}

void expand(vector<vector<int> > *list, vector<int> x){
    /*
      cout << "with: ";
      printv(x);
    */

    if(x.size() > 2){
        for(unsigned int i = 2; i <= x.size()-1; ++i){
            sort(x.begin(), x.end());
            do{
                vector<int> ex;
                int first = 1;
                for(unsigned int j = 0; j < i; ++j){
                    first *= x[j];
                }
                ex.push_back(first);
                for(unsigned int j = i; j < x.size(); ++j){
                    ex.push_back(x[j]);
                }
                //printv(ex);
                sort(ex.begin(),ex.end());
                if(seen.count(ex) == 0){
                    expand(list,ex);
                    (*list).push_back(ex);
                    seen.insert(ex);
                }
            }while(next_permutation(x.begin(), x.end()));
        } 
    }
}

bool cmp(vector<int> a, vector<int> b){
    return prodv(&a) < prodv(&b);
}

int main(){
    vector<vector<int> > list;

    for(int i = 2; i <= 15000; ++i){
        if(i % 100 == 0){
            cout << i << endl;
        }
        vector<int> temp;
        temp = factor(i);
        if(temp.size() > 1){
            list.push_back(temp);
        }
        if(temp.size() > 2){
            expand(&list, temp);
        }
    }
    cout << "done generating" << endl;
    sort(list.begin(),list.end(),cmp);

    set<int> res;
    for(int i = 2; i <= 12000; ++i){ 
        if(i % 100 == 0){
            cout << i << endl;
        }
        for(unsigned int j = 0; j < list.size(); ++j){
            int diff = i - list[j].size();
            if(diff < 0){
                continue;
            }
            /*
              cout << "working on: ";
              printv(list[j]);
              cout << "add: " << diff+sumv(list[j]) << " prod: " << prodv(list[j]) << endl;
            */
            int prod = prodv(&list[j]);
            if(diff+sumv(&list[j]) == prod){
                //cout << i << ": " << prod << endl;
                res.insert(prod);
                break;
            }
        }
    }

    int total = 0;
    set<int>::iterator it;
    for(it = res.begin(); it != res.end(); ++it){
        total += *it;
    }

    cout << total << endl;

    return 0;
}
