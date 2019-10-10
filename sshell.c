#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	while (true)
	{
		printf("sshell$ ");
		// Maximum input line size is 512
		size_t buffersize = 512;
		char *cmd = (char *)malloc(buffersize * sizeof(char));
		// Check malloc allocation success
		if (cmd == NULL)
		{
			perror("malloc fails to allocate memory");
			exit(1);
		}
		// read line from user input currently assume just command
		getline(&cmd, &buffersize, stdin);
		int status = 0;
		// removes new line character, mark the end by end of line character
		if (cmd[strlen(cmd)-1] == '\n')
		{
    	cmd[strlen(cmd)-1] = '\0';
    }
		// exit the shell
	  if (strcmp(cmd, "exit") == 0) break;

		char *args[]={cmd, NULL};
		int pid = fork();
		if (pid == 0)
		{
			/* Child process, use execvp to execute command on env variable*/
			execvp(args[0], args);
			exit(1);
			perror("execvp is not successfully executed");
		} else if (pid == -1) {
			/*fork error printing*/
			exit(1);
			perror("fork fails to spawn a child");
		} else {
			/*parent process, waits for child execution*/
			wait(&status);
		}

		fprintf(stderr, "+ completed \'%s\': [%d]\n", cmd, status);
		// release memory for my command
		free(cmd);
	}
}
