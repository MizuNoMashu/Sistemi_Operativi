#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#include "common.h"




//clear the terminal
void clean_term(){
	printf("\033c");
}


void location(){
	char cwd[300];
	getcwd(cwd , sizeof(cwd));
	printf("%sLocation%s:%s~%s%s\n" , "\x1B[32m", "\x1B[0m" , "\x1B[34m", cwd , "\x1B[0m");
}

void exiting(){
	if(exit_count <= 1){
		kill(getpid() , SIGKILL);
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
	if(rl_bind_keyseq("\\e[A" , (rl_command_func_t*) keyup ));
}

void keyup(){
	printf("ciao\n");
}

void keydown(){

}