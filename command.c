#include <stdlib.h>
#include <readline/readline.h> 
#include <readline/history.h>

#include "common.h"
#include "command.h"

//take command in input
char* get_command(FILE* history){ 
	char buf[MAX_STR];
	char* cmd;
	location();
	cmd = readline("Command: "); //get current command
	if(!cmd[0]){
		free(cmd);
		return NULL;
	}
	else{
		strcpy(buf , cmd);
		fprintf(history, "%s\n", cmd);
		fclose(history);
		free(cmd);
		return strdup(buf); //return only used memory
	}
}


//get num of token
int get_num_token(char* command){
	char* array_token[MAX_STR];
	int j = 0;
	for(j;j<MAX_STR ; ++j){
		array_token[j] = NULL;
	}
	int i = 0;
	array_token[MAX_STR] = strtok(command , " \t\n"); //get the command and filter with " " "\t" "\n"
	while((array_token[i++] = strtok(NULL , " \t\n")) != NULL);
	return i;
}

//allocate struct
terminal_arg* allocate_term_arg(int length_command){
	terminal_arg * term = malloc(sizeof(terminal_arg));
	term->command = calloc(length_command+1 , sizeof(char*));
	term->length_command = 0;
	term->num_token = 0;
	return term;
}

//destroy allocated struct
void destroy_term_arg(terminal_arg* term){
	free(term->command);
	term->length_command = 0;
	term->num_token = 0;
	free(term);
}

//get token and insert into array of string
char** get_token(char* command , int length_command , int num_token){
	int i = 1 , j = 0;
	char** token = malloc((num_token+1) *sizeof(char*));
	if(token == NULL){
		return NULL;
	}
	for(j ; j < num_token ; j++){
		token[j] = NULL;
	}
	token[0] = strtok(command, " \t\n");
	for(i ; i < num_token ; i++){
		token[i] = strtok(NULL , " \t\n");
	}
	token[num_token] = NULL;
	return token;
}

//count number of & to make n+1 thread
int count_ecom(char** token){
	int i = 0;
	int n_thread = 0;
	while(token[i] != NULL){
		if(strcmp(token[i], "&") == 0){
			n_thread++;
		}
		i++;
	}
	return n_thread;
}

int count_pipe(char** token){
	int i = 0;
	int n_pipe = 0;
	while(token[i] != NULL){
		if(strcmp(token[i] , "|") == 0){
			n_pipe++;
		}
		i++;
	}
	return n_pipe;
}