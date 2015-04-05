#include <iostream>
#include <string.h>

using namespace std;

int main(){
    char arr[1000];
    memset(arr, 0, 1000);
    char res[1000];
    memset(res, 0, 1000);
    arr[999] = 1;

    char mult[3];
    memset(mult, 0, 3);
    /*
      cout << "arr is: ";
      for(int j = 0; j < 1000; j++){
      cout << (int)arr[j];
      }
      cout << endl;
    */
    for(int l = 1; l <= 100; l++){
        mult[2] =  l;
        for(int j = 2; j > 0; j--){
            if(mult[j] >= 10){
                mult[j-1] = mult[j] / 10;
                mult[j] %= 10;
            }
        }
        /*
          cout << "mult is: ";
          for(int j = 0; j < 3; j++){
          cout << (int)mult[j];
          }
          cout << endl;
        */
        int offset = 0;
        for(int j = 2; j >= 0; j--){
            for(int k = 999; k >= 0; k--){
                res[k-offset] += mult[j] * arr[k];
	
                if(res[k-offset] >= 10){
                    res[k-1-offset] += res[k-offset] / 10;
                    res[k-offset] %= 10;
                }
            }
            offset++;
        }

        for(int j = 0; j < 1000; j++){
            arr[j] = res[j];
        }
        memset(res,0,1000);
        /*
          cout << "arr is: ";
          for(int j = 0; j < 1000; j++){
          cout << (int)arr[j];
          }
          cout << endl;
        */
    }
  
    int sum = 0;
    for(int j = 0; j < 1000; j++){
        sum += arr[j];
    }

    cout << sum << endl;

    return 0;
}
