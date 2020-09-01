#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

set<int> primes;
map<int, vector<int> > factors;

void gen_primes() {
    primes.insert(2);
    primes.insert(3);
    primes.insert(5);
    primes.insert(7);
    primes.insert(11);
    primes.insert(13);
    primes.insert(17);
    primes.insert(19);
    primes.insert(23);
    primes.insert(29);
    primes.insert(31);
    primes.insert(37);
    primes.insert(41);
    primes.insert(43);
    primes.insert(47);
    primes.insert(53);
    primes.insert(59);
    primes.insert(61);
    primes.insert(67);
    primes.insert(71);
    primes.insert(73);
    primes.insert(79);
    primes.insert(83);
    primes.insert(89);
    primes.insert(97);
}

void factor(int x) {
    int index = x;
    
    set<int>::iterator itr;

    vector<int> result;
    
    for(itr = primes.begin(); itr != primes.end(); ++itr) {
        int count = 0;
        
        while(x % *itr == 0) {
            ++count;
            x /= *itr;
        }

        result.push_back(count);
    }

    factors[index] = result;
}

void add(vector<int> &result, vector<int> &to_add) {
    for(unsigned int i = 0; i < to_add.size(); ++i) {
        result[i] += to_add[i];
    }
}

void print(vector<int> result) {
    int index = 0;
    
    for(unsigned int i = 0; i < result.size(); ++i) {
        if(result.at(i) != 0) {
            index = i;
        }
    }

    for(int i = 0; i <= index; ++i) {
        if((i != 0) && (i % 15 == 0)) {
            cout << endl << "      ";
        }
        
        cout << setw(3) << result.at(i);
    }
}

void factorial(int x) {
    vector<int> result = factors.at(x);
    
    for(int i = x - 1; i > 1; --i) {
        add(result, factors.at(i));
    }

    cout << setw(3) << x << "! =";
    print(result);
    cout << endl;
}

int main(int argc, char *argv[]) {
    gen_primes();
    
    for(int i = 2; i <= 100; ++i) {
        factor(i);
    }

    int value;
    
    while(cin >> value) {
        if(value == 0) {
            break;
        }
        else {
            factorial(value);
        }
    }
    
    return 0;
}
