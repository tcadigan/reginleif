#include <stdio.h>

int main(){
    unsigned int init, limit;
    int num = 0;

    while(1){
        scanf("%d %d", &init, &limit);

        if((int)init < 0 && (int)limit < 0){
            break;
        }

        num++;
        printf("Case %d: A = %d, limit = %d, number of terms = ", num, init, limit);

        int terms = 0;

        while(init <= limit){
            if(init == 1){
                terms += 1;
                break;
            }
            if(init % 2 == 0){
                init = init / 2;
            }
            else{
                init = 3 * init + 1;
            }
            terms++;
        }
        printf("%d\n", terms);
    }
    return 0;
}
