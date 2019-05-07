#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include <sys/stat.h>

int main(int arc, const char* argv[]){

	pid_t pid = getpid();
	char *path = serverPipe;
	char *fifo = (char *) malloc(strlen(path) * sizeof(char));

	if(fork() == 0){ //código que envia as instruções para o servidor
		int nBytes = 0;
		int nArgs; //0 se a instrução tiver apenas 1 argumento, 1 se tiver 2
		char *line = (char *) malloc(50);
		char *token;
		pid_t pid = getpid();
		while(1){
			nBytes = read(0, line, sizeof(line));
			nArgs = 0;
			for(int i = 0; i < strlen(line); i++){
				if(line[i] == ' ')nArgs = 1;
			}
			token = strtok(line," ");

			Action act = (Action) malloc(sizeof(action));

			act->pid = pid;
			act->codigo = atoi(token);
			token = strtok(NULL," ");
			if(token != NULL){
				act->quantidade = atoi(token);
			}else{	
				act->quantidade = 0;
			}
			
			int fd = open(serverPipe, O_WRONLY);
			write(fd, &action,sizeof(action));
		}



	}else{ //código que recebe os resultados das instruções e o escreve para stdout

		char *clientPipe;
		sprintf(clientPipe, "/temp/%d", pid);

		mkfifo(clientPipe, 0666);
		int fd1 =open(clientPipe, O_RDONLY);

		Answer a = (Answer) malloc(sizeof(answer));

		while(1){
			read(fd1, &a, sizeof(answer));
			if(a->preco == 0){
				printf("O novo stock do produto é: %d", a->stock);
			}else{
				printf("O preço do produto é: %d", a->preco);
				printf("O stock do produto é: %d",a->stock);
			}
		}


	}

}