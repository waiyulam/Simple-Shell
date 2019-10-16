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
#include <unistd.h>
#include "cmdLineOperations.h"
#include "cmdOperations.h"


// currently assume maximum command line argument is 16
#define Max_ARG 16
extern int errno;
// keep track if sshell exit 
bool EXIT = false; 
// Keep track of active jobs 
int activeJobs = 0;
int myCmdHandler(Command *command) ;
int redirection(Command *command,int pipeCount);
void execute (Pipe *mypipe,char *user_input);
int executePipe (Pipe *mypipe,char *user_input);
Pipe* ExecStatus (Pipe* pipeHead);

int main(int argc, char *argv[])
{
	// The head of command line for command line linked list 
	Pipe* pipeHead = NULL;
	
	while (1)
	{	
		if (pipeHead != NULL){
			pipeHead = ExecStatus(pipeHead);
		}
		// Maximum input line size is 512
		int buffersize = 512;
		char* user_input = (char *)malloc(buffersize * sizeof(char));
		printf("sshell$ ");
		fflush(stdout);

		/* Get command line */
		fgets(user_input, buffersize, stdin);
		/* Print command line if we're not getting stdin from the
		 * terminal */
		if (!isatty(STDIN_FILENO)) {
			printf("%s", user_input);
			fflush(stdout);
		}
		if (user_input[0] == '\n' || user_input[0] == '\0'){ 
			continue; 
		} // no command and user press enter 
		// Remove trailing newline from command line

		if (user_input[strlen(user_input)-1] == '\n'){ user_input[strlen(user_input)-1] = '\0'; }

		// Create Pipe data structure : parsing the string and store commands
		char* temp = (char *)malloc(buffersize * sizeof(char)); 
		strcpy(temp,user_input); // To deep copy the user input : avoid pasring change original string
		// fprintf(stderr,"commands : %s\n",user_input);
		
		Pipe* curPipe = pipeHead; // current pipe 
		// allocate new pipe 
		if (pipeHead == NULL){
			pipeHead = Pipe__create(temp);	
			// Check malloc allocation success
			if (pipeHead == NULL)
			{ 
				perror("malloc fails to alloscate memory");
				exit(1);
			}
			if (pipeHead->FAIL){ 
				pipeHead = NULL;
				continue; 
			}
			// Empty string 
			if (pipeHead->cmdCount == 0){ 
				pipeHead = NULL;
				continue; 
			}
			curPipe = pipeHead;
		}else{
			// Find the tail of linked list 
			Pipe* prevPipe = NULL;
			while (curPipe != NULL)
			{
				prevPipe = curPipe;
				curPipe = curPipe->nextPipe;
			}
			// Put new command line into tail of linked list 
			curPipe = Pipe__create(temp);
			// Handle error command line
			// Check if parsing command line are valid for error handling
			// Check malloc allocation success
			if (curPipe == NULL)
			{ 
				perror("malloc fails to alloscate memory");
				exit(1);
			} 
			if (curPipe->FAIL){ 
				curPipe = NULL;
				continue; 
				}
			// Empty string 
			if (curPipe->cmdCount == 0){ 
				curPipe = NULL;
				continue; 
			}
			// linked list : tail is curPipe	
			prevPipe->nextPipe = curPipe; 
		}
		// command line process created and running on the foreground/background
		activeJobs++; 
		// Execute if there is single command 
		if (curPipe->cmdCount == 1){
			// executing .. 
			execute(curPipe,user_input);
			if (EXIT){
				// Exit shell
				return EXIT_SUCCESS;
			}
		}
		// execute multiple commands
		else{
			// executing .. 
			executePipe(curPipe,user_input);
			if (EXIT){
				// Exit shell
				return EXIT_SUCCESS;
			}
    	} // if else 
	} // while loop 

	Pipe__destroy(pipeHead);
	return EXIT_SUCCESS;
} // main 


// Function to execute builtin commands
// Return 1 if command program match builtin cmd , 0 otherwise 
int myCmdHandler(Command *command) 
{ 
    int numBuiltin = 3,switchArg = 0; 
    char* myCmdsList[numBuiltin]; 
    myCmdsList[0] = "cd"; 
	char path[150];
    myCmdsList[1] = "pwd"; 
	char s[100];
    myCmdsList[2] = "exit"; 
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
	case 3: //exit
		if (activeJobs == 1){ // only exit process in foreground and no command running in background 
			fprintf(stderr,"Bye...\n");
			EXIT = true;
			command->status = 0;
		}else{
			fprintf(stderr,"Error: active jobs still running\n");
			command->status = 1;
		}
		return 1; 
    default: 
        break; 
    }
    return 0; 
} 

