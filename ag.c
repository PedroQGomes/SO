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

typedef struct _agregStruct {
    int id;
    int qnt;
    int total;
} AgregStruct;

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
    return open(getTimeStamp(),O_CREAT | O_RDWR);
}


int main(int argc, char**argv) {
    int fileWithTimeStamp = createFileWithTimeStamp();
    if(fileWithTimeStamp <= 0) return 1;
    Sale sale;
    while (read(0,&sale,sizeof(Sale))>0) {
       AgregStruct tmp;
       lseek(fileWithTimeStamp,sizeof(sale)*sale.ID,SEEK_SET);
       if(read(fileWithTimeStamp,&tmp,sizeof(AgregStruct))>0) {
           /*if(tmp.total > 0 ){
           } */
           tmp.qnt += sale.qnt;
           tmp.total += (sale.qnt * sale.price);
       }
       write(fileWithTimeStamp,&tmp,sizeof(AgregStruct));
    } 
    return 0;
    
}
