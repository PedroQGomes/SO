#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"


Artigo* getArtigo(int codigo,int *preco){
    int fd1;
    fd1 = open(PATHARTIGOS,O_RDONLY);
    if(fd1 <= 0 ) perror("ERRO AO ABRIR ARTIGOS");
    Artigo *atg = malloc(sizeof(Artigo));
    lseek(fd1,codigo * sizeof(Artigo),SEEK_SET);
    if(read(fd1,atg,sizeof(Artigo)) > 0) {
        *preco = atg->price;
        return atg;
    }   
    return NULL;
}// done

void getStock2(int codigo,int *stock){ //POSSIVEL RETORNO DA ESTRUTURA STOCK
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDONLY);
    lseek(fd1,codigo*sizeof(Stocks),SEEK_CUR);
    if (read(fd1,&stk,sizeof(Stocks)) > 0 ) {
        *stock = stk.qnt;
    } else *stock = 0;
}  // done 

void getStock(int codigo,int *stock){
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDONLY);
    while (read(fd1,&stk,sizeof(Stocks))){
        //printf("estou com o codigo %d\n",stk.numCod);
        if(stk.numCod == codigo){
            //printf("encontrei o codigo\n");
            *stock = stk.qnt;
            break;
        }
    }
}  // done 

int atualizaStock(int codigo,int quantidade){ // nao está a atualizar como devia, so está a dar add
    int fd1,flag = 0;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDWR);
    lseek(fd1,0,SEEK_SET);
    while(read(fd1,&stk,sizeof(Stocks))){
        if((stk.numCod) == codigo){
            lseek(fd1,-((sizeof(stk))),SEEK_CUR);
            stk.qnt = (stk.qnt) + quantidade;
            if((stk.qnt) < 0){stk.qnt = 0;}
            write(fd1,&stk,sizeof(stk));
            return (stk.qnt);
        }
    }
    lseek(fd1,0,SEEK_END);
    stk.numCod = codigo;
    stk.qnt = quantidade;
    write(fd1,&stk,sizeof(stk));
    
}


int atualizaStock2(int codigo, int quantidade) { // retorna o stock resultante
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDWR);
    if(fd1 <= 0) perror("A Atualizar o stock");
    lseek(fd1,sizeof(Stocks)*codigo,SEEK_SET);
    if(read(fd1,&stk,sizeof(Stocks)) > 0) {
        stk.qnt += quantidade;
    } else {
        stk.numCod = codigo;
        stk.qnt = quantidade;
        write(fd1,&stk,sizeof(stk));    
    }
    close(fd1);
    return stk.qnt;
}

void atualizaVenda(int codigo,int quantidade){ // done mas por testar
    int preco,fd1;
    getArtigo(codigo,&preco);
    Sale venda;
    venda.price = quantidade * preco;
    venda.ID = codigo;
    venda.qnt = quantidade;
    fd1 = open(PATHVENDAS,O_WRONLY);
    lseek(fd1,0,SEEK_END);
    write(fd1,&venda,sizeof(Sale));
}

void answerBack(char* pid,Answer ans){
    int fd2;
    char buffer[100];
    sprintf(buffer,"%s%s",PATH,pid);
    fd2 = open(buffer,O_WRONLY);
    printf("PID A QUEM ENVIA %s\n",pid);
    ssize_t res = write(fd2,ans,sizeof(struct answer));
    printf("TAMANHO A ENVIAR: %zd\n",res);
    if(res == -1) perror("MENSAGEM");
    close(fd2);
}

void lookStock(char* pid,int cod,Answer ans){ // qnd o cliente pede uma consulta de stock
    int tmpStock,tmpPrice;
    getStock(cod,&tmpStock);
    getArtigo(cod,&tmpPrice);
    ans->stock = tmpStock;
    printf("STOCK TMP:%d\n",tmpStock);
    ans->preco = tmpPrice;
    answerBack(pid,ans);
}

void entryStock(char* pid,int cod, int qnt,Answer ans){
    int finalStock; 
    finalStock = atualizaStock(cod,qnt);
    ans->preco = 0;
    ans->stock = finalStock;
    answerBack(pid,ans);
}

void entrySale(char* pid,int cod,int qnt,Answer ans){
    int tmpStock;
    atualizaVenda(cod,qnt);
    tmpStock = atualizaStock(cod,qnt);
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
    if(server < 0){perror("FIFO");} else printf("Server is open\n");
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
    atualizaStock(1234,3);
    atualizaStock(1235,1);
    atualizaStock(1234,7);
    atualizaStock(1235,(-3));

    getStock(1234,&x);
    printf("stock de 1234 %d\n",x);
    getStock(1235,&x);
    printf("stock de 1235 %d\n",x);
    //sv();
    return 0;
}

