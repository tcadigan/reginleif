#include <iostream>
#include <string>
#include <vector>

int set = 1;

using namespace std;

int main(){
    int keys;
    int excuses;
    int max = 0;
    string junk;
    vector<string> keywords;
    vector<string>::iterator k_itr;
    vector<string> lines;
    vector<string>::iterator l_itr;
    vector<string> r_lines;
    vector<string>::iterator rl_itr;
    vector<int> count;
    vector<int>::iterator c_itr;

    cin >> keys >> excuses;

    if(!cin.eof()){
        getline(cin, junk);
        while(keys > 0){
            getline(cin,junk);
            keywords.push_back(junk);
            //cout << junk << endl;
            keys--;
        }
        while(excuses > 0){
            getline(cin,junk);
            r_lines.push_back(junk);
            for(unsigned int i = 0; i < junk.length(); i++){
                junk[i] = tolower(junk[i]);
            }
            lines.push_back(junk);
            //cout << junk << endl;
            excuses--;
        }

        unsigned long int found;
        int counter = 0;
        string ex;
        string key;
        for(l_itr = lines.begin(); l_itr < lines.end(); ++l_itr){
            for(k_itr = keywords.begin(); k_itr < keywords.end(); ++k_itr){
                ex = *l_itr;
                key = *k_itr;
                //cout << ex << " with " << key << endl;

                found = ex.find(key,0);
                while(found != string::npos){
                    //cout << "testing at: " << found << ": " << "\"" << ex[found-1] << "\" \"" << ex[found+key.length()] << "\"" << endl;
                    if(found == 0 && !isalpha(ex[key.length()])){
                        //cout << "found at: " << found << endl;
                        counter++;
                        found = ex.find(key,key.length());
                    }
                    else if(!isalpha(ex[found-1]) && !isalpha(ex[found+key.length()])){
                        //cout << "found at: " << found << endl;
                        counter++;
                        found = ex.find(key,found+key.length());	    
                    }
                    else{
                        found = ex.find(key,found+key.length());
                    }
                }
            }
            count.push_back(counter);
            //cout << *l_itr << " has " << counter << endl;
            if(counter > max){
                max = counter;
            }
            counter = 0;
        }

        cout << "Excuse Set #" << set << endl;
        for(c_itr = count.begin(), rl_itr = r_lines.begin(); rl_itr < r_lines.end(); c_itr++, rl_itr++){
            if(*c_itr == max){
                cout << *rl_itr << endl;
            }
        }

        cout << endl;

        set++;
        main();
    }  
    return 0;
}
