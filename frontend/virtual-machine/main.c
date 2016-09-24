#include "codeWriter.h"
#include "parse.h"

#include <dirent.h>
#include <stdio.h>
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
        DIR *dd = opendir(argv[1]);

        if(dd == NULL) {
            fprintf(stderr, "Unable to open source directory\n");

            return -1;
        }

        struct dirent *dirp = readdir(dd);
        int first = 1;
        
        while(dirp) {
            if((dirp->d_name[strlen(dirp->d_name) - 1] == 'm')
               && (dirp->d_name[strlen(dirp->d_name) - 2] == 'v')
               && (dirp->d_name[strlen(dirp->d_name) - 3] == '.')) {               
                if(first) {
                    char filename[] = { 'm', 'i', 'x', 'e', 'd', '.', 'a', 's', 'm', '\0' };
                    codeWriterConstructor(filename);
                    first = 0;
                }
                else {
                    
                    setFileName(dirp->d_name);
                }
                
                parseConstructor(dirp->d_name);
                
                while(hasMoreCommands()) {
                    advance();
                    VM_TYPE type = commandType();
                    
                    if((type == C_PUSH) || (type == C_POP)) {
                        writePushPop(type, arg1(), arg2());
                    }
                    else if(type == C_ARITHMETIC) {
                        writeArithmetic(arg1());
                    }
                }
            }

            dirp = readdir(dd);
        }
    }
    else if(S_ISREG(buf.st_mode)) {
        codeWriterConstructor(argv[1]);
        parseConstructor(argv[1]);

        while(hasMoreCommands()) {
            advance();
            
            VM_TYPE type = commandType();

            if((type == C_PUSH) || (type == C_POP)) {
                printf("TC_DEBUG: arg1() -> \'%s\' arg2() -> \'%d\'\n", arg1(), arg2());
                writePushPop(type, arg1(), arg2());
            }
            else if(type == C_ARITHMETIC) {
                printf("TC_DEBUG: arg1() -> \'%s\'\n", arg1());
                writeArithmetic(arg1());
            }
        }
    }
    
    Close();
    
    return 0;
}
