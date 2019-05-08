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
    fd1 = open(PATHARTIGOS,O_RDONLY);
    Artigo atg;
    while(read(fd1,&atg,sizeof(atg))){
        if(atg.ID == codigo){
            *preco = atg.price;
            break;
        }
    }
}// done
void getStock(int codigo,int *stock){
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDONLY);
    while (read(fd1,&stk,sizeof(stk))){
        if(stk.numCod == codigo){
            *stock = stk.qnt;
            break;
        }
    }
}  // done

void atualizaStock(int codigo,int quantidade){ // nao está a atualizar como devia, so está a dar add
    int fd1,flag = 0;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDWR);
    lseek(fd1,0,SEEK_SET);
    while(read(fd1,&stk,sizeof(Stocks))){
        if((stk.numCod) == codigo){
            //lseek(fd1,-(2*(sizeof(stk))),SEEK_CUR);
            stk.qnt = quantidade;
            write(fd1,&stk,sizeof(stk));
            flag = 1;
        }
    }
    if(flag == 0){
        lseek(fd1,0,SEEK_END);
        stk.numCod = codigo;
        stk.qnt = quantidade;
        write(fd1,&stk,sizeof(stk));
    }
}

void atualizaVenda(int codigo,int quantidade){ // done mas por testar
    int preco,fd1;
    getPreco(codigo,&preco);
    Sale venda;
    venda.price = quantidade * preco;
    venda.ID = codigo;
    venda.qnt = quantidade;
    fd1 = open(PATHVENDAS,O_WRONLY);
    lseek(fd1,0,SEEK_END);
    write(fd1,&venda,sizeof(venda));
}

void answerBack(char* pid,Answer ans){
    int fd2;
    mkfifo(pid,0666);
    fd2 = open(pid,O_WRONLY);
    write(fd2,ans,sizeof(struct answer));
    close(fd2);
}

void lookStock(char* pid,int cod,Answer ans){ // qnd o cliente pede uma consulta de stock
    int tmpStock,tmpPrice;
    getStock(cod,&tmpStock);
    getPreco(cod,&tmpPrice);
    ans->stock = tmpStock;
    ans->preco = tmpPrice;
    answerBack(pid,ans);
}

void entryStock(char* pid,int cod, int qnt,Answer ans){
    int tmpStock;
    atualizaStock(cod,qnt);
    getStock(cod,&tmpStock);
    ans->preco = 0;
    ans->stock = tmpStock;
    answerBack(pid,ans);
}

void entrySale(char* pid,int cod,int qnt,Answer ans){
    int tmpStock;
    atualizaVenda(cod,qnt);
    atualizaStock(cod,qnt);
    getStock(cod,&tmpStock);
    ans->preco = 0;
    ans->stock = tmpStock;
    answerBack(pid,ans); 
}

void updateCache(int codigo,int quantidade){

}

void sv(){
    int server,lerdados,fd1,cod,qnt,status;
    Action dados = (Action) malloc(sizeof(struct action));
    char pid[10];
    Artigo cache[10];
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

            if((res = fork()) == 0){ 
                Answer ans = (Answer) malloc(sizeof(struct answer));
                if((dados->pid) < 0){
                    updateCache(cod,qnt);
                }
                else if(qnt == 0){ // consulta
                    lookStock(pid,qnt,ans);
                }else if(qnt > 0){ // acrescentar ao stock
                    entryStock(pid,cod,qnt,ans);
                }else if( qnt < 0){ // venda
                    entrySale(pid,cod,qnt,ans);
                }
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
    int x= 0;
    atualizaStock(1235,1);
    //atualizaStock(1235,1);
    //atualizaStock(1236,3);
    //atualizaStock(1235,2);
    //getStock(1235,&x);
    //printf("resultado %d\n",x);
    //sv();
    return 0;
}

