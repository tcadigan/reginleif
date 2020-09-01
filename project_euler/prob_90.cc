#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <string.h>
#include <sstream>

using namespace std;

int main(){
    string d1 = "0123456789";
    string d2 = "0123456789";
    set<string> d1seen;
    set<string> d2seen;

    set<string> comp;

    string temp;
    do{
        temp = d1.substr(4);
        sort(temp.begin(), temp.end());
        d1seen.insert(temp);
    }while(next_permutation(d1.begin(), d1.end()));
  
    do{
        temp = d2.substr(4);
        sort(temp.begin(), temp.end());
        d2seen.insert(temp);
    }while(next_permutation(d2.begin(), d2.end()));

    set<string>::iterator it;
    set<string>::iterator it2;
    string d1str, d2str;
    int count = 0;
    for(it = d1seen.begin(); it != d1seen.end(); ++it){
        d1str = *it;
    
        int firstd[10];
        memset(firstd,0,sizeof(int)*10);
        for(unsigned int i = 0; i < d1str.size(); ++i){
            if(d1str[i]-'0' == 9){
                firstd[6]++;
            }
            else{
                firstd[d1str[i]-'0']++;
            }
        }

        for(it2 = d2seen.begin(); it2 != d2seen.end(); ++it2){
            d2str = *it2;
    
            int secondd[10];
            memset(secondd,0,sizeof(int)*10);
            for(unsigned int i = 0; i < d2str.size(); ++i){
                if(d2str[i]-'0' == 9){
                    secondd[6]++;
                }
                else{
                    secondd[d2str[i]-'0']++;
                }
            }
      
            if(!((firstd[0] != 0 && secondd[1] != 0) || (firstd[1] != 0 && secondd[0] != 0))){
                continue;
            }
      
            if(!((firstd[0] != 0 && secondd[4] != 0) || (firstd[4] != 0 && secondd[0] != 0))){
                continue;
            }

            if(!((firstd[0] != 0 && secondd[6] != 0) || (firstd[6] != 0 && secondd[0] != 0))){
                continue;
            }

            if(!((firstd[1] != 0 && secondd[6] != 0) || (firstd[6] != 0 && secondd[1] != 0))){
                continue;
            }

            if(!((firstd[2] != 0 && secondd[5] != 0) || (firstd[5] != 0 && secondd[2] != 0))){
                continue;
            }

            if(!((firstd[3] != 0 && secondd[6] != 0) || (firstd[6] != 0 && secondd[3] != 0))){
                continue;
            }

            if(!((firstd[6] != 0 && secondd[4] != 0) || (firstd[4] != 0 && secondd[6] != 0))){
                continue;
            }

            if(!((firstd[8] != 0 && secondd[1] != 0) || (firstd[1] != 0 && secondd[8] != 0))){
                continue;
            }

            count++;
            //cout << d1str+d2str << endl;
        }
    }
    cout << count/2 << endl;

    return 0;
}
