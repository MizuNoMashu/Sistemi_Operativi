#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifndef COMMON
#define COMMON
 
int exit_count;
FILE *history;
int up_or_down;
int state;

void clean_term();
void location();
void exiting();
void control_keyboard();
void keyup();
void keydown();
void restart_process();
void help();
#endif
