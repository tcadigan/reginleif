#include <cmath>
#include <iostream>
#include <set>

using namespace std;

struct triplet {
    long long m;
    long long d;
    long long a;
};

struct tripletCompare {
    bool operator()(struct triplet rhs, struct triplet lhs)
    {
        if(rhs.m == lhs.m) {
            if(rhs.d == lhs.d) {
                return rhs.a < lhs.a;
            }
            return rhs.d < lhs.d;
        }
        return rhs.m < lhs.m;
    }
};

struct triplet compute(struct triplet trip, int s)
{
    triplet result;
    result.m = (trip.d * trip.a) - trip.m;
    result.d = (s - (result.m * result.m)) / trip.d;
    result.a = floor((sqrt(s) + result.m) / result.d);

    return result;
}

int main(int argc, char *argv[])
{
    int odd;

    for(int i = 2; i < 10000; ++i) {
        if(floor(sqrt(i)) == ceil(sqrt(i))) {
            continue;
        }

        set<struct triplet, tripletCompare> triplets;
        
        struct triplet trip;
        trip.m = 0;
        trip.d = 1;
        trip.a = floor(sqrt(i));
        
        triplets.insert(trip);
        
        struct triplet result;
        int iterations = 0;
        bool first = true;

        cout << "sqrt(" << i << ")=[" << trip.a << ";(";
        while(true) {            
            result = compute(trip, i);
            
            if(triplets.find(result) == triplets.end()) {
                triplets.insert(result);
            trip = result;
            }
            else {
                break;
            }
            ++iterations;
            
            if(!first) {
                cout << ",";
            }
            else {
                first = false;
            }

            cout << trip.a;
        }
        
        cout <<")], period=" << iterations << endl;

        if(iterations % 2 == 1) {
            ++odd;
        }
    }

    cout << "Odd iterations -> " << odd << endl;

    return 0;
}
