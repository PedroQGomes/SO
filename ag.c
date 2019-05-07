#include <sys/types.h>
#include <sys/sat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "constants.h"
int main(int argc, char**argv) {
    int n = 0;
    if(argc > 1) {
        n = atoi(argv[2]);
    }
    int fileDescriptor = open(artigos,O_RDONLY);
    dup2(fileDescriptor,0);
    
}
