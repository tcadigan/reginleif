#include <iostream>
#include <cstring>
#include <set>
using namespace std;

pair<char,char> findzero(char board[9][9]){
    pair<char,char> result = make_pair(-1,-1);

    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            if(board[i][j] == 0){
                result.first = i;
                result.second = j;
                return result;
            }
        }
    }

    return result;
}

set<char> setintersect(set<char> a, set<char> b){
    set<char> result;

    for(int i = 1; i < 10; ++i){
        if(a.count(i) != 0 && b.count(i) != 0){
            result.insert(i);
        }
    }

    return result;
}

bool checkboard(char board[9][9]){
    char count[9];

    for(int i = 0; i < 9; ++i){
        memset(count, 0, sizeof(char)*9);
        for(int j = 0; j < 9; ++j){
            count[board[i][j]-1]++;
        }

        for(int j = 0; j < 9; ++j){
            if(count[j] != 1){
                return false;
            }
        }
    }

    for(int j = 0; j < 9; ++j){
        memset(count, 0, sizeof(char)*9);
        for(int i = 0; i < 9; ++i){
            count[board[i][j]-1]++;
        }

        for(int i = 0; i < 9; ++i){
            if(count[i] != 1){
                return false;
            }
        }
    }
  
    // check minor squares
    for(int i = 0; i < 9; i += 3){
        for(int j = 0; j < 9; j += 3){
            memset(count, 0, sizeof(char)*9);
            for(int k = i; k < i+3; ++k){
                for(int l = j; l < j+3; ++l){
                    count[board[k][l]-1]++;
                }
            }
            for(int k = 0; k < 9; ++k){
                if(count[k] != 1){
                    return false;
                }
            }
        }
    }

    return true;
}

set<char> rowleft(char board[9][9], int r){
    set<char> left;

    char count[10];
    memset(count, 0, sizeof(char)*10);

    for(int i = 0; i < 9; ++i){
        count[(int)board[r][i]]++;
    }

    for(int i = 1; i < 10; ++i){
        if(count[i] == 0){
            left.insert(i);
        }
    }
  
    return left;
}

set<char> colleft(char board[9][9], int c){
    set<char> left;

    char count[10];
    memset(count, 0, sizeof(char)*10);

    for(int i = 0; i < 9; ++i){
        count[(int)board[i][c]]++;
    }

    for(int i = 1; i < 10; ++i){
        if(count[i] == 0){
            left.insert(i);
        }
    }
  
    return left;
}

set<char> sqleft(char board[9][9], pair<int,int> p){
    set<char> left;

    char count[10];
    memset(count, 0, sizeof(char)*10);

    for(int i = p.first-(p.first % 3); i < p.first-(p.first % 3)+3; ++i){ 
        for(int j = p.second-(p.second % 3); j < p.second-(p.second % 3)+3; ++j){
            count[(int)board[i][j]]++;
        }
    }

    for(int i = 1; i < 10; ++i){
        if(count[i] == 0){
            left.insert(i);
        }
    }
 
    return left;
}

void printboard(char board[9][9]){
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            cout << (int)board[i][j];
            if(j != 8){
                cout << "\t";
            }
        }
        cout << endl;
    }
}

int solve(char board[9][9]){
    pair<char,char> zero = findzero(board);
    if(zero.first == -1 && zero.second == -1){
        if(checkboard(board)){
            cout << "Solution" << endl;
            printboard(board);
            return board[0][0]*100+board[0][1]*10+board[0][2];
        }
        else{
            return -1;
        }
    }
    else{
    
        set<char> possible = setintersect(rowleft(board,zero.first), colleft(board,zero.second));
        possible = setintersect(possible,sqleft(board,zero));
        set<char>::iterator it;
        for(it = possible.begin(); it != possible.end(); ++it){
            char nboard[9][9];
            memcpy(nboard,board, sizeof(char)*81);
            nboard[(int)zero.first][(int)zero.second] = *it;
            int res = solve(nboard);
            if(res != -1){
                return res;
            }
        }
        return -1;
    }
}

int main(){
    string inp;
    int num;

    char board[9][9];

    int total = 0;
    while(cin >> inp >> num){
        cout << inp << " " << num << endl;
        memset(board, 0, sizeof(char)*81);
        string line;
        for(int i = 0; i < 9; ++i){
            cin >> line;
            for(unsigned int j = 0; j < line.size(); ++j){
                board[i][j] = line[j]-'0';
            }
        }

        printboard(board);
        total += solve(board);
        //break;
    }

    cout << total << endl;

    return 0;
}
