#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"
#include "command.h"
#include "execution.h"
#include "signal.h"

int status;
pid_t terminale;
struct sigaction sig;


int main(int argc, char const *argv[]){
	
	clean_term();
	
	char** token;

	while(1){

		sig.sa_handler = &handle_signal;
		sig.sa_flags = SA_RESTART;

		//take command
		temp_command = get_command();
		if(temp_command == NULL){
			free(temp_command);
			handle_error("Error:");
		} 

		//allocate struct
		terminal_arg* term = allocate_term_arg(strlen(temp_command));

		//copy string to avoid problem with pointer
		strncpy(term->command , temp_command , sizeof(char)*strlen(temp_command));
		
		term->length_command = strlen(term->command);
		term->num_token = get_num_token(temp_command);

		token = get_token(term->command , term->length_command , term->num_token);
		
		if(!token[0]){
			free(temp_command);
			destroy_term_arg(term);
			free(token);
			continue;
		}
		
		free(temp_command);

		terminale = fork();

		if(terminale == -1 ){
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
			destroy_term_arg(term);
			free(token);
			_exit(EXIT_FAILURE); 
		}
		else{

			if(sigaction( SIGINT , &sig , NULL) == -1){
				printf("Error SIGINT\n");
			}

			if(sigaction( SIGCHLD , &sig , NULL) == -1){
				printf("Error SIGCHLD\n");
			}

			int terminale_padre = wait(&status);
			// if(terminale_padre == -1){
			// }
			kill(terminale , SIGKILL);
		}

		destroy_term_arg(term);
		free(token);
	}
}