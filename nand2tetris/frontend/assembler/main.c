#include "code.h"
#include "parser.h"
#include "symbol-table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *int_to_binary(int val)
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

char *decimal_to_binary(char *str)
{
    return int_to_binary(atoi(str));
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
    
    initialize_parser(argv[1]);
    construct_symbol_table();

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
    
    while(parser_has_more_commands()) {
        advance_parser();

        char *s;

        switch(get_command_type()) {
        case A_COMMAND:
        case C_COMMAND:
            ++rom_address;

            break;
        case L_COMMAND:
            s = get_symbol();

            if(s) {
                add_entry(s, rom_address);
                free(s);
            }
            
            break;
        default:
            fprintf(stderr, "Unknown command type encountered in first pass.\n");
        }
    }

    initialize_parser(argv[1]);
    
    int ram_address = 16;
    while(parser_has_more_commands()) {
        advance_parser();

        char *text;
        char *binary;
        char *buf = (char *)malloc(strlen("XXXXXXXXXXXXXXXX") + 1);

        if(buf == NULL) {
            fprintf(stderr, "Unable to allocate space for output");

            continue;
        }
        
        switch(get_command_type()) {
        case A_COMMAND:
            text = get_symbol();

            if(!text) {
                free(buf);
                
                continue;
            }
            
            binary = NULL;

            strncpy(buf, "0", strlen("0"));
            
            if((text[0] >= '0') && (text[0] <= '9')) {
                binary = decimal_to_binary(text);
                free(text);
            }
            else {
                if(!contains(text)) {
                    add_entry(text, ram_address);
                    ++ram_address;
                }

                binary = int_to_binary(get_address(text));
                free(text);
            }

            if(!binary) {
                free(buf);
                free(text);

                continue;
            }

            strncpy(buf + strlen("0"), binary, strlen(binary));
            free(binary);
            
            break;
        case C_COMMAND:
            strncpy(buf, "111", strlen("111"));

            text = get_comp();

            if(!text) {
                free(buf);

                continue;
            }
            
            binary = comp(text);
            free(text);
            
            if(!binary) {
                free(buf);

                continue;
            }

            strncpy(buf + strlen("111"), binary, strlen(binary));
            free(binary);
            text = get_dest();

            if(!text) {
                free(buf);

                continue;
            }

            binary = dest(text);
            free(text);
            
            if(!binary) {
                free(buf);
                
                continue;
            }

            strncpy(buf + strlen("111XXXXXXX"), binary, strlen(binary));
            free(binary);
            text = get_jump();

            if(!text) {
                free(buf);

                continue;
            }

            binary = jump(text);
            free(text);
            
            if(!binary) {
                free(buf);
                
                continue;
            }

            strncpy(buf + strlen("111XXXXXXXXXX"), binary, strlen(binary));
            free(binary);
            
            break;
        case L_COMMAND:
            free(buf);
            continue;

            break;
        default:
            fprintf(stderr, "Unknown command type encountered in second pass.\n");
            free(buf);

            continue;
        }

        buf[strlen("XXXXXXXXXXXXXXXX")] = '\0';
        fprintf(fd, "%s\n", buf);
        free(buf);
    }

    fclose(fd);
    free(output_filename);
    destroy_symbol_table();
    destroy_parser();
    
    return 0;
}
