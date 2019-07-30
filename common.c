#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h> 
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