#include<iostream>
#include<vector>

using namespace std;

int main(){
    vector<vector<int> > grid;

    int num;

    for(int i = 0; i < 80; ++i){
        for(int j = 0; j < 80; ++j){
            cin >> num;
            if(j == 0){
                vector<int> row;
                row.push_back(num);
                grid.push_back(row);
            }
            else{
                grid[i].push_back(num);
            }
        }
    }
    /*
      for(int i = 0; i < 80; ++i){
      for(int j = 0; j < 80; ++j){
      cout << grid[i][j] << " ";
      }
      cout << endl;
      }
    */

    for(int i = 0; i < 80; ++i){
        for(int j = 0; j < 80; ++j){
            if(i == 0 && j == 0){
                continue;
            }
            else if(i == 0){
                grid[i][j] += grid[i][j-1];
            }
            else if(j == 0){
                grid[i][j] += grid[i-1][j];
            }
            else{
                grid[i][j] += min(grid[i-1][j],grid[i][j-1]);
            }
        }
    }

    cout << grid[grid.size()-1].at(grid[grid.size()-1].size()-1) << endl;
    return 0;
}
