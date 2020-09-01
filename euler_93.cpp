#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <sstream>
#include <cstring>

using namespace std;

double eval(double a, char op, double b){
    switch(op){
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    default:
        if(b != 0){
            return a / b;
        }
        else{
            return -99999999;
        }
    }
}

int main(){
    string digits = "00000";
    string ops = "+++---///***";
    set<string> seen;
    int freq[10];
    string resmax = "";
    int max = 0;
    while(digits[4] != '1'){
        memset(freq,0,sizeof(int)*10);
        bool flag = false;
        for(int i = 0; i < 5; ++i){
            freq[digits[i]-'0']++;
            if(freq[digits[i]-'0'] > 1){
                flag = true;
                break;
            }
        }
        if(!flag){
            string check = digits;
            sort(check.begin(),check.end());
            if(seen.count(check) == 0){
                seen.insert(check);
                cout << "real: " << digits << " sorted: " << check << endl;
                set<double> res;
                do{
                    set<string> opseen;
                    //cout << "\t" << check << ": "; 
                    sort(ops.begin(),ops.end());
                    do{
                        string op = ops.substr(0,3);
                        if(opseen.count(op) == 0){
                            opseen.insert(op);
                            //cout << endl<< "\t\t" << op << ": ";

                            double result1 = eval(check[1]-'0',op[0],check[2]-'0');
                            if(result1 != -99999999){
                                result1 = eval(result1,op[1],check[3]-'0');
                                if(result1 != -99999999){
                                    result1 = eval(result1,op[2],check[4]-'0');
                                    if(result1 != -99999999){
                                        res.insert(result1);
                                        //cout << result1 << " ";
                                    }
                                }
                            }
	      
                            result1 = eval(check[2]-'0',op[1],check[3]-'0');
                            if(result1 != -99999999){
                                result1 = eval(check[1]-'0',op[0],result1);
                                if(result1 != -99999999){
                                    result1 = eval(result1,op[2],check[4]-'0');
                                    if(result1 != -99999999){
                                        res.insert(result1);
                                        //cout << result1 << " ";
                                    }
                                }
                            }

                            result1 = eval(check[3]-'0',op[2],check[4]-'0');
                            if(result1 != -99999999){
                                result1 = eval(check[2]-'0',op[1],result1);
                                if(result1 != -99999999){
                                    result1 = eval(check[1]-'0',op[0],result1);
                                    if(result1 != -99999999){
                                        res.insert(result1);
                                        //cout << result1 << " ";
                                    }
                                }
                            }
	      
                            result1 = eval(check[2]-'0',op[1],check[3]-'0');
                            if(result1 != -99999999){
                                result1 = eval(result1,op[2],check[4]-'0');
                                if(result1 != -99999999){
                                    result1 = eval(check[1]-'0',op[0],result1);
                                    if(result1 != -99999999){
                                        res.insert(result1);
                                        //cout << result1 << " ";
                                    }
                                }
                            }

                            result1 = eval(check[1]-'0',op[1],check[2]-'0');
                            double result2 = eval(check[3]-'0',op[2],check[4]-'0');
                            if(result1 != -99999999 && result2 != -99999999){
                                result1 = eval(result1,op[1],result2);
                                if(result1 != -99999999){
                                    res.insert(result1);
                                    //cout << result1 << " ";
                                }
                            }
                            //do formulas here;
                        }
                    }while(next_permutation(ops.begin(),ops.end()));
                    //cout << endl;
                }while(next_permutation(check.begin()+1,check.end()));
                //exit(0);
                int j = 1;
                for(;j < 10000;++j){
                    if(res.count(j) == 0){
                        --j;
                        break;
                    }
                }

                if(j > max){
                    max = j;
                    resmax = check;
                    cout << "max now: " << resmax << " " << max << endl;
                } 
            }
        }
      
        digits[0]++;
        for(int i = 0; i < 5; ++i){
            if(digits[i]-'0' > 9){
                digits[i+1]++;
                digits[i]='0';
            }
        }

    }

    return 0;
}
