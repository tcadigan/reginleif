#include <iostream>
#include <vector>

using namespace std;

long long  max(vector<int> const &input) {
    long long here = input.at(0);
    long long so_far = input.at(0);
    
    for(unsigned int i = 1; i < input.size(); ++i) {
        if(input.at(i) > (here + input.at(i))) {
            here = input.at(i);
        }
        else {
            here += input.at(i);
        }
        
        if(here > so_far) {
            so_far = here;
        }
    }

    // cout << "so_far -> " << so_far << endl;

    return so_far;
}

vector<int> collapse(vector<vector<int> > const &input, int top, int bottom)
{
    vector<int> result;
    for(unsigned int i = 0; i < input.at(top).size(); ++i) {
        int sum = 0;
        for(int j = top; j <= bottom; ++j) {
            sum += input.at(j).at(i);
        }

        result.push_back(sum);
    }

    return result;
}

int main(int argc, char *argv[])
{
    vector<vector<int> > input;
    int size;

    cin >> size;
    for(int i = 0; i < size; ++i) {
        vector<int> inner;

        for(int j = 0; j < size; ++j) {
            int x;
            cin >> x;
            inner.push_back(x);
        }

        input.push_back(inner);
    }

    // cout << "size -> " << size << endl;
    // for(int i = 0; i < size; ++i) {
    //     for(int j = 0; j < size; ++j) {
    //         if(j != 0) {
    //             cout << " ";
    //         }
    //         cout << input.at(i).at(j);
    //     }
    //     cout << endl;
    // }

    long long best = input.at(0).at(0);

    for(int i = 0; i < size; ++i) {
        for(int j =  i; j < size; ++j) {
            vector<int> collapsed = collapse(input, i, j);

            // cout << "collapsed -> ";
            // for(int k = 0; k < size; ++k) {
            //     if(k != 0) {
            //         cout << " ";
            //     }
            //     cout << collapsed.at(k);
            // }
            // cout << endl;

            long long result = max(collapsed);

            if(result > best) {
                best = result;
            }

            // cout << "best -> " << best << endl;
        }
    }

    cout << best << endl;
    
    return 0;
}
