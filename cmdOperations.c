// This file contains the data structure for command 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "cmdOperations.h"
// currently assume maximum command line argument is 16
#define Max_ARG 16

// Constructor (without command allocation)
void command__init(Command* self, char *user_input) {
      // allocate memory for command fields
      size_t buffersize = 512;
      self->cmd_line = (char *)malloc(buffersize * sizeof(char));
      self->program = (char *)malloc(buffersize * sizeof(char));
      self->in_redirect = (char *)malloc(buffersize * sizeof(char));
      self->out_redirect = (char *)malloc(buffersize * sizeof(char));
      self->FAIL = false;
      self->status = 0;
      self->nextCommand = NULL;
      
      char *tempArgs[Max_ARG];
      for (int i = 0; i < Max_ARG; i++)
		{
			tempArgs[i] = (char *)malloc(buffersize * sizeof(char));
		}
    // Initialize the cmd_line 
    strcpy(self->cmd_line,user_input);
    // Parse command line input to arguments 
    char s[2] = " ";
    // store the number of arguments
    int argsCount = 0;
    int parseCount = 0;
    char* token;
    // save pointer 
    char* saveSpace = NULL;
    // https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
    // Different strings may be parsed concurrently using sequences of calls to strtok_r() that specify different saveptr arguments.
    // Don't use strtok : not thread safe 
    token = strtok_r(user_input, s, &saveSpace);
    if (token != NULL){
        parseCount++;
    }else{
      fprintf(stderr, "Error: missing command\n");
      self->FAIL = true;
      return;
    }
    while (token != NULL)
    { 
      /* Parsing the current command line */

      // Error : too many arguments 
      if (parseCount > 16){
         fprintf(stderr, "Error: too many process arguments\n");
         self->FAIL = true;
         break;
      }      

      // Find current token contains output redirection files 
      if (strchr(token, '>')!=NULL){
         char* temp;
         char* saveOut = NULL;
         // example :  world!>temp.txt
         if (token[0] != '>'){
            temp = strtok_r(token,">",&saveOut);
            tempArgs[argsCount] = temp; 
            argsCount++;
            temp = strtok_r(NULL,">",&saveOut);
         }else{
            // example: > temp.txt
            temp = strtok_r(token,">",&saveOut);
         }
         if (temp == NULL){
            token = strtok_r(NULL,s,&saveSpace);
            if (token != NULL){
               parseCount++;
            }
            if (token == NULL){
               // example: echo gogo > 
               fprintf(stderr, "Error: no output file\n");
               self->FAIL = true;
               break;
            }else{
               // example : echo gogo > temp.txt
               strcpy(self->out_redirect,token);
               //   printf("output file:  %s\n",token);
               int fd;
               // fprintf(stderr, "Output direct file %s\n",command__outdirect(command));
               fd = open(self->out_redirect,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
               // check if the file user inputted can not be opened
               if (fd < 0) {
                   fprintf(stderr, "Error: cannot open output file\n");
                   self->FAIL = true; // error -> continue to prompt user for new command line 
               }
               close(fd);
            }
         }else{
            // example: >temp.txt
            strcpy(self->out_redirect,temp);
           // printf("output file:  %s\n",temp);
            int fd;
            // fprintf(stderr, "Output direct file %s\n",command__outdirect(command));
            fd = open(self->out_redirect,O_WRONLY|O_CREAT|O_TRUNC);
            // check if the file user inputted can not be opened
            if (fd < 0) {
               fprintf(stderr, "Error: cannot open output file\n");
                  self->FAIL = true; // error -> continue to prompt user for new command line 
            }
            close(fd);
         }
      }
      // Find current token contains input redirection files 
      else if (strchr(token, '<')!=NULL){
         char* temp;
         char* saveIn = NULL;
         // example :  world!<temp.txt
         if (token[0] != '<'){
            temp = strtok_r(token,"<",&saveIn);
            tempArgs[argsCount] = temp; 
            argsCount++;
            temp = strtok_r(NULL,"<",&saveIn);
         }else{
            // example: < temp.txt
            temp = strtok_r(token,"<",&saveIn);
         }
         if (temp == NULL){
            token = strtok_r(NULL,s,&saveSpace);
            if (token != NULL){
               parseCount++;
            }
            if (token == NULL){
               // example: echo gogo < 
               fprintf(stderr, "Error: no input file\n");
               self->FAIL = true;
               break;
            }else{
               // example : echo gogo < temp.txt
               strcpy(self->in_redirect,token);
               int fd;
				   fd = open(self->in_redirect, O_RDONLY);
               // check if the file user inputted can not be opened
				   if (fd < 0) {
					   fprintf(stderr, "Error: cannot open input file\n");
					   self->FAIL = true; // error -> continue to prompt user for new command line 
				   }
               close(fd);
               //printf("input file:  %s\n",token);
            }
         }else{
            // example: <temp.txt
            strcpy(self->in_redirect,temp);
            int fd;
            fd = open(self->in_redirect, O_RDONLY);
            // check if the file user inputted can not be opened
            if (fd < 0) {
               fprintf(stderr, "Error: cannot open input file\n");
               self->FAIL = true; // error -> continue to prompt user for new command line 
            }
            close(fd);
            //printf("input file:  %s\n",temp);
         }
      }else{
        tempArgs[argsCount] = token;
        // printf("%s\n",token);
        argsCount++;
      }
      token = strtok_r(NULL, s,&saveSpace);
      if (token != NULL){
        parseCount++;
      }
    }

    // Initialize program name
    strcpy(self->program,tempArgs[0]);
    // Initialize numArgs
    self->numArgs = argsCount;

    // Initialize cmdArgs: argsCount + 1 for last null arguments 
    self->cmdArgs = (char **)malloc(sizeof(char*) * (argsCount+1));
    for (int i = 0; i < argsCount; i++){
         self->cmdArgs[i] = (char *)malloc(buffersize * sizeof(char));
			strcpy(self->cmdArgs[i], tempArgs[i]);
        // printf("%s\n",self->cmdArgs[i]);
		}
	 self->cmdArgs[argsCount] = NULL;
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

// Equivalent to "command::indirect()" in C++ version
char* command__indirect(Command* self) {
   return self->in_redirect;
}

// Equivalent to "command::outdirect()" in C++ version
char* command__outdirect(Command* self) {
   return self->out_redirect;
}

bool command__Fail(Command* self){
   return self->FAIL;
}