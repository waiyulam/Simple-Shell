// A C/C++ program for splitting a string 
// using strtok() 
#include <stdio.h> 
#include <string.h> 
  
int main() 
{ 
    char str[] = "pwd |"; 
  
    // Returns first token 
    char* token = strtok(str, "|"); 
  
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL) { 
        printf("pass %s\n", token); 
        token = strtok(NULL, "|"); 
    } 
  
    return 0; 
} 