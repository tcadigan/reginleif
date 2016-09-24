#include "code.h"
#include "parser.h"
#include "symbol-table.h"

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

    int i;

    for(i = 0; i < 15; ++i) {
        result[i] = ((val & (1 << (14 - i))) >> (14 - i)) + '0';
    }
    
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
    strncpy(output_filename + strlen(argv[1]) - 3, "hack", strlen("hack"));
    output_filename[strlen(argv[1]) + 1] = '\0';

    FILE *fd = fopen(output_filename, "w");

    if(fd == NULL) {
        fprintf(stderr, "Unable to open output filename");

        return 1;
    }
    
    initialize(argv[1]);
    constructor();

    char *temp = (char *)malloc(strlen("SCREEN") + 1);

    if(temp == NULL) {
        fprintf(stderr, "Unable to allocate initial strings");

        return 1;
    }

    add_entry("SP",     0x00000000);
    add_entry("R0",     0x00000000);
    add_entry("LCL",    0x00000001);
    add_entry("R1",     0x00000001);
    add_entry("ARG",    0x00000002);
    add_entry("R2",     0x00000002);
    add_entry("THIS",   0x00000003);
    add_entry("R3",     0x00000003);
    add_entry("THAT",   0x00000004);
    add_entry("R4",     0x00000004);
    add_entry("R5",     0x00000005);
    add_entry("R6",     0x00000006);
    add_entry("R7",     0x00000007);
    add_entry("R8",     0x00000008);
    add_entry("R9",     0x00000009);
    add_entry("R10",    0x0000000a);
    add_entry("R11",    0x0000000b);
    add_entry("R12",    0x0000000c);
    add_entry("R13",    0x0000000d);
    add_entry("R14",    0x0000000e);
    add_entry("R15",    0x0000000f);
    add_entry("SCREEN", 0x00004000);
    add_entry("KBD",    0x00006000);

    int rom_address = 0;
    
    while(has_more_commands()) {
        advance();

        if((command_type() == A_COMMAND) || (command_type() == C_COMMAND)) {
            ++rom_address;
        }
        else {
            char *s = get_symbol();

            add_entry(s, rom_address);
        }
    }

    initialize(argv[1]);
    
    int ram_address = 16;
    while(has_more_commands()) {
        advance();

        if(command_type() == A_COMMAND) {
            char *s = get_symbol();
            char *bs = NULL;
            
            if((s[0] >= '0') && (s[0] <= '9')) {
                bs = decToBinary(s);
                printf("symbol -> \'%s\', \'0%s\'\n", s, bs);
                fprintf(fd, "0%s\n", bs);
            }
            else {
                if(contains(s)) {
                    bs = intToBinary(get_address(s));
                    printf("symbol -> \'%s\', \'0%s\'\n", s, bs);
                    fprintf(fd, "0%s\n", bs);
                }
                else {
                    add_entry(s, ram_address);
                    ++ram_address;
                    
                    bs = intToBinary(get_address(s));
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
        else if(command_type() == C_COMMAND) {
            char *d = get_dest();
            char *c = get_comp();
            char *j = get_jump();
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
