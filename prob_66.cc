#include<iostream>
#include<set>
#include<math.h>
#include<vector>
#include<sstream>
#include<cstdlib>
#include<algorithm>

using namespace std;

vector<int> gencfrac(int i){
    vector<int> res;

    int m = 0;
    int d = 1;
    int a = floor(sqrt(i));

    set<pair<int, pair<int,int> > > history;
    pair<int, int> inner;
    pair<int,pair<int, int> > outer;

    inner = make_pair(d,a);
    outer = make_pair(m,inner);

    int m1,d1,a1;
    while(history.count(outer) == 0){
        //cout << m  << " " << d << " " << a << endl;
        history.insert(outer);
        res.push_back(a);
    
        m1 = d*a-m;
        d1 = (i-m1*m1)/d;
        a1 = floor((sqrt(i)+m1)/d1);

        m = m1;
        d = d1;
        a = a1;
  
        inner = make_pair(d,a);
        outer = make_pair(m,inner);
    }
  
    if(res.size() != 2){
        res.pop_back();
    }
  
    return res;
}

string multiply(string a, string b){
    vector<int> res;
    res.resize(max(a.length(),b.length())*2,0);

    //cout << "in multiply with " << a << " " << b << endl;

    int offset = 0;
    for(unsigned int i = 1; i <= a.length(); ++i){
        int x = a[a.length()-i] - '0';
        for(unsigned int j = 1; j <= b.length(); ++j){
            int y = b[b.length()-j] - '0';
            //cout << res.size()-j-offset << " " << x << " " << y << endl;
            res[res.size()-j-offset] += x * y;
        }
        offset++;
    }
    /*
      cout << "after mult, before carry" << endl;
      for(unsigned int i = 0; i < res.size(); ++i){
      cout << res[i];
      }
      cout << endl;
    */
    int carry = 0;
    for(unsigned int i = 1; i < res.size(); ++i){
        res[res.size()-i] += carry;
        carry = (res[res.size()-i] - (res[res.size()-i] %10)) / 10;
        res[res.size()-i] %= 10;
    }
    if(carry){
        res[0] = carry;
    }
    /*
      cout << "after carry" << endl;
      for(unsigned int i = 0; i < res.size(); ++i){
      cout << res[i];
      }
      cout << endl;
    */
    unsigned int pos = 0;
    for(unsigned int i = 0; i < res.size(); ++i){
        if(res[i] != 0){
            pos = i;
            break;
        }
    }

    stringstream inp;
    for(unsigned int i = pos; i < res.size(); ++i){
        inp << res[i];
    }

    string result;
    inp >> result;
    /*
      cout << "final result" << endl;
      cout << result << endl;
    */
    return result;
}

string add(string a, string b){
    vector<int> res;
    res.resize(max(a.length(),b.length())+1,0);

    int carry = 0;
    for(unsigned int i = 1; i < res.size(); ++i){
        int x = 0;
        if(i <= a.length()){
            x = a[a.length()-i] -'0';
        }

        int y = 0;
        if(i <= b.length()){
            y = b[b.length()-i] - '0';
        }

        res[res.size()-i] = x + y + carry;
        carry = (res[res.size()-i] - (res[res.size()-i] % 10)) / 10;
        res[res.size()-i] %= 10;
    }
    if(carry){
        res[0] = carry;
    }

    unsigned int pos = 0;
    for(unsigned int i = 0; i < res.size(); ++i){
        if(res[i] != 0){
            pos = i;
            break;
        }
    }

    stringstream inp;
    for(unsigned int i = pos; i < res.size(); ++i){
        inp << res[i];
    }

    string result;
    inp >> result;

    return result;
}

pair<string,string> solvepell(vector<int> cfrac){
    stringstream inp;

    string num = "1";
    inp << cfrac[cfrac.size()-1];
    string denom;
    inp >> denom;
    inp.clear();

    for(unsigned int i = 2; i <= cfrac.size(); ++i){
        //cout << "start " << num << " " << denom << endl;
        inp << cfrac[cfrac.size()-i];
        string temp;
        inp >> temp;
        inp.clear();
        //cout << "multiplying by " << temp << endl;
        temp = multiply(temp,denom);
        //cout << "after multiply " << temp << endl;
        num = add(temp,num);
        //cout << "after add " << num << endl;

        temp = denom;
        denom = num;
        num = temp;
        //cout << "after " << num << " " << denom << endl;
    }

    pair<string,string> result;
    result = make_pair(denom,num);

    return result;
}

bool comp(pair<string,int> a, pair<string,int> b){
    if(a.first.length() > b.first.length()){
        return true;
    }
    if(a.first.length() < b.first.length()){
        return false;
    }
    else{
        return a.first > b.first;
    }
}

pair<string, string> check(pair<string,string> s, int i){
    string a = s.first;
    string b = s.second;

    stringstream inp;
    inp << i;
    string N;
    inp >> N;
    inp.clear();

    a = multiply(a,a);
    b = multiply(b,b);
    b = multiply(b,N);
    b = add(b,"1");

    //cout << "in check " << a << " " << b << endl;

    bool flag = true;
    for(unsigned int i = 0; i < a.length(); ++i){
        if(a[i] != b[i]){
            flag = false;
            break;
        }
    }

    string x;
    string y;

    if(!flag){
        string temp2;
        temp2 = multiply(s.second,s.second);
        temp2 = multiply(temp2, N);
        x = add(a,temp2);

        temp2 = multiply(s.first,s.second);
        temp2 = multiply("2",temp2);
        y = temp2;
    }
    else{
        x = s.first;
        y = s.second;
    }

    pair<string,string> result;
    result = make_pair(x,y);

    return result;
}

int main(){
    vector<int> cfrac;
    set<int> squares;
  
    vector<pair<string,int> > xdlist;
    for(int i = 2; i < 32; ++i){
        squares.insert(i*i);
    }

    for(int i = 2; i < 1001; ++i){
        if(squares.count(i) != 0){
            continue;
        }

        cfrac = gencfrac(i);
        /*
          cout << i << ": ";
          for(unsigned int j = 0; j < cfrac.size(); ++j){
          cout << cfrac[j] << " ";
          }
          //cout << endl;
          */

        pair<string,string> solution;
        solution = solvepell(cfrac);
        //cout << "1st solution: x=" << solution.first << " y=" << solution.second << " ";
        solution = check(solution,i);
        //cout << "2nd solution: x=" << solution.first << " y=" << solution.second << " ";
        //cout << solution.first << "^2-" << i << "*" <<  solution.second << "^2" << endl;
        pair<string,int> xd;
        xd = make_pair(solution.first, i);
        xdlist.push_back(xd);    
    }

    sort(xdlist.begin(), xdlist.end(), comp);
    cout << xdlist[0].second << endl;
    return 0;
}
