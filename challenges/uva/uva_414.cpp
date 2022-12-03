#include <stdio.h>

int main(){
    int number, i, counter, total, min;
    char character;
    int array[13];

    while(1){
        counter = 0;
        total = 0;
        min = 23;

        for(i = 0; i < 13; i++){
            array[i] = -1;
        }

        scanf("%d", &number);

        if(number == 0){
            break;
        }

        while(counter < number){
            for(i = 0; i <= 25; i++){
                scanf("%c", &character);
                if(character == ' '){
                    if(array[counter] == -1){
                        array[counter] = 1;
                    }
                    else{
                        array[counter] += 1;
                    }
                }
                if(character == 'X' && array[counter] == -1){
                    array[counter] = 0;
                }
            }
            counter++;
        }

        for(i = 0; i < 13; i++){
            if(array[i] != -1 && array[i] < min){
                min = array[i];
            }
        }
        for(i = 0; i < 13; i++){
            array[i] -= min;
            if(array[i] > 0){
                total += array[i];
            }
        }
        printf("%d\n", total);
    }

    return 0;
}
