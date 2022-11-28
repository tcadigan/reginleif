#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
    int rows;
    int columns;
    long field = 1;

    while(cin >> rows >> columns){
        if(rows == columns && rows == 0){
            break;
        }

        if(field != 1){
            cout << endl;
        }

        int grid[100][100];

        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                grid[i][j] = 0;
            }
        }

        char inp;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                cin >> inp;
                if(inp == '*'){
                    grid[i][j] = -1;
                    if(i - 1 >= 0){
                        if(j - 1 >= 0){
                            if(grid[i-1][j-1] != -1){
                                grid[i-1][j-1]++;
                            }
                        }
                        if(j + 1 < columns){
                            if(grid[i-1][j+1] != -1){
                                grid[i-1][j+1]++;
                            }
                        }
                        if(grid[i-1][j] != -1){
                            grid[i-1][j]++;
                        }
                    }
                    if(i + 1 < rows){
                        if(j - 1 >= 0){
                            if(grid[i+1][j-1] != -1){
                                grid[i+1][j-1]++;
                            }
                        }
                        if(j + 1 < columns){
                            if(grid[i+1][j+1] != -1){
                                grid[i+1][j+1]++;
                            }
                        }
                        if(grid[i+1][j] != -1){
                            grid[i+1][j]++;
                        }
                    }
                    if(j - 1 >= 0){
                        if(grid[i][j-1] != -1){
                            grid[i][j-1]++;
                        }
                    }
                    if(j + 1 < columns){
                        if(grid[i][j+1] != -1){
                            grid[i][j+1]++;
                        }
                    }
                }
            }
        }

        cout << "Field #" << field << ":" << endl;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                if(grid[i][j] == -1){
                    cout << '*';
                }
                else{
                    cout << grid[i][j];
                }
            }
            cout << endl;
        }

        ++field;
    }

    return 0;
}
