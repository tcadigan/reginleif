#include <iostream>

using namespace std;

int main(){
    string line;
    string::iterator it;

    bool first = true;
    while(getline(cin,line)){
    
        for(it = line.begin(); it != line.end(); ++it){
            if(*it == '"'){
                if(first){
                    line.replace(it, it+1, 2, '`');
                    first = false;
                }
                else{
                    line.replace(it,it+1,2,'\'');
                    first = true;
                }
                it = line.begin();
            }
      
        }

        cout << line << endl;
    }

    return 0;
}
