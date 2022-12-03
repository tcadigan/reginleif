#include <stdio.h>

int main(){
    char character;

    while(scanf("%c", &character) == 1){
        if(character == '\n'){
            printf("\n");
        }
        else{
            printf("%c", character-7);
        }
    }
    return 0;
}
