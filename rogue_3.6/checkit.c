#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char key[] = "magicword";
    
    printf(">>>%s<<<\n", crypt(key, "mT"));

    return 0;
}
