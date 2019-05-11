#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include <sys/stat.h>


void pai(){

}

void filho(){

}


int main(){

	pid_t pid = getpid();
	printf("PID: %d\n",pid);
	char *path = serverPipe;
	char *fifo = (char *) malloc(strlen(path) * sizeof(char));

	if(fork() != 0){ //código que envia as instruções para o servidor -> PAI
		int nBytes = 0;
		int nArgs; //0 se a instrução tiver apenas 1 argumento, 1 se tiver 2
		char *line = (char *) malloc(50);
		char *token;
		pid_t pid = getpid();
		int fd = open(serverPipe, O_WRONLY);
		Action act = (Action) malloc(sizeof(Action));
		if(fd > 0){
			while(1){
				nBytes = read(0, line, sizeof(line));

				token = strtok(line," ");

				act->pid = pid;
				act->codigo = atoi(token);
				token = strtok(NULL," ");
				if(token != NULL){
					act->quantidade = atoi(token);
				}else{	
					act->quantidade = 0;
				}
				printf("PID: %d, codigo: %d, quantidade :%d \n",act->pid, act->codigo,act->quantidade);
				write(fd, act, sizeof(struct action));
			} 
		}

		close(fd);

	} else{ //código que recebe os resultados das instruções e o escreve para stdout -> FILHO
		char *clientPipe;
		sprintf(clientPipe, "%s%d",PATH, pid);

		int teste = mkfifo(clientPipe, 0666);
		
		Answer a = (Answer) malloc(sizeof(struct answer));

		while(1){
			int fd1 =open(clientPipe, O_RDONLY);
			if(fd1 < 0 )break;
			read(fd1, a, sizeof(struct answer));
			if(a->preco == 0){
				printf("O stock do produto é: %d\n", a->stock);
			}else if(a->preco == -1){
				printf("Não existe artigo referente ao código enviado");
			}else{
				printf("O preço do produto é: %d\n", a->preco);
				printf("O stock do produto é: %d\n",a->stock);
			}
			close(fd1);
		}
		
		_exit(0);
	}
	return 0;
}