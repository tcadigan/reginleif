#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

int main(){
  
    double size = 20;

    double result = 1;

    // Iterative formula for nCr, not going to work well for larger values...
    for(double i = 1; i <= size; i++){
        result = result * ((2*size-size+i)/i);
    }

    // ceil needed for numerical imprecision
    cout << setprecision(200) << ceil(result) << endl;

    return 0;
}

/*
// The brute force way.

#include <iostream>

using namespace std;

unsigned long count = 0;

void rec(int x, int y, int places, int size){
if(places == size + size){
count++;
}
else if(x == size){
places++;
rec(x,y+1,places,size);
}
else if(y == size){
places++;
rec(x+1,y,places,size);
}
else{
places++;
rec(x+1,y,places,size);
rec(x,y+1,places,size);
}
}

int main(){

rec(0,0,0,20);

cout << count << endl;

return 0;
}
*/
