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
// Maximum input line size is 512
size_t buffersize = 512;
extern int errno;

int parseCmd(char* cmd, char** cmdArgs);

int main(int argc, char *argv[])
{
	while (true)
	{
		printf("sshell$ ");
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
	  if (strcmp(cmd, "exit") == 0){
			fprintf(stderr, "Bye...\n");
			exit(0);
		}

		//parse command line to arguments 
		int argSize = parseCmd(cmd, cmdArgs);
		// Try to open a directory for cd command
		// error handling type setting

		/*printf("PATH : %s\n", getenv("PATH"));*/
		char *args[argSize+1];
		for (int i = 0; i < argSize; i++){
			args[i] = cmdArgs[i];
		}
		args[argSize] = NULL;
		
		int pid = fork();
		if (pid == 0){
			/* Child process, use execvp to execute command on env variable*/
			if (strcmp(cmdArgs[0], "pwd")==0){
				// 100 is default -> check with porkett
				char s[100];
				fprintf(stderr, "%s\n", getcwd(s, 100));
				exit(0);
			} else if (strcmp(cmdArgs[0], "cd") == 0){
				// char s[100];
				char path[150];
				strcpy(path,cmdArgs[1]);
				// sprintf(path,"%s%s%s",getcwd(s, 100),"/",cmdArgs[1]);
				if (chdir(path) == 0) {
					exit(0);
				} else {
					fprintf(stderr, "Error: no such directory\n");
					exit(1);
				}
			} else{
				execvp(args[0], args);
				// This is not the error handle for command not found
				exit(1);
			}
		} else if (pid == -1) {
			/*fork error printing*/
			perror("fork fails to spawn a child");
			exit(1);
		} else {
			// parent process, waits for child execution
			wait(&status);
			if (strcmp(cmdArgs[0], "cd")==0)
			{
				// char s[100];
				char path[150];
				strcpy(path,cmdArgs[1]);
				// sprintf(path,"%s%s%s",getcwd(s, 100),"/",cmdArgs[1]);
				chdir(path);
			}		
		}
			fprintf(stderr, "+ completed \'%s\' [%d]\n",cmd,WEXITSTATUS(status));
			free(cmd);
	}

}

int parseCmd(char* cmd, char** cmdArgs)
{
		// make a temp string to avoid cmd altered 
		char *temp = (char *)malloc(buffersize * sizeof(char));
		strcpy(temp,cmd);
		char s[2] = " ";
		cmdArgs[0] = strtok(temp, s);
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
