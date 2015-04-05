#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

int upright(vector<vector<pair<int,int> > > grid, int i, int j){

    int up=9999999, right=999999;
    if(i != 0){
        up = grid[i-1][j].second;
    }

    right = grid[i][j+1].first;

    return grid[i][j].first + min(up,right);
}

int downright(vector<vector<pair<int,int> > > grid, int i, int j){
    int down=9999999, right=999999;
    if((unsigned)i != grid.size()-1){
        down = grid[i+1][j].second;
    }

    right = grid[i][j+1].first;

    return grid[i][j].first + min(down,right);
}


int main(){
    vector<vector<pair<int,int> > > grid;

    int num;

    int size = 80;

    pair<int,int> p;
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            cin >> num;
            p = make_pair(num,9999999);
            if(j == 0){
                vector<pair<int,int> > row;
                row.push_back(p);
                grid.push_back(row);
            }
            else{
                grid[i].push_back(p);
            }
        }
    }

    for(int i = 0; (unsigned)i < grid.size()-1; ++i){
        for(int j = 0; (unsigned)j < grid.size(); ++j){
            //cout << i << " " << j << endl;
            grid[grid.size()-1-j][grid.size()-2-i].second = min(grid[grid.size()-1-j][grid.size()-2-i].second,downright(grid,grid.size()-1-j,grid.size()-2-i));
        }
        /*
          cout << "after: " << endl;
          for(int a = 0; (unsigned)a < grid.size(); ++a){
          for(int b = 0; (unsigned)b < grid.size(); ++b){
          cout << "<" << grid[a][b].first << "," <<  grid[a][b].second << "> ";
          }
          cout << endl;
          }
        */
        for(int j = 0; (unsigned)j < grid.size(); ++j){
            //cout << i << " " << j << endl;
            grid[j][grid.size()-2-i].second = min(grid[j][grid.size()-2-i].second, upright(grid,j,grid.size()-2-i));
        }
        /*
          cout << "after: " << endl;
          for(int a = 0; (unsigned)a < grid.size(); ++a){
          for(int b = 0; (unsigned)b < grid.size(); ++b){
          cout << "<" << grid[a][b].first << "," <<  grid[a][b].second << "> ";
          }
          cout << endl;
          }
        */
        for(int j = 0; (unsigned)j < grid.size(); ++j){
            grid[j][grid.size()-2-i].first = grid[j][grid.size()-2-i].second;
        } 
    }

    int minimum = 9999999;
    for(int i = 0; (unsigned)i < grid.size(); ++i){
        if(grid[i][0].second < minimum){
            minimum = grid[i][0].second;
        }
    }

    cout << minimum << endl;

    return 0;
}
