#include <stdio.h>

int main(){
    int v, t;

    while(scanf("%d %d", &v, &t) == 2){
        printf("%d", v * 2 * t);
    }

    return 0;
}
