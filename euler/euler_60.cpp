#include <iostream>
#include <set>
#include <math.h>
#include <sstream>
#include <cstdlib>

using namespace std;

bool isprime(int x){
    if(x % 2 == 0){
        return false;
    }
    for(int i = 3; i <= sqrt(x); ++i){
        if(x % i == 0){
            return false;
        }
    }

    return true;
}

int main(){
    set<int> primes;
    set<int> exprimes;
    primes.insert(2);
    exprimes.insert(2);

    for(int i = 3; i < 10000; i += 2){
        if(isprime(i)){
            primes.insert(i);
            exprimes.insert(i);
        }
    }
    for(int i = 10001; i < 100000000; i += 2){
        if(i % 1000001 == 0){
            cout << i << endl;
        }
        if(isprime(i)){
            exprimes.insert(i);
        }
    }

    set<int>::iterator it1;
    set<int>::iterator it2;
    set<int>::iterator it3;
    set<int>::iterator it4;
    set<int>::iterator it5;

    stringstream inp;
    for(it1 = primes.begin(); it1 != primes.end(); ++it1){
        for(it2 = it1; it2 != primes.end(); ++it2){
            string temp;
            inp << *it1 << *it2;
            inp >> temp;
            inp.clear();
            if(exprimes.count(atoi(temp.data())) == 0){
                continue;
            }
      
            inp << *it2 << *it1;
            inp >> temp;
            inp.clear();
            if(exprimes.count(atoi(temp.data())) == 0){
                continue;
            }
      
            for(it3 = it2; it3 != primes.end(); ++it3){
                string temp;
                inp << *it1 << *it3;
                inp >> temp;
                inp.clear();
                if(exprimes.count(atoi(temp.data())) == 0){
                    continue;
                }
	
                inp << *it3 << *it1;
                inp >> temp;
                inp.clear();
                if(exprimes.count(atoi(temp.data())) == 0){
                    continue;
                }

                inp << *it3 << *it2;
                inp >> temp;
                inp.clear();
                if(exprimes.count(atoi(temp.data())) == 0){
                    continue;
                }
	
                inp << *it2 << *it3;
                inp >> temp;
                inp.clear();
                if(exprimes.count(atoi(temp.data())) == 0){
                    continue;
                }

                for(it4 = it3; it4 != primes.end(); ++it4){
                    string temp;
                    inp << *it1 << *it4;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }
	
                    inp << *it4 << *it1;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }

                    inp << *it4 << *it2;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }
	
                    inp << *it2 << *it4;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }

                    inp << *it4 << *it3;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }
	
                    inp << *it3 << *it4;
                    inp >> temp;
                    inp.clear();
                    if(exprimes.count(atoi(temp.data())) == 0){
                        continue;
                    }

                    for(it5 = it4; it5 != primes.end(); ++it5){
                        string temp;
                        inp << *it1 << *it5;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }
	
                        inp << *it5 << *it1;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }

                        inp << *it5 << *it2;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }
	
                        inp << *it2 << *it5;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }

                        inp << *it5 << *it3;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }
	
                        inp << *it3 << *it5;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }

                        inp << *it5 << *it4;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }
	
                        inp << *it4 << *it5;
                        inp >> temp;
                        inp.clear();
                        if(exprimes.count(atoi(temp.data())) == 0){
                            continue;
                        }
                        cout << *it1 << "," << *it2 << "," << *it3 << "," << *it4 << "," << *it5 << " = ";
                        cout << *it1 + *it2 + *it3 + *it4 + *it5 << endl;
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}	    
