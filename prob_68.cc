#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

bool comp(pair<int, string> a, pair <int, string> b){
    return a.first < b.first;
}

int main(){
    vector<int> nums;
    vector<pair<int, string > > solns;
    pair<int, string > grp;
    stringstream inp;
    string temp;

    for(int i = 1; i <=10; ++i){
        nums.push_back(i);
    }

    do{
        bool min;
        min = nums[0] < nums[3];
        min = (nums[0] < nums[5]) && min;
        min = (nums[0] < nums[7]) && min;
        min = (nums[0] < nums[9]) && min;
        if(!min){
            continue;
        }
        bool equal;
        /*
          cout << "{" << nums[0] << "," << nums[1] << "," << nums[2] << "} ";
          cout << "{" << nums[3] << "," << nums[2] << "," << nums[4] << "} ";
          cout << "{" << nums[5] << "," << nums[4] << "," << nums[1] << "} ";
        */
        equal = nums[0] + nums[1] + nums[2] == nums[3] + nums[2] + nums[4];
        equal = (nums[0] + nums[1] + nums[2] == nums[5] + nums[4] + nums[6]) && equal;
        equal = (nums[0] + nums[1] + nums[2] == nums[7] + nums[6] + nums[8]) && equal;
        equal = (nums[0] + nums[1] + nums[2] == nums[9] + nums[8] + nums[1]) && equal;

        equal = (nums[3] + nums[2] + nums[4] == nums[5] + nums[4] + nums[6]) && equal;
        equal = (nums[3] + nums[2] + nums[4] == nums[7] + nums[6] + nums[8]) && equal;
        equal = (nums[3] + nums[2] + nums[4] == nums[9] + nums[8] + nums[1]) && equal;

        equal = (nums[5] + nums[4] + nums[6] && nums[7] + nums[6] + nums[8]) && equal;
        equal = (nums[5] + nums[4] + nums[6] && nums[9] + nums[8] + nums[1]) && equal;

        equal = (nums[7] + nums[6] + nums[8] == nums[9] + nums[8] + nums[1]) && equal;

        if(!equal){
            continue;
        }

        inp << nums[0] << nums[1] << nums[2];
        inp << nums[3] << nums[2] << nums[4];
        inp << nums[5] << nums[4] << nums[6];
        inp << nums[7] << nums[6] << nums[8];
        inp << nums[9] << nums[8] << nums[1];
        inp >> temp;
        inp.clear();

        grp = make_pair(nums[0] + nums[1] + nums[2], temp);

        solns.push_back(grp);
    }while(next_permutation(nums.begin(), nums.end()));

  
    sort(solns.begin(), solns.end(), comp);

  
    string maximum = "0000000000000000";
    for(unsigned int i = 0; i < solns.size(); ++i){
        //cout << solns[i].first << ": " << solns[i].second;
        if(solns[i].second.length() == 16 && solns[i].second > maximum){
            maximum = solns[i].second;
        }
    }
  
    cout << maximum << endl;

    return 0;
}
