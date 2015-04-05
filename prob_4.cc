#include <iostream>
#include <sstream>

using namespace std;

bool isp(const char * d, unsigned int s){
    int i, j;
    for(i = 0, j = s-1; i < j; i++, j--){
        if(d[i] != d[j]){
            return false;
        }
    }
    return true;
}


int main(){

    stringstream val;
    string res;
    int largest = 0;
    for(int i = 100; i < 1000; i++){
        for(int j = 100; j < 1000; j++){
            int result = i * j;
            val << result;
            val >> res;
            if(isp(res.data(),res.size())){
                if(result > largest){
                    largest = result;
                }
            }
            val.clear();
        }
    }
  
    cout << largest << endl;
    return 0;
}
