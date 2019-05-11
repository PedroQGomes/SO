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




void compactStrings() {
    printf("COMPACTANDO\n");
    int newStringsFile = open("tmpStrings",O_CREAT | O_WRONLY | O_APPEND,0666);
    Artigo tmp = {0,0,0};
    lseek(artigosFile,0,SEEK_SET);
    while(read(artigosFile,&tmp,sizeof(Artigo)) > 0) {
        off_t _stringRef = lseek(newStringsFile,0,SEEK_CUR);
        lseek(stringsFile,tmp.stringRef,SEEK_SET);
        char buffer[100];
        readln(stringsFile,buffer);
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

void callServer(int flagpipe,int ID) {
    int fifo = open(serverPipe,O_WRONLY);
    Action act = {0};
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

// FALTA COMUNICAÇAO COM O SERVER

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


// FALTA COMUNICAÇAO COM O SERVER

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
    artigosFile = open(PATHARTIGOS, O_RDWR);
    stringsFile = open(PATHTSTRINGS, O_RDWR);
    if (artigosFile <= 0 || stringsFile <= 0)
    {
        perror("File Not Found");
        return 0;
    }
    return 1;
}

int main()
{

    if(initFileDescriptors() == 0) return 0; 
    int flag = 0;
    while (flag == 0)
    {
        char buffer[100] = ""; //PARA AGUENTAR OS 84 caracteres que as strings podem ter + o preço
        ssize_t readSize = read(0, buffer, 100);
        if(readSize <= 0) break;
        char *token = strtok(buffer, " ");
        char *fields[3];
        for (int i = 0; token; i++)
        {
            fields[i] = strdup(token);
            token = strtok(NULL, " ");
        }
        if (strcmp(fields[0], "i") == 0)
            insereArtigo(fields[1], atoi(fields[2]));
        else if (strcmp(fields[0], "n") == 0)
            alteraNomeArtigo(atoi(fields[1]), fields[2]);
        else if (strcmp(fields[0], "p") == 0)
            alteraPrecoArtigo(atoi(fields[1]), atoi(fields[2]));
        else if (strlen(buffer) == 1 && buffer[0] == 'c')
            compactStrings();
        else if (strlen(buffer) == 1 && buffer[0] == 'a')
            callServer(-3,-1);
        else
            flag = 1;
    } 
    close(artigosFile);
    close(stringsFile); 
    return 0;
}