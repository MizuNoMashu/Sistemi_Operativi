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
void custom_execvp(char** token , pid_t child , int n_pipe , int n_thread , int still_good){
	int i = 0;
	char* command_separator = "";

	if(token[0] == NULL){
		return;
	}

	while(token[i] != NULL){
		
		if(strcmp(token[i] , "&&") == 0){
			command_separator = "&&";
			if(do_custom_execvp(token , command_separator , child , n_pipe , n_thread , still_good) == -1){
				return;
			}

			if(still_good == 1){
				break;
			}
		}
		
		else if(strcmp(token[i] , "||") == 0){
			command_separator = "||";
			if(do_custom_execvp(token , command_separator , child , n_pipe , n_thread , still_good) == -1){
				return;
			}

			if(still_good == 1){
				break;
			}
		}

		else if(strcmp(token[i] , ";") == 0){
			do_custom_execvp_semicolon(token , child);
			return;
		}

		else if(strcmp(token[i] , ">") == 0){
			command_separator = ">";
			do_custom_execvp_redirect(token , command_separator , child , n_thread , n_pipe , still_good);
			return;
		}

		else if(strcmp(token[i] , ">>") == 0){
			command_separator = ">>";
			do_custom_execvp_redirect(token , command_separator , child , n_thread , n_pipe , still_good);
			return;
		}

		else if(strcmp(token[i] , "<") == 0){
			command_separator = "<";
			do_custom_execvp_redirect(token , command_separator , child , n_thread , n_pipe , still_good);
			return;
		}

		i++;
	}
	execvp(token[0] , token);
	fprintf(stderr, "%s Comando non disponibile\n", token[0]);
	return;
}

//execute select case from custom_execvp
int do_custom_execvp(char** token , char* command_separator , pid_t child , int n_pipe , int n_thread , int still_good){
	int status_e;	
	int i = 0;
	int j = 0;

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
		custom_execvp(token_first_half , child , n_pipe , n_thread , still_good);
		free(token_first_half);
		free(token_second_half);
	}
	else{
		int padre_e = waitpid(pid_e , &status_e , 0);
		free(token_first_half);
		
		int k;
		int check;

		//handle "and" 
		if(command_separator == "&&" && status_e != 0 ){
			k = 0;
			check = 0;
			//check if "or" is here
			while(token_second_half[k] != NULL){
				if(strcmp(token_second_half[k] , "||") != 0){
					k++;
				}
				else{
					check = 1;
					k++;
					break;
				}
			}

			if(check != 0){
				still_good = 1;
				custom_execvp(token_second_half + k , child , n_pipe , n_thread , still_good);
				free(token_second_half);
				return -1;
			}
			else{
				free(token_second_half);
				return -1;
			}
		}
		else if(command_separator == "&&" && status_e == 0){			
			still_good = 1;

			custom_execvp(token_second_half , child , n_pipe , n_thread , still_good);
			free(token_second_half);
		}

		//handle or
		if(command_separator == "||" && status_e == 0){
			k = 0;
			check = 0;

			while(token_second_half[k] != NULL){
				if(strcmp(token_second_half[k] , "&&") != 0){
					k++;
				}
				else{
					check = 1;
					k++;
					break;
				}
			}

			if(check != 0){
				still_good = 1;
				custom_execvp(token_second_half + k , child , n_pipe , n_thread , still_good);
			}
			else{
				free(token_second_half);
				return -1;
			}
		}
		else if(command_separator == "||" && status_e != 0){
			still_good = 1;

			custom_execvp(token_second_half , child , n_pipe , n_thread , still_good);
			free(token_second_half);
		}

		//father get error
		if(padre_e == -1){
			free(token_first_half);
			free(token_second_half);
			handle_error("Error");
		}
		return -1;	
	}
	return -1;
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
		custom_execvp(token_first_half , child , 0 , 0 , 0);
		free(token_first_half);
		free(token_second_half);
	}
	else{
		free(token_first_half);
		int status_semicolon;
		int father = wait(&status_semicolon);
		if(father == -1){
				return;
		}
		custom_execvp(token_second_half , child , 0 , 0 , 0);
		free(token_second_half);
	}		
}

//exec in case of &
void do_custom_execvp_ecom(char** token , pid_t child , int n_thread , int n_pipe , int still_good){
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
		custom_execvp(token_first_half , child , n_pipe , n_thread , still_good);
		free(token_first_half);
		free(token_second_half);
	}
	else{
		free(token_first_half);
		if(n_thread == 1){
			custom_execvp(token_second_half , child , n_pipe , n_thread , still_good);
			free(token_second_half);
		}
		else if(n_thread > 1){
			do_custom_execvp_ecom(token_second_half , child , --n_thread , n_pipe , still_good);
			free(token_second_half);
		}
	}
	return;
}

void do_custom_execvp_pipe(char** token , int n_pipe , int pipe_fd , int last_pipe , pid_t child){
	
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
		custom_execvp(token , child , 0 , 0 , 0);
		// free(token);
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
		custom_execvp(token_first_half , child , 0 , 0 , 0);
		free(token_first_half);
	}
	else{
		free(token_first_half);
		if(n_pipe == 1){
			if(close(pipefd[1]) == -1 || close(pipe_fd) == -1){
				handle_error("Error");
			}
			do_custom_execvp_pipe(token_second_half , n_pipe , pipefd[0] , 1 , child);
			free(token_second_half);
		}
		else{
        	if(close(pipefd[1]) == -1 || close(pipe_fd) == -1){
				handle_error("Error");
			}
			do_custom_execvp_pipe(token_second_half , --n_pipe , pipefd[0] , 0 , child);
			free(token_second_half);
		}
	}
}

//this function is to handle input and output redirect
void do_custom_execvp_redirect(char** token , char* command_separator , pid_t child , int n_thread , int n_pipe , int still_good){
	int i = 0;
	int j = 0;
	int status_redirect;

	while(strcmp(token[i], command_separator ) != 0){
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
	
	pid_t pid_redirect = fork();
	
	if(pid_redirect == -1){
		free(token_first_half);
		free(token_second_half);
		handle_error("Error");
	}
	else if(pid_redirect == 0){
		if(command_separator == ">"){
			int fd1 = creat(token_second_half[0] , 0644);
			dup2(fd1 , STDOUT_FILENO);
			close(fd1);
			custom_execvp(token_first_half , child , n_pipe , n_thread , still_good);
		}
		else if(command_separator == ">>"){
			int fd2 = open(token_second_half[0] , O_CREAT | O_WRONLY | O_APPEND , 0644);
			dup2(fd2 , STDOUT_FILENO);
			close(fd2);
			custom_execvp(token_first_half , child , n_pipe , n_thread , still_good);
		}
		else if(command_separator == "<"){
			int fd3 = open(token_second_half[0] , O_RDONLY , 0);
			dup2(fd3 , STDIN_FILENO);
			close(fd3);
			custom_execvp(token_first_half , child , n_pipe , n_thread , still_good);
		}
	}
	else{
		waitpid(pid_redirect , &status_redirect , 0);
	}
}