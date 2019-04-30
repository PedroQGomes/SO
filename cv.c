#include <sys/types.h>
#include <sys/sat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int arc, const char* argv[]){


	mkfifo("/tmp/serverFIFO");

}