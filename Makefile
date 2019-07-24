
output: term.o 
	gcc -o Term term.c -lreadline

term.o: term.c
	gcc -c term.c

clean:
	rm *.o Term