#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_STR 256

char* temp_command;

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


//take command in input
char* get_command(){ 
	char buf[MAX_STR];
	if(fgets(buf , sizeof(buf) , stdin) == NULL){ //check if some command was written
		return NULL;
	}
	return strdup(buf); //return only used memory
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

//clear the terminal
void clean_term(){
	printf("\33c\e[3J");
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
			if(check == -1){
				return;
			}
			
		}
		else if(strcmp(token[i] , "||") == 0){
			command_separator = "||";
		}
		i++;
	}
	if(check != -1 && command_separator != "&&"){
		execvp(token[0] , token);
	}
	return;
}

//execute select case from custom_execvp
int do_custom_execvp(char** token , char* command_separator , pid_t child){
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
		if(execvp(token_first_half[0],token_first_half) == -1){
			free(token_first_half);
			free(token_second_half);
			if(command_separator == "&&"){
				kill(getppid() , SIGKILL);
			}
			return -1;
		}
		handle_error("Error");
	}
	else{
		int status_e;	
		int padre_e = wait(&status_e);
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