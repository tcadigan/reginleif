#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

int main(){
    stringstream inp;
    for(int i = 1; i < 1000001; ++i){
        inp << i;
        string num;
        inp >> num;

        int counts[10];
        memset(counts, 0, sizeof(int)*10);

        for(unsigned int j = 0; j < num.length(); ++j){
            counts[num[j]-'0']++;
        }

        inp.clear();

        int multiple;
        for(unsigned int k = 2; k <= 6; ++k){
            multiple = i * k;

            inp << multiple;
            string mnum;
            inp >> mnum;

            int mcounts[10];
            memset(mcounts, 0, sizeof(int)*10);

            for(unsigned int j = 0; j < mnum.length(); ++j){
                mcounts[mnum[j]-'0']++;
            }

            inp.clear();

            bool flag = true;
            for(unsigned int l = 0; l < 10; ++l){
                if(counts[l] != mcounts[l]){
                    flag = false;
                }
            }
            if(!flag){
                break;
            }
            else if(flag && k == 6){
                cout << i << endl;
                return 0;
            }
        }
    }

    return 0;
}
