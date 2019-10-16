sshell : sshell.c cmdLineOperations.h cmdLineOperations.o cmdOperations.h cmdOperations.o 
		gcc -o sshell sshell.c  cmdOperations.o cmdLineOperations.o 

cmdLineOperations.o: cmdLineOperations.c 
		gcc -c -Wall -Werror -g cmdLineOperations.c

cmdOperations.o: cmdOperations.c 
		gcc -c -Wall -Werror -g cmdOperations.c

clean :
		rm -f sshell *.o
