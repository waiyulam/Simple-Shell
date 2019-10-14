// This data structure is for pipeline that manipulate one or more command 

#include <stdlib.h>
#include <stdio.h>
#include "cmdOperations.h"

#ifndef PIPEOPERATIONS_H_
#define PIPEOPERATIONS_H_

typedef struct {
    // this is command array and each element contains one command line execution
    Command **commands;
    // cmdCount is a variable that keep track of number of command lines 
    int cmdCount;
    // background variable is to tell if the command line is background command 
    bool background;
    // boolean variable indicate if command constructor succeed or fail
    bool FAIL;
} Pipe;

// Constructor (without allocation)
void Pipe__init(Pipe* self, char *user_input) ;

// Allocation + initialization (equivalent to "new Pipe(x, y)")
Pipe* Pipe__create(char *user_input);

// Destructor (without deallocation)
void Pipe__reset(Pipe* self);

// Destructor + deallocation (equivalent to "delete Pipe")
void Pipe__destroy(Pipe* Pipe);


// function for finding pipe 
int parsePipe(Pipe *mypipe, char* str, char** strpiped) ;



#endif /* PIPEOPERATIONS_H_ */