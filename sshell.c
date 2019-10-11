#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>

// currently assume maximum command line argument is 16
#define Max_ARG 16
extern int errno;

int parseCmd(char* cmd, char** cmdArgs);

int main(int argc, char *argv[])
{
	while (true)
	{
		printf("sshell$ ");
		// Maximum input line size is 512
		size_t buffersize = 512;
		char *cmd = (char *)malloc(buffersize * sizeof(char));
		char *cmdArgs[Max_ARG];
		for (int i = 0; i < Max_ARG; i++)
		{
			cmdArgs[i] = (char *)malloc(buffersize * sizeof(char));
		}
		// Check malloc allocation success
		if (cmd == NULL)
		{
			perror("malloc fails to alloscate memory");
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

		int argSize = parseCmd(cmd, cmdArgs);
		// Try to open a directory for cd command
		DIR* dir = opendir(cmdArgs[1]);
		// error handling type setting

		/*printf("PATH : %s\n", getenv("PATH"));*/
		char *args[] = {cmdArgs[0], NULL};

		int pid = fork();
		if (pid == 0){
			/* Child process, use execvp to execute command on env variable*/
			execvp(cmdArgs[0], args);
			fprintf(stderr, "Error: command not found\n");
			exit(1);
		} else if (pid == -1) {
			/*fork error printing*/
			exit(1);
			perror("fork fails to spawn a child");
		} else {
			/*parent process, waits for child execution*/
			wait(&status);
		}

		if (strcmp(cmdArgs[0], "cd")==0)
		{
			if (dir) {
				closedir(dir);
			} else if (ENOENT == errno) {
				printf("Error: no such directory\n");
			} else {
				exit(1);
				perror("some other system errors\n");
			}
		}

		// Error handling printing option for cd
		fprintf(stderr, "+ completed \'");
		for (int i = 0; i<argSize; i++)
		{	if (i < argSize-1){
			fprintf(stderr, "%s ", cmdArgs[i]);
			} else{
			fprintf(stderr, "%s", cmdArgs[i]);
			}
		}

		fprintf(stderr, "\' [%d]\n", WEXITSTATUS(status));
		// release memory for my command
		free(cmd);
	}
}

int parseCmd(char* cmd, char** cmdArgs)
{
		char s[2] = " ";
		cmdArgs[0] = strtok(cmd, s);
		int count = 1;
		char* token;
		while (true)
		{
			token = strtok(NULL, s);
			if (token != NULL){
				cmdArgs[count] = token;
			} else{
				break;
			}
			count++;
		}
		return count;
}
