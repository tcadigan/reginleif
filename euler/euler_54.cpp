#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <string.h>

using namespace std;

void printhand(vector< pair<int,char> > hand){
    for(unsigned int i = 0; i < hand.size(); ++i){
        cout << hand[i].first << hand[i].second;
        if(i != hand.size() -1){
            cout << " ";
        }
    }
}

bool compare(pair<int,char> a, pair<int,char> b){
    return a.first > b.first;
}

bool hasflush(vector< pair<int,char> > hand){
    char s = hand[0].second;
    for(unsigned int i = 1; i < hand.size(); ++i){
        if(hand[i].second != s){
            return false;
        }
    }
    return true;
}
  
bool hasstraight(vector< pair<int,char> > hand){
    int start = hand[0].first;
    for(unsigned int i = 1; i < hand.size(); ++i){
        if((unsigned) hand[i].first != start-i){
            return false;
        }
    }
    return true;
}

int evalhand(vector< pair<int,char> > hand1){
    int valp1 = 1;

    bool straight = hasstraight(hand1);
    bool flush = hasflush(hand1);

    if(flush && straight){
        if(hand1[0].first == 14){
            valp1 = 10;
        }
        else{
            valp1 = 9;
        }
    }
    if(flush && !straight){
        valp1 = 6;
    }
    if(!flush && straight){
        valp1 = 5;
    }

    int vals[15];
    memset(vals,0,sizeof(int)*15);

    for(unsigned int i = 0; i < hand1.size(); ++i){
        vals[hand1[i].first]++;
    }

    bool threekind = false;
    bool twokind = false;
    for(unsigned int i = 0; i < 15; ++i){
        if(vals[i] == 4){
            valp1 = max(valp1,8);
        }
        if(vals[i] == 3){
            valp1 = max(valp1,4);
            threekind = true;
        }
        if(vals[i] == 2){
            if(twokind){
                twokind = false;
                valp1 = max(valp1,3);
            }
            else{
                valp1 = max(valp1,2);
                twokind = true;
            }
        }
    }

    if(twokind && threekind){
        valp1 = max(valp1,7);
    }

    return valp1;
}