// Execute single command 
void execute (Pipe *mypipe,char *user_input){
	/* parent process : keep the file descriptor for STDIN and STDOUT for later use */
	int myStdin=dup(0); // tmpin = 3 = STDIN
	int mySdtout=dup(1); // tempout = 4 = STDOUT
	if (strlen(command__indirect(mypipe->cmdHead)) != 0){
		// Check if command need input redirection 
		int in_fd;  // fd for input 
		in_fd = open(command__indirect(mypipe->cmdHead), O_RDONLY);
		dup2(in_fd, 0);
 		close(in_fd);
	}
	// Check if command need output redirection 
	if (strlen(command__outdirect(mypipe->cmdHead)) != 0){
		int out_fd;
		out_fd = open(command__outdirect(mypipe->cmdHead),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
		// change the input stream to fd
		dup2(out_fd, 1);
		close(out_fd);
	}

	// execute if command program is not built in commands 
	if (myCmdHandler(mypipe->cmdHead) == 0){
		int status = 0;
		int pid = fork();
		if (pid == 0){
			execvp(command__program(mypipe->cmdHead), command__cmdArgs(mypipe->cmdHead));
			// execvp will not return (unless Args[0] is not a valid executable file)
			fprintf(stderr, "Error: command not found\n");
			exit(1); 
		} else if (pid == -1) {
			/*fork error printing*/
			perror("fork fails to spawn a child");
			exit(-1);
		}else{
			// Collec child program pid 
			mypipe->cmdHead->pid = pid;
			// parent process, waits for child execution
			if (!mypipe->background){
				waitpid(pid,&status,0);
				mypipe->cmdHead->status = WEXITSTATUS(status);
				mypipe->FINISHED = true;
			}
		}
	}else{
		mypipe->FINISHED = true;
	}

	// Restore the stdin and stdout for parent process 
	dup2(myStdin,0);
	dup2(mySdtout,1);
	close(myStdin);
	close(mySdtout);
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
	
	if (strlen(command__indirect(mypipe->cmdHead)) != 0){
		// Check if command need input redirection 
		in_fd = open(command__indirect(mypipe->cmdHead), O_RDONLY);
	}else{
		// if no command need input redirection : use STDIN -> dup2 later 
		in_fd = dup(myStdin);
	}

	int out_fd; // fd for output 
	Command *node = mypipe->cmdHead;
	for (int i =0; i < mypipe->cmdCount;i++){
		// Redirect input : first command : 0/input file, later command : pipe[0]
		dup2(in_fd, 0);
		close(in_fd); // close now unused file descriptor 
		// Redirect output 
		if (i == mypipe->cmdCount - 1){
			// for last command : output can be redirect to output file 
			if (strlen(command__outdirect(node)) != 0){
				out_fd = open(command__outdirect(node),O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
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
			if (myCmdHandler(node) == 0){ 
				execvp(command__program(node), command__cmdArgs(node));
				// execvp will not return (unless Args[0] is not a valid executable file)
				fprintf(stderr, "Error: command not found\n");
				exit(1); // child process will not call fork()
			}else{
				exit(node->status);
			}
		}else{
			node->pid = pids[i];// keep track of all child process pid 
		}
		node = node->nextCommand;
	} // for 

	// Restore the stdin and stdout for parent process 
	dup2(myStdin,0);
	dup2(mySdtout,1);
	close(myStdin);
	close(mySdtout);

	if (!mypipe->background){
		Command *node = mypipe->cmdHead;
		/* wait until all child process compledted */ 
		for (int i=0;i<mypipe->cmdCount;i++){
			waitpid(pids[i],&status[i],0); 
			node->status = WEXITSTATUS(status[i]);
			// fprintf(stderr, "command %s exit status %d\n",mypipe->commands[i]->program,mypipe->commands[i]->status);
			// check status for all child process 
			node = node->nextCommand;
		}//for
		mypipe->FINISHED = true;
	}
	
	return 1;
}

/* 
	* This function do the following thing 
	* 1. walk though our pipe data structure and Detect any zombie process 
	* 2. Delete node in linked list if it is finished 
	* 3. update command head pointer if necessary 
	* 4. print "+Completed..." 
*/
Pipe* ExecStatus (Pipe* pipeHead){
	int status;
	Pipe *temp = pipeHead;
	Pipe *prevTemp = NULL;
	while(temp){
		// First check if cuurent pipe has all command finished 
		if (!temp->FINISHED){ // only check for command line not finished -> call waitpid twice will change status
			/* WNOHANG:	return immediately if no child has exited
			* If WNOHANG was specified in options and there were no children in a waitable state, then waitid() returns 0
			*/
			int finishedCmd = 0; // counting the number of finished process in current pipe 
			// wait until last command finished
			// Multiple program wiht pipe line 
			Command *cmdTemp = temp->cmdHead;
			for (int i = 0; i< temp->cmdCount;i++){
				if (waitpid(cmdTemp->pid,&status,WNOHANG) != 0){ 
					finishedCmd++;
					cmdTemp->status = WEXITSTATUS(status);
				}
				cmdTemp = cmdTemp->nextCommand;
			}
			if (finishedCmd == temp->cmdCount){
				// All the program has been done 
				temp->FINISHED = true;
			}
		}
		if (temp->FINISHED == true){ 
			activeJobs--;
			fprintf(stderr, "+ completed \'%s\' ",temp->user_input);
			Command *cmdTemp = temp->cmdHead;
			for (int i=0;i< temp->cmdCount;i++){
				fprintf(stderr,"[%d]",cmdTemp->status);
				cmdTemp = cmdTemp->nextCommand;
			}
			fprintf(stderr, "\n");
			if (prevTemp == NULL) { // prevTemp of head is null
				// store the old value of head pointer 
       			Pipe *oldValue = pipeHead;
				// Change head pointer to point to next pipe
				pipeHead = pipeHead->nextPipe;
				// delete memory allocated for the previous head node 
				free(oldValue); 
				prevTemp = NULL;
				temp = temp->nextPipe;
			}else{
				prevTemp->nextPipe = temp->nextPipe;
				temp = temp->nextPipe;
			}	
		}else{
			prevTemp = temp;
			temp = temp->nextPipe;
		}
	}

	return pipeHead;

}


