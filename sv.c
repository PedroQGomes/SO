#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

PCache arr[CACHE_SIZE];


int getPriceCache(int codigo,int *preco){ // retorna -1 se o codigo nao está na cache
    int x = (-1);
    for(int i = 0; i < CACHE_SIZE;i++){
        if(((arr[i])->ID) == codigo){
            (arr[i])->acessos += 1;
            *preco = ((arr[i])->price);
            x = 0;
            break;
        }
    }
    return x;
}


int getPriceFile(int codigo,int *preco){ // retorna -1 se o artigo nao existe, 0 se tudo bem
    int fd1,tmp = (-1);
    fd1 = open(PATHARTIGOS,O_RDONLY);
    if(fd1 <= 0 ) perror("ERRO AO ABRIR ARTIGOS");
    Artigo *atg = malloc(sizeof(Artigo));
    lseek(fd1,codigo * sizeof(Artigo),SEEK_SET);
    read(fd1,atg,sizeof(Artigo));
    if((atg->ID) == codigo){
        *preco = atg->price;
        tmp = 0;
    }
    return tmp;
}// done

// incrementa o nuemro de acessos de uma artigo
int atualizaFileCache(int codigo,int *preco){
    int fd1;
    fd1 = open(PATHFCACHE,O_RDWR);
    File c;
    lseek(fd1,codigo *(sizeof (struct file)),SEEK_SET);
    read(fd1,&c,sizeof(struct file));
    if(c.codigo == codigo){
        c.acessos += 1; 
    }
    else{
        c.codigo = codigo;
        c.acessos = 1;
    }
    lseek(fd1,codigo *(sizeof (struct file)),SEEK_SET);
    write(fd1,&c,sizeof(struct file));
    return (c.acessos);
}
int isEmptyCache(){ // retorna -1 se estiver td cheia, se nao retorna um indice que esteja vaizio
    int res = (-1);
    for(int i = 0; i < CACHE_SIZE;i++){
        if((arr[i]->ID) < 0){
            res = i;
            break;
        }

    }
    return res;
}

void lookUpCache(int codigo,int *preco,int ac){
    int tmpacessos,tmpIndice = 0; 
    int x = isEmptyCache();
    if(x < 0){ // a cache está cheia -> verificar se tem mais iterações do ques os existentes para mudar
        for(int i = 1; i < CACHE_SIZE;i++){ // serve para ir buscar o indice com menor acessos e o numero menor de acessos
            tmpacessos = (arr[0])->acessos;
            if((arr[i]->acessos) < tmpacessos){
                tmpacessos = (arr[i])->acessos;
                tmpIndice = i;
            }
        }

        if(ac > tmpacessos){
            (arr[tmpIndice])->ID = codigo;
            (arr[tmpIndice])->price = *preco;
            (arr[tmpIndice])->acessos = ac;
        }

    }else{ // tem lugar na cache
        arr[x]->ID = codigo;
        arr[x]->price = *preco;
        arr[x]->acessos = ac;
    }

}


void manageArtigo(int codigo,int *preco){
    int acessos;
    int x = getPriceCache(codigo,preco);
    if(x < 0) { // nao existe em cache 
        x = getPriceFile(codigo,preco);
        if(x < 0){ // o artigo nao existe -> mensagem de erro
            *preco = (-1);
        }
        else{
            acessos = atualizaFileCache(codigo,preco);
            lookUpCache(codigo,preco,acessos);
        }
    }
    
}


void getStock(int codigo,int *stock){ //POSSIVEL RETORNO DA ESTRUTURA STOCK
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDONLY);
    lseek(fd1,codigo*sizeof(Stocks),SEEK_SET);
    if (read(fd1,&stk,sizeof(Stocks)) > 0 && stk.qnt > 0) {
        *stock = stk.qnt;
    } else *stock = 0;
}  // done 



