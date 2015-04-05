#include <iostream>
#include <math.h>
#include <set>

using namespace std;

int main(){
    int ototal = 0;

    set<int> squares;
    for(int i = 0; i <= 100; ++i){
        squares.insert(i*i);
    }

    for(int i = 2; i <= 10000; ++i){
        //cout << i << endl;
        if(squares.count(i) != 0){
            continue;
        }

        set<pair<int, pair<int,int> > > history;
        pair<int,int> shalf;
        pair<int,pair<int,int> > fhalf;

        int m_n = 0;
        int d_n = 1;
        int a_n = floor(sqrt(i));
    
        shalf = make_pair(d_n,a_n);
        fhalf = make_pair(m_n,shalf);

        history.insert(fhalf);

        int m_n1, d_n1, a_n1,num = 0;
        while(1){
            m_n1 = d_n*a_n-m_n;
            d_n1 = (i - m_n1*m_n1)/d_n;
            a_n1 = floor((sqrt(i)+m_n1)/d_n1);

            shalf = make_pair(d_n1,a_n1);
            fhalf = make_pair(m_n1,shalf);

            /*
              cout << "***" << endl;
              cout << m_n << " " << d_n << " " << a_n << endl;
              cout << m_n1 << " " << d_n1 << " " << a_n1 << endl;
            */
            if(history.count(fhalf) != 0){
                if(num % 2 == 1){
                    //cout << i << endl;
                    ototal++;
                }
                break;
            }
            else{
                history.insert(fhalf);
            }

            m_n = m_n1;
            d_n = d_n1;
            a_n = a_n1;
            num++;
        }
    }

    cout << ototal << endl;

    return 0;
}
