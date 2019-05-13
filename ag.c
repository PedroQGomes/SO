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


void agrega(int fileDescriptor) {
    Sale sale = {0,0,0};
    int fAgreg = open("agregRes",O_CREAT | O_RDWR , 0666);
    while(read(fileDescriptor,&sale,sizeof(Sale)) > 0) {
        Sale tmp = {0,0,0};
        lseek(fAgreg,sale.ID*sizeof(Sale),SEEK_SET);
        read(fAgreg,&tmp,sizeof(Sale));
        lseek(fAgreg,sale.ID*sizeof(Sale),SEEK_SET);
        tmp.ID = sale.ID;
        tmp.qnt += sale.qnt;
        tmp.price += (sale.price*sale.qnt);
        write(fAgreg,&tmp,sizeof(Sale));
    }
    close(fAgreg);
}


void writeToString() {
    int fAgreg = open("agregRes",O_CREAT | O_RDWR , 0666);
    Sale tmp = {0,0,0};
    while(read(fAgreg,&tmp,sizeof(Sale)) > 0) {
        if(tmp.qnt <= 0) continue;
        char buffer[150] = "";
        sprintf(buffer,"ID: %d , Quantidade : %d , Total: %d \n",tmp.ID,tmp.qnt,tmp.price);
        write(1,buffer,strlen(buffer));
    }
    close(fAgreg);
}

void agregaFicheiros() {
    for(int i = 0;i<CONCURRENTAGG; i++) {
        char buffer[3] = "";
        sprintf(buffer,"%d",i);
        int fd = open(buffer,O_RDWR);
        agrega(fd);
        close(fd);
        remove(buffer);
    }
    writeToString();
}

void agregaEmIntervalo(int intervalo,char* indName) {
    Sale sale = {0,0,0};
    int fAgreg = open(indName,O_CREAT | O_RDWR , 0666);
    while(read(0,&sale,sizeof(Sale)) > 0 && intervalo > 0) {
        Sale tmp = {0,0,0};
        lseek(fAgreg,sale.ID*sizeof(Sale),SEEK_SET);
        read(fAgreg,&tmp,sizeof(Sale));
        lseek(fAgreg,sale.ID*sizeof(Sale),SEEK_SET);
        tmp.ID = sale.ID;
        tmp.qnt += sale.qnt;
        tmp.price += (sale.price*sale.qnt);
        write(fAgreg,&tmp,sizeof(Sale));
        intervalo--;
    }
    close(fAgreg);
}



int main(int argc, char**argv) {
    if(argc > 1) {
        int res = atoi(argv[1]);
        if(res == 0) {
            agregaFicheiros();
        } else
        agregaEmIntervalo(res,argv[2]);
    } else {
        agrega(0);
    }   
    return 0;
    
}