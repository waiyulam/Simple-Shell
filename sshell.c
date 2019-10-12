#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cmdOperations.h"

// currently assume maximum command line argument is 16
#define Max_ARG 16
extern int errno;

int parseCmd(char* cmd, char** cmdArgs);
int redirectionCondCheck(char* input);
int redirection(char* input, int cond);
void removeSpaces(char* input, char* output);

int main(int argc, char *argv[])
{
	while (1)
	{	
		// Maximum input line size is 512
		size_t buffersize = 512;
		char* user_input = (char *)malloc(buffersize * sizeof(char));
		printf("sshell$ ");
		// read line from user input currently assume just command
		getline(&user_input, &buffersize, stdin);

		Command* command = Command__create(user_input);

		// Check if the command is valid 
		if (command__Fail(command)){
			continue; // error 
		}

		// Check malloc allocation success
		if (command == NULL)
		{
			perror("malloc fails to alloscate memory");
			exit(1);
		}

		// exit the shell
	 	if (strcmp(command__program(command), "exit") == 0){
			fprintf(stderr, "Bye...\n");
			exit(0);
		}

		int status = 0;
		int pid = fork();
		if (pid == 0){
			/* Child process, use execvp to execute command on env variable*/
					// Check if command need input redirection 
			if (strlen(command__indirect(command)) != 0){
				int fd;
				// fprintf(stderr, "Input direct file %s\n",command__indirect(command));
				fd = open(command__indirect(command), O_RDONLY);
				// change the input stream to fd
				dup2(fd, 0);
				close(fd);
			}

			// Check if command need output redirection 
			if (strlen(command__outdirect(command)) != 0){
				int fd;
				// fprintf(stderr, "Output direct file %s\n",command__outdirect(command));
				fd = open(command__outdirect(command),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
				// change the input stream to fd
				dup2(fd, 1);
				close(fd);
			}

			if (strcmp(command__program(command), "pwd")==0){
				// 100 is default -> check with porkett
				char s[100];
				fprintf(stderr, "%s\n", getcwd(s, 100));
				exit(0);
			} else if (strcmp(command__program(command), "cd") == 0){
				// char s[100];
				char path[150];
				strcpy(path,command__cmdArgs(command)[1]);
				// sprintf(path,"%s%s%s",getcwd(s, 100),"/",cmdArgs[1]);
				if (chdir(path) == 0) {
					exit(0);
				} else {
					fprintf(stderr, "Error: no such directory\n");
					exit(1);
				}
			} else{
				execvp(command__program(command), command__cmdArgs(command));
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
			if (strcmp(command__program(command), "cd")==0)
			{
				// char s[100];
				char path[150];
				strcpy(path,command__cmdArgs(command)[1]);
				// sprintf(path,"%s%s%s",getcwd(s, 100),"/",cmdArgs[1]);
				chdir(path);
			}
		}
			fprintf(stderr, "+ completed \'%s\' [%d]\n",command__cmd_line(command),WEXITSTATUS(status));
			Command__destroy(command);
	}
}

