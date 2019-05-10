#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "constants.h"
// ESTRUTURA PARA GUARDAR POSICAO ONDE FICAMOS NO FICHEIRO VENDAS
typedef struct _agregState {
    off_t offset;
    //VendaAgreg vendaAgreg[1000];
} AgregState;



char* getTimeStamp() {
    struct tm *timeStampStruct;
    char timeStampStr[80];
    time_t currTime;
    time(&currTime);
    timeStampStruct = localtime(&currTime);
    strftime(timeStampStr,80,"%Y-%m-%dT%X",timeStampStruct);
    return strdup(timeStampStr);
}

int createFileWithTimeStamp() {
    return open(getTimeStamp(),O_CREAT | O_RDWR,0666);
}


int main(int argc, char**argv) {
    pid_t pid = getpid();
    int fileWithTimeStamp = createFileWithTimeStamp();
    if(fileWithTimeStamp <= 0) return 1;
    Sale sale;
    int fd = open(PATHVENDAS,O_RDONLY);
    while (read(fd,&sale,sizeof(Sale))>0) {
       AgregStruct tmp;
       lseek(fileWithTimeStamp,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       read(fileWithTimeStamp,&tmp,sizeof(AgregStruct));
       tmp.ID = sale.ID;
       tmp.qnt += sale.qnt;
       tmp.total += (sale.qnt * sale.price);
       lseek(fileWithTimeStamp,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       write(fileWithTimeStamp,&tmp,sizeof(AgregStruct));
    } 
    return 0;
    
}
