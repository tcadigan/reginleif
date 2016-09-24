#include "code.h"
#include "parser.h"
#include "symbolTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *intToBinary(int val)
{
    char *result = (char *)malloc(16);

    if(result == NULL) {
        fprintf(stderr, "Unable to allocate space for decToBinary");

        return NULL;
    }
    
    result[0] = ((val & 0x00004000) >> 14) + '0';
    result[1] = ((val & 0x00002000) >> 13) + '0';
    result[2] = ((val & 0x00001000) >> 12) + '0';
    result[3] = ((val & 0x00000800) >> 11) + '0';
    result[4] = ((val & 0x00000400) >> 10) + '0';
    result[5] = ((val & 0x00000200) >> 9) + '0';
    result[6] = ((val & 0x00000100) >> 8) + '0';
    result[7] = ((val & 0x00000080) >> 7) + '0';
    result[8] = ((val & 0x00000040) >> 6) + '0';
    result[9] = ((val & 0x00000020) >> 5) + '0';
    result[10] = ((val & 0x00000010) >> 4) + '0';
    result[11] = ((val & 0x00000008) >> 3) + '0';
    result[12] = ((val & 0x00000004) >> 2) + '0';
    result[13] = ((val & 0x00000002) >> 1) + '0';
    result[14] = ((val & 0x00000001) >> 0) + '0';
    result[15] = '\0';
    
    return result;
}

char *decToBinary(char *str)
{
    int val = atoi(str);

    return intToBinary(val);
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <asm_file>\n", argv[0]);

        return 1;
    }

    char *output_filename = (char *)malloc(strlen(argv[1]) + 2);

    if(output_filename == NULL) {
        fprintf(stderr, "Unable to allocate output filename");

        return 1;
    }
    
    memcpy(output_filename, argv[1], strlen(argv[1]) - 3);
    output_filename[strlen(argv[1]) - 3] = 'h';
    output_filename[strlen(argv[1]) - 2] = 'a';
    output_filename[strlen(argv[1]) - 1] = 'c';
    output_filename[strlen(argv[1])] = 'k';
    output_filename[strlen(argv[1]) + 1] = '\0';

    FILE *fd = fopen(output_filename, "w");

    if(fd == NULL) {
        fprintf(stderr, "Unable to open output filename");

        return 1;
    }
    
    initialize(argv[1]);
    Constructor();

    char *temp = (char *)malloc(strlen("SCREEN") + 1);

    if(temp == NULL) {
        fprintf(stderr, "Unable to allocate initial strings");

        return 1;
    }

    temp[0] = 'S';
    temp[1] = 'P';
    temp[2] = '\0';
    addEntry(temp, 0x00000000);

    temp[0] = 'L';
    temp[1] = 'C';
    temp[2] = 'L';
    temp[3] = '\0';
    addEntry(temp, 0x00000001);

    temp[0] = 'A';
    temp[1] = 'R';
    temp[2] = 'G';
    temp[3] = '\0';
    addEntry(temp, 0x00000002);

    temp[0] = 'T';
    temp[1] = 'H';
    temp[2] = 'I';
    temp[3] = 'S';
    temp[4] = '\0';
    addEntry(temp, 0x00000003);

    temp[0] = 'T';
    temp[1] = 'H';
    temp[2] = 'A';
    temp[3] = 'T';
    temp[4] = '\0';
    addEntry(temp, 0x00000004);

    temp[0] = 'R';
    temp[1] = '0';
    temp[2] = '\0';
    addEntry(temp, 0x00000000);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '\0';
    addEntry(temp, 0x00000001);

    temp[0] = 'R';
    temp[1] = '2';
    temp[2] = '\0';
    addEntry(temp, 0x00000002);

    temp[0] = 'R';
    temp[1] = '3';
    temp[2] = '\0';
    addEntry(temp, 0x00000003);

    temp[0] = 'R';
    temp[1] = '4';
    temp[2] = '\0';
    addEntry(temp, 0x00000004);

    temp[0] = 'R';
    temp[1] = '5';
    temp[2] = '\0';
    addEntry(temp, 0x00000005);

    temp[0] = 'R';
    temp[1] = '6';
    temp[2] = '\0';
    addEntry(temp, 0x00000006);

    temp[0] = 'R';
    temp[1] = '7';
    temp[2] = '\0';
    addEntry(temp, 0x00000007);

    temp[0] = 'R';
    temp[1] = '8';
    temp[2] = '\0';
    addEntry(temp, 0x00000008);

    temp[0] = 'R';
    temp[1] = '9';
    temp[2] = '\0';
    addEntry(temp, 0x00000009);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '0';
    temp[3] = '\0';
    addEntry(temp, 0x0000000a);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '1';
    temp[3] = '\0';
    addEntry(temp, 0x0000000b);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '2';
    temp[3] = '\0';
    addEntry(temp, 0x0000000c);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '3';
    temp[3] = '\0';
    addEntry(temp, 0x0000000d);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '4';
    temp[3] = '\0';
    addEntry(temp, 0x0000000e);

    temp[0] = 'R';
    temp[1] = '1';
    temp[2] = '5';
    temp[3] = '\0';
    addEntry(temp, 0x0000000f);

    temp[0] = 'S';
    temp[1] = 'C';
    temp[2] = 'R';
    temp[3] = 'E';
    temp[4] = 'E';
    temp[5] = 'N';
    temp[6] = '\0';
    addEntry(temp, 0x00004000);

    temp[0] = 'K';
    temp[1] = 'B';
    temp[2] = 'D';
    temp[3] = '\0';
    addEntry(temp, 0x00006000);

    int rom_address = 0;
    
    while(hasMoreCommands()) {
        advance();

        if((commandType() == A_COMMAND) || (commandType() == C_COMMAND)) {
            ++rom_address;
        }
        else {
            char *s = symbol();

            addEntry(s, rom_address);
        }
    }

    initialize(argv[1]);
    
    int ram_address = 16;
    while(hasMoreCommands()) {
        advance();

        if(commandType() == A_COMMAND) {
            char *s = symbol();
            char *bs = NULL;
            
            if((s[0] >= '0') && (s[0] <= '9')) {
                bs = decToBinary(s);
                printf("symbol -> \'%s\', \'0%s\'\n", s, bs);
                fprintf(fd, "0%s\n", bs);
            }
            else {
                if(contains(s)) {
                    bs = intToBinary(GetAddress(s));
                    printf("symbol -> \'%s\', \'0%s\'\n", s, bs);
                    fprintf(fd, "0%s\n", bs);
                }
                else {
                    addEntry(s, ram_address);
                    ++ram_address;
                    
                    bs = intToBinary(GetAddress(s));
                    printf("symbol -> \'%s\', \'0%s\'\n", s, bs);
                    fprintf(fd, "0%s\n", bs);
                }
            }

            if(s) {
                free(s);
            }

            if(bs) {
                free(bs);
            }
        }
        else if(commandType() == C_COMMAND) {
            char *d = dest();
            char *c = comp();
            char *j = jump();
            char *bd = dest(d);
            char *bc = comp(c);
            char *bj = jump(j);
            printf("dest -> \'%s\', comp -> \'%s\', jump -> \'%s\', \'111%s%s%s\'\n", d, c, j, bc, bd, bj);
            fprintf(fd, "111%s%s%s\n", bc, bd, bj);
            free(bd);
            free(bc);
            free(bj);
            free(d);
            free(c);
            free(j);
        }
    }

    fclose(fd);
    free(output_filename);
    
    return 0;
}
