#ifndef EXECUTION
#define EXECUTION

void custom_execvp(char** token , pid_t child , int n_pipe , int n_thread , int still_good);
int do_custom_execvp(char** token , char* command_separator , pid_t child , int n_pipe , int n_thread , int still_good);
void do_custom_execvp_ecom(char** token , pid_t child , int n_thread , int n_pipe , int still_good);
void do_custom_execvp_semicolon(char** token , pid_t child);
void do_custom_execvp_pipe(char** token , int n_pipe , int pipe_fd , int last_pipe , pid_t child);
void do_custom_execvp_redirect(char** token , char* command_separator , pid_t child , int n_thread , int n_pipe , int still_good);
#endif