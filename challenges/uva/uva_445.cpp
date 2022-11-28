#include <stdio.h>
#include <stdlib.h>

int main(){
    char character;
    int number, i;
    int pre = 0;
    int total = 0;
    int pre_n = 0;
  
    while(scanf("%c", &character) == 1){
        if(pre_n == 1 && character == '\n'){
            printf("\n");
            pre = 0;
            pre_n = 0;
            total = 0;
        }
        else{
            if(character == '\n'){
                printf("\n");
                pre_n = 1;
            }
            else{
                if(character == '!'){
                    printf("\n");
                }
                else{
                    if(character >= 48 && character <= 57){
                        number = atoi(&character);
                        if(pre){
                            total += number;
                        }
                        else{
                            total = number;
                            pre = 1;
                        }
                    }
                    else{
                        if(pre){
                            for(i = 0; i < total; i++){
                                if(character == 'b'){
                                    printf(" ");
                                }
                                else{
                                    printf("%c", character);
                                }
                                pre = 0;
                            }
                        }
                        else{
                            printf("%c", character);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
