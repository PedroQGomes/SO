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

void writeToFDAgreg(int fileDescriptor) {
    AgregStruct tmp = {0,0,0};
    int fd = open("agregRes",O_RDONLY);
    while(read(fd,&tmp,sizeof(AgregStruct)) > 0) {
        if(tmp.qnt <= 0 ) continue;
        char tmpBuffFile[100];
        sprintf(tmpBuffFile,"Codigo: %d, Quantidade: %d, Preço: %d\n",tmp.ID,tmp.qnt, tmp.total);
        write(fileDescriptor,tmpBuffFile,strlen(tmpBuffFile));
    }
    close(fileDescriptor);
    close(fd);
}

int createFileWithTimeStamp() {
    int fileWStamp = open(getTimeStamp(),O_CREAT | O_RDWR,0666);
    writeToFDAgreg(fileWStamp);
    return 1;
}

off_t readPosicaoAgreg() {
    int saveState = open(PATHAGREGSTATE,O_CREAT | O_RDWR,0666);
    AgregState aS = {0};
    read(saveState,&aS,sizeof(AgregState));
    close(saveState);
    return aS.offset;
}

void savePosicaoAgreg(off_t ot) {
    int saveState = open(PATHAGREGSTATE,O_CREAT | O_RDWR,0666);
    AgregState aS = {0};
    read(saveState,&aS,sizeof(AgregState));
    aS.offset = ot;
    lseek(saveState,0,SEEK_SET);
    write(saveState,&aS,sizeof(AgregState)); 
    close(saveState);
}

off_t agregaEmIntervalo(int intervalo,off_t offset) {
    /*char buffTmp[10];
    sprintf(buffTmp,"%d",getpid()); */
    int filePid = open("agregRes",O_CREAT | O_RDWR , 0666);
    Sale sale;
    int fd = open(PATHVENDAS,O_RDONLY);
    if(lseek(fd,0,SEEK_END) == offset) return offset;
    lseek(fd,offset,SEEK_SET);
    while (read(fd,&sale,sizeof(Sale))>0 && intervalo > 0) {
       AgregStruct tmp = {0,0,0};
       lseek(filePid,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       read(filePid,&tmp,sizeof(AgregStruct));
       tmp.ID = sale.ID;
       tmp.qnt += sale.qnt;
       tmp.total += (sale.qnt * sale.price);
       lseek(filePid,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       write(filePid,&tmp,sizeof(AgregStruct));
       intervalo--;
    } 
    close(filePid);
    return (lseek(fd,0,SEEK_CUR));
}

off_t agrega(off_t offset) {
    /*char buffTmp[10];
    sprintf(buffTmp,"%d",getpid()); */
    int filePid = open("agregRes",O_CREAT | O_RDWR , 0666);
    Sale sale;
    int fd = open(PATHVENDAS,O_RDONLY);
    off_t res = lseek(fd,0,SEEK_END);
    if( res == offset) return offset;
    lseek(fd,offset,SEEK_SET);
    while (read(fd,&sale,sizeof(Sale))>0) {
       AgregStruct tmp = {0,0,0};
       lseek(filePid,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       read(filePid,&tmp,sizeof(AgregStruct));
       tmp.ID = sale.ID;
       tmp.qnt += sale.qnt;
       tmp.total += (sale.qnt * sale.price);
       lseek(filePid,sizeof(AgregStruct)*sale.ID,SEEK_SET);
       write(filePid,&tmp,sizeof(AgregStruct));
    } 
    close(filePid);
    return (lseek(fd,0,SEEK_CUR));
}


int main(int argc, char**argv) {
    off_t otread = readPosicaoAgreg();
    if(argc > 1) {
        otread = agregaEmIntervalo(atoi(argv[1]),otread);
    } else {
        otread = agrega(otread);
    }
    savePosicaoAgreg(otread);
    writeToFDAgreg(1);
    return 0;
    
}
