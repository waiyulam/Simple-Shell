// This file contains the data structure for command 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "pipeOperations.h"
#include "cmdOperations.h"

// currently assume maximum command line argument is 16
#define Max_ARG 16


// Constructor (without allocation)
void Pipe__init(Pipe* self, char *user_input) {
    char *tempArgs[512];
    // parse the user_input to multiple command 
    self->cmdCount = parsePipe(user_input,tempArgs);

    char **pipes = (char **)malloc(sizeof(char*) * self->cmdCount );
    for (int i = 0; i < self->cmdCount ; i++){
         pipes[i] = (char *)malloc(512 * sizeof(char));
		   strcpy(pipes[i], tempArgs[i]);
         //printf("%s\n",pipes[i]);
		}

    self->commands = (Command **)malloc(sizeof(Command*) * self->cmdCount);
    for (int i=0; i < self->cmdCount;i++){
		self->commands[i] = Command__create(pipes[i]);
    }
 }

// Allocation + initialization (equivalent to "new Pipe(x, y)")
Pipe* Pipe__create(char *user_input) {
   Pipe* result = (Pipe*) malloc(sizeof(Pipe));
   Pipe__init(result,user_input);
   return result;
}

// Destructor (without deallocation)
void Pipe__reset(Pipe* self) {
}

// Destructor + deallocation (equivalent to "delete Pipe")
void Pipe__destroy(Pipe* Pipe) {
  if (Pipe) {
     Pipe__reset(Pipe);
     free(Pipe);
  }
}

// function for finding pipe 
int parsePipe(char* str, char** strpiped) 
{ 
    int pipeCount = 0;
    char* token;
    // save pointer 
    char* savePipe = NULL;
    token = strtok_r(str, "|", &savePipe);
    if (token == NULL){
        return 0; // returns zero if no command is found. 
    }

    while (token != NULL)
     { 
        strpiped[pipeCount] = (char *)malloc(512 * sizeof(char));
        strpiped[pipeCount] = token;
        pipeCount++;
        token = strtok_r(NULL, "|", &savePipe);
     }

    return pipeCount; 
} 
  

