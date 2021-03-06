#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "constants.h"


int artigosFile,stringsFile;

ssize_t readln(int fd, char* buffer) {
    int size = 0;
    char c;
    while(read(fd,&c,1) > 0) {
        buffer[size] = c;
        size++;
        if(c == '\n') return size;
        if(c == '\0') return size;
    }
    return size;
}


int initFileDescriptors() {
    artigosFile = open(PATHARTIGOS, O_RDWR | O_APPEND);
    stringsFile = open(PATHTSTRINGS, O_RDWR | O_APPEND);
    if (artigosFile <= 0 || stringsFile <= 0)
    {
        perror("File Not Found");
        return 0;
    }
    return 1;
}

void testeMA() {
    if(initFileDescriptors() == 0) return; 
    Artigo tmp;
    while (read(artigosFile,&tmp,sizeof(Artigo)))
    { 
        lseek(stringsFile,tmp.stringRef,SEEK_SET);
        char buffer[100];
        readln(stringsFile,buffer);
        printf("ID:%d , STRINGREF: %lld, String: %s , PRICE: %d\n",tmp.ID,tmp.stringRef,buffer,tmp.price);

    }
    close(artigosFile);
    close(stringsFile);
}

void testeAG(char *fd) {
    int fileDescriptor = open(fd,O_RDONLY);
    Sale sale;
    while (read(fileDescriptor,&sale,sizeof(Sale)) > 0)
    { 
        printf("ID:%d , QNT: %d, MONTANTE:%d\n",sale.ID,sale.qnt,sale.price);

    }
    close(fileDescriptor);
}

void testeStock() {
    int fileDescriptor = open(PATHSTOCKS,O_RDONLY);
    Stocks stk;
    while (read(fileDescriptor,&stk,sizeof(Stocks)) > 0)
    { 
        printf("ID:%d , Stock: %d\n",stk.numCod,stk.qnt);

    }
    close(fileDescriptor);
}

void testeVendas() {
    int fileDescriptor = open(PATHVENDAS,O_RDONLY);
    Sale sale;
    while (read(fileDescriptor,&sale,sizeof(Sale)) > 0)
    { 
        printf("ID:%d , QNT: %d, MONTANTE:%d\n",sale.ID,sale.qnt, sale.price);

    }
    close(fileDescriptor);
}

void testeRead() {
    while(1) {
        char buffer[100] = "";
        read(0,buffer,100);
        write(1,buffer,100);
    }
}

int main()
{
    //testeRead();
    //testeMA();
    //testeStock();
    testeVendas();
    printf("\n");
    testeAG("0");
    printf("\n");
    testeAG("1");
    return 0;
    
}