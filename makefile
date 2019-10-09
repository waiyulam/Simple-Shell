sshell : sshell.o 
		cc -o sshell sshell.o 
sshell.o : sshell.c 
		cc -c -Wall -Werror sshell.c 
clean :
		rm sshell sshell.o