#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include <sys/stat.h>

ssize_t readln(int fd, char* buffer, ssize_t buffLength) {
    int size = 0;
	ssize_t res = 0;
	char c ;
    while((res = read(fd,&c,1)) > 0 && size < buffLength) {
		buffer[size] = c;
		if(c == '\n') return size;
        size++;
    }
	buffer[size] = '\0';
    return size;
}

void inthandler(){
	char clientPipe[50] = "";
	pid_t pid = getppid();
	sprintf(clientPipe, "%s%d",PATH, pid);
	remove(clientPipe);
	_exit(0);
	
}

void pai(){
	pid_t pid = getpid();
	int fd = open(serverPipe, O_WRONLY);
	Action act = (Action) malloc(sizeof(Action));
	if(fd > 0){
		printf("Conectado ao servidor\n");
		while(1){
			char line[20] = "";
			if(readln(0,line,20) > 0) {
					printf("STRING:%s\n",line);
			if(strlen(line) > 0) {
				char *token;
				token = strtok(line," ");
				act->pid = pid;
				act->codigo = atoi(token);
				token = strtok(NULL," ");
				if(token != NULL){
					act->quantidade = atoi(token);
				}else{	
					act->quantidade = 0;
				}
				//printf("PID: %d, codigo: %d, quantidade :%d \n",act->pid, act->codigo,act->quantidade);
				write(fd, act, sizeof(struct action));
				}
				
			}
		}
		
			 	
		 
	} else printf("Servidor está desligado\n");

	close(fd);
}

void filho(){
		char clientPipe[50] = "";
		pid_t pid = getppid();
		sprintf(clientPipe, "%s%d",PATH, pid);
		mkfifo(clientPipe, 0666);
		Answer a = (Answer) malloc(sizeof(struct answer));
		while(1){
			int fd1 = open(clientPipe, O_RDONLY);
			if(fd1 < 0 )break;
			ssize_t res = read(fd1, a, sizeof(struct answer));
			if(res > 0) {
				if(a->preco == 0){
					printf("O stock do produto é: %d\n", a->stock);
				}else if(a->preco == -1){
					printf("Não existe artigo referente ao código enviado\n");
				}else{
					printf("O preço do produto é: %d\n", a->preco);
					printf("O stock do produto é: %d\n",a->stock);
				}
			}
			
			close(fd1);
		}
		remove(clientPipe);
		_exit(0);
}


int main(){
	signal(SIGINT, inthandler);
	if(fork() != 0){ //código que envia as instruções para o servidor -> PAI
		pai();
	} else{ //código que recebe os resultados das instruções e o escreve para stdout -> FILHO
		filho();
	}
	return 0;
}