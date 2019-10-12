#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
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

int redirectionCondCheck(char* input)
{
	// strchr returns the pointer to the first occurence of character, null otherwise

	// Check if '<' and '>' both occur, return 1 if true
	if ((strchr(input, '<')!=NULL) && (strchr(input, '>')!=NULL))
	{
		return 1;
	}
	// Check if '<' occurs but '>' not occurs, return 2 if true
	else if ((strchr(input, '<')!=NULL) && (strchr(input, '>')==NULL))
	{
		return 2;
	}
	// Check if '>' occurs but '<' not occurs, return 3 if true
	else if ((strchr(input, '<')==NULL) && (strchr(input, '>')!=NULL))
	{
		return 3;
	}
	// Check if '>' and '<' both not occur, return 0
	else {
		return 0;
	}
	return -1;
}

int redirection(char* input, int cond)
{
	if (cond == 2) {
		char* command = (char *)malloc(20 * sizeof(char));
		char* token = (char *)malloc(100 * sizeof(char));
		char* filename = (char *)malloc(100 * sizeof(char));
		//  Empty space token for further use
		char s[2] = " ";
		char* temp = (char *)malloc(100 * sizeof(char));
 		command = strtok(input, s);
		temp = strtok(NULL, "<");
		removeSpaces(temp, token);
		temp = strtok(NULL, "<");
		removeSpaces(temp, filename);
		printf("current command: %s\n", command);
		printf("token portion: %s\n", token);
		printf("file portion: %s\n", filename);
		printf("length of file: %zu\n", strlen(token));
		printf("length of file: %zu\n", strlen(filename));
		//free(command);
		//free(filename);
		//free(token);
		//free(temp);
		return 0;
	}
	return -1;
}


void removeSpaces(char* input, char* output)
{	// output has its own index differing from input
	int j = 0;
	for(int i = 0; i < strlen(input); i++) {
		if (input[i]!=' ') {
			output[j] = input[i];
			j++;
		}
	}
}
