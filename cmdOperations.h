#include <stdlib.h>
#include <stdio.h>

#ifndef CMDOPERATIONS_H_
#define CMDOPERATIONS_H_

typedef struct {
    // cmd_line is a string variable that stores command line input 
    char *cmd_line;
    // cmdArgs is a string array that stores parsing result of command line input
    // assume we have 16 maximum arguments  
    char **cmdArgs;
    // numArgs keep track of the number of arguments in command line input 
    int numArgs;
    // program name 
    char *program;
    // input redirect file name 
    char *in_redirect;
    // output redirect file name 
    char *out_redirect;
} Command;

// Constructor (without command allocation)
void command__init(Command* self, char *user_input);

// Allocation + initialization (equivalent to "new Point(x, y)")
Command* Command__create(char *user_input) ;

// Destructor (without deallocation)
void Command__reset(Command* self);

// Destructor + deallocation (equivalent to "delete Command")
void Command__destroy(Command* command);

// Equivalent to "command::cmd_line()" in C++ version
char* command__cmd_line(Command* self);

// Equivalent to "command::cmdArgs()" in C++ version
char** command__cmdArgs(Command* self);

// Equivalent to "command::numArgs()" in C++ version
int command__numArgs(Command* self);

char* command__program(Command* self);




#endif /* CMDOPERATIONS_H_ */