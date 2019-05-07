#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

void getPreco(int codigo,int *preco){
    int fd1;
    char buf[10];
    fd1 = open(PATHARTIGOS,O_RDONLY);
    off_t artOffset = (TAM_ARTIGO_STR * (codigo));
    artOffset += 11; // TAMANHO DA STRING + ESPAÇO
    lseek(fd1, artOffset, SEEK_SET);
    read(fd1,buf,sizeof(buf));
    *preco = atoi(buf);
}// done
void getStock(int codigo,int *stock){
    int fd1,i,tmpCod;
    fd1 = open(PATHSTOCKS,O_RDONLY);
    char buf[34];
    char *tmp;
    char *arr[2];
        while(read(fd1,buf,sizeof(buf))){
            tmp = strtok(buf," ");
            for(i = 0;tmp;i++){
                arr[i] = strdup(tmp);
                tmp = strtok(NULL, " ");
            }
            tmpCod = atoi(arr[0]);
            if( tmpCod == codigo){
                *stock = atoi(arr[1]);
            }
        } 
}  // done

void atualizaStock(int codigo,int quantidade){ // tenho de procurar e se nao ouver add
    int fd1,lidos,tmpCod;
    int flag = 0;
    char add[TAM_ARTIGO_STR_WITH_NEWLINE];
    char string[TAM_ARTIGO_STR_WITH_NEWLINE];
    char tmpQnt[12];
    char* token;
    char* fildes[2];
    fd1 = open(PATHSTOCKS,O_RDWR);
    while(read(fd1,string,strlen(string))){
        token = strtok(string," ");
        for(int i = 0;token;i++){
            fildes[i] = strdup(token);
            token = strtok(NULL, " ");
        }
        tmpCod = atoi(fildes[0]);
        if(tmpCod == codigo){
            //lseek();
            sprintf(tmpQnt, "%010d\n",quantidade);
            write(fd1,tmpQnt,strlen(tmpQnt));
            flag = 1;
        }
    }
    if(flag == 0){ 
        lseek(fd1,0,SEEK_END);
        sprintf(add, "%010d %010d\n",codigo, quantidade);
        write(fd1,add,strlen(add));
    }
}

void atualizaVenda(int codigo,int quantidade){ // done mas por testar
    int preco,fd1,montante;
    char string[34];
    getPreco(codigo,&preco);
    montante = quantidade * preco;
    fd1 = open(PATHVENDAS,O_WRONLY);
    lseek(fd1,0,SEEK_END);
    sprintf(string, "%010d %010d %010d\n",codigo,quantidade,montante);
    write(fd1,string,strlen(string));
}

void lookStock(int cod,Answer ans){ // qnd o cliente pede uma consulta de stock
    int tmpStock,tmpPrice;
    getStock(cod,&tmpStock);
    getPreco(cod,&tmpPrice);
    ans->stock = tmpStock;
    ans->preco = tmpPrice;
}

void entryStock(int cod, int qnt,Answer ans){
    int tmpStock;
    atualizaStock(cod,qnt);
    getStock(cod,&tmpStock);
    ans->preco = 0;
    ans->stock = tmpStock; 
}

void entrySale(int cod,int qnt,Answer ans){
    int tmpStock;
    atualizaVenda(cod,qnt);
    atualizaStock(cod,qnt);
    getStock(cod,&tmpStock);
    ans->preco = 0;
    ans->stock = tmpStock; 
}


void sv(){
    int server,lerdados,fd1,fd2,cod,qnt,status;
    Action dados = (Action) malloc(sizeof(struct action));
    char pid[10];
    pid_t res;

    server = mkfifo(serverPipe,0666);
    //if(server < 0){printf("Erro a criar o fifo do servidor\n");}
    while (1){
        // abrir o fifo
        fd1 = open(serverPipe,O_RDONLY);
        //if(fd1 < 0){printf("Erro a abrir o fifo do servidor\n");}
        lerdados = read(fd1,dados,sizeof(struct action));
        if(lerdados > 0){
            sprintf(pid,"%d",dados->pid);
            cod = dados->codigo;
            qnt = dados->quantidade;

            if((res = fork()) == 0){ // falta ver a cena do zé
                Answer ans = (Answer) malloc(sizeof(struct answer));
                if(qnt == 0){ // consulta
                    lookStock(qnt,ans);
                }else if(qnt > 0){ // acrescentar ao stock
                    entryStock(cod,qnt,ans);
                }else if( qnt < 0){ // venda
                    entrySale(cod,qnt,ans);
                }

                mkfifo(pid,0666);
                fd2 = open(pid,O_WRONLY);
                write(fd2,ans,sizeof(struct answer));
                close(fd2);
                _exit(0);
            }else{ 
                waitpid(res,&status,WNOHANG);
                //WIFEXITED(status);
            }

        }
        close(fd1);
    }
}


int main(){
    //sv();
    return 0;
}

