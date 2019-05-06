#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#define PATHARTIGOS "artigos"
#define PATHTSTRINGS "strings"
#define TAM_ARTIGO_STR_WITH_NEWLINE 23
#define TAM_ARTIGO_STR 22



void insereArtigo(char* name , int price) {
    int artigosFile = open(PATHARTIGOS,O_RDWR|O_APPEND);
    int stringsFile = open(PATHTSTRINGS,O_RDWR|O_APPEND);
    if(artigosFile <= 0 || stringsFile <= 0) {
        perror("File Not Found");
        return;
    }
    char tmpData[TAM_ARTIGO_STR_WITH_NEWLINE] = "";
    ssize_t stringRef = lseek(stringsFile,0,SEEK_END);
    sprintf(tmpData,"%010lu %010d\n",stringRef,price);
    strcat(name,"\n");
    lseek(artigosFile,0,SEEK_END);
    write(artigosFile,tmpData,strlen(tmpData));
    write(stringsFile,name,strlen(name));
    close(artigosFile);
    close(stringsFile);
}


void alteraNomeArtigo(int id,char* name) {
    int artigosFile = open(PATHARTIGOS,O_RDWR);
    int stringsFile = open(PATHTSTRINGS,O_RDWR);
    if(artigosFile <= 0 || stringsFile <= 0) {
        perror("File Not Found");
        return;
    }
    off_t  artOffset = (TAM_ARTIGO_STR * (id-1));
    lseek(artigosFile,artOffset,SEEK_CUR);
    char tmpData[TAM_ARTIGO_STR_WITH_NEWLINE] = "";
    ssize_t stringRef = lseek(stringsFile,0,SEEK_END);
    sprintf(tmpData,"%010lu",stringRef);
    write(artigosFile,tmpData,strlen(tmpData));
    write(stringsFile,name,strlen(name));
    close(artigosFile);
    close(stringsFile);
}

void alteraPrecoArtigo(int id , int price) {
    int artigosFile = open(PATHARTIGOS,O_RDWR);
    if(artigosFile <= 0) {
        perror("File Not Found");
        return;
    }
    off_t  artOffset = (TAM_ARTIGO_STR * (id-1));
    artOffset += 11; // TAMANHO DA STRING + ESPAÇO
    lseek(artigosFile,artOffset,SEEK_CUR);
    char tmpData[TAM_ARTIGO_STR_WITH_NEWLINE] = "";
    sprintf(tmpData,"%010d",price);
    write(artigosFile,tmpData,strlen(tmpData));
    close(artigosFile);    
}

int main() {
    int flag = 0;
    while(flag == 0) {
    char buffer[100] = ""; //PARA AGUENTAR OS 84 caracteres que as strings podem ter + o preço
    size_t bufSiz;
    bufSiz = read(0,buffer,100);
    char* token = strtok(buffer," ");
    char* fields[3];
    for(int i = 0 ; token; i++) {
        fields[i] = strdup(token);
        token = strtok(NULL," ");
    } 
    if(strcmp(fields[0],"i") == 0) insereArtigo(fields[1],atoi(fields[2]));
    else if(strcmp(fields[0],"n") == 0) alteraNomeArtigo(atoi(fields[1]),fields[2]);
    else if(strcmp(fields[0],"p") == 0) alteraPrecoArtigo(atoi(fields[1]),atoi(fields[2]));
    else flag = 1;  
    } 
    return 0;
}