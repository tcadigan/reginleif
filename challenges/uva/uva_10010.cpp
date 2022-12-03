#include <stdio.h>
#include <ctype.h>

int main(){
    int cases, i, j, k, rows, columns, num_words;
    char matrix[50][50];
    char word[50];

    scanf("%d\n\n", &cases);

    while(cases > 0){

        for(i = 0; i < 50; i++){
            for(j = 0; j < 50; j++){
                matrix[i][j] = 0;
            }
        }

        scanf("%d %d\n", &rows, &columns);

        for(i = 0; i < rows; i++){
            for(j = 0; j < columns; j++){
                scanf("%c", &matrix[i][j]);
                matrix[i][j] = tolower(matrix[i][j]);
            }
            scanf("\n");
        }

        scanf("%d", &num_words);

        while(num_words > 0){

            for(i = 0 ; i < 50; i++){
                word[i] = 0;
            }

            scanf("%s\n", word);

            for(i = 0; i < 49; i++){
                word[i] = tolower(word[i]);
            }

            int end_pos=0;
            for(i = 49; i >= 0; i--){
                if(word[i] != 0 && word[i] != '\0'){
                    end_pos = i;
                    break;
                }
            }

            int match;
            for(i = 0; i < rows; i++){
                for(j = 0; j < columns; j++){
                    match = 1;

                    /*up check*/
                    for(k = 0; k <= end_pos; k++){
                        if(i-k < 0 || word[k] != matrix[i-k][j]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }


                    /* down check */
                    for(k = 0; k <= end_pos; k++){
                        if(i+k >= 50 || word[k] != matrix[i+k][j]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* left check */
                    for(k = 0; k <= end_pos; k++){
                        if(j-k < 0 || word[k] != matrix[i][j-k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* right check */
                    for(k = 0; k <= end_pos; k++){
                        if(j+k >= 50 || word[k] != matrix[i][j+k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* right-up check */
                    for(k = 0; k <= end_pos; k++){
                        if((i-k < 0 || j+k >= 50) || word[k] != matrix[i-k][j+k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* right-down check */
                    for(k = 0; k <= end_pos; k++){
                        if((i+k >= 50 || j+k >= 50) || word[k] != matrix[i+k][j+k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* left-up check */
                    for(k = 0; k <= end_pos; k++){
                        if((i-k < 0 || j-k < 0) || word[k] != matrix[i-k][j-k]){
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1){
                        break;
                    }
                    else{
                        match = 1;
                    }

                    /* left-down check */
                    for(k = 0; k <= end_pos; k++){
                        if((i+k >= 50 || j-k < 0) || word[k] != matrix[i+k][j-k]){
                            match = 0;
                            break;
                        }
                    }
                }
                if(match == 1){
                    printf("%d %d\n", i+1, j+1);
                    break;
                }
            }
            num_words--;
        }
        if(cases != 1){
            printf("\n");
        }
        cases--;
    }
    return 0;
}
