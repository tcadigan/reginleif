#include "code-writer.h"
#include "parser.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <src_loc>\n", argv[0]);

        return -1;
    }

    struct stat buf;

    int result = stat(argv[1], &buf);

    if(result == -1) {
        fprintf(stderr, "Unable to stat source location\n");

        return -1;
    }
    
    if(S_ISDIR(buf.st_mode)) {
        char *full_path = (char *)malloc(PATH_MAX + 1);

        if(full_path == NULL) {
            fprintf(stderr, "Unable to allocate full path\n");

            return -1;
        }

        full_path = realpath(argv[1], full_path);

        if(full_path == NULL) {
            fprintf(stderr, "Unable to resolve real path\n");

            return -1;
        }

        DIR *dd = opendir(full_path);

        if(dd == NULL) {
            fprintf(stderr, "Unable to open source directory\n");

            return -1;
        }

        struct dirent *dirp = readdir(dd);
        int first = 1;
        
        while(dirp) {
            if((strncmp(dirp->d_name, ".", strlen(dirp->d_name)) == 0)
               || (strncmp(dirp->d_name, "..", strlen(dirp->d_name)) == 0)) {
                dirp = readdir(dd);
                
                continue;
            }
            
            char *suffix = strrchr(dirp->d_name, '.');
            
            if((suffix != NULL) && (strncmp(suffix, ".vm", strlen(suffix)) == 0)) {
                if(first) {
                    char *folder = strrchr(full_path, '/');
                    
                    if(folder != NULL) {
                        while((*(folder + 1) == '\0') && (folder != NULL)) {
                            *folder = '\0';
                            folder = strrchr(full_path, '/');
                        }

                        if(folder) {
                            construct_code_writer(folder + 1);
                            write_init();
                            first = 0;
                        }
                    }
                }
                else {
                    set_filename(dirp->d_name);
                }

                char *file_path = (char *)malloc(PATH_MAX + NAME_MAX + 2);

                if(file_path == NULL) {
                    fprintf(stderr, "Unable to allocate file path\n");

                    return -1;
                }

                strncpy(file_path, full_path, strlen(full_path));
                strncpy(file_path + strlen(full_path), "/", strlen("/"));
                
                strncpy(file_path + strlen(full_path) + strlen("/"),
                        dirp->d_name,
                        strlen(dirp->d_name));

                file_path[strlen(full_path) + strlen("/") + strlen(dirp->d_name)] = '\0';
                
                construct_parser(file_path);
                
                while(parser_has_more_commands()) {
                    advance_parser();
                    enum VM_TYPE type = get_command_type();
                    
                    if((type == C_PUSH) || (type == C_POP)) {
                        printf("TC_DEBUG: arg1() -> \'%s\' arg2() -> \'%d\'\n", get_arg1(), get_arg2());
                        write_push_pop(type, get_arg1(), get_arg2());
                    }
                    else if(type == C_ARITHMETIC) {
                        printf("TC_DEBUG: (arithmetic) arg1() -> \'%s\'\n", get_arg1());
                        write_arithmetic(get_arg1());
                    }
                    else if(type == C_LABEL) {
                        printf("TC_DEBUG: (label) arg1() -> \'%s\'\n", get_arg1());
                        write_label(get_arg1());
                    }
                    else if(type == C_GOTO) {
                        printf("TC_DEBUG: (goto) arg1() -> \'%s\'\n", get_arg1());
                        write_goto(get_arg1());
                    }
                    else if(type == C_IF) {
                        printf("TC_DEBUG: (if) arg1() -> \'%s\'\n", get_arg1());
                        write_if(get_arg1());
                    }
                    else if(type == C_RETURN) {
                        printf("TC_DEBUG: (return)\n");
                        write_return();
                    }
                    else if(type == C_FUNCTION) {
                        printf("TC_DEBUG: (function) arg1() -> \'%s\' arg2() -> %d\n", get_arg1(), get_arg2());
                        write_function(get_arg1(), get_arg2());
                    }
                    else if(type == C_CALL) {
                        printf("TC_DEBUG: (call) arg1() -> \'%s\' arg2() -> %d\n", get_arg1(), get_arg2());
                        write_call(get_arg1(), get_arg2());
                    }
                }
            }

            dirp = readdir(dd);
        }
    }
    else if(S_ISREG(buf.st_mode)) {
        construct_parser(argv[1]);        
        construct_code_writer(argv[1]);

        while(parser_has_more_commands()) {
            advance_parser();
            
            enum VM_TYPE type = get_command_type();

            if((type == C_PUSH) || (type == C_POP)) {
                printf("TC_DEBUG: arg1() -> \'%s\' arg2() -> \'%d\'\n", get_arg1(), get_arg2());
                write_push_pop(type, get_arg1(), get_arg2());
            }
            else if(type == C_ARITHMETIC) {
                printf("TC_DEBUG: (arithmetic) arg1() -> \'%s\'\n", get_arg1());
                write_arithmetic(get_arg1());
            }
            else if(type == C_LABEL) {
                printf("TC_DEBUG: (label) arg1() -> \'%s\'\n", get_arg1());
                write_label(get_arg1());
            }
            else if(type == C_GOTO) {
                printf("TC_DEBUG: (goto) arg1() -> \'%s\'\n", get_arg1());
                write_goto(get_arg1());
            }
            else if(type == C_IF) {
                printf("TC_DEBUG: (if) arg1() -> \'%s\'\n", get_arg1());
                write_if(get_arg1());
            }
            else if(type == C_RETURN) {
                printf("TC_DEBUG: (return)\n");
                write_return();
            }
            else if(type == C_FUNCTION) {
                printf("TC_DEBUG: (function) arg1() -> \'%s\' arg2() -> %d\n", get_arg1(), get_arg2());
                write_function(get_arg1(), get_arg2());
            }
            else if(type == C_CALL) {
                printf("TC_DEBUG: (call) arg1() -> \'%s\' arg2() -> %d\n", get_arg1(), get_arg2());
                write_call(get_arg1(), get_arg2());
            }
        }
    }
    
    close_code_writer();
    
    return 0;
}
