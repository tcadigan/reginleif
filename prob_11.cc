#include <iostream>

using namespace std;

int main(){
    int grid[20][20];
  
    int temp;
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 20; j++){
            cin >> temp;
            grid[i][j] = temp;
        }
    }
    /*
      for(int i = 0; i < 20; i++){
      for(int j = 0; j < 20; j++){
      cout << grid[i][j];
      if(j != 19){
      cout << "\t";
      }
      else{
      cout << endl;
      }
      }
      }
    */
    int best = 0;

    // Check down
    for(int i = 0; i < 17; i++){
        for(int j = 0; j < 20; j++){
            int sum = grid[i][j] * grid[i+1][j] * grid[i+2][j] * grid[i+3][j];
            if(sum > best){
                best = sum;
            }
        }
    }
  
    // check up
    for(int i = 3; i < 20; i++){
        for(int j = 0; j < 20; j++){
            int sum = grid[i][j] * grid[i-1][j] * grid[i-2][j] * grid[i-3][j];
            if(sum > best){
                best = sum;
            }
        }
    }

    // check right
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 17; j++){
            int sum = grid[i][j] * grid[i][j+1] * grid[i][j+2] * grid[i][j+3];
            if(sum > best){
                best = sum;
            }
        }
    }

    // check left
    for(int i = 0; i < 20; i++){
        for(int j = 3; j < 20; j++){
            int sum = grid[i][j] * grid[i][j-1] * grid[i][j-2] * grid[i][j-3];
            if(sum > best){
                best = sum;
            }
        }
    }
  
    // upper right
    for(int i = 3; i < 20; i++){
        for(int j = 0; j < 17; j++){
            int sum = grid[i][j] * grid[i-1][j+1] * grid[i-2][j+2] * grid[i-3][j+3];
            if(sum > best){
                best = sum;
            }
        }
    }

    // upper left
    for(int i = 3; i < 20; i++){
        for(int j = 3; j < 20; j++){
            int sum = grid[i][j] * grid[i-1][j-1] * grid[i-2][j-2] * grid[i-3][j-3];
            if(sum > best){
                best = sum;
            }
        }
    }

    // lower right
    for(int i = 0; i < 17; i++){
        for(int j = 0; j < 17; j++){
            int sum = grid[i][j] * grid[i+1][j+1] * grid[i+2][j+2] * grid[i+3][j+3];
            if(sum > best){
                best = sum;
            }
        }
    }

    // lower left
    for(int i = 0; i < 17; i++){
        for(int j = 3; j < 20; j++){
            int sum = grid[i][j] * grid[i+1][j-1] * grid[i+2][j-2] * grid[i+3][j-3];
            if(sum > best){
                best = sum;
            }
        }
    }

    cout << best << endl;

    return 0;
}
