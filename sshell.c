#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
 #include <sys/wait.h>

int main(int argc, char *argv[])
{
	char *cmd = "/bin/date -u";
	int retval = 0;

	int pid = fork();
	if (pid == 0) {
		/* Child process*/
		retval = execl("/bin/sh", "sh", "-c", cmd, (char *) NULL);
		perror("execl");
		exit(1);
	}
	wait(NULL);

	fprintf(stdout, "Return status value for '%s': %d\n", cmd, retval);

	return EXIT_SUCCESS;
}
