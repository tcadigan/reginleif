#include <iostream>

using namespace std;

int main(){
    int count = 0;

    for(int a = 0; a <= 1; a++){
        for(int b = 0; b <= 2; b++){
            if(a*200+b*100 > 200){
                break;
            }
            for(int c = 0; c <= 4; c++){
                if(a*200+b*100+c*50 > 200){
                    break;
                }
                for(int d = 0; d <= 10; d++){ 
                    if(a*200+b*100+c*50+d*20 > 200){
                        break;
                    }
                    for(int e = 0; e <= 20; e++){
                        if(a*200+b*100+c*50+d*20+e*10 > 200){
                            break;
                        }
                        for(int f = 0; f <= 40; f++){
                            if(a*200+b*100+c*50+d*20+e*10+f*5 > 200){
                                break;
                            }
                            for(int g = 0; g <= 100; g++){
                                if(a*200+b*100+c*50+d*20+e*10+f*5+g*2 > 200){
                                    break;
                                }
                                for(int h = 0; h <= 200; h++){
                                    if(a*200+b*100+c*50+d*20+e*10+f*5+g*2+h*1 == 200){
                                        count++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << count << endl;

    return 0;
}
