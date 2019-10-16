# Simple-Shell
## goal
The goal of this project is to understand important UNIX system calls by implementing a simple shell called sshell. A shell is a command-line interpreter: it accepts input from the user under the form of command lines and executes them.

## High level idea of the program design:
When user inputs command, we first decide if user has input or not. If user input <enter>, removing tailing newline from command line. When user has valid input, we use function executeStatus to manage active processes and delete zombie processes, printing complete statement when entire processes in the linked list are done. If we don't have any other processes, current command is the head of processes' linked list. If we have other active processes, we add current command into the tail of the linked list and build command as pipe structure. Then in current process, we decide if command line is a pipe structure or not. If it is a pipe structure, we use function executePige to execute it and if it is a single command, we use our execute function to execute it. Specific implementation details about executePige,execute,ExecStatus and myCmdHandler will be discussed in the module <implementation details> with respect to each phase. 
  

## Implementation details:
Regarding to the phase 1, we handle the redirecting error output using printf(stderr, ...) and input &status pointer into wait() in order to obtain the exit value for the child process. 


Regarding to phase 2 and 3, when dealing with commands that read from the input and arguments that user inputs with specific program, our group designed a Command data struture in order to handle command parsing and store values for the single command. In the command struct, we store command line input, all the comand arguments, number of arguments, specific program name, in_direct file name, out_direct name, FAIL variable indicating command constructor succeed or fail, PID for command as well as exit status of the command. Overall goal of this design is trying to abstract "features" out of single command, using encapulation to reduce redundent code while tring to obtain states and results of commands. Command's parsing implementation is inside method command_init of command struct. In method command_init, we parse arguments individually into tempArgs and store command name into field called program. Most importantly, it reduces more complicated user command line which we encountered in phase 6 and 7 down to the single command problem. As it will detect "<" and ">" when parsing command line and store input or output file into field, in_direct or out_direct, opening the appropriate files. Later when implementing phase 6 and 7, we can simply replace stdin and stdout with files indicated by those two files. As discussed in high level idea, specific execution happens inside execute function. If it is not built in, it will call execvp running in the child process. While errors messages mostly occurred when parsing commands, if it happened when building command struct, struct will not be constructed and print error message. if it happened when executing, process will exit with 1 and print error messages.


Regaring to phase 4, we use function myCmdHandler to handle 3 builtin function's implementations. Inside of this function, we use switch statements to choose different functionalities.


Regarding to phase 5 and 6, as described in struct command, file names will be stored inside of either in_direct or out_direct field of command. In execute function, we use dup2() to replace stdin and stdout with in_direct or out_direct filenames, then executing as phase 2 and 3.


Regarding to phase 7, we designed another data structure called pipe using struct. Pipe, while building its structure, it parses user input using strtok, storing each command in the command array. During the parsing, it will also check whether the command line is background. Inside of executePipe function, it assembles execute function, instead it loops through the command array. Replacing stdin or out with infile or outfile, it uses execvp to execute non-built in and myCmdHandler for the built-in. 

Regarding to phase 8, 

### Citation details
1. Different strings can be parsed concurrently using sequences of calls to strtok_r() that specify different saveptr arguments. The reason that we did not use strtok when parsing is not thread safe.
https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
2. Understand how strtok works and effects when put string input as its parameters.  
https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
