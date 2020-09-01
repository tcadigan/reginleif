#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>

using namespace std;

int distance(int x1, int y1, int z1, int x2, int y2, int z2){
    return (int)sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

int main(){
    int x, y, z;

    int counts[10] = {0};
    vector<pair<int, pair<int,int> > > points;

    while(cin >> x >> y >> z){
   
        if(x == 0 && x == y && x == z){
            break;
        }

        pair<int,int> p(y,z);
        pair<int, pair<int,int> > result(x, p);
        points.push_back(result);
    }

    int result;
    int closest;
    for(unsigned int x = 0; x < points.size(); ++x){
        closest = 11;
        for(unsigned int y = 0; y < points.size(); ++y){
            if(x != y){
                result = distance(points[x].first, points[x].second.first, points[x].second.second, points[y].first, points[y].second.first, points[y].second.second);
                if(result < closest){
                    closest = result;
                }
            }
        }
        if(closest < 10){
            counts[closest] += 1;
        }
    }

    for(int x = 0; x < 10; ++x){
        cout << right << setw(4) << counts[x];
    }

    cout << endl;

    return 0;
}

