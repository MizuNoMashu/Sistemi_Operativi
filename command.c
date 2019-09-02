#include <stdlib.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <unistd.h>

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
		if(fclose(history) != 0){
			handle_error("Error fclose:");
		}
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
	int i = 0 , j = 0;
	char** token = malloc((MAX_STR)*sizeof(char*));
	char* save_token;
	int expand_alias = 0;
	if(token == NULL){
		return NULL;
	}
	for(j ; j < MAX_STR ; j++){
		token[j] = NULL;
	}
	// token[0] = strtok(command, " \t\n");
	for(i ; i < num_token ; i++){
		if(i == 0){
			token[i] = strtok_r(command, " \t\n" , &save_token);
		}
		else{
			token[i] = strtok_r(NULL , " \t\n" , &save_token);
		}
		
		if(strcmp(token[i] , "mkdir") == 0 || strcmp(token[i] , "touch") == 0 || strcmp(token[i] , "rm") == 0 || strcmp(token[i] , "cd") == 0){
			expand_alias = 1;
		}

		if(strcmp(token[i] , "&&") == 0 || strcmp(token[i] , "||") == 0 || strcmp(token[i] , "|") == 0 
			|| strcmp(token[i] , "&") == 0 || strcmp(token[i] , ">") == 0 || strcmp(token[i] , ">>") == 0 
			|| strcmp(token[i] , "<") == 0){
			expand_alias = 0;
			continue;
		}
		if(expand_alias != 0){
			continue;
		}
		// char* temp_system = malloc(1 + strlen("bash /home/") + strlen(getenv("USERNAME")) + strlen("/Desktop/Sistemi_Operativi_prove/copia_progetto/.my_bashrc.sh ") + strlen(token[i]) + strlen(" 2> /dev/null"));
		char* temp_system = malloc(1 + strlen("bash /home/") + strlen(getenv("USERNAME")) + strlen("/Desktop/Sistemi_Operativi/.my_bashrc.sh ") + strlen(token[i]) + strlen(" 2> /dev/null"));
		strcpy(temp_system , "bash /home/");
		strcat(temp_system , getenv("USERNAME"));
		// strcat(temp_system , "/Desktop/Sistemi_Operativi_prove/copia_progetto/.my_bashrc.sh ");
		strcat(temp_system , "/Desktop/Sistemi_Operativi/.my_bashrc.sh ");
		strcat(temp_system , token[i]);
		strcat(temp_system , " 2> /dev/null");
		
		if(system(temp_system) == 512){
			// char* path = malloc(1 + strlen("/home/") + strlen(getenv("USERNAME")) + strlen("/Desktop/Sistemi_Operativi_prove/copia_progetto/.temp_bashrc"));
			char* path = malloc(1 + strlen("/home/") + strlen(getenv("USERNAME")) + strlen("/Desktop/Sistemi_Operativi/.temp_bashrc"));
			strcpy(path , "/home/");
			strcat(path , getenv("USERNAME"));
			// strcat(path , "/Desktop/Sistemi_Operativi_prove/copia_progetto/.temp_bashrc");
			strcat(path , "/Desktop/Sistemi_Operativi/.temp_bashrc");
			FILE* system_replace = fopen(path , "r");
			if(system_replace == NULL){
				handle_error("Error in fopen");
			}
			int k = 0;

			if(fseek(system_replace , 0L, SEEK_SET) == -1){
				handle_error("Error in fseek:");
			}
			while(fgetc(system_replace) != '\n'){
				if(fseek(system_replace , k++ , SEEK_SET) == -1){
					handle_error("Error in fseek:");
				}
			}
			char* system_replace_command = malloc((k)*sizeof(char));

			if(fseek(system_replace , 0L, SEEK_SET) == -1){
				handle_error("Error in fseek:");
			}
			fgets(system_replace_command , k , system_replace);
			if(fclose(system_replace) != 0){
				handle_error("Error in fclose:");
			}
			free(path);

			char* save_system;
			token[i] = strtok_r(system_replace_command , " \t\n" , &save_system);
			while(token[i] != NULL){
				i++;
				num_token++;
				token[i] = strtok_r(NULL , " \t\n" , &save_system);
			}
			
			if(token[i] == NULL){
				i--;
				num_token--;
			}	
		}
	
		free(temp_system);
	}
	token[MAX_STR-1] = NULL;
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