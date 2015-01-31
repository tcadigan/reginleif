#include <crypt.h>
#include <stdio.h>
#include <termios.h>

int main(int argc, char *argv[])
{
    printf("Crypted: %s\n", crypt(getpass("Password: "), "mT"));
}
