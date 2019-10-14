// This file contains the data structure for command 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "cmdLineOperations.h"
#include "cmdOperations.h"

// currently assume maximum command line argument is 16
#define Max_ARG 16


// Constructor (without allocation)
void Pipe__init(Pipe* self, char *user_input) {
    char *tempArgs[512];
    self->background = false;
    self->nextPipe = NULL;
    self->user_input = (char *)malloc(512 * sizeof(char)); 
    strcpy(self->user_input,user_input);
    // parse the user_input to multiple command 
    if (parsePipe(self,user_input,tempArgs)){
      self->FAIL = false;
      self->FINISHED = false;
      char **pipes = (char **)malloc(sizeof(char*) * self->cmdCount );
      for (int i = 0; i < self->cmdCount ; i++){
            pipes[i] = (char *)malloc(512 * sizeof(char));
            strcpy(pipes[i], tempArgs[i]);
            //printf("%s\n",pipes[i]);
         }

      self->commands = (Command **)malloc(sizeof(Command*) * self->cmdCount);
      for (int i=0; i < self->cmdCount;i++){
         self->commands[i] = Command__create(pipes[i]);
         self->commands[i]->cmdIndex = i;
         if (command__Fail(self->commands[i])){
				self->FAIL = true;
            return;
			}
      }
    }else{
       // mislocated background 
       self->FAIL = true;
    }
 }

// Allocation + initialization (equivalent to "new Pipe(x, y)")
Pipe* Pipe__create(char *user_input) {
   Pipe* result = (Pipe*) malloc(sizeof(Pipe));
   Pipe__init(result,user_input);
   return result;
}

// Destructor + deallocation (equivalent to "delete Pipe")
void Pipe__destroy(Pipe* head) {
   Pipe* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->nextPipe;
       free(tmp);
    }
}

// function for finding pipe 
int parsePipe(Pipe *mypipe, char* str, char** strpiped) 
{ 
   // Find background command flag 
   char *background;
   int index;
   background = strchr(str, '&');
   if (background != NULL){
      index = (int)(background - str);
      // The background sign may only appear as the last token of a command line.
      if (index == (strlen(str)-1)){
         str = strtok(str,"&");
         mypipe->background = true;
      }else{
         //Error: mislocated background sign
         fprintf(stderr,"Error: mislocated background sign\n");
         return 0;
      }
   }

   // Parsing command line with "|"
   if (str == NULL){
		fprintf(stderr,"Error: missing command\n");
      return 0;
	}
   char *temp_str = (char *)malloc(strlen(str) * sizeof(char));
   strcpy(temp_str,str);

   int pipeCount = 0;
   char* token;
   // save pointer 
   char* savePipe = NULL;
   token = strtok_r(str, "|", &savePipe);
   if (token == NULL){
      fprintf(stderr,"Error: missing command\n");
      return 0;
   }
   while (token != NULL)
   { 
      // fprintf(stderr,"token: %s\n",token);
      strpiped[pipeCount] = (char *)malloc(512 * sizeof(char));
      strpiped[pipeCount] = token;
      pipeCount++;
      token = strtok_r(NULL, "|", &savePipe);
   }

   int temp = 0; // count the occurence of "|" in command line 
   for (temp=0; temp_str[temp]; temp_str[temp]=='|' ? temp++ : *temp_str++);
   if (pipeCount < temp+1){
      fprintf(stderr,"Error: missing command\n");
      return 0;
   }
   mypipe->cmdCount = pipeCount;
   return 1; 
} 
  

