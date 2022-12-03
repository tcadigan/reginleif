#include <stdio.h>

int main(){
    int cases, farmers;
    int size_farm, num_animals, friends;
    double animal_area, result, total;

    scanf("%d", &cases);

    while(cases > 0){
        scanf("%d", &farmers);
        total = 0;
        while(farmers > 0){
            scanf("%d %d %d", &size_farm, &num_animals, &friends);
            if(num_animals == 0){
                animal_area = 0;
            }
            else{
                animal_area = (double)size_farm / num_animals;
            }
            result = animal_area * num_animals * friends;
            total += result;
            farmers--;
        }

        printf("%d\n", (int)total);
        cases--;
    }

    return 0;
}
