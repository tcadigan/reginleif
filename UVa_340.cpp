#include <iostream>
#include <string.h>

using namespace std;

int main(){
    int game = 1;

    int length;

    int code[1000];
    int code_count[10];
    int code_working_count[10];
    int guess_count[10];

    while(cin >> length){
        if(length == 0){
            break;
        }

        cout << "Game " << game << ":" << endl;

        memset(code, 0, sizeof(int)*1000);
        memset(code_count, 0, sizeof(int)*10);
    
        int temp;
        for(int i = 0; i < length; ++i){
            cin >> temp;
            code[i] = temp;
            code_count[temp] += 1;
        }

        while(1){
            int strong = 0;
            int weak = 0;
            memset(guess_count, 0, sizeof(int)*10);
            memcpy(code_working_count, code_count, sizeof(int)*10);
      
            for(int i = 0; i < length; ++i){
                cin >> temp;
                if(temp == code[i]){
                    ++strong;
                    code_working_count[temp] -= 1;
                }
                else{
                    guess_count[temp] += 1;
                }
            }

            if(guess_count[0] == length){
                break;
            }

            for(int i = 0; i < 10; ++i){
                if(guess_count[i] < code_working_count[i]){
                    weak += guess_count[i];
                }
                else{
                    weak += code_working_count[i];
                }
            }

            cout << "    (" << strong << "," << weak << ")" << endl;
        }

        ++game;
    }

    return 0;
}
