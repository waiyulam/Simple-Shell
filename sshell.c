#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	char *cmd = "/bin/date -u";
	int retval;

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
