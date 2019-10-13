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
#include "pipeOperations.h"
#include "cmdOperations.h"


// currently assume maximum command line argument is 16
#define Max_ARG 16
extern int errno;
int myCmdHandler(Command *command) ;
int redirection(Command *command,int pipeCount);
int execute (Pipe *mypipe);
int executePipe (Pipe *mypipe,char *user_input);

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
		if (user_input[0] == '\n'){
			continue;
		}
		// Process string removes new line character, mark the end by end of line character
		if (user_input[strlen(user_input)-1] == '\n'){ user_input[strlen(user_input)-1] = '\0'; }
		// Create myPipe data structure : parsing the string and store commands
		char* temp = (char *)malloc(buffersize * sizeof(char));
		strcpy(temp,user_input);
		Pipe* myPipe = Pipe__create(temp);
		// Check malloc allocation success
		if (myPipe == NULL)
		{ 
			perror("malloc fails to alloscate memory");
			exit(1);
		}
		// Check if command line are valid 
		if (myPipe->FAIL){
			continue;
		}
		// Empty string 
		if (myPipe->cmdCount == 0){
			continue;
		}
		// Execute if there is single command 
		if (myPipe->cmdCount == 1){
			// exit the shell
			if (strcmp(command__program(myPipe->commands[0]), "exit") == 0){
				fprintf(stderr, "Bye...\n");
				exit(0);
			}
			// executing .. 
			execute(myPipe);
			Pipe__destroy(myPipe);
		}
		// execute multiple commands
		else{
			// Exit if any commands has exit flag 
			for (int i=0; i < myPipe->cmdCount;i++){
				if (strcmp(command__program(myPipe->commands[i]), "exit") == 0){
					fprintf(stderr, "Bye...\n");
					exit(0);
				}
			} // for 
			// executing .. 
			executePipe(myPipe,user_input);
			Pipe__destroy(myPipe);
    	} // if else 
	} // while loop 
} // main 


// Function to execute builtin commands
// Return 1 if command program match builtin cmd , 0 otherwise 
int myCmdHandler(Command *command) 
{ 
    int numBuiltin = 2,switchArg = 0; 
    char* myCmdsList[numBuiltin]; 
    myCmdsList[0] = "cd"; 
	char path[150];
    myCmdsList[1] = "pwd"; 
	char s[100];

    for (int i = 0; i < numBuiltin; i++) { 
        if (strcmp(command->cmdArgs[0], myCmdsList[i]) == 0) { 
            switchArg = i + 1; 
            break; 
        } 
    } 
    switch (switchArg) { 
    case 1: //cd 
		strcpy(path,command->cmdArgs[1]);
		if (chdir(path) == 0) {
			command->status = 0;
		} else {
			fprintf(stderr, "Error: no such directory\n");
			command->status = 1;
		}
        return 1; 
    case 2: // pwd
		fprintf(stdout, "%s\n", getcwd(s, 100));
		command->status = 0;;
        return 1; 
    default: 
        break; 
    }

    return 0; 
} 

