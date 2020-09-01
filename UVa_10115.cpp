#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(){
    int cases;
    string junk;

    cin >> cases;
    getline(cin,junk);

    while(cases != 0){
        string find;
        string with;
        string inp;
        vector<string> fin;
        vector<string> wit;

        for(int i = 0; i < cases; i++){
            getline(cin,find);
            fin.push_back(find);
            getline(cin,with);
            wit.push_back(with);

            //cout << find << " : " << with << endl;
        }
        getline(cin,inp);


        for(unsigned int i = 0; i < fin.size(); i++){
            string test = fin[i];
            unsigned long int find = inp.find(test, 0);
            while(find != string::npos){
                //cout << "found at: " << find << endl;
                string before = inp.substr(0,find);
                string after = inp.substr(find+test.length(),inp.length()-find+test.length());
                inp = "";
                inp.append(before);
                inp.append(wit[i]);
                inp.append(after);
                //cout << "result: " << inp << endl;
                find = inp.find(test,0);
            }
        }

        cout << inp << endl;

        cin >> cases;
        getline(cin,junk);
    }

    return 0;
}
