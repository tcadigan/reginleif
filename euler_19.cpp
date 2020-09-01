#include <iostream>

using namespace std;

int main(){

    int count = 0;
    int day = 1;

    for(int i = 1900; i <= 2000; i++){
        for(int j = 1; j <= 12; j++){
            day = day % 7;
            if(day == 0 && i != 1900){
                //cout << i << " " << j << endl;
                count++;
            }
            switch(j){
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                day += 31;
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                day += 30;
                break;
            default:
                //cout << i << " ";
                if(i % 4 == 0 && i % 100 == 0 && i % 400 == 0){
                    day += 29;
                    //cout << "leap" << endl;
                }
                else if(i % 4 == 0 && i % 100 == 0){
                    day += 28;
                    //cout << "not leap" << endl;
                }
                else if(i % 4 == 0){
                    day += 29;
                    //cout << "leap" << endl;
                }
                else{
                    day += 28;
                    //cout << "not leap" << endl;
                }
            }
        }
    }

    cout << count << endl;

    return 0;
}
