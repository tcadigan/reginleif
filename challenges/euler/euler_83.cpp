#include<iostream>
#include<vector>

using namespace std;

pair<int,int> findcurr(vector<vector<pair<int,int> > > dgrid){
    int min = 9999999;
    pair<int,int> p;
    p = make_pair(-1,-1);

    for(unsigned int i = 0; i < dgrid.size(); ++i){
        for(unsigned int j = 0; j < dgrid[0].size(); ++j){
            if(dgrid[i][j].second == 0 && dgrid[i][j].first < min){
                p = make_pair(i,j);
                min = dgrid[i][j].first;
            }
        }
    }

    return p;
}

int main(){
    int size = 80;

    vector<vector<int> > grid;
    vector<vector<pair<int,int> > > dgrid;

    int num;
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
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

    pair<int, int> p;
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            if(j == 0){
                vector<pair<int,int> > row;
                if(i == 0){
                    p = make_pair(grid[0][0],0);
                }
                else{
                    p = make_pair(9999999,0);
                }
                row.push_back(p);
                dgrid.push_back(row);
            }
            else{
                p = make_pair(9999999,0);
                dgrid[i].push_back(p);
            }
        }
    }

    pair<int, int> current;
    current = make_pair(0,0);

    while(current.first != -1 && current.second != -1){
        if(current.first != 0 && dgrid[current.first-1][current.second].second == 0){
            dgrid[current.first-1][current.second].first = min(dgrid[current.first-1][current.second].first, dgrid[current.first][current.second].first+grid[current.first-1][current.second]);
        }
        if(current.first != size-1 && dgrid[current.first+1][current.second].second == 0){
            dgrid[current.first+1][current.second].first = min(dgrid[current.first+1][current.second].first, dgrid[current.first][current.second].first+grid[current.first+1][current.second]);
        }
        if(current.second != 0 && dgrid[current.first][current.second-1].second == 0){
            dgrid[current.first][current.second-1].first = min(dgrid[current.first][current.second-1].first, dgrid[current.first][current.second].first+grid[current.first][current.second-1]);
        }
        if(current.second != size-1 && dgrid[current.first][current.second+1].second == 0){
            dgrid[current.first][current.second+1].first = min(dgrid[current.first][current.second+1].first, dgrid[current.first][current.second].first+grid[current.first][current.second+1]);
        }

        dgrid[current.first][current.second].second = 1;
        /*
          for(int i = 0; i < size; ++i){
          for(int j = 0; j < size; ++j){
          cout << "<" << dgrid[i][j].first << "," << dgrid[i][j].second << "> ";
          }
          cout << endl;
          }
        */
        current = findcurr(dgrid);
        //cout << "new current is: " << "(" << current.first << "," << current.second << ")" << endl;
    }

    cout << dgrid[size-1][size-1].first << endl;
  
    return 0;
}
