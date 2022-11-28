#include <iostream>

using namespace std;

int main(){
    int num_stack;

    cin >> num_stack;

    int trial = 1;

    while(num_stack != 0){
        int heights[50] = {0};
        int sum = 0;
        for(int i = 0; i < num_stack; i++){ 
            cin >> heights[i];
            sum += heights[i];
        }

        int avg = sum / num_stack;
        int moves = 0;

        for(int i = 0; i < num_stack; i++){
            if(heights[i] > avg){
                moves += heights[i] - avg;
            }
        }

        cout << "Set #" << trial << endl;
        cout << "The minimum number of moves is " << moves << "." << endl << endl;

        trial++;
        cin >> num_stack;
    }

    return 0;
}