int atualizaStock(int codigo, int quantidade) { // retorna o stock resultante
    int fd1;
    Stocks stk;
    fd1 = open(PATHSTOCKS,O_RDWR);
    if(fd1 <= 0) perror("A Atualizar o stock");
    lseek(fd1,sizeof(Stocks)*codigo,SEEK_SET);
    if(read(fd1,&stk,sizeof(Stocks)) > 0) {
        stk.qnt += quantidade;
    } 

    if(stk.qnt < 0 ) stk.qnt = 0;
    lseek(fd1,sizeof(Stocks)*codigo,SEEK_SET);
    write(fd1,&stk,sizeof(stk));    
    close(fd1);
    return stk.qnt;
}

void atualizaVenda(int codigo,int quantidade){ // done mas por testar
    if(quantidade == 0) return;
    int preco,fd1;
    manageArtigo(codigo,&preco);
    Sale venda;
    venda.price = abs(quantidade) * preco;
    venda.ID = codigo;
    venda.qnt = abs(quantidade);
    fd1 = open(PATHVENDAS,O_WRONLY);
    lseek(fd1,0,SEEK_END);
    write(fd1,&venda,sizeof(Sale));
    close(fd1);
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

void lookStock(char* pid,int cod,Answer ans){ // retornar o stock atualizado e o preço do artigo
    int tmpStock = 0,tmpPrice = 0;
    getStock(cod,&tmpStock);
    manageArtigo(cod,&tmpPrice);
    ans->stock = tmpStock;
    printf("Codigo: %d , STOCK TMP:%d\n",cod,tmpStock);
    ans->preco = tmpPrice;
    answerBack(pid,ans);
}

void entryStock(char* pid,int cod, int qnt,Answer ans){ // o preço retornado-> (-1) se nao existe o artigo ou 0 se existe
    int finalStock,finalPrice; 
    finalStock = atualizaStock(cod,qnt);
    manageArtigo(cod,&finalPrice);
    if(finalPrice > 0){finalPrice = 0;}
    ans->preco = finalPrice;
    ans->stock = finalStock;
    answerBack(pid,ans);
}

void entrySale(char* pid,int cod,int qnt,Answer ans){ // o preço retornado ->(-1) se nao existe o artigo ou 0 se existe
    int tmpStock = 0, res = 0,tmpPrice = 0;
    getStock(cod,&tmpStock);
    res = tmpStock - abs(qnt);
    if((res) >= 0) {
        atualizaVenda(cod,qnt);
    } else {
        atualizaVenda(cod,tmpStock);
    }
    tmpStock = atualizaStock(cod,qnt);
    manageArtigo(cod,&tmpPrice);
    if(tmpPrice > 0){tmpPrice = 0;}
    ans->preco = tmpPrice;
    ans->stock = tmpStock;
    answerBack(pid,ans); 
}

void initCache(){
    PCache c;
    int i = 0;
    while(i < CACHE_SIZE){
        c->ID = (-2);
        c->price = (-2);
        c->acessos = 0;
        arr[i] = c;
        i++;
    }
}

void priceUpdCache(int cod,int qnt){
    for(int i=0; i< CACHE_SIZE;i++){
        if((arr[i])->ID == cod){
            (arr[i])->price = qnt;
            break;
        }
    }
}


void sv(){
    int server,lerdados,fd1,cod,qnt,status;
    Action dados = (Action) malloc(sizeof(struct action));
    char pid[10];
    pid_t res;

    server = mkfifo(serverPipe,0666);
    if(server < 0){perror("FIFO");} else printf("Server is open\n");
    
    initCache();
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
                    priceUpdCache(cod,qnt);
                }
                else if(qnt == 0){ // consulta
                    lookStock(pid,cod,ans); 
                }else if(qnt > 0){ // acrescentar ao stock
                    entryStock(pid,cod,qnt,ans);
                }else if( qnt < 0){ // venda
                    entrySale(pid,cod,qnt,ans); // retornar o preço -1 se nao existir
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
    //atualizaStock(1234,3);
/*    atualizaStock(1235,1);
    atualizaStock(1234,7);
    atualizaStock(1235,(-3));

    getStock(1234,&x);
    printf("stock de 1234 %d\n",x);
    getStock(1235,&x);
    printf("stock de 1235 %d\n",x); */
    sv();
    return 0;
}

