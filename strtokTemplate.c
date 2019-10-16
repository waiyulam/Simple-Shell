// A C/C++ program for splitting a string 
// using strtok() 
#include <stdio.h> 
#include <string.h> 
  
int main() 
{ 
    char str[] = "ls |      "; 
    char* saveStr = NULL;

    // Returns first token 
    char* token = strtok_r(str, "|",&saveStr); 
  
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL) { 
        printf("token: %s\n", token); 
                if (saveStr == NULL){
            printf("pass %s\n", saveStr); 
        }
        token = strtok_r(NULL, "|",&saveStr); 
    } 
  
    return 0; 
} 