#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifndef COMMON
#define COMMON

void clean_term();
void location();
#endif
