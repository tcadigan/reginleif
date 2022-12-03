#include <iostream>
#include <set>
#include <cmath>

using namespace std;

int main(){

    for(int size = 1817; size < 1819; ++size){

        set<pair<int, pair<int, int> > >seen;
        pair<int, int> inner;
        pair<int, pair<int,int> > outer;

        int total = 0;

        for(int len = 1; len <= size; ++len){
            if(len % 100 == 0){
                cout << len << endl;
            }
            for(int wid = 1; wid <= size; ++wid){
                for(int hei = 1; hei <= size; ++hei){

                    double result = sqrt((wid+hei) * (wid+hei) + len * len);
                    double result2 = sqrt((hei+len) * (hei+len) + wid * wid);
                    double result3 = sqrt((wid+len) * (wid+len) + hei * hei);

                    if(result == min(result,min(result2,result3)) && result - floor(result) == 0){
                        inner = make_pair(wid,hei);
                        outer = make_pair(len,inner);

                        if(seen.count(outer) != 0){
                            continue;
                        }

                        seen.insert(outer);
                        inner = make_pair(hei,wid);
                        outer = make_pair(len,inner);
                        seen.insert(outer);

                        cout << len << " " << wid << " " << hei << " " << result << " " << result2 << " " << result3 << endl;

                        total++;
                    }
                    if(result2 == min(result,min(result2,result3)) && result2 - floor(result2) == 0){
                        inner = make_pair(len,hei);
                        outer = make_pair(wid,inner);

                        if(seen.count(outer) != 0){
                            continue;
                        }

                        seen.insert(outer);
                        inner = make_pair(hei,len);
                        outer = make_pair(wid,inner);
                        seen.insert(outer);

                        //cout << len << " " << wid << " " << hei << " " << result << " " << result2 << " " << result3 << endl;
                        total++;
                    }
                    if(result3 == min(result,min(result2,result3)) && result3 - floor(result3) == 0){
                        inner = make_pair(wid,len);
                        outer = make_pair(hei,inner);

                        if(seen.count(outer) != 0){
                            continue;
                        }

                        seen.insert(outer);
                        inner = make_pair(len,wid);
                        outer = make_pair(hei,inner);
                        seen.insert(outer);

                        //cout << len << " " << wid << " " << hei << " " << result << " " << result2 << " " << result3 << endl;
                        total++;
                    }

                }
            }
        }

        cout << size << ": " << total << endl;

        if(total >= 1000000){
            cout << size << endl;
        }
    }

    return 0;
}
