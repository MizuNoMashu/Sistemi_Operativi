#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"

//to handle signal when the main proc is forked
void handle_signal(int signal){
	switch (signal){
		case SIGINT:
			write(1 , "\n" , 2);
			kill(getpid() , SIGCONT);
			fflush(stdout);
			return;
		case SIGCHLD:
			fflush(stdout);
			return;
	}
}

//to handle signal when before fork
void handle_signal_less(int signal){
	switch (signal){
		case SIGINT:
			write(1 , "\n" , 2);
			fflush(stdout);
			return;
	}
}