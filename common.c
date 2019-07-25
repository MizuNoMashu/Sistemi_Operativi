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

