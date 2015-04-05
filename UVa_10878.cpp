#include <iostream>
#include <string>
#include <math.h>

using namespace std;

int main(){
    string line;
  
    getline(cin,line);
    getline(cin,line);

    while(line != "___________"){
        int sum = 0;
        int power = 0;
        char p;
        for(int i = line.length()-2; i > 1; i--){
            p = line[i];
            //cout << p << endl;
            if(p == 'o'){
                sum += pow(2,power);
            }
            if(p != '.'){
                power++;
            }
            //cout << "power: " << power << " sum: " << sum << endl;
        }

        cout << (char)sum;
        getline(cin,line);
    }

    return 0;
}
