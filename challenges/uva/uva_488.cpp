#include <stdio.h>

int main(){
    int cases, amp, freq, i, j, k;

    scanf("%d\n", &cases);

    while(cases > 0){
        scanf("\n%d\n", &amp);
        scanf("%d\n", &freq);

        for(i = 0; i < freq; i++){
            for(j = 1; j <= amp; j++){
                for(k = 1; k <= j; k++){
                    printf("%d", j);
                }
                printf("\n");
            }
            for(j = amp-1; j >= 1; j--){
                for(k = j; k >= 1; k--){
                    printf("%d", j);
                }
                printf("\n");
            }
            if(i != freq -1){
                printf("\n");
            }
        }
        if(cases != 1){
            printf("\n");
        }
        cases--;
    }
    return 0;
}
