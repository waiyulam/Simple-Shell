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

// currently assume maximum command line argument is 16
#define Max_ARG 16
// Maximum input line size is 512
size_t buffersize = 512;
extern int errno;

int parseCmd(char* cmd, char** cmdArgs);
int redirectionCondCheck(char* input);
int redirection(char* input, int cond);
void removeSpaces(char* input, char* output);

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
			if (strcmp(cmdArgs[0], "pwd")==0)
			{
				// 100 is default -> check with porkett
				char s[100];
				fprintf(stderr, "%s\n", getcwd(s, 100));
				exit(0);
			} else if (strcmp(cmdArgs[0], "cd") == 0)
			{
				// char s[100];
				char path[150];
				strcpy(path,cmdArgs[1]);
				// sprintf(path,"%s%s%s",getcwd(s, 100),"/",cmdArgs[1]);
				if (chdir(path) == 0)
				{
					exit(0);
			  }
				else
			  {
					fprintf(stderr, "Error: no such directory\n");
					exit(1);
				}
			} else if (redirectionCondCheck(cmd)>0)
			{ // redirection operation
				fprintf(stderr, "I am here");
				redirection(cmd, redirectionCondCheck(cmd));
			} else
			{
				execvp(args[0], args);
				// This is not the error handle for command not found
				exit(1);
			}
		} else if (pid == -1)
		{
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
		int fd = 0;
 		command = strtok(input, s);
		temp = strtok(NULL, "<");
		removeSpaces(temp, token);
		temp = strtok(NULL, "<");
		removeSpaces(temp, filename);
		// Debugging purposes to clarity command, file and token
		printf("current command: %s\n", command);
		printf("token portion: %s\n", token);
		printf("file portion: %s\n", filename);
		printf("length of token: %zu\n", strlen(token));
		printf("length of file: %zu\n", strlen(filename));
		// check if user didn't type input file

		if (filename == NULL) {
			fprintf(stderr, "Error: no input file\n");
			exit(1);
		}

		/*fprintf(stderr, "%s\n", filename);*/
		fd = open(filename, O_RDONLY);
		// check if the file user inputted can not be opened
		if (fd < 0) {
			fprintf(stderr, "Error: cannot open input file\n");
			exit(1);
		}
		// change the input stream to fd
		dup2(fd, 0);
		char *args[3];
		// store program and arguments together all in the arguments
		args[0] = command;
		args[1] = token;
		args[2] = NULL;

		free(filename);
		execvp(args[0], args);
		fprintf(stderr, "exectution not successful");
		exit(1);
		//free(command);
		free(token);
		//free(temp);
		return 0;
	}
	return -1;
}


void removeSpaces(char* input, char* output)
{	// output has its own index differing from input
	int j = 0;
	int frontCount = 0;
	int endCount = 0;
	int frontflag = 1;
	int endflag = 1;
	for(int i = 0; i < strlen(input); i++) {
		if ((input[i]==' ') && frontflag) {
			frontCount++;
		} else{
			frontflag = 0;
		}
	}
	for(int i = strlen(input)-1; i >= 0; i--) {
		if ((input[i]==' ') && endflag) {
			endCount++;
		} else{
			endflag = 0;
		}
	}
	for(int i = frontCount; i < strlen(input)-endCount; i++) {
		output[j] = input[i];
		j++;
	}
}
