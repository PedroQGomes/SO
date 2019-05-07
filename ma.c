#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "constants.h"


int artigosFile,stringsFile;



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
    tmp.price = _price;
    write(artigosFile, &tmp, sizeof(Artigo));
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
        else
            flag = 1;
    } 
    close(artigosFile);
    close(stringsFile);
    return 0;
}