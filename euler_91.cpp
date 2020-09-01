#include <iostream>

using namespace std;

bool islinear(pair<int,int> one, pair<int,int> two, pair<int,int> three){
    return one.first*(two.second-three.second)+two.first*(three.second-one.second)+three.first*(one.second-two.second) == 0;
}

// square distance
int dist(pair<int,int> one, pair<int, int> two){
    return (one.first-two.first)*(one.first-two.first) + (one.second-two.second)*(one.second-two.second);
}

void printp(pair<int,int> one){
    cout << "(" << one.first << "," << one.second << ")" << endl;
}

int main(){
    pair<int,int> one;
    pair<int,int> two;
    pair<int,int> three;

    one = make_pair(0,0);
    two = make_pair(-1,-1);
    three = make_pair(-1,-1);

    int size = 50;

    int d1, d2, d3, count = 0;
    for(int i = size; i >= 0; --i){
        two.second = i;
        for(int j = 0; j <= size; ++j){
            two.first = j;
      
            //cout << "two: ";
            //printp(two);

            for(int k = i; k >= 0; --k){
                three.second = k;
                int l;
                for(k==i?l=j+1:l=0; l <= size; ++l){
                    three.first = l;

                    if(islinear(one,two,three)){
                        continue;
                    }
                    //cout << "three: ";
                    //printp(three);

                    d1 = dist(one,two);
                    d2 = dist(one,three);
                    d3 = dist(two,three);

                    if(d2 < d1){
                        int temp = d1;
                        d1 = d2;
                        d2 = temp;
                    }
                    if(d3 < d1){
                        int temp = d1;
                        d1 = d3;
                        d3 = temp;
                    }
                    if(d3 < d2){
                        int temp = d2;
                        d2 = d3;
                        d3 = temp;
                    }

                    if(d1 + d2 == d3){
                        count++;
                    }
                }
            }
        }
    }

    cout << count << endl;
    return 0;
}
