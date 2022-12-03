#include <stdio.h>

int main(){
    char letter;
    char matrix[100][100];

    for (int i = 0; i <= 99; ++i) {
        for (int j = 0; j <= 99; ++j) {
            matrix[i][j] = 0;
        }
    }

    int i = 99;
    int j = 99;

    int max = 0;
    int count = 0;
    int sent_num = 0;

    while(scanf("%c", &letter) == 1){
        if(letter == '\n'){
            i--;
            j = 99;
            sent_num++;
            count = 0;
        }
        else{
            matrix[i][j] = letter;
            j--;
            count++;
            if(count > max){
                max = count;
            }
        }
    }

    for(j = 99; j >= 100-max; j--){
        for(i = 100-sent_num; i < 100; i++){
            if(matrix[i][j] != 0){
                printf("%c", matrix[i][j]);
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
    return 0;
}
