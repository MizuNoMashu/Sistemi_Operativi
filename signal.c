#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"

void handle_signal(int signal){
	// const char* signal_type;
         
	switch (signal){
		case SIGINT:
			printf("\n");
			kill(getpid() , SIGCONT);
			fflush(stdout);
			return;
		case SIGCHLD:
			fflush(stdout);
			return;
	}
}