// Execute single command 
int execute (Pipe *mypipe){
	if (myCmdHandler(mypipe->commands[0]) == 0){
		int status = 0;
		int pid = fork();
		if (pid == 0){
			/* Child process, use execvp to execute command on env variable*/
			// Check if command need input redirection 
			if (strlen(command__indirect(mypipe->commands[0])) != 0){
				int fd;
				fd = open(command__indirect(mypipe->commands[0]), O_RDONLY);
				dup2(fd, 0);
				close(fd);
			}
			// Check if command need output redirection 
			if (strlen(command__outdirect(mypipe->commands[0])) != 0){
				int fd;
				fd = open(command__outdirect(mypipe->commands[0]),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
				// change the input stream to fd
				dup2(fd, 1);
				close(fd);
			}
			// execute if command program is not built in commands 
			execvp(command__program(mypipe->commands[0]), command__cmdArgs(mypipe->commands[0]));
			// execvp will not return (unless Args[0] is not a valid executable file)
			fprintf(stderr, "Error: command not found\n");
			exit(1); 
		} else if (pid == -1) {
			/*fork error printing*/
			perror("fork fails to spawn a child");
			exit(-1);
		}else{
			// parent process, waits for child execution
			wait(&status);
			mypipe->commands[0]->status = WEXITSTATUS(status);
		}
	}
	if (mypipe->commands[0]->status != 255){
		fprintf(stderr, "+ completed \'%s\' [%d]\n",command__cmd_line(mypipe->commands[0]),mypipe->commands[0]->status);
	}
	return 1;
}

// Execute pipeline command 
int executePipe (Pipe *mypipe,char *user_input){
	/* parent process : keep the file descriptor for STDIN and STDOUT for later use */
	int myStdin=dup(0); // tmpin = 3 = STDIN
	int mySdtout=dup(1); // tempout = 4 = STDOUT
	// Initial pid array for all single commands 
	int pids[mypipe->cmdCount];
	// Initial exit status array for all command process
	int status[mypipe->cmdCount];
	/* For fist command child process : 
	* File descriptor : 0 STDIN is link to input redirect file or STDIN 
	* File descriptor : 1 STDOUT is link to first Pipe[1] */
	int in_fd;  // fd for input 

	// Pipe redirect errror : 
	for (int i =0; i<mypipe->cmdCount;i++){
		if (strlen(command__outdirect(mypipe->commands[i])) != 0){
			if (i != ( mypipe->cmdCount-1) ){
				fprintf(stderr,"Error: mislocated output redirection\n");
				return 0; // error -> don't execute 
			}
		}
		if (strlen(command__indirect(mypipe->commands[i])) != 0){
			if ( i != 0 ){
				fprintf(stderr,"Error: mislocated input redirection\n");
				return 0; // error -> don't execute 
			}
		}
	} // for 

	if (strlen(command__indirect(mypipe->commands[0])) != 0){
		// Check if command need input redirection 
		in_fd = open(command__indirect(mypipe->commands[0]), O_RDONLY);
	}else{
		// if no command need input redirection : use STDIN -> dup2 later 
		in_fd = dup(myStdin);
	}

	int out_fd; // fd for output 
	for (int i =0; i < mypipe->cmdCount;i++){
		// Redirect input : first command : 0/input file, later command : pipe[0]
		dup2(in_fd, 0);
		close(in_fd); // close now unused file descriptor 
		// Redirect output 
		if (i == mypipe->cmdCount - 1){
			// for last command : output can be redirect to output file 
			if (strlen(command__outdirect(mypipe->commands[i])) != 0){
				out_fd = open(command__outdirect(mypipe->commands[i]),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
			}else{
				out_fd = dup(mySdtout);
			}
		}else{
			// not last single command 
			int pipefd[2];
			if (pipe(pipefd) < 0) { 
				perror("Pipe could not be initialized\n"); 
				return 0; // error -> don't execute 
			}
			out_fd=pipefd[1];
			in_fd =pipefd[0];
		}
		// Redirect output 
		dup2(out_fd,1);
		close(out_fd); // close now unused file descriptor 

		// Create child process for single command 
		pids[i] = fork();
		if (pids[i] == -1){
			/* received for parent  process */
			/*fork error printing*/
			perror("fork fails to spawn a child");
			return 0;
		}else if (pids[i] == 0){
			// received for child process 
			// execute if command program is not built in commands 
			if (myCmdHandler(mypipe->commands[i]) == 0){ 
				execvp(command__program(mypipe->commands[i]), command__cmdArgs(mypipe->commands[i]));
				// execvp will not return (unless Args[0] is not a valid executable file)
				fprintf(stderr, "Error: command not found\n");
				exit(1); // child process will not call fork()
			}else{
				exit(mypipe->commands[i]->status);
			}
		}
	} // for 

	// Restore the stdin and stdout for parent process 
	dup2(myStdin,0);
	dup2(mySdtout,1);
	close(myStdin);
	close(mySdtout);

	for (int i=0;i<mypipe->cmdCount;i++){
		waitpid(pids[i],&status[i],0); // wait until all child process compledted 
		// check status for all child process 
		if (WEXITSTATUS(status) == 255){
			// error for command not found when execute child process 
			return 0;
		} // if 
	}//for 

	fprintf(stderr, "+ completed \'%s\' ",user_input);
	for (int i=0;i<mypipe->cmdCount;i++){
		if (WEXITSTATUS(status[i])!= EXIT_SUCCESS){
		fprintf(stderr,"[%d]",mypipe->commands[i]->cmdIndex+1);
		}else{
		fprintf(stderr,"[%d]",WEXITSTATUS(status[i]));
		}
	}
	fprintf(stderr, "\n");
	return 1;
}


