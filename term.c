#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"
#include "command.h"
#include "execution.h"
int status;
pid_t terminale;

int main(int argc, char const *argv[]){
	
	clean_term();
	while(1){
		char cwd[300];
		getcwd(cwd , sizeof(cwd));
		printf("%sLocation%s:%s~%s%s" , "\x1B[32m", "\x1B[0m" , "\x1B[34m", cwd , "\x1B[0m");
		//take command
		temp_command = get_command();
		if(temp_command == NULL){
			handle_error("Error:");
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
			printf("%sLocation%s:%s~%s%s" , "\x1B[32m", "\x1B[0m" , "\x1B[34m", cwd , "\x1B[0m");
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
			handle_error("Errore nella fork");
		}
		else if(terminale == 0){

			pid_t child = getpid();
			int n_thread = 0;
			n_thread = count_ecom(token);
			
			if(n_thread != 0){
				do_custom_execvp_ecom(token , child , n_thread);
			}
			else{
				custom_execvp(token , child);
			}
			
			free(temp_command);	
			destroy_term_arg(term);
			free(token);
			exit(EXIT_FAILURE); 
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