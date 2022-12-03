#include <stdio.h>

int lookup(char a){
    if(a == '.'){
        return 1;
    }
    if(a == ' '){
        return 0;
    }
    if(a == 'x'){
        return 2;
    }
    return 3;
}

char translate(int a){
    if(a == 1){
        return '.';
    }
    if(a == 0){
        return ' ';
    }
    if(a == 2){
        return 'x';
    }
    return 'W';
}

int main(){
    int cases;

    scanf("%d", &cases);

    while(cases > 0){
        int i;
        int program[10] = {0};
        char dishes[40];
        char nextday[40];
        for(i = 0; i < 10; i++){
            scanf("%d", &program[i]);
        }

        for(i = 0; i < 40; i++){
            if(i == 19){
                dishes[i] = '.';
            }
            else{
                dishes[i] = ' ';
            }
            nextday[i] = dishes[i];
            printf("%c", dishes[i]);
        }
        printf("\n");

        int days = 1;
        while(days < 50){
            nextday[0] = translate(program[lookup(dishes[0]) + lookup(dishes[1])]);
            printf("%c", nextday[0]);

            for(i = 1; i < 39; i++){
                nextday[i] = translate(program[lookup(dishes[i-1]) + lookup(dishes[i]) + lookup(dishes[i+1])]);
                printf("%c", nextday[i]);
            }

            nextday[39] = translate(program[lookup(dishes[38]) + lookup(dishes[39])]);
            printf("%c\n", nextday[39]);

            for(i = 0; i < 40; i++){
                dishes[i] = nextday[i];
            }
            days++;
        }
        if(cases != 1){
            printf("\n");
        }
        cases--;
    }
    return 0;
}
