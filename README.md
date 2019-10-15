# Simple-Shell
### goal
The goal of this project is to understand important UNIX system calls by implementing a simple shell called sshell. A shell is
a command-line interpreter: it accepts input from the user under the form of command lines and executes them.
### High level idea of the program design:

### Implementation details:
Regarding to the phase 1, we handle the redirecting error output using printf(stderr, ...) and input &status pointer into wait() in order to obtain the exit value for the child process. 


Regarding to phase 2 and 3, when dealing with commands that read from the input and arguments that user inputs with specific program, our group designed a Command data struture in order to handle command parsing and store values for the single command. In the command struct, we store command line input, all the comand arguments, number of arguments, specific program name, in_direct file name, out_direct name, FAIL variable indicating command constructor succeed or fail, PID for command as well as exit status of the command. Overall goal of this design is trying to abstract "features" out of single command, using encapulation to reduce redundent code while tring to obtain states and results of commands. Command's parsing implementation is inside method command_init of command struct. In method command_init, we parse arguments individually into tempArgs and store command name into field called program. Most importantly, it reduces more complicated user command line which we encountered in phase 6 and 7 down to the single command problem. As it will detect "<" and ">" when parsing command line and store input or output file into field, in_direct or out_direct, opening the appropriate files. Later when implementing phase 6 and 7, we can simply replace stdin and stdout with files indicated by those two files. Specific execution happens inside execute function. While errors messages mostly occurred when parsing commands, if it happened when building command struct, struct will not be constructed and print error message. if it happened when executing, process will exit with 1 and print error messages.


Regaring to phase 4, we use function myCmdHandler to handle 3 builtin function's implementations. Inside of this function, we use switch statements to choose different functionalities.


Regarding to phase 5 and 6, as described in struct command, file names will be stored inside of either in_direct or out_direct field of command. In execute function, we use dup2() to replace stdin and stdout with in_direct or out_direct filenames, then executing as phase 2 and 3.


Regarding to phase 7 and 8, we designed another data structure called pipe using struct. Pipe stores the user input for command line, commands array where each elements contains one command line. cmdCount keeps track of number of command lines. 

### Citation details
1. Different strings can be parsed concurrently using sequences of calls to strtok_r() that specify different saveptr arguments. The reason that we did not use strtok when parsing is not thread safe.
https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
