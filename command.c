// This file contains the data structure for command 
#define Max_ARG 16
size_t buffersize = 512;

struct Command {
    // cmd_line is a string variable that stores command line input 
    char *cmd_line;
    // cmdArgs is a string array that stores parsing result of command line input 
    char *cmdArgs[Max_ARG];
    // numArgs keep track of the number of arguments in command line input 
    int numArgs;
}

// Constructor (without command allocation)
void command__init(Command* self, char *user_input) {
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
   Command* result = (Command*) malloc(sizeof(Point));
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