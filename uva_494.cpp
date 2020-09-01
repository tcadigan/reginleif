#include <stdio.h>

using namespace std;

int main(){
    char character;
    int prev = 0;
    int wordcount = 0;
  
    while(scanf("%c", &character) == 1){
        if(character == '\n'){
            if(prev){
                wordcount += 1;
            }
            printf("%d\n", wordcount);
            prev = 0;
            wordcount = 0;
        }
        else if(character == ' '){
            if(prev){
                wordcount += 1 ;
            }
            prev = 0;
        }
        else if((character <= 90) && (character >= 60)){
            prev = 1;
        }
        else if((character <= 122) && (character >= 97)){
            prev = 1;
        }
        else{
            if(prev){
                wordcount += 1;
            }
            prev = 0;
        }
    }
    return 0;
}
