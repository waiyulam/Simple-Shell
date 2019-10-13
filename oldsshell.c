/* This is simple shell for executing single or two command */

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
int myCmdHandler(char** args) ;
int redirection(Command *command,int pipeCount);

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
		char* temp = (char *)malloc(buffersize * sizeof(char));
		strcpy(temp,user_input);
		if (user_input[strlen(user_input)-1] == '\n')
   		{
     		user_input[strlen(user_input)-1] = '\0';
   		}
		Pipe* myPipe = Pipe__create(temp);
		// for (int i=0; i < myPipe->cmdCount;i++){
     	// 	for (int j =0; j < myPipe->commands[i]->numArgs;j++){
        // 	fprintf(stderr,"%s ",myPipe->commands[i]->cmdArgs[j]);
      	// 	}		
		// 	fprintf(stderr,"\n");
   		// }

		// Check malloc allocation success
		if (myPipe == NULL)
		{
			perror("malloc fails to alloscate memory");
			exit(1);
		}

		// empty string 
		if (myPipe->cmdCount == 0){
			continue;
		}

		// Check if the commands are valid 
		int valid_command = 1;
		for (int i=0; i < myPipe->cmdCount;i++){
			if (command__Fail(myPipe->commands[i])){
				valid_command = 0;
				break;
			}
		}

		if (valid_command == 0){
			continue; // error 
		}

		// Execute if there is single command 
		if (myPipe->cmdCount == 1){
			Command *command = myPipe->commands[0];
			// exit the shell
			if (strcmp(command__program(command), "exit") == 0){
				fprintf(stderr, "Bye...\n");
				exit(0);
			}
			int status = 0;
			int pid = fork();
			if (pid == 0){
				/* Child process, use execvp to execute command on env variable*/
				// check if command has redirection flags
				int redirect = redirection(command,myPipe->cmdCount); 
				if (redirect == 0){
					exit(-1); // error exist and command not execute 
				}
				// execute if command program is not built in commands 
				if (myCmdHandler(command__cmdArgs(command)) == 0){ 
					execvp(command__program(command), command__cmdArgs(command));
					// execvp will not return (unless Args[0] is not a valid executable file)
					fprintf(stderr, "Error: command not found\n");
					exit(-1); 
				}
			} else if (pid == -1) {
				/*fork error printing*/
				perror("fork fails to spawn a child");
				exit(-1);
			} else {
				// parent process, waits for child execution
				wait(&status);
				if (strcmp(command__program(command), "cd")==0)
				{
					char path[150];
					strcpy(path,command__cmdArgs(command)[1]);
					chdir(path);
				}
			}
				// fprintf(stderr,"status: %d\n",WEXITSTATUS(status));
				if (WEXITSTATUS(status) != 255){
					fprintf(stderr, "+ completed \'%s\' [%d]\n",command__cmd_line(command),WEXITSTATUS(status));
				}
				Pipe__destroy(myPipe);
		}else{
			/* execute multiple commands */
			// Exit if any commands has exit flag 
			for (int i=0; i < myPipe->cmdCount;i++){
				if (strcmp(command__program(myPipe->commands[i]), "exit") == 0){
					fprintf(stderr, "Bye...\n");
					exit(0);
				}
			}
			Command *command1 = myPipe->commands[0];
			Command *command2 = myPipe->commands[1];
			int p1_status = 0;
			int p2_status = 0;
			// 0 is read end, 1 is write end 
			int pipefd[2];  
			int p1, p2; 
			if (pipe(pipefd) < 0) { 
        		perror("Pipe could not be initialized\n"); 
        		exit(-1);
			}
			p1 = fork();
			if (p1 == -1){
				/* children process */
				/*fork error printing*/
				perror("fork fails to spawn a child");
				exit(-1);
			}else if (p1 == 0){
				/* Child 1 , use execvp to execute command on env variable*/
				// Don't need read access to pipe 
				close(pipefd[0]); 
				// Replace stdout with the pipe 
        		dup2(pipefd[1], STDOUT_FILENO); 
				// close now unused file descriptor 
        		close(pipefd[1]);
				// check if command has redirection flags
				int redirect = redirection(command1,myPipe->cmdCount); 
				if (redirect == 0){
					exit(-1); // error exist and command not execute 
				}
				// execute if command program is not built in commands 
				if (myCmdHandler(command__cmdArgs(command1)) == 0){ 
					execvp(command__program(command1), command__cmdArgs(command1));
					// execvp will not return (unless Args[0] is not a valid executable file)
					fprintf(stderr, "Error: command not found\n");
					exit(-1);
				}
			}else{
				/* parent process */
				p2 = fork(); 
				if (p2 == -1){
					/* children process */
					/*fork error printing*/
					perror("fork fails to spawn a child");
					exit(-1);
				}else if (p2 ==0){
					// Don't need write access to pipe 
					close(pipefd[1]); 
					// Replace stdin with the pipe 
            		dup2(pipefd[0], STDIN_FILENO); 
					// close now unused file descriptor 
            		close(pipefd[0]); 
					// check if command has redirection flags
					int redirect = redirection(command2,myPipe->cmdCount); 
					if (redirect == 0){
						exit(-1); // error exist and command not execute 
					}
					// execute if command program is not built in commands 
					if (myCmdHandler(command__cmdArgs(command2)) == 0){ 
						execvp(command__program(command2), command__cmdArgs(command2));
						// This is not the error handle for command not found
						// execvp will not return (unless Args[0] is not a valid executable file)
						fprintf(stderr, "Error: command not found\n");
						exit(-1);
					}
				}else{
						// wait for child 1 completed 
						// parent executing, waiting for two children 
						waitpid(p1,&p1_status,0);
            			waitpid(p2,&p2_status,0);
					}
				}
				if (WEXITSTATUS(p1_status) != 255 && WEXITSTATUS(p2_status) != 255){
					fprintf(stderr, "+ completed \'%s\' ",user_input);
					if (WEXITSTATUS(p1_status)!= EXIT_SUCCESS){
						fprintf(stderr,"[%d]",command1->cmdIndex+1);
					}else{
						fprintf(stderr,"[%d]",WEXITSTATUS(p1_status));
					}
					if (WEXITSTATUS(p2_status)!= EXIT_SUCCESS){
						fprintf(stderr,"[%d]",command2->cmdIndex+1);
					}else{
						fprintf(stderr,"[%d]",WEXITSTATUS(p2_status));
					}
					fprintf(stderr, "\n");
				}
				Pipe__destroy(myPipe);
    		} 
		}	
}


