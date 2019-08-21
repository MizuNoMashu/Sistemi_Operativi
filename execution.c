#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "command.h"
#include "execution.h"
#include "common.h"

//make do_custom_execvp to controll more cases
void custom_execvp(char** token , pid_t child){
	int i = 0;
	int check = 0;
	char* command_separator = "";
	while(token[i] != NULL){
		if(strcmp(token[i] , "&&") == 0){
			command_separator = "&&";
			check = do_custom_execvp(token , command_separator , child);
			if(check == -1){ // in case left command get error return
				return;
			}
			
		}
		
		else if(strcmp(token[i] , "||") == 0){
			command_separator = "||";
			check = do_custom_execvp(token , command_separator , child);
			if(check == 0){ // in case left command get exec return
				return;
			}
		}
		else if(strcmp(token[i] , ";") == 0){
			do_custom_execvp_semicolon(token , child);
			return;
		}
		i++;
	}
	if(check != -1 && command_separator != "&&"){
		execvp(token[0] , token);
		fprintf(stderr, "%s Comando non disponibile\n", token[0]);
		return;
	}
	return;
}

void do_custom_execvp_semicolon(char** token , pid_t child){
	int i = 0;
	int j = 0;
	while(strcmp(token[i],";") != 0){
		i++;
	}
	while(token[j+i] != NULL){
		j++;
	}
	char** token_first_half = malloc((i+1)*sizeof(char*));
	char** token_second_half = malloc(j*sizeof(char*));
	memcpy(token_first_half , token , i*sizeof(char*));
	memcpy(token_second_half , token + i + 1 ,j*sizeof(char*));
	token_first_half[i] = NULL;
	pid_t pid_semicolon = fork();
	if(pid_semicolon == -1){
		free(token_first_half);
		free(token_second_half);
		handle_error("Error");
	}
	else if(pid_semicolon == 0){
		custom_execvp(token_first_half , child);
		free(token_first_half);
	}
	else{
		int status_semicolon;
		int father = wait(&status_semicolon);
		if(father == -1){
				return;
		}
		custom_execvp(token_second_half , child);
		free(token_second_half);
	}		
}

//exec in case of &
void do_custom_execvp_ecom(char** token , pid_t child , int n_thread){
	int i = 0;
	int j = 0;
	while(strcmp(token[i],"&") != 0){
		i++;
	}
	while(token[j+i] != NULL){
		j++;
	}
	char** token_first_half = malloc((i+1)*sizeof(char*));
	char** token_second_half = malloc(j*sizeof(char*));
	memcpy(token_first_half , token , i*sizeof(char*));
	memcpy(token_second_half , token + i + 1 ,j*sizeof(char*));
	token_first_half[i] = NULL;
	pid_t pid_ecom = fork();
	if(pid_ecom == -1){
		free(token_first_half);
		free(token_second_half);
		handle_error("Error");
	}
	else if(pid_ecom == 0){
		custom_execvp(token_first_half , child);
		free(token_first_half);
	}
	else{
		if(n_thread == 1){
			custom_execvp(token_second_half , child);
			free(token_second_half);
		}
		else if(n_thread > 1){
			do_custom_execvp_ecom(token_second_half , child , --n_thread);
			free(token_second_half);
		}
	}		
	return;
}

//execute select case from custom_execvp
int do_custom_execvp(char** token , char* command_separator , pid_t child){
	int i = 0;
	int j = 0;
	int exec_ret = 0;
	while(strcmp(token[i],command_separator) != 0){
		i++;
	}
	while(token[j+i] != NULL){
		j++;
	}
	char** token_first_half = malloc((i+1)*sizeof(char*));
	char** token_second_half = malloc(j*sizeof(char*));
	memcpy(token_first_half , token , i*sizeof(char*));
	memcpy(token_second_half , token + i + 1 ,j*sizeof(char*));
	token_first_half[i] = NULL;

	pid_t pid_e = fork();
	if(pid_e == -1){
		free(token_first_half);
		free(token_second_half);
		handle_error("Error");
	}
	else if(pid_e == 0){
		exec_ret = execvp(token_first_half[0],token_first_half);
		if(exec_ret == -1){
			fprintf(stderr, "%s Comando non disponibile\n", token_first_half[0]);
			free(token_first_half);
			if(command_separator == "&&"){
				free(token_second_half);
				kill(getppid() , SIGKILL);
			}
			else if(command_separator == "||"){
				custom_execvp(token_second_half , child);
			}
			return exec_ret;
		}
		handle_error("Error");
	}
	else{
		int status_e;	
		int padre_e = wait(&status_e);
		if(command_separator == "||" && !exec_ret){
			free(token_first_half);
			free(token_second_half);
			kill(child , SIGKILL);
		}
		if(padre_e == -1){
			free(token_first_half);
			free(token_second_half);
			handle_error("Error");
		}

		
	}
	free(token_first_half);
	custom_execvp(token_second_half , child);
	free(token_second_half);
	return -1;
}

void do_custom_execvp_pipe(char** token , int n_pipe , int pipe_fd , int last_pipe){
	
	int i = 0;
	int j = 0;

	if(last_pipe == 1 && n_pipe == 1){
		if(pipe_fd != STDIN_FILENO){
		    if(dup2(pipe_fd, STDIN_FILENO) != -1){
		    	close(pipe_fd);
		    }
		    else{
		    	handle_error("Error write");
		    }
		}
		custom_execvp(token , 0);
		free(token);
		return;
	}

	while(strcmp(token[i],"|") != 0){
		i++;
	}
	while(token[j+i] != NULL){
		j++;
	}
	char** token_first_half = malloc((i+1)*sizeof(char*));
	char** token_second_half = malloc(j*sizeof(char*));
	memcpy(token_first_half , token , i*sizeof(char*));
	memcpy(token_second_half , token + i + 1 ,j*sizeof(char*));
	token_first_half[i] = NULL;

	int pipefd[2];
	pipe(pipefd);

	pid_t pid_pipe = fork();
	if(pid_pipe == -1){
		free(token_first_half);
		free(token_second_half);
		handle_error("Error");
	}
	else if(pid_pipe == 0){
 		if(dup2(pipe_fd , STDIN_FILENO) == -1){
 			handle_error("Error read");
 		}
        if(dup2(pipefd[1] , STDOUT_FILENO) == -1){
        	handle_error("Error write");
        }
        if(close(pipefd[1]) == -1){
        	handle_error("Error");
        }
		custom_execvp(token_first_half , 0);
		free(token_first_half);
	}
	else{
		if(n_pipe == 1){
			if(close(pipefd[1]) == -1 || close(pipe_fd) == -1){
				handle_error("Error");
			}
			do_custom_execvp_pipe(token_second_half , n_pipe , pipefd[0] , 1);
			free(token_second_half);
		}
		else{
        	if(close(pipefd[1]) == -1 || close(pipe_fd) == -1){
				handle_error("Error");
			}
			do_custom_execvp_pipe(token_second_half , --n_pipe , pipefd[0] , 0);
			free(token_second_half);
		}
	}
}