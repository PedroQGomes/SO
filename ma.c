#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "constants.h"


int artigosFile,stringsFile;

ssize_t readln(int fd, char* buffer, ssize_t buffLength) {
    int size = 0;
    char c;
    while(read(fd,&c,1) > 0 && size < buffLength) {
        if(c == -1) return -1;
        buffer[size] = c;
        size++;
        if(c == '\n') return size;
        if(c == '\0') return size;
    }
    return size;
}



void compactStrings() {
    int newStringsFile = open("tmpStrings",O_CREAT | O_WRONLY | O_APPEND,0666);
    Artigo tmp = {0,0,0};
    lseek(artigosFile,0,SEEK_SET);
    while(read(artigosFile,&tmp,sizeof(Artigo)) > 0) {
        off_t _stringRef = lseek(newStringsFile,0,SEEK_CUR);
        lseek(stringsFile,tmp.stringRef,SEEK_SET);
        char buffer[100];
        readln(stringsFile,buffer,100);
        write(newStringsFile,buffer,strlen(buffer));
        lseek(artigosFile,tmp.ID*(sizeof(Artigo)),SEEK_SET);
        tmp.stringRef = _stringRef;
        write(artigosFile,&tmp,sizeof(Artigo));
    }
    close(newStringsFile);
    close(stringsFile);
    remove(PATHTSTRINGS);
    rename("tmpStrings",PATHTSTRINGS);
    stringsFile = open(PATHTSTRINGS,O_RDWR);
}

int stringFileOverPercentage() {
    ssize_t current = lseek(artigosFile,0,SEEK_SET);
    ssize_t endOfArtigos = lseek(artigosFile,0,SEEK_END);
    ssize_t endOfStrings = lseek(stringsFile,0,SEEK_END);
    lseek(artigosFile,current,SEEK_SET);
    if(1.2 * endOfArtigos < endOfStrings) {
        compactStrings();
        return 1;
    }
    return 0;
    
}



void callServer(int flagpipe,int ID) {
    int fifo = open(serverPipe,O_WRONLY);
    Action act = malloc(sizeof(Action));
    act->pid = flagpipe;
    act->codigo = ID;
    write(fifo,act,sizeof(Action));
    close(fifo);
}

void insereArtigo(char *name, int _price)
{
    ssize_t strRef = lseek(stringsFile, 0, SEEK_END);
    Artigo artigo;
    artigo.ID = lseek(artigosFile,0,SEEK_END)/sizeof(artigo);
    artigo.stringRef = strRef;
    artigo.price = _price;
    lseek(artigosFile, 0, SEEK_END);
    write(artigosFile, &artigo, sizeof(Artigo));
    strcat(name,"\n");
    write(stringsFile, name, strlen(name));
}


void alteraNomeArtigo(int id, char *name)
{   

    off_t artOffset = (sizeof(Artigo) * (id));
    lseek(artigosFile, artOffset, SEEK_SET);
    ssize_t _stringRef = lseek(stringsFile, 0, SEEK_END);
    Artigo tmp;
    read(artigosFile,&tmp,sizeof(Artigo));
    tmp.stringRef = _stringRef;
    lseek(artigosFile, artOffset, SEEK_SET);
    write(artigosFile, &tmp, sizeof(Artigo));
    write(stringsFile, name, strlen(name));
}


void alteraPrecoArtigo(int id, int _price)
{
    off_t artOffset = (sizeof(Artigo) * (id));
    lseek(artigosFile, artOffset, SEEK_SET);
    Artigo tmp;
    read(artigosFile,&tmp,sizeof(Artigo));
    lseek(artigosFile, artOffset, SEEK_SET);
    tmp.price = _price;
    write(artigosFile, &tmp, sizeof(Artigo));
    callServer(-2,id);
}

int initFileDescriptors() {
    artigosFile = open(PATHARTIGOS,O_CREAT | O_RDWR, 0666);
    stringsFile = open(PATHTSTRINGS,O_CREAT | O_RDWR,0666);
    if (artigosFile <= 0 || stringsFile <= 0)
    {
        perror("File Not Found");
        return 0;
    }
    return 1;
}

void readInput(char* buffer,int *flag) {
    char *token = strtok(buffer, " ");
    char *fields[3];
    for (int i = 0; token; i++)
        {
            fields[i] = strdup(token);
            token = strtok(NULL, " ");
        }
        if (strcmp(fields[0], "i") == 0){
            insereArtigo(fields[1], atoi(fields[2]));
            printf("Artigo Inserido\n");
        }
        else if (strcmp(fields[0], "n") == 0) {
            alteraNomeArtigo(atoi(fields[1]), fields[2]);
            printf("Nome alterado\n");
        }
        else if (strcmp(fields[0], "p") == 0) {
            alteraPrecoArtigo(atoi(fields[1]), atoi(fields[2]));
            printf("Preco alterado \n");
        }
        else if (strlen(buffer) == 2 && buffer[0] == 'c') {
            compactStrings();
            printf("Strings Compactadas\n");
        } 
        else if (strlen(buffer) == 2 && buffer[0] == 'a') {
            callServer(-3,-1);
            printf("Agregador a correr\n");
        }
        else
            *flag = 1;
}

int main()
{

    if(initFileDescriptors() == 0) return 0;
    if(stringFileOverPercentage() == 1) printf("Strings Compactadas\n"); 
    int flag = 0;
    while (flag == 0)
    {
        char buffer[100] = ""; //PARA AGUENTAR OS 84 caracteres que as strings podem ter + o preço
        if(readln(0, buffer,20) > 0) {
            readInput(buffer,&flag);
        } else flag = 1;
        
        
    } 
    close(artigosFile);
    close(stringsFile); 
    return 0;
}