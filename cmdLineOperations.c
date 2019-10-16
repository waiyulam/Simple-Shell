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
    self->FINISHED = false;
    self->nextPipe = NULL;
    self->user_input = (char *)malloc(512 * sizeof(char)); 
    self->cmdHead = NULL;

    strcpy(self->user_input,user_input);
    // parse the user_input to multiple command 
    if (parsePipe(self,user_input,tempArgs)){
      self->FAIL = false;
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
   if (str == NULL){
		fprintf(stderr,"Error: missing command\n");
      return 0;
	}
   // temp_str is for error handling of pipe sign and number of commands matching 
   // e.g. pwd | pwd | 
   char *temp_str = (char *)malloc(strlen(str) * sizeof(char));
   strcpy(temp_str,str);
   // Parsing command line with "|"
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
      // tempstore string is for parsing single command line with "&"
      char* tempstore = (char *)malloc(512 * sizeof(char));
      strcpy(tempstore,token);
      char* subtoken;
      char* saveBackground = NULL;
      //missing command : e.g. & | ls 
      if (tempstore[0] == '&'){
         fprintf(stderr,"Error: missing command\n");
         return 0;
      }
      // subtoken now is the single command line to create comamnd 
      // e.g : ls & | ls
      subtoken = strtok_r(tempstore,"&",&saveBackground);
      // Create new single command 
      Command* curCmd = mypipe->cmdHead;
      if (mypipe->cmdHead == NULL){
         mypipe->cmdHead = Command__create(subtoken);
         if (mypipe->cmdHead == NULL)
         { 
            perror("malloc fails to alloscate memory");
            exit(1);
         }
         if (mypipe->cmdHead->FAIL){ 
            return 0; 
         }
         curCmd = mypipe->cmdHead;
      }else{
         while (curCmd->nextCommand != NULL)
         {
            curCmd = curCmd->nextCommand;
         }
         // Put new command into tail of linked list 
         curCmd->nextCommand = Command__create(subtoken);
         if (curCmd->nextCommand == NULL)
         { 
            perror("malloc fails to alloscate memory");
            exit(1);
         } 
         if (curCmd->nextCommand->FAIL){ 
            return 0;
         }
         curCmd = curCmd->nextCommand;
      }
      // Increment one single command in mypipe 
      pipeCount++;
      // mislocated input redirection 
      if (pipeCount != 1 && strlen(curCmd->in_redirect) != 0){
         fprintf(stderr,"Error: mislocated input redirection\n");
			return 0; 
      }else if(pipeCount == 1 && strlen(curCmd->in_redirect) != 0){
         // cannot open input file
         int fd;
         fd = open(curCmd->in_redirect, O_RDONLY);
         // check if the file user inputted can not be opened
         if (fd < 0) {
            fprintf(stderr, "Error: cannot open input file\n");
            close(fd);
            return 0; // error -> continue to prompt user for new command line 
         }
         close(fd);
      }
      // keep the previous command 
      char* prev_token = (char *)malloc(512 * sizeof(char));
      strcpy(prev_token,token);
      token = strtok_r(NULL, "|", &savePipe);
      // savePipe is the unchanged string from previous call 
      // if temp == NULL : this is last command 
      if (token != NULL){ 
         // mislocated output redirection 
         if (strlen(curCmd->out_redirect) != 0){
            fprintf(stderr,"Error: mislocated output redirection\n");
				return 0; 
         }
         // mislocated background redirection 
         char *background;
         background = strchr(prev_token, '&');
         if (background != NULL){
            fprintf(stderr,"Error: mislocated background sign\n");
            return 0;
         }
      }else{
         // check output open error 
         if (strlen(curCmd->out_redirect) != 0){
            int fd;
            // fprintf(stderr, "Output direct file %s\n",command__outdirect(command));
            fd = open(curCmd->out_redirect,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
            // check if the file user inputted can not be opened
            if (fd < 0) {
                  fprintf(stderr, "Error: cannot open output file\n");
                  close(fd);
                  return 0; // error -> continue to prompt user for new command line 
            }
            close(fd);
         }
         // The background sign may only appear as the last token of a command line.
         char *background;
         background = strchr(prev_token, '&');
         if (background != NULL){
            int index = (int)(background - prev_token);
            // "&" has string on left hand side 
            if (index == (strlen(prev_token)-1)){
               mypipe->background = true;
            }else{
               //Error: mislocated background sign
               fprintf(stderr,"Error: mislocated background sign\n");
               return 0;
            }
         } 
      }

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
  

