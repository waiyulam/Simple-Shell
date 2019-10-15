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
    self->cmdHead = NULL;

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

      Command* curCmd = self->cmdHead;
      for (int i=0; i < self->cmdCount;i++){
         if (self->cmdHead == NULL){
            self->cmdHead = Command__create(pipes[i]);
            if (self->cmdHead == NULL)
            { 
               perror("malloc fails to alloscate memory");
               exit(1);
            }
            if (self->cmdHead->FAIL){ 
               self->FAIL = true;
               return; 
            }
            curCmd = self->cmdHead;
         }else{
            while (curCmd->nextCommand != NULL)
			   {
				   curCmd = curCmd->nextCommand;
			   }
            // Put new command into tail of linked list 
			   curCmd->nextCommand = Command__create(pipes[i]);
            if (curCmd->nextCommand == NULL)
            { 
               perror("malloc fails to alloscate memory");
               exit(1);
            } 
            if (curCmd->nextCommand->FAIL){ 
               self->FAIL = true;
               return;
            }
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
// Find parsing error from left to right and output the leftmost error 
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
   // missing command : e.g.    | pwd 
   if (str[0] == '|'){
      fprintf(stderr,"Error: missing command\n");
      return 0;
   }
   token = strtok_r(str, "|", &savePipe);
   while (token != NULL)
   { 
      // fprintf(stderr,"token: %s\n",token);
      strpiped[pipeCount] = (char *)malloc(512 * sizeof(char));
      strpiped[pipeCount] = token;
      pipeCount++;
      token = strtok_r(NULL, "|", &savePipe);
   }

   // missing command error handling : "pwd | pwd | "
   int temp = 0; 
   for (temp=0; temp_str[temp]; temp_str[temp]=='|' ? temp++ : *temp_str++);
   if (pipeCount < temp+1){
      fprintf(stderr,"Error: missing command\n");
      return 0;
   }

   mypipe->cmdCount = pipeCount;
   
   return 1; 
} 
  

