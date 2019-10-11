sshell : sshell.o cmdOperations.o
		gcc -o sshell sshell.o cmdOperations.o
sshell.o : sshell.c
		gcc -c -Wall -Werror -g sshell.c
cmdOperations.o: cmdOperations.c 
		gcc -c -Wall -Werror -g cmdOperations.c
clean :
		rm -f sshell sshell.o
