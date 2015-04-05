#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

bool good(vector<string> x){
    sort(x.begin(), x.end());

    do{
        string back = x[0].substr(x[0].length()-2);

        bool flag = true;
        for(unsigned int i = 1; i < x.size(); ++i){
            string front = x[i].substr(0,2);
            if(front.compare(back) != 0){
                flag = false;
                break;
            }
            back = x[i].substr(x[i].length()-2);
        }
        if(flag){
            return flag;
        }
    }while(next_permutation(x.begin(),x.end()));

    return false;
}

bool finalcheck(vector<string> x){
    sort(x.begin(), x.end());

    do{
        string back = x[0].substr(x[0].length()-2);

        bool flag = true;
        for(unsigned int i = 1; i < x.size(); ++i){
            string front = x[i].substr(0,2);
            if(front.compare(back) != 0){
                flag = false;
                break;
            }
            back = x[i].substr(x[i].length()-2);
        }
        if(flag){
            string front = x[0].substr(0,2);
            if(back.compare(front) == 0){
                return flag;
            }
        }
    }while(next_permutation(x.begin(),x.end()));

    return false;
}

int main(){
    vector<int> trinum;
    vector<int> sqnum;
    vector<int> pentnum;
    vector<int> hexnum;
    vector<int> hepnum;
    vector<int> octnum;

    for(int i = 0; i < 145; ++i){
        if((i*(i+1))/2 < 10000 && (i*(i+1))/2 > 999){
            trinum.push_back((i*(i+1))/2);
        }
        if(i*i < 10000 && i*i > 999){
            sqnum.push_back(i*i);
        }
        if((i*(3*i-1))/2 < 10000 && (i*(3*i-1))/2 > 999){
            pentnum.push_back((i*(3*i-1))/2);
        }
        if(i*(2*i-1) < 10000 && i*(2*i-1) > 999){
            hexnum.push_back(i*(2*i-1));
        }
        if((i*(5*i-3))/2 < 10000 && (i*(5*i-3))/2 > 999){
            hepnum.push_back((i*(5*i-3))/2);
        }
        if(i*(3*i-2) < 10000 && i*(3*i-2) > 999){
            octnum.push_back(i*(3*i-2));
        }
    }

    stringstream inp;
    vector<string> seq;      
    for(unsigned int i = 0; i < trinum.size(); ++i){
        inp << trinum[i];
        string num1;
        inp >> num1;
        inp.clear();

        seq.push_back(num1);    
        for(unsigned int j = 0; j < sqnum.size(); ++j){
            inp << sqnum[j];
            string num2;
            inp >> num2;
            inp.clear();
      
            seq.push_back(num2);
            if(!good(seq)){
                seq.pop_back();
                continue;
            }
            else{
                //cout << num1 << " " << num2 << endl;
            }
            for(unsigned int k = 0; k < pentnum.size(); ++k){
                inp << pentnum[k];
                string num3;
                inp >> num3;
                inp.clear();
	
                seq.push_back(num3);
                if(!good(seq)){
                    seq.pop_back();
                    continue;
                }
                else{
                    //cout << num1 << " " << num2 << " " << num3 << endl;
                }
	
                for(unsigned int l = 0; l < hexnum.size(); ++l){
                    inp << hexnum[l];
                    string num4;
                    inp >> num4;
                    inp.clear();

                    seq.push_back(num4);
                    if(!good(seq)){
                        seq.pop_back();
                        continue;
                    }
                    else{
                        //cout << num1 << " " << num2 << " " << num3 << " " << num4 << endl;
                    }
	  
                    for(unsigned int m = 0; m < hepnum.size(); ++m){
                        inp << hepnum[m];
                        string num5;
                        inp >> num5;
                        inp.clear();

                        seq.push_back(num5);
                        if(!good(seq)){
                            seq.pop_back();
                            continue;
                        } 
                        else{
                            //cout << num1 << " " << num2 << " " << num3 << " " << num4 << " " << num5 << endl;
                        }
	    
                        for(unsigned int n = 0; n < octnum.size(); ++n){
                            inp << octnum[n];
                            string num6;
                            inp >> num6;
                            inp.clear();

                            seq.push_back(num6);
                            if(!good(seq)){
                                seq.pop_back();
                                continue;
                            }
                            else{
                                if(finalcheck(seq)){
                                    cout << num1 << " " << num2 << " " << num3 << " " << num4 << " " << num5 << " " << num6 << endl;
                                    int sum = 0;
                                    for(unsigned int a = 0; a < seq.size(); ++a){
                                        sum += atoi(seq[a].data());
                                    }
                                    cout << sum << endl;
                                    return 0;
                                }
                            }
                            seq.pop_back();
                        }
                        seq.pop_back();
                    }
                    seq.pop_back();
                }
                seq.pop_back();
            }
            seq.pop_back();
        }
        seq.pop_back();
    }
  
    return 0;
}


