#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(){
    char key[] = "aaa";

    while(1){

        ifstream in;
        in.open("cipher1.txt");
    
        string temp = "", temp2 = "", temp3 = "";
        bool first = true;
        int sum = 0;
        while(in >> temp >> temp2 >> temp3){
            char res = key[0]^atoi(temp.data());
            char res2 = key[1]^atoi(temp2.data());
            char res3 = key[2]^atoi(temp3.data());
            sum += (int)res + (int)res2 + (int)res3;
            if(first && (res != '(' || res2 != 'T' || res3 != 'h')){
                break;
            }
            if(first){
                cout << key[0] << "," << key[1] << "," << key[2] << endl;
                first = false;
            }
      
            cout << res << res2 << res3;
        }
        if(!first){
            if(temp.length() != 0){
                char res = key[0]^atoi(temp.data());
                sum += res;
                cout << res;
            }

            cout << endl << sum << endl;
        }

        key[0]++;
        if(key[0] > 'z'){
            key[0] -= 26;
            key[1]++;
            if(key[1] > 'z'){
                key[1] -= 26;
                key[2]++;
                if(key[2] > 'z'){
                    break;
                }
            }
        }
        in.close();
    }
}
