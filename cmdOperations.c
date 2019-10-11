#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmdOperations.h"
// currently assume maximum command line argument is 16
#define Max_ARG 16

// Constructor (without command allocation)
void command__init(Command* self, char *user_input) {
   // removes new line character, mark the end by end of line character
    if (user_input[strlen(user_input)-1] == '\n')
   {
      user_input[strlen(user_input)-1] = '\0';
   }
    strcpy(self->cmd_line,user_input);
    char s[2] = " ";
    self->cmdArgs[0] = strtok(user_input, s);
    int count = 1;
    char* token;
    while (true)
    {
        token = strtok(NULL, s);
        if (token != NULL){
            self->cmdArgs[count] = token;
        } else{
            break;
        }
        count++;
    }
    self->numArgs = count;
 }

 // Allocation + initialization (equivalent to "new Point(x, y)")
Command* Command__create(char *user_input) {
   Command* result = (Command*) malloc(sizeof(Command));
   // This file contains the data structure for command 
   size_t buffersize = 512;
   result->cmd_line = (char *)malloc(buffersize * sizeof(char));
   for (int i = 0; i < Max_ARG; i++)
		{
			result->cmdArgs[i] = (char *)malloc(buffersize * sizeof(char));
		}
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