#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "common.h"

int status;
pid_t terminale;

int main(int argc, char const *argv[]){
	
	clean_term();

	while(1){
		printf("Term: ");
		//take command
		temp_command = get_command();
		if(temp_command == NULL){
			handle_error("oi");
		}

		//allocate struct
		terminal_arg* term = allocate_term_arg(strlen(temp_command));

		//copy string to avoid problem with pointer
		strncpy(term->command , temp_command , sizeof(char)*strlen(temp_command));
		
		term->length_command = strlen(term->command);
		term->num_token = get_num_token(temp_command);

		char** token = get_token(term->command , term->length_command , term->num_token);
		while(!token[0]){
			free(temp_command);	
			destroy_term_arg(term);
			free(token);
			temp_command = get_command();
			term = allocate_term_arg(strlen(temp_command));
			strncpy(term->command , temp_command , sizeof(char)*strlen(temp_command));
			term->length_command = strlen(term->command);
			term->num_token = get_num_token(temp_command);
			token = get_token(term->command , term->length_command , term->num_token);
		}

		terminale = fork();

		if(terminale == -1 ){
			free(temp_command);	
			destroy_term_arg(term);
			free(token);
			handle_error("Errore nella vfork");
		}
		else if(terminale == 0){
			if(execvp(token[0] , token) == -1){
				free(temp_command);	
				destroy_term_arg(term);
				free(token);
				handle_error("Error");	
			}
			_exit(EXIT_SUCCESS);
		}
		else{
			int terminale_padre = wait(&status);
			if(terminale_padre == -1){
				free(temp_command);	
				destroy_term_arg(term);
				free(token);
			}
		}
		free(temp_command);	
		destroy_term_arg(term);
		free(token);
	}
}