// This file contains the data structure for command 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmdOperations.h"
// currently assume maximum command line argument is 16
#define Max_ARG 16

// Constructor (without command allocation)
void command__init(Command* self, char *user_input) {
      // allocate memory for command fields
      size_t buffersize = 512;
      self->cmd_line = (char *)malloc(buffersize * sizeof(char));
      self->program = (char *)malloc(buffersize * sizeof(char));
      char *tempArgs[Max_ARG];
      for (int i = 0; i < Max_ARG; i++)
		{
			tempArgs[i] = (char *)malloc(buffersize * sizeof(char));
		}

   // removes new line character, mark the end by end of line character
    if (user_input[strlen(user_input)-1] == '\n')
   {
      user_input[strlen(user_input)-1] = '\0';
   }
    // Initialize the cmd_line 
    strcpy(self->cmd_line,user_input);
    // Parse command line input to arguments 
    char s[2] = " ";
    tempArgs[0] = strtok(user_input, s);
    strcpy(self->program,tempArgs[0]);
    int count = 1;
    char* token;
    while (true)
    {
        token = strtok(NULL, s);
        if (token != NULL){
            tempArgs[count] = token;
        } else{
            break;
        }
        count++;
    }
    // Initialize numArgs
    self->numArgs = count;
    // Initialize cmdArgs: count + 1 for last null arguments 
    self->cmdArgs = (char **)malloc(sizeof(char*) * (count+1));
    for (int i = 0; i < count; i++){
         self->cmdArgs[i] = (char *)malloc(buffersize * sizeof(char));
			strcpy(self->cmdArgs[i], tempArgs[i]);
		}
		self->cmdArgs[count] = NULL;
 }

 // Allocation + initialization (equivalent to "new Point(x, y)")
Command* Command__create(char *user_input) {
   Command* result = (Command*) malloc(sizeof(Command));
   command__init(result,user_input);
   return result;
}

// Destructor (without deallocation)
void Command__reset(Command* self) {
}

// Destructor + deallocation (equivalent to "delete Command")
void Command__destroy(Command* command) {
  if (command) {
     Command__reset(command);
     free(command);
  }
}

// Equivalent to "command::cmd_line()" in C++ version
char* command__cmd_line(Command* self) {
   return self->cmd_line;
}

// Equivalent to "command::cmdArgs()" in C++ version
char** command__cmdArgs(Command* self) {
   return self->cmdArgs;
}

// Equivalent to "command::numArgs()" in C++ version
int command__numArgs(Command* self) {
   return self->numArgs;
}

// Equivalent to "command::program()" in C++ version
char* command__program(Command* self) {
   return self->program;
}