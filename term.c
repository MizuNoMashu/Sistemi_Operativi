#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>

#include "common.h"
#include "command.h"
#include "execution.h"
#include "signal.h"

int status;
pid_t terminal;
struct sigaction sig;
struct sigaction sig_less;

int main(int argc, char const *argv[]){
	char true_path[300];
	getcwd(true_path , 300);
	
	char* path_history = malloc(1 + strlen(true_path) + strlen("/.history.txt"));
	strcpy(path_history , true_path);
	strcat(path_history ,"/.history.txt");
	history = fopen(path_history , "w+");
	if(history == NULL){
		handle_error("Error in fopen:");
	}
	fprintf(history, "\n");
	if(fclose(history) != 0){
		handle_error("Error fclose:");
	}
	clean_term();
	
	char** token;
	
	while(1){
		history = fopen(path_history , "a+");
		if(history == NULL){
			handle_error("Error fopen:");
		}
		// free(path);

		exit_count = 3;
		up_or_down = 0;
		state = 0;

		sig.sa_handler = &handle_signal;
		sig.sa_flags = SA_RESTART;
		sig_less.sa_handler = &handle_signal_less;

		if(sigaction( SIGINT , &sig_less , NULL) == -1){
			handle_error("Error in SIGINT");
		}
		// signal(SIGTSTP , SIG_IGN);
		if(sigaction( SIGTSTP , &sig_less ,NULL) == -1){
			handle_error("Error in SIGTSTP");
		}
		if(sigaction( SIGQUIT , &sig_less , NULL) == -1){
			handle_error("Error in SIGQUIT");
		}
		if(sigaction( SIGWINCH , &sig_less , NULL) == -1){
			handle_error("Error in SIGWINCH");
		}


		control_keyboard();

		//take command
		temp_command = get_command(history);
		if(temp_command == NULL){
			free(temp_command);
			continue;
		} 

		//allocate struct
		terminal_arg* term = allocate_term_arg(strlen(temp_command));

		//copy string to avoid problem with pointer
		strncpy(term->command , temp_command , sizeof(char)*strlen(temp_command));
		
		term->length_command = strlen(term->command);
		term->num_token = get_num_token(temp_command);	

		token = get_token(term->command , term->length_command , term->num_token , true_path);
		
		if(strcmp(token[0] , "cd") == 0){
			if(!token[1]){
				char* path_cd = malloc(1 + strlen(getenv("HOME")));
				strcpy(path_cd , getenv("HOME"));
				if(chdir(path_cd) == -1){
					free(path_cd);
					handle_error("Error chdir:");
				}
				free(path_cd);
			}
			else{
				if(chdir(token[1]) == -1){
					printf("%s not exist!!\n", token[1]);
					free(temp_command);
					destroy_term_arg(term);
					free(token);
					continue;
				}
			}
			if(token[2]){
				printf("Per ora posso solo interpretare cd senza altri comandi vicino\n");
			}
			free(temp_command);
			destroy_term_arg(term);
			free(token);
			continue;
		}

		if(!token[0]){
			free(temp_command);
			destroy_term_arg(term);
			free(token);
			continue;
		}
		
		free(temp_command);

		terminal = fork();

		if(terminal == -1 ){
			destroy_term_arg(term);
			free(token);
			handle_error("Errore nella fork");
		}
		else if(terminal == 0){
			pid_t child = getpid();
			printf("%d\n", getpid());
			int n_thread = 0;
			n_thread = count_ecom(token);
			
			int n_pipe = 0;
			n_pipe = count_pipe(token);

			if(n_thread != 0){
				do_custom_execvp_ecom(token , child , n_thread , n_pipe , 0);
			}
			else if(n_pipe != 0){
				do_custom_execvp_pipe(token , n_pipe , STDIN_FILENO , 0 , child);
			}
			else{
				custom_execvp(token , child , n_pipe , n_thread , 0);
			}
			destroy_term_arg(term);
			free(token);
			_exit(EXIT_FAILURE); 
		}
		else{

			if(sigaction( SIGINT , &sig , NULL) == -1){
				handle_error("Error SIGINT\n");
			}
			if(sigaction( SIGCHLD , &sig , NULL) == -1){
				handle_error("Error SIGCHLD\n");
			}
			if(sigaction( SIGQUIT , &sig , NULL) == -1){
				handle_error("Error in SIGQUIT");
			}
			if(sigaction( SIGWINCH , &sig , NULL) == -1){
				handle_error("Error in SIGWINCH");
			}
			if(sigaction( SIGTSTP , &sig ,NULL) == -1){
				handle_error("Error in SIGTSTP");
			}
			// int terminal_father = wait(&status);
			int terminal_father = waitpid(terminal , &status , WUNTRACED | WCONTINUED);

			if(WIFSTOPPED(status)){
				continue;
			}
			kill(terminal , SIGKILL);
		}

		destroy_term_arg(term);
		free(token);
	}
}