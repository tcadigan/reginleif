#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

using namespace std;

class Team{
public:
    string name;
    int wins;
    int ties;
    int losses;
    int scored;
    int against;

    Team();
};
 
Team::Team(){
    name = "";
    wins = 0;
    ties = 0;
    losses = 0;
    scored = 0;
    against = 0;
}

bool cmp(Team t1, Team t2){
    if(t1.wins*3 + t1.ties > t2.wins*3 + t2.ties){
        return true;
    }
    else if(t1.wins*3 + t1.ties < t2.wins*3 + t2.ties){
        return false;
    }
    if(t1.wins > t2.wins){
        return true;
    }
    else if(t1.wins < t2.wins){
        return false;
    }
    if(t1.scored-t1.against > t2.scored-t2.against){
        return true;
    }
    else if(t1.scored-t1.against < t2.scored-t2.against){
        return false;
    }
    if(t1.scored > t2.scored){
        return true;
    }
    else if(t1.scored < t2.scored){
        return false;
    }
    if(t1.wins+t1.ties+t1.losses < t2.wins+t2.ties+t2.losses){
        return true;
    }
    else if(t1.wins+t1.ties+t1.losses < t2.wins+t2.ties+t2.losses){
        return false;
    }

    string team_1="";
    for(unsigned int i = 0; i < t1.name.size(); ++i){
        team_1 += tolower(t1.name[i]);
    }
    string team_2="";
    for(unsigned int i = 0; i < t2.name.size(); ++i){
        team_2 += tolower(t2.name[i]);
    }

    return team_1 < team_2;
}

void printTeam(Team tm, int place){
    cout << place << ") " << tm.name << " ";
    cout << tm.wins*3 + tm.ties << "p, ";
    cout << tm.wins+tm.ties+tm.losses << "g (";
    cout << tm.wins << "-" << tm.ties << "-" << tm.losses << "), ";
    cout << tm.scored-tm.against << "gd (";
    cout << tm.scored << "-" << tm.against << ")" << endl;
}

int findTeam(vector<Team> &list, string name){
    int result = -1;
    for(unsigned int i = 0; i < list.size(); ++i){
        if(list[i].name == name){
            result = i;
            break;
        }
    }
    return result;
}

int main(){
    int tourneys;
    string tourneyName;
    cin >> tourneys;

    getline(cin,tourneyName);
    while(tourneys > 0){
        getline(cin,tourneyName);
        int numTeams;

        cin >> numTeams;
        string junk;
        getline(cin,junk);

        vector<Team> teams;
        while(numTeams > 0){
            Team *temp = new Team();
            getline(cin,temp->name);
      
            teams.push_back(*temp);
            --numTeams;
        }

        int numGames;
        cin >> numGames;
        string game;
        string team_1;
        string team_2;
        int score_1;
        int score_2;
        int idx_1;
        int idx_2;
        int idx_3;
        getline(cin, junk);
        while(numGames > 0){
            getline(cin, game);

            idx_1 = game.find('#');
            idx_2 = game.find('@');
            idx_3 = game.rfind('#');

            team_1 = game.substr(0,idx_1);
            score_1 = atoi(game.substr(idx_1+1, idx_2).data());
            score_2 = atoi(game.substr(idx_2+1, idx_3).data());
            team_2 = game.substr(idx_3+1, game.size());

            Team *t1 = &teams[findTeam(teams, team_1)];
            Team *t2 = &teams[findTeam(teams, team_2)];

            if(score_1 > score_2){
                t1->wins += 1;
                t2->losses += 1;
            }
            else if(score_1 < score_2){
                t2->wins += 1;
                t1->losses += 1;
            }
            else{
                t1->ties += 1;
                t2->ties += 1;
            }

            t1->scored += score_1;
            t1->against += score_2;
            t2->scored += score_2;
            t2->against += score_1;

            --numGames;
        }
    
        --tourneys;

        sort(teams.begin(), teams.end(), cmp);

        cout << tourneyName << endl;
        for(unsigned int i = 0; i < teams.size(); ++i){
            printTeam(teams[i], i+1);
        }
        if(tourneys != 0){
            cout << endl;
        }
    }

    return 0;
}
