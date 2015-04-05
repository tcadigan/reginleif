#include <iostream>
#include <string>
#include <vector>

int number = 1;

using namespace std;

int main(){
    string input;
    vector<string> ins;

    cin >> input;

    while(!cin.fail()){
        if(input == "9"){
            int good = 1;
            string tester;
            int found;
            for(unsigned int i = 0; i < ins.size(); i++){
                tester = ins[i];
                for(unsigned int j = 0; j < ins.size(); j++){
                    //cout << tester << ", " << ins[j] << endl;
                    if(i != j){
                        found = ins[j].find(tester,0);
                        if(found == 0){
                            good = 0;
                        }
                    }
                }
            }
            if(good){
                cout << "Set " << number << " is immediately decodable" << endl;
            }
            else{
                cout << "Set " << number << " is not immediately decodable" << endl;
            }
            number++;
            main();
        }
    
        ins.push_back(input);
        cin >> input;
    }
  

    return 0;
}
