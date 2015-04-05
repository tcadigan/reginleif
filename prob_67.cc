#include <iostream>
#include <string.h>
#include <math.h>

using namespace std;

int main(){
    int ydim = 100;
    int xdim = 199;
    int grid[ydim][xdim];
    memset(grid, -1, sizeof(grid));

    int temp;
    for(int i = ydim-1; i >= 0; i--){
        for(int j = 0; j <= i; j++){
            cin >> temp;
            grid[i][(ydim-1-i)+j*2] = temp;
        }
    }

    /*
      for(int i = 0; i < ydim; i++){
      for(int j = 0; j < xdim; j++){
      cout << grid[i][j];
      if(j == xdim-1){
      cout << endl;
      }
      else{
      cout << " ";
      }
      }
      }
    */

    for(int i = ydim-2; i >= 0; i--){
        for(int j = 0; j <= i; j++){
            grid[i][(ydim-1-i)+j*2] += max(grid[i+1][(ydim-1-i)+j*2-1], grid[i+1][(ydim-1-i)+j*2+1]);
        }
        for(int j = 0; j < xdim; j++){
            grid[i+1][j] = -1;
        }
    
        /*
          cout << endl;
          for(int i = 0; i < ydim; i++){
          for(int j = 0; j < xdim; j++){
          cout << grid[i][j];
          if(j == xdim-1){
          cout << endl;
          }
          else{
          cout << " ";
          }
          }
          }
        */
    }

    cout << grid[0][ydim-1] << endl;
  
    return 0;
}