// Function to execute builtin commands
// Return 1 if command program match builtin cmd , 0 otherwise 
int myCmdHandler(char** args) 
{ 
    int numBuiltin = 2,switchArg = 0; 
    char* myCmdsList[numBuiltin]; 

    myCmdsList[0] = "cd"; 
	char path[150];
    myCmdsList[1] = "pwd"; 
	char s[100];

    for (int i = 0; i < numBuiltin; i++) { 
        if (strcmp(args[0], myCmdsList[i]) == 0) { 
            switchArg = i + 1; 
            break; 
        } 
    } 
    switch (switchArg) { 
    case 1: //cd 
		strcpy(path,args[1]);
		if (chdir(path) == 0) {
			exit(0);
		} else {
			fprintf(stderr, "Error: no such directory\n");
			exit(1);
		}
        return 1; 
    case 2: // pwd
		fprintf(stderr, "%s\n", getcwd(s, 100));
		exit(0);
        return 1; 
    default: 
        break; 
    } 
    return 0; 
} 

// Redirection 
// return 1 if redirection succeed or redirection not exist, 0 otherwise 
int redirection(Command *command,int pipeCount){
	// Check if command need input redirection 
	if (strlen(command__indirect(command)) != 0){
		// In a pipeline of commands, only the first command can have its input redirected
		if (pipeCount > 1 && command->cmdIndex != 0 ){
			fprintf(stderr, "Error: mislocated input redirection\n");
			return 0;
		}
		int fd;
		// fprintf(stderr, "Input direct file %s\n",command__indirect(command));
		fd = open(command__indirect(command), O_RDONLY);
		// change the input stream to fd
		dup2(fd, 0);
		close(fd);
	}

	// Check if command need output redirection 
	if (strlen(command__outdirect(command)) != 0){
		// In a pipeline of commands, only the last command can have its output redirected
		if (pipeCount > 1 && command->cmdIndex != (pipeCount-1) ){
			fprintf(stderr, "Error: mislocated output redirection\n");
			return 0;
		}
		int fd;
		// fprintf(stderr, "Output direct file %s\n",command__outdirect(command));
		fd = open(command__outdirect(command),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
		// change the input stream to fd
		dup2(fd, 1);
		close(fd);
	}

	return 1;
}


