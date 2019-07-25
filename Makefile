
output: term.o common.o command.o execution.o
	gcc -o Term term.o common.o command.o execution.o  -lreadline

term.o: term.c
	gcc -c term.c 

common.o: common.c common.h
	gcc -c common.c

command.o: command.c command.h 
	gcc -c command.c

execution.o: execution.c execution.h command.h common.h
	gcc -c execution.c

clean:
	rm *.o Term