#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

int main (int argc, char* argv[]){
    //cout << "Hello World!" << endl;

    string first = "";
    string second = "";
    bool strip = true;

    while(cin >> first >> second){
        int size = first.length() + second.length();
        int res[size];
        for(int k = 0; k < size; k++){
            res[k] = 0;
        }

        int offset = 0;
        //cout << first << " " << first.length() << " " << second << " " << second.length() << endl;

        for(int i = second.length()-1; i >= 0; --i){
            int current = 1;
            for(int j = first.length()-1; j >= 0; --j){
                res[size-current-offset] += (second[i] -'0') * (first[j] -'0');
                if(res[size-current-offset] >= 10){
                    res[size-current-offset-1] += res[size-current-offset]/10;
                    res[size-current-offset] = res[size-current-offset] % 10;
                }
                current++;
            }
            offset++;
        }

        for(int i = 0; i < size; i++){
            if(res[i] == 0 && strip){
                continue;
            }
            else{
                cout << res[i];
                strip = false;
            }
        }
        if(strip == true){
            cout << 0;
        }
        cout << endl;

        strip = true;
        first = "";
        second = "";
    }
    return 0;
}
