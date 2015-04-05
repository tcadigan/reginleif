#include <iostream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <stdlib.h>

using namespace std;

int main(){
    int value = 1000;
    double val = pow(2,value);

    stringstream inp;
    inp << setprecision(500) << val;
    string res;
    inp >> res;

    double sum = 0;
    for(unsigned int i = 0; i < res.size(); i++){
        char a = res[i];
        sum += atoi(&a);
    }

    cout << sum << endl;
	
    return 0;
}
