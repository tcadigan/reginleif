#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

int main(){
    stringstream inp;
    bool f1 = false; 
    bool f10 = false; 
    bool f100 = false;
    bool f1000 = false; 
    bool f10000 = false;
    bool f100000 = false;
    bool f1000000 = false;
    int count = 0, sum = 1;
    string curr = "";
    char temp;
    for(int i = 1; i < 1500000; ++i){
        inp << i;
        inp >> curr;
        count += curr.size();
        if(!f1 && count >= 1){
            temp = curr[curr.size()-1 - (count-1)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f1 = true;
        }
        else if(!f10 && count >= 10){
            temp = curr[curr.size()-1 - (count-10)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f10 = true;
        }
        else if(!f100 && count >= 100){
            temp = curr[curr.size()-1 - (count-100)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f100 = true;
        }
        else if(!f1000 && count >= 1000){
            temp = curr[curr.size()-1 - (count-1000)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f1000 = true;
        }
        else if(!f10000 && count >= 10000){
            temp = curr[curr.size()-1 - (count-10000)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f10000 = true;
        }
        else if(!f100000 && count >= 100000){
            temp = curr[curr.size()-1 - (count-100000)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f100000 = true;
        }
        else if(!f1000000 && count >= 1000000){
            temp = curr[curr.size()-1 - (count-1000000)];
            //cout << i << " " << sum << " " << count << " " << temp << endl;
            sum *= atoi(&temp);
            f1000000 = true;
        }

        inp.clear();
    }

    cout << sum << endl;

    return 0;
}
