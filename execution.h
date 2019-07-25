#ifndef EXECUTION
#define EXECUTION

void custom_execvp(char** token , pid_t child);
int do_custom_execvp(char** token , char* command_separator , pid_t child);
void do_custom_execvp_ecom(char** token , pid_t child , int n_thread);
void do_custom_execvp_semicolon(char** token , pid_t child);

#endif