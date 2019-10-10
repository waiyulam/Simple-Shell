#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char *cmd = "/bin/date -u";
	int pid = fork();
	int status = 0;
	if (pid == 0) {
		/* Child process*/
		execl("/bin/sh", "sh", "-c", cmd, (char *) NULL);
		exit(1);
	} else{
		wait(&status);
	}

	fprintf(stderr, "+ completed \'%s\': [%d]\n", cmd, status);

	return EXIT_SUCCESS;
}