int tiebreak(vector< pair<int,char> > hand1, vector< pair<int,char> > hand2, int s){
    switch(s){
    case 9:
    case 5:
        {
            for(unsigned int i = 0; i < hand1.size(); ++i){
                if(hand1[i].first != hand2[i].first){
                    return hand1[i].first > hand2[i].first;
                }
            }
            break;
        }
    case 7:
        {
            int vals[15];
            int vals2[15];
            memset(vals,0,sizeof(int)*15);
            memset(vals2,0,sizeof(int)*15);

            for(unsigned int i = 0; i < hand1.size(); ++i){
                vals[hand1[i].first]++;
                vals2[hand2[i].first]++;
            }

            int threeloc = -1;
            int twoloc = -1;
            int threeloch2 = -1;
            int twoloch2 = -1;

            for(unsigned int i = 0; i < 15; ++i){
                if(vals[i] == 3){
                    threeloc = i;
                }
                if(vals[i] == 2){
                    twoloc = i;
                }
                if(vals2[i] == 3){
                    threeloch2 = i;
                }
                if(vals2[i] == 2){
                    twoloch2 = i;
                }
            }

            if(threeloc != threeloch2){
                return threeloc > threeloch2;
            }
            if(twoloc != twoloch2){
                return twoloc > twoloch2;
            }
            for(unsigned int i = 0; i < hand1.size(); ++i){
                if(hand1[i].first != hand2[i].first){
                    return hand1[i].first > hand2[i].first;
                }
            }
            break;
        }
    case 3:
        {
            int vals[15];
            int vals2[15];
            memset(vals,0,sizeof(int)*15);
            memset(vals2,0,sizeof(int)*15);

            for(unsigned int i = 0; i < hand1.size(); ++i){
                vals[hand1[i].first]++;
                vals2[hand2[i].first]++;
            }

            int twoloc1 = -1;
            int twoloc2 = -1;
            int twoloc1h2 = -1;
            int twoloc2h2 = -1;

            for(unsigned int i = 0; i < 15; ++i){
                if(vals[i] == 2){
                    if(twoloc1 == -1){
                        twoloc1 = i;
                    }
                    else{
                        twoloc2 = i;
                    }
                }
                if(vals2[i] == 2){
                    if(twoloc1h2 == -1){
                        twoloc1h2 = i;
                    }
                    else{
                        twoloc2h2 = i;
                    }
                }
            }

            if(twoloc1 != twoloc1h2){
                return twoloc1 > twoloc1h2;
            }
            if(twoloc2 != twoloc2h2){
                return twoloc2 > twoloc2h2;
            }
            for(unsigned int i = 0; i < hand1.size(); ++i){
                if(hand1[i].first != hand2[i].first){
                    return hand1[i].first > hand2[i].first;
                }
            }
            break;
        }
    case 8:
    case 4:
    case 2:
        {
            int vals[15];
            int vals2[15];
            memset(vals,0,sizeof(int)*15);
            memset(vals2,0,sizeof(int)*15);
      
            for(unsigned int i = 0; i < hand1.size(); ++i){
                vals[hand1[i].first]++;
                vals2[hand2[i].first]++;
            }

            int loc = -1;
            int loch2 = -1;

            for(unsigned int i = 0; i < 15; ++i){
                if(s == 8 && vals[i] == 4){
                    loc = i;
                }
                if(s == 4 && vals[i] == 3){
                    loc = i;
                }
                if(s == 2 && vals[i] == 2){
                    loc = i;
                }
                if(s == 8 && vals2[i] == 4){
                    loch2 = i;
                }
                if(s == 4 && vals2[i] == 3){
                    loch2 = i;
                }
                if(s == 2 && vals2[i] == 2){
                    loch2 = i;
                }
            }

            if(loc != loch2){
                return loc > loch2;
            }
            for(unsigned int i = 0; i < hand1.size(); ++i){
                if(hand1[i].first != hand2[i].first){
                    return hand1[i].first > hand2[i].first;
                }
            }
            break;
        }
    default:
        return hand1[0].first > hand2[0].first;
        break;
    }

    return -1;
}

int main(){
    string line;
    stringstream inp;

    int p1wins = 0;

    while(getline(cin, line)){
        inp << line;
        string card;
        int i = 0;
        vector<pair<int,char> > p1;
        vector<pair<int,char> > p2;

        while(inp >> card){
            pair<int, char> pcard;
            char suit = card[card.length()-1];
            string number = card.substr(0,card.length()-1);
            int num;
            if(number.compare("A") == 0){
                num = 14;
            }
            else if(number.compare("K") == 0){
                num = 13;
            }
            else if(number.compare("Q") == 0){
                num = 12;
            }
            else if(number.compare("J") == 0){
                num = 11;
            }
            else if(number.compare("T") == 0){
                num = 10;
            }
            else{
                num = atoi(number.data());
            }
            pcard = make_pair(num,suit);

            if(i < 5){
                p1.push_back(pcard);
            }
            else{
                p2.push_back(pcard);
            }
            ++i;
        }

        sort(p1.begin(), p1.end(), compare);
        sort(p2.begin(), p2.end(), compare);

        int score1 = evalhand(p1);
        int score2 = evalhand(p2);


        if(score1 > score2){
            p1wins++;
            cout << "Player 1 wins! \t";
        }
        else if(score1 == score2){
            if(tiebreak(p1,p2, score1)){
                p1wins++;
                cout << "Player 1 wins! \t";
            }
            else{
                cout << "Player 2 wins! \t";
            }
        }
        else{
            cout << "Player 2 wins! \t";
        }

        cout << "Player 1: ";
        printhand(p1);
        cout << " {score: " << score1 << "}\t";
        cout << "Player 2: ";
        printhand(p2);
        cout << " {score: " << score2 << "}" << endl;
    
        inp.clear();
    }

    cout << endl << p1wins << endl;

    return 0;
}
