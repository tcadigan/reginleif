#include <iostream>
#include <cmath>

using namespace std;

int main(){
    int number, lower_coord, upper_coord, lower_time, upper_time;
    double mid;

    while(cin >> number){
        if (number == 0){
            break;
        }

        mid = sqrt(number);
        lower_coord = floor(mid);
        upper_coord = ceil(mid);

        if(lower_coord - upper_coord == 0){
            if(number % 2 == 0){
                cout << lower_coord << " " << 1 << endl;
            }
            else{
                cout << 1 << " " << upper_coord << endl;
            }
            continue;
        }

        lower_time = lower_coord * lower_coord;
        upper_time = upper_coord * upper_coord;
    
        if (upper_time % 2 == 1){ 
            if(upper_time - number < upper_coord){
                cout << upper_time - number + 1 << " " << upper_coord << endl;
            }
            else{
                cout << lower_coord + 1 << " " << number - lower_time << endl;
            }
        }
        else{
            if(upper_time - number < upper_coord){
                cout << upper_coord << " " << upper_time - number + 1 << endl;
            }
            else{
                cout << number - lower_time << " " << lower_coord + 1<< endl;
            }
        }
    }

    return 0;
}
