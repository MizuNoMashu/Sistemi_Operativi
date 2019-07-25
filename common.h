#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_STR 1000

char* temp_command;
int status_semicolon;
//struct to controll the terminal
typedef struct term_arg{
	char* command;
	int length_command;
	int num_token;
}terminal_arg;


char* get_command();
int get_num_token(char* command);
terminal_arg* allocate_term_arg(int len_temp_command);
void destroy_term_arg(terminal_arg* term);
char** get_token(char* command , int length_command , int num_token);
void clean_term();
void custom_execvp(char** token , pid_t child);
int do_custom_execvp(char** token , char* command_separator , pid_t child);
int count_ecom(char** token);
void custom_execvp_ecom(char** token , pid_t child , int n_thread);
void do_custom_execvp_semicolon(char** token , pid_t child);

//take command in input
char* get_command(){ 
	char buf[MAX_STR];
	char* cmd;
	cmd = readline("\nCommand: "); //get current command
	if(cmd == NULL){
		return NULL;
	}
	else{
		strcpy(buf , cmd);
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
	array_token[MAX_STR] = strtok(command , " \t\n"); //dato command lo filtra in base a " " \t e \n
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

//clear the terminal
void clean_term(){
	printf("\033[H\033[J");
}

//make do_custom_execvp to controll more cases
void custom_execvp(char** token , pid_t child){
	int i = 0;
	int check = 0;
	char* command_separator = "";
	while(token[i] != NULL){
		if(strcmp(token[i] , "&&") == 0){
			command_separator = "&&";
			check = do_custom_execvp(token , command_separator , child);
			if(check == -1){ // in caso il comando a sinistra del && va in errore esci
				return;
			}
			
		}
		
		else if(strcmp(token[i] , "||") == 0){
			command_separator = "||";
			check = do_custom_execvp(token , command_separator , child);
			if(check == 0){ // in caso il comando a sinistra del || viene eseguito esci
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