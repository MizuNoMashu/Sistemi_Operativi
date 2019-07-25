#ifndef COMMAND
#define COMMAND

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
int count_ecom(char** token);
terminal_arg* allocate_term_arg(int len_temp_command);
void destroy_term_arg(terminal_arg* term);
char** get_token(char* command , int length_command , int num_token);

#endif