#include <stdio.h>

char mirror(char a){
    switch(a){
    case 'A':
    case 'H':
    case 'I':
    case 'M':
    case 'O':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case '1':
    case '8':
        return a;
        break;
    case 'E':
        return '3';
        break;
    case 'J':
        return 'L';
        break;
    case 'L':
        return 'J';
        break;
    case 'S':
        return '2';
        break;
    case 'Z':
        return '5';
        break;
    case '2':
        return 'S';
        break;
    case '3':
        return 'E';
        break;
    case '5':
        return 'Z';
        break;
    default:
        return -1;
    }
}

int main(){
    char word[21];
    int i;

    while(1){
        for(i = 0; i < 21; i++){
            word[i] = -1;
        }

        scanf("%s\n", word);
        if(*word == EOF){
            break;
        }

        int end_pos = 20;
        for(i = 20; i >= 0; i--){
            if(word[i] != -1 && word[i] != '\0'){
                end_pos = i;
                break;
            }
        }

        int start_pos;
        int pal = 1;
        int rev = 1;
        for(start_pos = 0; start_pos <= end_pos; start_pos++, end_pos--){
            if(word[start_pos] != word[end_pos]){
                pal = 0;
            }
            if(word[start_pos] != mirror(word[end_pos])){
                rev = 0;
            }
        }

        if(rev){
            printf("%s -- is a mirrored ", word);
            if(pal){
                printf("palindrome.\n");
            }
            else{
                printf("string.\n");
            }
        }
        else{
            printf("%s -- is ", word);
            if(pal){
                printf("a regular palindrome.\n");
            }
            else{
                printf("not a palindrome.\n");
            }
        }
        printf("\n");
    }

    return 0;
}
      
