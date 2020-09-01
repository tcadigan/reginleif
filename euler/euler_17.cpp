#include <iostream>

using namespace std;

int main(){
    string ones[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    string teens[] = { "zero", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
    string tens[] = { "zero", "ten", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};

    double total = 0;

    for(int i = 1; i < 1000; i++){
        //cout << i << " ";
        if(i < 10){
            total += ones[i].size();
            //cout << ones[i] << " ";
        }
        else if( i > 10 && i < 20){
            total += teens[i % 10].size();
            //cout << teens[i % 10] << " ";
        }
        else if( i < 100 && i % 10 == 0){
            total += tens[i / 10].size();
            //cout << tens[i / 10] << " ";
        }
        else if( i < 100){
            total += tens[i / 10].size() + ones[i % 10].size();
            //cout << tens[i / 10] << " " << ones[i % 10] << " ";
        }
        else if( i % 100 == 0){
            total += ones[i/100].size() + 7;
            //cout << ones[i/100] << " hundred ";
        }
        else if( i > 100){
            if( i % 10 == 0){
                total += ones[i/100].size() + 7 + tens[(i % 100) / 10].size() + 3;
                //cout << ones[i/100] << " hundred and " << tens[(i % 100) / 10] << " ";
            }
            else if( i % 100 < 20 && i % 100 > 10){
                total += ones[i/100].size() + 7 + teens[i%10].size() + 3;
                //cout << ones[i/100] << " hundred and " << teens[i%10] << " ";
            }
            else if( i % 100 < 10){
                total += ones[i/100].size() + 7 + ones[i%10].size() + 3;
                //cout << ones[i/100] << " hundred and " << ones[i%10] << " ";
            }	
            else{
                total += ones[i/100].size() + 7 + tens[(i % 100) / 10].size() + 3 + ones[i % 10].size();
                //cout << ones[i/100] << " hundred and " << tens[(i % 100) / 10] << "-" << ones[i % 10] << " ";
            }
        }
        //cout << total << endl;
    }
  
    total += 3 + 8;
    cout << total << endl;

    return 0;
}
