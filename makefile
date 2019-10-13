sshell : sshell.o pipeOperations.h pipeOperations.o cmdOperations.h cmdOperations.o 
		gcc -o sshell sshell.o  cmdOperations.o pipeOperations.o 

sshell.o: sshell.c 
		gcc -c -Wall -Werror -g sshell.c 

pipeOperations.o: pipeOperations.c 
		gcc -c -Wall -Werror -g pipeOperations.c

cmdOperations.o: cmdOperations.c 
		gcc -c -Wall -Werror -g cmdOperations.c

clean :
		rm -f sshell *.o
