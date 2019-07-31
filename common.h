#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifndef COMMON
#define COMMON

#include <readline/readline.h> 
int exit_count;
void clean_term();
void location();
void exiting();
void controll_keyboard();
void keyup();
void keydown();
#endif
