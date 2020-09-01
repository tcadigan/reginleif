#include <iostream>
#include <sstream>
#include <string.h>
#include <math.h>

using namespace std;

int main(){
    double R; 
    unsigned int n;
    string stringR;
    stringstream inp;
    cin >> R >> n;

    while(!cin.eof()){
        inp << R;
        inp >> stringR;
        inp.clear();
        //cout << stringR << " " << n << endl;

        unsigned int length = n;
        for(unsigned int i = 1; i < n; i++){
            length += n;
        }
    
        //cout << "length is: " << length << endl;
    
        char a[length+1];
        memset(a, 0, sizeof(a));
        char b[length+1];
        memset(b,0,sizeof(b));
        char res[length+1];
        memset(res,0,sizeof(res));

        int j;
        unsigned int i;
        int dplace;
        int hasd = 0;

        for(int i = stringR.size()-1; i >= 0; i--){
            if(stringR[i] == '.'){
                dplace = stringR.size()-i-1;
                hasd = 1;
            }
        }
    
        dplace = dplace * n;
        //cout << "final decimal spot: " << dplace << endl;

        for(i = length-stringR.size()+hasd,j=0; i < length; i++,j++){
            if(stringR[j] == '.'){
                i--;
                continue;
            }
            a[i] = stringR[j] - '0';
            b[i] = stringR[j] - '0';
        }

        /*
          cout << "a is: ";
          for(unsigned int i = 0; i < length; i++){
          cout << (int)a[i];
          }
          cout << endl;
          cout << "b is: ";
          for(unsigned int k = 0; k < length; k++){
          cout << (int)b[k];
          }
          cout << endl;
        */
        for(unsigned int l = 2; l <= n; l++){
            int offset = 0;

            for(int i = length-1; i >= 0; i--){
                for(int j = length-1; j >= 0; j--){
                    //cout << length << " " << offset << " " << i << " " << j <<  endl;
                    res[j-offset] += b[i] * a[j];
	  
                    if(res[j-offset] >= 10){
                        res[j-offset-1] += res[j-offset] / 10;
                        res[j-offset] = res[j-offset] % 10;
                    }
                }	
                offset++;
            }
      
            for(unsigned int i = 0; i < sizeof(res); i++){
                a[i] = res[i];
            }
            /*
              cout << "a is: ";
              for(unsigned int i = 0; i < length; i++){
              cout << (int)a[i];
              }
              cout << endl;
            */
            memset(res,0,length);
        }

        char final[length+1];
        memset(final,0,length);

        for(unsigned int i = 0; i < length+1; i++){
            if(i == length-dplace){
                final[i] = '.';
            }
            else if(i < length-dplace){
                final[i] = a[i];
            }
            else{
                final[i] = a[i-1];
            }
        }
    
        int start = 0;
        for(unsigned int i = 0; i < length+1; i++){
            if(final[i] != 0){
                start = i;
                break;
            }
        }

        unsigned int end = length+1;
        for(int i = length; i >= 0; i--){
            if(final[i] != 0){
                end = i;
                break;
            }
        }

        //cout << "final is: ";
        for(unsigned int i = start; i <= end; i++){
            if(final[i] == '.'){
                cout << final[i];
            }
            else{
                cout << (int)final[i];
            }
        }
        cout << endl;

        cin >> R >> n;
    }
    return 0;
}
