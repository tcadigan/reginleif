#include <iostream>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>

using namespace std;

string board[] = {
    "GO", "A1", "CC1", "A2", "T1", "R1", "B1", "CH1", "B2", "B3",
    "JAIL", "C1", "U1", "C2", "C3", "R2", "D1", "CC2", "D2", "D3",
    "FP", "E1", "CH2", "E2", "E3", "R3", "F1", "F2", "U2", "F3",
    "G2J", "G1", "G2", "CC3", "G3", "R4", "CH3", "H1", "T2", "H2"};

string chest[] = { 
    "GO", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK",
    "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "JAIL"};
int chestnum;

string chance[] = {
    "GO", "BLANK", "JAIL", "BLANK", "C1", "E3", "BLANK", "H2",
    "R1", "NEXTR", "BLANK", "NEXTR", "BLANK", "NEXTU", "BLANK", "BACK3"};
int chancenum;

void shuffle(string deck){
    int first, second;
    string temp;
    if(deck == "chest"){
        for(int i = 0; i < 10000000; ++i){
            first = rand() % 16;
            temp = chest[first];
            second = rand() % 16;
            chest[first] = chest[second];
            chest[second] = temp;
        }
    }
    else{
        for(int i = 0; i < 10000000; ++i){
            first = rand() % 16;
            temp = chance[first];
            second = rand() % 16;
            chance[first] = chance[second];
            chance[second] = temp;
        }
    }
}    

bool cmp(pair<string,double> a, pair<string,double> b){
    return a.second > b.second;
}

int main(){
    srand(time(NULL));

    chestnum = 0;
    chancenum = 0;
    shuffle("chest");
    shuffle("chance");
    /*
      for(int i = 0; i < 16; ++i){
      cout << chest[i] << " ";
      }
      cout << endl;

      for(int i = 0; i < 16; ++i){
      cout << chance[i] << " ";
      }
      cout << endl;
    */
    int d1, d2, numrolls = 100000000, loc = 0, move = 0;
    int dierange = 4;
    map<string,double> prob;
    for(int i = 0; i < numrolls; ++i){
        if(i % 10000000 == 0){
            cout << i << endl;
        }
        d1 = rand() % dierange + 1;
        d2 = rand() % dierange + 1;

        move = d1 + d2;
        if(d1 == d2){
            d1 = rand() % dierange + 1;
            d2 = rand() % dierange + 1;
      
            move += d1 + d2;
            if(d1 == d2){
                d1 = rand() % dierange + 1;
                d2 = rand() % dierange + 1;
	
                move += d1 + d2;
                if(d1 == d2){
                    //jail time!
                    if(loc <= 10){
                        move = 10 - loc;
                    }
                    else{
                        move = 40-(loc-10);
                    }
                }
            }
        }

        //cout << "before: " << loc << " " << move << endl;
        loc = (loc+move) % 40;
        //cout << "after: " << loc << " " << move << endl;
        string place = board[loc];

        if(place == "CC1" || place == "CC2" || place == "CC3"){
            string result = chest[chestnum];
            if(result == "GO"){
                loc = 0;
                place = result;
            }
            else if(result == "JAIL"){
                loc = 10;
                place = result;
            }

            chestnum = (chestnum+1) % 16;
        }

        if(place == "CH1" || place == "CH2" || place == "CH3"){
            string result = chance[chancenum];
            if(result == "GO"){
                place = result;
                loc = 0;
            }
            else if(result == "JAIL"){
                loc = 10;
                place = result;
            }
            else if(result == "BACK3"){
                loc = (loc+37) % 40;
                place = board[loc];
            }
            else if(result == "E3"){
                loc = 24;
                place = result;
            }
            else if(result == "H2"){
                loc = 39;
                place = result;
            }
            else if(result == "R1"){
                loc = 5;
                place = result;
            }
            else if(result == "C1"){
                loc = 11;
                place = result;
            }
            else if(result == "NEXTR"){
                loc = (loc+10) % 40;
                while(loc % 5 != 0){
                    --loc;
                }
                if(loc % 10 == 0){
                    loc -= 5;
                    if(loc < 0){
                        loc = 35;
                    }
                }
                place = board[loc];
            }
            else if(result == "NEXTU"){
                if (loc > 28 || loc < 12){
                    loc = 12;
                }
                else{
                    loc = 28;
                }
                place = board[loc];
            }

            chancenum = (chancenum+1) % 16;
        }

        if(place == "G2J"){
            loc = 10;
            place = board[loc];
        }
  
        prob[place]++;
    }

    map<string,double>::iterator it;
    vector<pair<string,double> > list;
    pair<string,double> p;
    for(it = prob.begin(); it != prob.end(); ++it){
        cout << it->first << " " << (it->second/(double)numrolls) << endl;
        p = make_pair(it->first, it->second/(double)numrolls);
        list.push_back(p);
    }

    sort(list.begin(), list.end(), cmp);

    for(int i = 0; i < 40; ++i){
        if(board[i] == list[0].first){
            cout << setw(2) << setfill('0') << i;
        }
    }
    for(int i = 0; i < 40; ++i){
        if(board[i] == list[1].first){
            cout << setw(2) << setfill('0') << i;
        }
    }
    for(int i = 0; i < 40; ++i){
        if(board[i] == list[2].first){
            cout << setw(2) << setfill('0') << i;
        }
    }
    cout << endl;

    return 0;
}
