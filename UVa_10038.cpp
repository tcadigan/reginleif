#include<iostream>
#include<set>
#include<cmath>
#include<algorithm>

using namespace std;

int main(){
  int nums;

  while(cin >> nums){
    int count = nums;
    set<int> diffs;

    int val = 0;
    int prev = 0;
    for(int i = 0; i < count; ++i){
      cin >> val;
      if(i != 0){
	diffs.insert(abs(val - prev));
      }
      prev = val;
    }

    bool aborted = false;
    for(int i = 1; i < nums; ++i){
      if(diffs.find(i) == diffs.end()){
	aborted = true;
	break;
      }
    }
    if(!aborted){
      cout << "Jolly" << endl;
    }
    else{
      cout << "Not jolly" << endl;
    }

  }

  return 0;
}
