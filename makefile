sshell : sshell.o
		gcc -o sshell sshell.o
sshell.o : sshell.c
		gcc -c -Wall -Werror -g sshell.c
clean :
		rm -f sshell sshell.o
