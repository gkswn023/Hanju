#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

#define MAX_LEN 987654321

int exit_shell(char* sentence);
int parsing(char* sentence, char** token, char* word);
void execute(char** token, int num);

int main(int argc, char* argv[]){
	FILE* fp;
	char* token[256];
	char buf[256];
	int i;

	if(argc == 1){
		printf("prompt> ");
		while(fgets(buf, MAX_LEN, stdin) != NULL){
			i = parsing(buf, token, ";");
			execute(token, i);
			printf("prompt> ");
		}
	}
	else{
		fp = fopen(argv[1], "r");
		if(fp == NULL){
			perror("FILE CAN'T FIND\n");
			exit(1);
		}
		while(fgets(buf, MAX_LEN, fp) != NULL){
			i = parsing(buf, token, ";");
			execute(token, i);
		}
	}

	return 0;
}

int exit_shell(char* sentence){
	if(!strcmp(sentence, "quit")){
		return 1;
	}
	else return 0;
}

int parsing(char* sentence, char** token, char* word){
	int i = 0;

	sentence = strtok(sentence, word);

	while(sentence != NULL){
		token[i] = sentence;
		i++;
		sentence = strtok(NULL, word);
	}
	token[i] = '\0';

	return i;
}

void execute(char** token, int num){
	int i, j, k = 0, pid, status;
	char* result[256];

	if(num == 0) return;
	else{
		for(i = 0; i < num; i++){
			j = parsing(token[i], result, " \n");

			if(j == 0) continue;
			if(exit_shell(result[0])) exit(0);
			k++;
			pid = fork();
			if(pid == 0){
				execvp(result[0],result);
				printf("execvp error\n");
				exit(1);
			}
			else if(pid < 0){
				perror("FORK FAILED!\n");
				exit(1);
			}
		}
		for(i = 0; i < k; i++){
			wait(&status);
		}
	}
}