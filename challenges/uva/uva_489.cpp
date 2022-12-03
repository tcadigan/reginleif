#include <stdio.h>

int main(){
    int round, i, j, misses, flag, result;
    char guess;

    while(1){
        scanf("%d\n", &round);

        if(round == -1){
            break;
        }

        printf("Round %d\n", round);

        char solution[128];
        char guesses[128];

        for(i = 0; i < 128; i++){
            solution[i] = -1;
            guesses[i] = -1;
        }

        for(i = 0; i < 128; i++){
            char temp;
            scanf("%c", &temp);
            if(temp == '\n'){
                break;
            }
            else{
                solution[i] = temp;
            }
        }
        for(i = 0; i < 128; i++){
            char temp;
            scanf("%c", &temp);
            if(temp == '\n'){
                break;
            }
            else{
                guesses[i] = temp;
            }
        }

        misses = 0;
        result = 0;
        for(i = 0; i < 128; i++){
            guess = guesses[i];
            flag = 0;
            for(j=0; j < 128; j++){
                if(guess == solution[j]){
                    solution[j] = -1;
                    flag = 1;
                }
            }
            if(flag == 0 && guess != -1){
                misses += 1;
            }

            if(misses >= 7){
                printf("You lose.\n");
                result = 1;
                break;
            }

            flag = 0;
            for(j = 0; j < 128; j++){
                if(solution[j] != -1){
                    flag = 1;
                }
            }
            if(flag == 0){
                printf("You win.\n");
                result = 1;
                break;
            }
        }
        if(result != 1){
            printf("You chickened out.\n");
        }
    }
    return 0;
}
