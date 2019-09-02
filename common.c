#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>

#include "common.h"




//clear the terminal
void clean_term(){
	printf("\033c");
}


void location(){
	char* cwd = malloc((300) * sizeof(char));
	if(getcwd(cwd , 300) == NULL){
		handle_error("Error getcwd:");
	}
	printf("%sLocation%s:%s~%s%s\n" , "\x1B[32m", "\x1B[0m" , "\x1B[34m", cwd , "\x1B[0m");
	free(cwd);
}

void exiting(){
	if(exit_count <= 1){
		kill(getppid() , SIGKILL);
	}
	else{
		exit_count--;
		printf("\nPress more %d time to exit.", exit_count );
	}
}

void controll_keyboard(){
	if(rl_bind_key(27 , (rl_command_func_t*) exiting) != 0){
		handle_error("Error in ESC");
	}
	if(rl_bind_keyseq("\\e[A" , (rl_command_func_t*) keyup));
	if(rl_bind_keyseq("\\e[B" , (rl_command_func_t*) keydown));
}

void keyup(){
	rl_delete_text(0 , 1000);
	rl_point = 0;
	if(state == -10){
		rl_ding();
		return;
	}
	if(fseek(history , 0, SEEK_END) == -1){
		handle_error("Error in fseek:");
	}
	int i = 0;
	up_or_down++;
	while((up_or_down - state) != -1){
		while(fgetc(history) != '\n'){
			if(fseek(history , i-- , SEEK_END) == -1){
				state = -10;
				return;
			}
		}
		state++;
	}
	state = 0;
	if(fseek(history , i+2 , SEEK_END) == -1){
		handle_error("Error in fseek:");
	}
	char s[1000];
	if(fgets(s , 1000 , history) == NULL){
		handle_error("Error in fgets");
	}
	rl_insert_text(strtok(s,"\n"));
}

void keydown(){
	rl_delete_text(0 , 1000);
	rl_point = 0;
	fseek(history , 0, SEEK_END);
	int i = 0;
	state = 0;
	if(up_or_down == 0){
		rl_ding();
		return;
	}
	if(up_or_down == 1){
		up_or_down--;
		return;
	}
	up_or_down--;
	while((up_or_down - state) != -1){
		while(fgetc(history) != '\n'){
			fseek(history , i-- , SEEK_END);
		}
		state++;
	}
	state = 0;
	if(fseek(history , i+2 , SEEK_END) == -1){
		handle_error("Error fseek:");
	}
	char s[1000];
	if(fgets(s , 1000 , history) == NULL){
		handle_error("Error in fgets");
	}
	rl_insert_text(strtok(s,"\n"));
}