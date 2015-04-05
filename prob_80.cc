#include<iostream>
#include<set>
#include<sstream>
#include<iomanip>
#include<math.h>
#include<vector>
#include<cstdlib>
#include<string>
#include<algorithm>

using namespace std;

string mult(string a, string b){
    stringstream inp;

    vector<int> temp;
    temp.resize(2*max(a.length(),b.length()),0);
  
    int offset = 0;
    for(unsigned int i = 0; i < b.length(); ++i){
        int x = b[b.length()-i-1]-'0';
        for(unsigned int j = 0; j < a.length(); ++j){
            int y = a[a.length()-j-1]-'0';
      
            temp[temp.size()-j-offset-1] += x * y;
        }
        ++offset;
    }

    for(unsigned int i = 0; i < temp.size()-1; ++i){
        temp[temp.size()-(i+1)-1] += temp[temp.size()-i-1] / 10;
        temp[temp.size()-i-1] %= 10;
    }

    for(unsigned int i = 0; i < temp.size(); ++i){
        inp << temp[i];
    }

    string result;
    inp >> result;

    if(result.length() >= 2){
        unsigned int pos;
        for(pos = 0; result[pos]-'0' == 0 && pos < result.length()-2; ++pos){}
    
        result = result.substr(pos);
    }

    return result;
}

string sub(string a, string b){
    stringstream inp;
    vector<int> top;

    for(unsigned int i = 0; i < a.length(); ++i){
        top.push_back(a[i] -'0');
    }

    for(unsigned int i = 0; i < top.size(); ++i){
        int x = top[top.size()-i-1];
        int y = 0;
        if(i < b.length()){
            y = b[b.length()-i-1]-'0';
        }

        if(y > x){
            top[top.size()-(i+1)-1]--;
            x += 10;
        }

        inp << x-y;
    }

    string result;
    inp >> result;
  
    reverse(result.begin(), result.end());
    if(result.length() >= 2){
        unsigned int pos;
        for(pos = 0; result[pos]-'0' == 0 && pos < result.length()-2; ++pos){}
        result = result.substr(pos);
    }

    return result;
}

string add(string a, string b){
    stringstream inp;

    int carry = 0;
    for(unsigned int i = 0; i < max(a.length(),b.length()); ++i){
        int x = 0;
        if(i < a.length()){
            x = a[a.length()-i-1]-'0';
        }

        int y = 0;
        if(i < b.length()){
            y = b[b.length()-i-1]-'0';
        }

        inp << (x+y+carry) % 10;

        carry = (x+y+carry) / 10;
    }
    if(carry){
        inp << carry;
    }

    string result;
    inp >> result;

    reverse(result.begin(),result.end());

    return result;
}

bool isgreat(string a, string b){
    string temp = "";
    if(a.length() != b.length()){
        for(int i = 0; i < abs((int)a.length()-(int)b.length()); ++i){
            temp += "0";
        }
        if(a.length() < b.length()){
            a = temp + a;
        }
        else{
            b = temp + b;
        }
    }

    return a > b;
}

string finddigit(string a, string b){
    string curr = "9";
    string zero = "0";
    string one = "1";
    string twenty = "20";

    string result = mult(a,twenty);
    result = add(result,curr);
    result = mult(result,curr);
  
    while(isgreat(result,b) && curr.compare(zero) != 0){
        //cout << curr << " " << result << " " << b << endl;
        curr = sub(curr,one);
        result = mult(a,twenty);
        result = add(result,curr);
        result = mult(result,curr);
    }

    return curr;
}

int main(){
    set<int> sqs;

    for(int i = 1; i <= 10; ++i){
        sqs.insert(i*i);
    }

    stringstream inp;
  
    string P, c, result, temp, curr, ndigit, remove;
    string twenty = "20";
    int total = 0;
    int offset, val;
    for(int i = 2; i <= 100; ++i){
        inp << setw(4);
        inp << setfill('0');

        if(sqs.count(i) != 0){
            continue;
        }
    
        inp << i;
        for(int j = 0; j < 50; ++j){
            inp << "0000";
        }
    
        inp >> result;
        inp.clear();

        P = "";

        offset = 0;
        curr = result.substr(offset,2);
        offset += 2;
        val = floor(sqrt(atoi(curr.data())));
    
        inp << val;
        inp >> temp;
        inp.clear();
    
        P += temp;

        c = mult(temp,temp);
        c = sub(curr,c);

        // loop starts here, 100 times (I think)
        for(int j = 0; j < 100; ++j){
            c += result.substr(offset,2);
            offset += 2;
            //cout << "starting: " << P << " " << c << endl;
      
            ndigit = finddigit(P,c);
      
            //cout << "finished finding: " << ndigit << endl;
            remove = mult(P,twenty);
            remove = add(remove,ndigit);
            remove = mult(remove,ndigit);
            //cout << "remove final: " << remove << endl;
            c = sub(c,remove);
      
            P += ndigit;
    
            //cout << "P: " << P << " c: " << c;
        }
    
        int sum = 0;
        for(unsigned int j = 0; j < P.length(); ++j){
            sum += P[j]-'0';
        }
        total += sum;
        cout << "P: " << P << " c: " << c << " sum: " << sum << endl;
    }

    cout << " total: " << total << endl;
    /*
      string a = "1";
      string b = "1";

      cout << a << " " << b << " '" << mult(a,b) << "' '" << sub(a,b) << "'" << endl;
    */
    return 0;
}
