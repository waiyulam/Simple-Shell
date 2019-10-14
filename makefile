sshell : sshell.c pipeOperations.h pipeOperations.o cmdOperations.h cmdOperations.o 
		gcc -o sshell sshell.c  cmdOperations.o pipeOperations.o 

pipeOperations.o: pipeOperations.c 
		gcc -c -Wall -Werror -g pipeOperations.c

cmdOperations.o: cmdOperations.c 
		gcc -c -Wall -Werror -g cmdOperations.c

clean :
		rm -f sshell *.o
