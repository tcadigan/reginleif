#include <iostream>

using namespace std;

int main(){
    unsigned int bin1[3];
    unsigned int bin2[3];
    unsigned int bin3[3];

    while(cin >> bin1[0] >> bin1[1] >> bin1[2] >> bin2[0] >> bin2[1] >> bin2[2] >> bin3[0] >> bin3[1] >> bin3[2]){

        int totalBrown = bin1[0] + bin2[0] + bin3[0];
        int totalGreen = bin1[1] + bin2[1] + bin3[1];
        int totalClear = bin1[2] + bin2[2] + bin3[2];

        //BCG
        int BCG = totalBrown - bin1[0] + totalClear - bin2[2] + totalGreen - bin3[1];

        //BGC
        int BGC = totalBrown - bin1[0] + totalGreen - bin2[1] + totalClear - bin3[2];

        //CBG
        int CBG = totalClear - bin1[2] + totalBrown - bin2[0] + totalGreen - bin3[1];

        //CGB
        int CGB = totalClear - bin1[2] + totalGreen - bin2[1] + totalBrown - bin3[0];

        //GBC
        int GBC = totalGreen - bin1[1] + totalBrown - bin2[0] + totalClear - bin3[2];

        //GCB
        int GCB = totalGreen - bin1[1] + totalClear - bin2[2] + totalBrown - bin3[0];

        int min = -1;
        string str = "";
        if(BCG <= BGC){
            min = BCG;
            str = "BCG";
        }
        else{
            min = BGC;
            str = "BGC";
        }

        if(CBG < min){
            min = CBG;
            str = "CBG";
        }

        if(CGB < min){
            min = CGB;
            str = "CGB";
        }

        if(GBC < min){
            min = GBC;
            str = "GBC";
        }

        if(GCB < min){
            min = GCB;
            str = "GCB";
        }

        cout << str << " " << min << endl;

    } 

    return 0;
}
